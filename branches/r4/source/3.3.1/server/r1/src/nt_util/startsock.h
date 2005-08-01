/*
 startsock.h
 */

#ifndef __STARTSOCK_H__
#define __STARTSOCK_H__

#if defined(__cplusplus)
extern "C"
{
#endif

#ifdef _WIN32

extern int startWinsock();
extern int cleanWinsock();

#endif

#if defined(__cplusplus)
}
#endif

#endif  /* __STARTSOCK_H__ */