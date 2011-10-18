#ifndef _IP_H
#define _IP_H

#include <sys/ioctl.h> // pour ioctl
#include <net/if.h> // pour ifreq
#include <string.h> // pour strcpy
#include <unistd.h> // pour close
#include <arpa/inet.h> // pour inet_ntoa
#include <stdio.h> // pour printf
#include <errno.h> // pour errno
#include <stdlib.h> // pour malloc

char* getIP();

#endif /* _IP_H */
