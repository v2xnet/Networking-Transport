/**
 * @file util.h
 */

#ifndef __UTIL_H__
#define __UTIL_H__ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define TIME_SEC_MSEC 1000
#define TIME_SEC_NSEC 1000000000
#define TIME_MSEC_NSEC 1000000

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define tstomsec(tv) ((tv).tv_sec * TIME_SEC_MSEC + (tv).tv_nsec / TIME_MSEC_NSEC)

#define tstodsec(tv) ((double)(tv).tv_sec + (double)(tv).tv_nsec / TIME_SEC_NSEC)

#define tsisset(tv) ((tv).tv_sec || (tv).tv_nsec)
#define tsclear(tv) ((tv).tv_sec = (tv).tv_nsec = 0)

#define tscmp(a, b, CMP) (((a).tv_sec == (b).tv_sec) ? ((a).tv_nsec CMP(b).tv_nsec) : ((a).tv_sec CMP(b).tv_sec))

#define tsadd(a, b, result)                           \
    do {                                              \
        (result).tv_sec = (a).tv_sec + (b).tv_sec;    \
        (result).tv_nsec = (a).tv_nsec + (b).tv_nsec; \
        if((result).tv_nsec >= TIME_SEC_NSEC) {       \
            ++(result).tv_sec;                        \
            (result).tv_nsec -= TIME_SEC_NSEC;        \
        }                                             \
    } while(0)

#define tssub(a, b, result)                           \
    do {                                              \
        (result).tv_sec = (a).tv_sec - (b).tv_sec;    \
        (result).tv_nsec = (a).tv_nsec - (b).tv_nsec; \
        if((result).tv_nsec < 0) {                    \
            --(result).tv_sec;                        \
            (result).tv_nsec += TIME_SEC_NSEC;        \
        }                                             \
    } while(0)

#define tsafter(a, b) tscmp(a, b, <)
#define tsbefore(a, b) tscmp(a, b, >)

#define tscpy(to, from)                \
    do {                               \
        (to).tv_sec = (from).tv_sec;   \
        (to).tv_nsec = (from).tv_nsec; \
    } while(0)

#define tsset(tv, sec, nsec)   \
    do {                       \
        (tv).tv_sec = (sec);   \
        (tv).tv_nsec = (nsec); \
    } while(0)

#define tssetsec(tv, sec)    \
    do {                     \
        (tv).tv_sec = (sec); \
        (tv).tv_nsec = 0;    \
    } while(0)

#define tssetmsec(tv, msec)                                       \
    do {                                                          \
        (tv).tv_sec = (msec) / TIME_SEC_MSEC;                     \
        (tv).tv_nsec = ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
    } while(0)

#define tssetdsec(tv, sec)                                            \
    do {                                                              \
        (tv).tv_sec = (long)(sec);                                    \
        (tv).tv_nsec = (long)(((sec) - (tv).tv_sec) * TIME_SEC_NSEC); \
    } while(0)

#define tsinc(tv, sec, nsec)                \
    do {                                    \
        (tv).tv_sec += (sec);               \
        (tv).tv_nsec += (nsec);             \
        if((tv).tv_nsec >= TIME_SEC_NSEC) { \
            ++(tv).tv_sec;                  \
            (tv).tv_nsec -= TIME_SEC_NSEC;  \
        }                                   \
    } while(0)

#define tsincmsec(tv, msec)                                        \
    do {                                                           \
        (tv).tv_sec += (msec) / TIME_SEC_MSEC;                     \
        (tv).tv_nsec += ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
        if((tv).tv_nsec >= TIME_SEC_NSEC) {                        \
            ++(tv).tv_sec;                                         \
            (tv).tv_nsec -= TIME_SEC_NSEC;                         \
        }                                                          \
    } while(0)

#define tsdec(tv, sec, nsec)               \
    do {                                   \
        (tv).tv_sec -= sec;                \
        (tv).tv_nsec -= nsec;              \
        if((tv).tv_nsec < 0) {             \
            --(tv).tv_sec;                 \
            (tv).tv_nsec += TIME_SEC_NSEC; \
        }                                  \
    } while(0)

#define tsdecmsec(tv, msec)                                        \
    do {                                                           \
        (tv).tv_sec -= (msec) / TIME_SEC_MSEC;                     \
        (tv).tv_nsec -= ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
        if((tv).tv_nsec < 0) {                                     \
            --(tv).tv_sec;                                         \
            (tv).tv_nsec += TIME_SEC_NSEC;                         \
        }                                                          \
    } while(0)

#define tsmin(a, b) tsbefore((a), (b)) ? (b) : (a)

#define tsmax(a, b) tsafter((a), (b)) ? (b) : (a)


static inline uint32_t timeval_to_ms(const struct timeval* tv)
{
    return ((uint32_t)(tv->tv_sec * 1000) + (tv->tv_usec / 1000));
}

static inline uint64_t timeval_to_us(const struct timeval* tv)
{
    return ((uint64_t)(tv->tv_sec * 1000000) + (tv->tv_usec));
}
#endif /* __UTIL_H__ */
