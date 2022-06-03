#include <iostream>
#include "stdafx.h"
#include <Windows.h>
#include <cstdio>
#include <conio.h>
#include <hwebcore.h>
#include "FileUtil.h"

HRESULT _cdecl _tmain(int argc, _TCHAR* argv[])
{
    std::cout << "Hello World!\n";

    // specify HRESULT for returning errors
    HRESULT hr = S_OK;

    // create arrays to hold paths
    WCHAR wszInetPath[MAX_PATH];
    WCHAR wszDllPath[MAX_PATH];
    WCHAR wszCfgPath[MAX_PATH];

    // path of inetsrv folder
    DWORD nSize = ::ExpandEnvironmentStringsW(L"%windir%\\system32\\inetsrv", wszInetPath, MAX_PATH);

    // exit if the path of inetsrv folder cannot be determined
    if (nSize == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        printf("Could not determine Inetsrv folder.\n");
        printf("Error: 0x%lx\n", hr);
        return hr;
    }

    // append web core dll name to Inetsrv path
    wcscpy_s(wszDllPath, MAX_PATH - 1, wszInetPath);
    wcscat_s(wszDllPath, MAX_PATH - 1, L"\\");
    wcscat_s(wszDllPath, MAX_PATH - 1, WEB_CORE_DLL_NAME);

    // append config file name
    const std::string binFolder = FilePathUtil::GetCurrentFolder();
    std::wstring widestr = std::wstring(binFolder.begin(), binFolder.end());
    wcscpy_s(wszCfgPath, MAX_PATH - 1, widestr.c_str());
    wcscat_s(wszCfgPath, MAX_PATH - 1, L"\\HostedWebTest.config");
    /*wcscpy_s(wszCfgPath, MAX_PATH - 1, wszInetPath);
    wcscat_s(wszCfgPath, MAX_PATH - 1, L"\\HostedWebTest.config");*/

    // pointer to WebCoreActivate
    PFN_WEB_CORE_ACTIVATE pfnWebCoreActivate = nullptr;
    // pinter to WebCoreShutdown
    PFN_WEB_CORE_SHUTDOWN pfnWebCoreShutdown = nullptr;

    // load web core dll
    HINSTANCE hDLL = ::LoadLibraryW(wszDllPath);
    if (hDLL == nullptr)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        printf("Could not load DLL.\n");
        printf("Error: 0x%lx\n", hr);
        return hr;
    }

	printf("Loaded dll.\n");
    pfnWebCoreActivate = reinterpret_cast<PFN_WEB_CORE_ACTIVATE>(GetProcAddress(hDLL, WEB_CORE_ACTIVATE_DLL_ENTRY));
    if (pfnWebCoreActivate == nullptr)
    {
        // Retrieve the last error.
        hr = HRESULT_FROM_WIN32(GetLastError());
        // Return an error status to the console.
        printf("Could not resolve WebCoreActivate.\n");
        printf("Error: 0x%lx\n", hr);
        return hr;
    }

    printf("WebCoreActivate successfully resolved.\n");
    // Retrieve the address for "WebCoreShutdown".
    pfnWebCoreShutdown = reinterpret_cast<PFN_WEB_CORE_SHUTDOWN>(GetProcAddress(hDLL, WEB_CORE_SHUTDOWN_DLL_ENTRY));
    if (pfnWebCoreShutdown == nullptr)
    {
        // Retrieve the last error.
        hr = HRESULT_FROM_WIN32(GetLastError());
        // Return an error status to the console.
        printf("Could not resolve WebCoreShutdown.\n");
        printf("Error: 0x%lx\n", hr);
        return hr;
    }

    printf("WebCoreShutdown successfully resolved.\n");
    // Return an activation status to the console.
    printf("Activating the Web core...\n");

    try {
        // Activate the Web core.
        hr = pfnWebCoreActivate(wszCfgPath, nullptr, L"TestWebSite");
        /*hr = pfnWebCoreActivate(
            L"C:\\work\\azs\\agent\\2203_azs-hostplugin\\out\\debug-amd64\\HostGAPlugin\\Data\\Config\\applicationHost.config", 
            nullptr, 
            L"Guest Information Server");*/

        // Test for an error.
        if (FAILED(hr))
        {
            // Return an error status to the console.
            printf("WebCoreActivate failed.\n");
            printf("Error: 0x%lx\n", hr);
            return hr;
        }

        // Return a success status to the console.
        printf("WebCoreActivate was successful.\n");
        // Prompt the user to continue.
        printf("Press any key to continue...\n");
        // Wait for a key press.
        //int iKeyPress = _getch();
        // Return a shutdown status to the console.
        printf("Shutting down the Web core...\n");
        // Shut down the Web core.
        hr = pfnWebCoreShutdown(0L);
        if (FAILED(hr))
        {
            // Return an error status to the console.
            printf("WebCoreShutdown failed.\n");
            printf("Error: 0x%lx\n", hr);
        }

        printf("WebCoreShutdown was successful.\n");
    }
    catch (...)
    {
        printf("Failed to activate.\n");
        hr = HRESULT_FROM_WIN32(GetLastError());
        printf("Error: 0x%lx\n", hr);
    }

    FreeLibrary(hDLL);
    return hr;
}
