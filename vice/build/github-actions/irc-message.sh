#!/bin/bash
set -o nounset

function err {
    >&2 echo -e $@
    >&2 echo
    exit 1
}

if [ $# != 1 ]; then
    err "Usage:\n\t$(basename $0) <message to log to IRC>"
fi

MSG="$1"

if [ -z "$IRC_PASS" ]; then
    err "Missing env IRC_PASS"
fi

set -o errexit

IRC_USER=vice-builder
HASH=$(echo -e -n "$IRC_USER\0$IRC_USER\0$IRC_PASS" | base64)
CHAN=#vice-dev

(
echo    "CAP REQ :sasl"
echo    "NICK $IRC_USER"
echo    "USER $IRC_USER 8 * : $IRC_USER"
echo    "AUTHENTICATE PLAIN"
echo    "AUTHENTICATE $HASH"
echo    "CAP END"
echo    "JOIN $CHAN"
echo -e "PRIVMSG $CHAN :\x01ACTION $MSG\x01"
echo QUIT
) | openssl s_client -quiet -ign_eof -connect irc.libera.chat:6697