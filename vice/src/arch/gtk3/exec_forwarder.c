/**********************************************************************
 * EXEC_FORWARDER - Seamlessly forward a command line to a different
 *                  program
 *
 * GTK3 requires VICE binaries to live in a bin/ directory in order
 * to correctly locate its own resources. This provides a tiny
 * shell that will extract its own name and location and forward
 * its commandline, suitable transformed and quoted, to the program
 * of the same name in the bin/ directory under its file location.
 *
 * This program should be built with the "Windows" subsystem and
 * with the multibyte character set (so that it will call main).
 * Despite not being compiled in "Unicode mode" it shuold be
 * capable of handling arbitrary Unicode in the command line or
 * the underlying file system.
 */

#include <windows.h>
#include <stdio.h>

/* Reimplement wcs functions so we don't have to worry about
 * possibly running afoul of some kind of windows CRT nonsense */
void our_wcscpy(WCHAR *dest, WCHAR *src)
{
    while (*dest++ = *src++);
}

void our_wcscat(WCHAR *dest, WCHAR *src)
{
    while (*dest) {
        dest++;
    }
    our_wcscpy(dest, src);
}

int our_wcslen(WCHAR *str)
{
    int n = 0;
    while (*str) {
        str++;
        n++;
    }
    return n;
}

/* In a Windows command line, quotation of an argument is only
 * necessary if there are spaces in it, but it's easier to ensure
 * consistent behavior of we also quote arguments with quotation
 * marks within them.
 *
 * IN THEORY we will never be handed an argument with quotes in it;
 * we only ever need to potentially quote a file name that has been
 * given to us by the Windows API itself, and quotation marks are
 * not legal characters in filenames.
 *
 * I'm pretty sure I've been able to get Windows to create files
 * with forbidden characters in them, though, so better safe than
 * sorry. */
int needs_quotation(WCHAR *str)
{
    while (*str) {
        if (*str == L' ' || *str == L'"') {
            return 1;
        }
        ++str;
    }
    return 0;
}

void copy_quoted(WCHAR *dest, WCHAR *src)
{
    if (!needs_quotation(src)) {
        our_wcscpy(dest, src);
    } else {
        WCHAR c = 0;
        *dest++ = L'"';
        while (c = *src++) {
            if (c == L'\\') {
                /* Backslashes must be escaped if and only if they are
                 * part of a string of backslashes preceding a quote. */
                int i, n = 1;
                while (*src == L'\\') {
                    src++;
                    n++;
                }
                /* We found n backslashes in a row. How many do we need
                 * to print out? */
                if (*src == L'"') {
                    n *= 2;
                }
                for (i = 0; i < n; ++i) {
                    *dest++ = L'\\';
                }
            } else {
                if (c == L'\"') {
                    /* Quote marks are always escaped. */
                    *dest++ = L'\\';
                }
                *dest++ = c;
            }
        }
        *dest++ = L'"';
        *dest = 0;
    }
}

WCHAR *skip_argument(WCHAR *src)
{
    int quoted = 0;
    while (*src) {
        if (*src == L'"') {
            quoted = 1-quoted;
            ++src;
        } else if (!quoted && (*src == L' ')) {
            return src;
        } else if (*src == L'\\') {
            int n = 0;
            while (*src == L'\\') {
                ++n;
                ++src;
            }
            if (*src == L'"' && (n % 1 == 1)) {
                /* This quotation mark has itself been quoted */
                ++src;
            }
        } else {
            /* Some other character */
            ++src;
        }
    }
    /* The token is the entire string. Return pointer to the null
     * terminator. */
    return src;
}
/* When allowed to completely go nuts, a Windows path name may be up
 * to 32,767 wide characters long. We can spare the 192KB to just ensure
 * that we never need to reallocate anything. */
WCHAR reported_path[32768], full_path[32768], cmdline[32768];

int main(int argc, char **argv)
{
    WCHAR *file_ptr = NULL;
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    GetModuleFileNameW(NULL, reported_path, 32768);
    GetFullPathNameW(reported_path, 32768, full_path, &file_ptr);
    if (!file_ptr) {
        MessageBoxW(NULL, L"Could not find the executable's own filename!\n\nThis should never happen.", L"Flagrant System Error", MB_ICONEXCLAMATION);
        return -1;
    }
    /* Worst case scenario is basically "all backslashes, then a quote", which will roughly
     * double the size of our filename when we quote it. We restrict the filename to 16KB-5
     * to leave room for that if needed. */
    if (our_wcslen(full_path) > 0x3FFA) {
        MessageBoxW(NULL, L"You're trying to launch VICE from a working directory whose name is\n16KiB or more long. Please don't do that.", L"VICE launch error", MB_ICONERROR);
        return -1;
    }
    our_wcscpy(reported_path, file_ptr);
    *file_ptr++ = L'b';
    *file_ptr++ = L'i';
    *file_ptr++ = L'n';
    *file_ptr++ = L'\\';
    our_wcscpy(file_ptr, reported_path);
    copy_quoted(cmdline, full_path);
    file_ptr = skip_argument(GetCommandLineW());
    if (our_wcslen(cmdline) + our_wcslen(file_ptr) > 32767) {
        MessageBoxW(NULL, L"Command line too long. Arguments will be ignored.", L"VICE launch warning", MB_ICONWARNING);
    } else {
        our_wcscat(cmdline, file_ptr);
    }

    ZeroMemory (&si, sizeof(si));
    ZeroMemory (&pi, sizeof(pi));
    si.cb = sizeof(si);
    if (!CreateProcessW(full_path, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        _swprintf(reported_path, L"Could not launch application (Error code %d)!", GetLastError());
        MessageBoxW(NULL, reported_path, L"VICE launch error", MB_ICONERROR);
        return -1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
