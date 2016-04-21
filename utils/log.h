#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#ifdef  WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <string.h>
#endif

class usr_log
{
public:
    usr_log()
    {
		char* name = nullptr;
#ifdef  WIN32
		char path[260];
		GetModuleFileNameA(NULL, path, 260);
		name = path;
		char* ext = strchr(name, '.');
		strcpy(ext, ".log");
#else
		char path[260];
		name = getenv("_");
		const char* pos = strrchr(name, '/') + 1;
		sprintf(path, "/var/log/%s.log", pos);
		name = path;
#endif
		out = fopen(name, "a+");
    }

	void print(char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vfprintf(out, fmt, args);
		va_end(args);
	}

    ~usr_log()
    {
		fclose(out);
    }

private:
	FILE* out;
};

static usr_log g_usr_log;

#define	LOG	g_usr_log.print

#endif // LOG_H
