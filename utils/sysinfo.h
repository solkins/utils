#ifndef SYSINFO_H
#define	SYSINFO_H

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static unsigned cpu_count()
{
#ifdef WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_CONF);
#endif
}

#endif	/* SYSINFO_H */

