#include <windows.h>
#include <commctrl.h>

// Copyright © 2019 Jonas Kümmerlin <jonas@kuemmerlin.eu>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

typedef int (*PyMainT)(int argc, WCHAR **argv);

static inline WCHAR *
wcsrchr(const WCHAR *s, int c)
{
    for (int i = lstrlenW(s)-1; i >= 0; --i) {
        if (s[i] == c)
            return (WCHAR*)&s[i];
    }
    return NULL;
}

static inline BOOL
fileExists(const WCHAR *p)
{
    return GetFileAttributesW(p) != (DWORD)-1;
}

static inline void
dieWithError(DWORD error, const WCHAR *title)
{
    WCHAR *errormsg = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   error,
                   0,
                   (WCHAR*)&errormsg,
                   0,
                   NULL);

    MessageBoxW(NULL, errormsg, title, MB_ICONHAND|MB_OK);
    LocalFree(errormsg);
    ExitProcess(error);
}

static inline void
dieWithLastError(const WCHAR *title)
{
    dieWithError(GetLastError(), title);
}

EXTERN_C void WINAPI
msvcWinMainCRTStartup()
{
    InitCommonControls();

    WCHAR pythonpath[MAX_PATH];
    GetModuleFileNameW(NULL, pythonpath, MAX_PATH);
    WCHAR *d = wcsrchr(pythonpath, '\\');

    HMODULE pythondll = NULL;
    lstrcpyW(d, L"\\3rdparty\\python3\\python3.dll");
    pythondll = LoadLibraryExW(pythonpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!pythondll) {
        lstrcpyW(d, L"\\python3\\python3.dll");
        pythondll = LoadLibraryExW(pythonpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    if (!pythondll) {
        lstrcpyW(d, L"\\python3.dll");
        pythondll = LoadLibraryExW(pythonpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }

    if (!pythondll)
        dieWithLastError(L"Loading python3.dll");


    WCHAR scriptpath[MAX_PATH];
    GetModuleFileNameW(NULL, scriptpath, MAX_PATH);

    WCHAR *n = wcsrchr(scriptpath, '.');
    lstrcpyW(n, L".py");
    if (!fileExists(scriptpath))
        lstrcpyW(n, L".pyc");
    if (!fileExists(scriptpath))
        lstrcpyW(n, L".pyz");
    if (!fileExists(scriptpath))
        lstrcpyW(n, L"\\__main__.py");
    if (!fileExists(scriptpath))
        lstrcpyW(n, L"\\__main__.pyc");
    if (!fileExists(scriptpath)) {
        MessageBoxW(NULL, L"couldn't find .py file", L"Error", MB_ICONHAND|MB_OK);
        ExitProcess(255);
    }

    int argc;
    WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    WCHAR **newargv = (WCHAR**)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR*) * (argc + 2));
    newargv[0] = argv[0];
    newargv[1] = scriptpath;
    for (int i = 1; i < argc; ++i)
        newargv[i+1] = argv[i];
    newargv[argc+1] = NULL;

    PyMainT main = (PyMainT)GetProcAddress(pythondll, "Py_Main");
    if (!main)
        dieWithLastError(L"GetProcAddress Py_Main");

    ExitProcess(main(argc + 1, newargv));
}

