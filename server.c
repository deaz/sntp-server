#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "server.h"

fixedpt64 ntp_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double usecf = (double)tv.tv_usec / 1000000;    // Перевод микросекунд в доли секунды
    uint32_t useci = (uint32_t)(usecf * 4294967296);    // 2^32

    fixedpt64 res = {htonl(tv.tv_sec + JAN_1900), htonl(useci)};
    return res;
}

void make_ntp_msg(ntpmsg *msg)
{
    msg->status = (msg->status & ~LI_MASK);     // Установка LI в 0
    msg->status = (msg->status & ~MODE_MASK) | 4;    // Установка режима в 4 (сервер)
    msg->stratum = 1;
    msg->precision = -6;    // Минимальная точность
    memset(&(msg->delay), 0, sizeof(msg->delay));   // Считаем, что сервер первичный
    memset(&(msg->dispersion), 0, sizeof(msg->dispersion));   // Считаем, что сервер первичный
    strncpy(msg->refid, "LOCL", 4); // Используются некалиброванные часы
    memset(&(msg->reftime), 0, sizeof(msg->reftime));
    msg->origintime = msg->transtime;
    msg->recvtime = ntp_time();
    msg->transtime = ntp_time();
}

int init_server(int *sockfd, sockaddr_in *servaddr)
{
    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sockfd < 0) {
        syslog(LOG_DEBUG, "opening socket: %s", strerror(errno));
        return -1;
    }

    int tr = 1;
    setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(tr));

    memset(servaddr, 0, sizeof(*servaddr));
    (*servaddr).sin_family = AF_INET;
    (*servaddr).sin_port = htons(NTP_PORT);
    (*servaddr).sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(*sockfd, (sockaddr*)servaddr, sizeof(*servaddr))) {
        syslog(LOG_DEBUG, "binding: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int server()
{
    int sockfd;
    sockaddr_in servaddr;
    if (init_server(&sockfd, &servaddr) < 0) {
        return -1;
    }

    sockaddr_in cliaddr;
    socklen_t cliaddr_size;
    int recvbytes;
    char buf[sizeof(ntpmsg)];
    while (1) {
        cliaddr_size = sizeof(cliaddr);
        recvbytes = recvfrom(sockfd, buf, sizeof(ntpmsg), 0, (sockaddr*)&cliaddr, &cliaddr_size);
        if (recvbytes < 0) {
            syslog(LOG_DEBUG, "receiving: %s", strerror(errno));
            continue;
        }
        if (recvbytes < sizeof(ntpmsg)) {
            syslog(LOG_DEBUG, "message too small");
            continue;
        }

        make_ntp_msg((ntpmsg*)buf);

        if (sendto(sockfd, buf, sizeof(ntpmsg), 0, (sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
            syslog(LOG_DEBUG, "sending: %s", strerror(errno));
            continue;
        }

        syslog(LOG_DEBUG, "client synced");
    }

    return 0;
}
