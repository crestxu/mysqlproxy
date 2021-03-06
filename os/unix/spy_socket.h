#ifndef _SPY_SOCKET_INCLUDE_H_
#define _SPY_SOCKET_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

typedef int spy_socket_t;

#define spy_socket          socket
#define spy_socket_n        "socket()"

#define spy_shutdown_socket    shutdown
#define spy_shutdown_socket_n  "shutdown()"

#define spy_close_socket    close
#define spy_close_socket_n  "close() socket"

#define spy_nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#define spy_nonblocking_n   "fcntl(O_NONBLOCK)"

#endif
