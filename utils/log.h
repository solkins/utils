#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#ifdef  WIN32
#include <windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
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
		FILE* out = fopen(name, "a+");
        orifd = dup(fileno(stdout));
        dup2(fileno(out), fileno(stdout));
		fclose(out);
    }

    ~usr_log()
    {
        dup2(orifd, fileno(stdout));
		close(orifd);
    }

private:
	int orifd;
};

#endif // LOG_H
