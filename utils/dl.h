#ifndef DL_H
#define DL_H

/* sample
 *
 * dl dll("shell32.dll");
 * typedef HINSTANCE (*pShellExecute)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);
 * dll.parse<pShellExecute>("ShellExecuteA")(NULL, "cmd", NULL, NULL, NULL, SW_SHOWNORMAL);
 *
 */

#ifdef WIN32
#include <Windows.h>
#elif
#include <dlfcn.h>
typedef void* HMODULE;
typedef int (*FARPROC)();
#endif

class dl
{
public:
    dl(const char* filename)
    {
#ifdef WIN32
            hmod = LoadLibraryA(filename);
#elif
            hmod = dlopen(filename, RTLD_LAZY);
#endif
    }

    ~dl()
    {
#ifdef WIN32
            FreeLibrary(hmod);
#elif
            dlclose(hmod);
#endif
            hmod = nullptr;
    }

    template<typename T>
    T parse(const char* name)
    {
#ifdef WIN32
        return (T)GetProcAddress(hmod, name);
#elif
        return (T)dlsym(hmod, name);
#endif
    }

private:
    HMODULE hmod;
};

#endif // DL_H
