#include "vice.h"

#include "uiserver.h"

#include "log.h"
#include "vicesocket.h"

static vice_network_socket_t *server_socket;

int uiserver_init(void)
{
    int retval = 0;
    
    vice_network_socket_address_t *address;
    
    /*
     * We let the OS choose the listen port, so multiple emu
     * can run at the same time. The port is passed to the UI
     * process when launched.
     *
     * If a use case is found for running the emulation on one
     * machine and the ui on another (old ARM devices?) then it
     * will be straightforward to bind to 0.0.0.0 or take a
     * specific address string from a resource.
     */
    
    address = vice_network_address_generate("ip4://127.0.0.1", 0);
    
    server_socket = vice_network_server("UI server", address);
    if (!server_socket) {
        retval = -1;
        goto done;
    }
    
done:
    vice_network_address_close(address);
    return retval;
}

void uiserver_add_screen(void)
{

}

void uiserver_await_ready(void)
{

}

void uiserver_shutdown(void)
{
    /* Notifiy all connected clients that we're shutting down */
    // TODO
    
    if (server_socket) {
        vice_network_socket_close(server_socket);
        server_socket = NULL;
    }
}
