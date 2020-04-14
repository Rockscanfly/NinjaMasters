// reference material
// https://github.com/awreece/monotonic_timer/blob/master/monotonic_timer.c

#include <unistd.h>
#include <assert.h>

#define NANOS_PER_SECF 1000000000.0
#define USECS_PER_SEC 1000000

#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
  // If we have it, use clock_gettime and CLOCK_MONOTONIC.

    #include <time.h>

    double init_timer()
    {
        struct timespec res;
        clock_getres(CLOCK_MONOTONIC, &res);
        return ((double) res.tv_nsec);
    }

    double monotonic_timer() {
        struct timespec time;
        // Note: Make sure to link with -lrt to define clock_gettime.
        clock_gettime(CLOCK_MONOTONIC, &time);
        return ((double) time.tv_sec) + ((double) time.tv_nsec / (NANOS_PER_SECF));
    }

#elif defined(_WIN32)
// On Windows, use QueryPerformanceCounter and QueryPerformanceFrequency.
    #include <windows.h>

    static double PCFreq = 0.0;

    // Accoring to http://stackoverflow.com/a/1739265/447288, this will
    // properly initialize the QueryPerformanceCounter.
    double init_timer()
    {
        LARGE_INTEGER li;
        int has_qpc = QueryPerformanceFrequency(&li);
        assert(has_qpc);
        PCFreq = ((double) li.QuadPart);
        return PCFreq;
    }

    double monotonic_timer() {

        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return ((double) li.QuadPart) / PCFreq;
    }
#else
        #error "Timer not supported on this system"
#endif
