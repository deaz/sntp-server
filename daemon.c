#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "daemon.h"

int daemon_init(const char *pname, int facility) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    umask(0);

    if (setsid() < 0) {
        return -1;
    }
    if (chdir("/") < 0) {
        return -1;
    }
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog(pname, LOG_PID, facility);

    syslog(LOG_DEBUG, "started");

    return 0;
}
