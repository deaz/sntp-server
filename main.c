#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include "daemon.h"
#include "server.h"

int main(int argc, char *argv[]) {
    if (daemon_init(argv[0], 0) < 0) {
        exit(-1);
    }

    int status;
    status = server();

    syslog(LOG_DEBUG, "exited with %d", status);

    return status;
}
