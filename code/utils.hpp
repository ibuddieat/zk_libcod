#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "gsc.hpp"

qboolean IsNullVector(float *vec);
void VectorClampLength(float *vec, double max);

uint64_t Sys_Milliseconds64(void);

int getProtocolFromShortVersion(const char *shortVersion);
const char * getShortVersionFromProtocol(int protocol);

qboolean IsLocalIPAddress(unsigned char *ip);

#ifndef Q_vsnprintf
int Q_vsnprintf(char *s0, size_t size, const char *fmt, va_list args);
#endif

#ifndef strcmp_constant_time
int strcmp_constant_time(const char *s1, const char *s2);
#endif

#endif