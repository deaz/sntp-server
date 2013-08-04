#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>
#include <stdint.h>

#define JAN_1900 2208988800UL   // 1970 - 1900 в секундах

#define NTPMSG_SIZE sizeof(ntpmsg)

#define NTP_PORT 123

#define LI_MASK 0xC0
#define VER_MASK 0x38
#define MODE_MASK 0x07

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

struct fixedpt32 {
    uint16_t integer;
    uint16_t fraction;
};

struct fixedpt64 {
    uint32_t integer;
    uint32_t fraction;
};

typedef struct fixedpt32 fixedpt32;
typedef struct fixedpt64 fixedpt64;

struct ntpmsg {
    uint8_t status;
    uint8_t stratum;
    uint8_t poll;
    int8_t precision;
    fixedpt32 delay;
    fixedpt32 dispersion;
    char refid[4];
    fixedpt64 reftime;
    fixedpt64 origintime;
    fixedpt64 recvtime;
    fixedpt64 transtime;
};

typedef struct ntpmsg ntpmsg;

int server();

#endif
