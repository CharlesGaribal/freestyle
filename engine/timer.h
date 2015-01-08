/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef TIMER_H
#define TIMER_H

/**
        @author Mathias Paulin <Mathias.Paulin@irit.fr>
*/
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif
#ifdef OS_WIN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#else
#include <sys/time.h>
#endif
#include <unistd.h>
#include <stdlib.h>

namespace vortex{
class Timer {
public:

    typedef double Time;

    Timer() : sum_(0.), last_ ( 0. ) {
        reset();
    }

    ~Timer() {
    }

    inline void start ( void ) {
        last_ = getTime();
    }
    inline void stop ( void ) {
        sum_ += getTime() - last_;
    }
    inline void reset ( void ) {
        sum_ = 0.;
    }

    inline Time restart ( void ) {
        stop();
        double v = value();
        reset();
        start();
        return ( Time )v;
    }

    inline Time value ( void ) const {
        return sum_;
    }

    static Time getTime ( void ) {
        struct timeval tv;
        struct timezone tz;
        gettimeofday ( &tv, &tz );
        return ( Time ) ( ( double ) tv.tv_sec + 1.e-6 * ( double ) tv.tv_usec );
    }

protected:
    Time sum_, last_;
};
}
#endif
