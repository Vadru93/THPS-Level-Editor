#ifndef __DEBUG__H
#define __DEBUG__H

#include "windows.h"
#include <stdio.h>
#include "Psapi.h"
#include <string.h>
#include <stdlib.h>

void Trace(LPCSTR format, ...)
{
	if(format)
	{		
		va_list vl;
		char	str[4096];

		va_start(vl, format);
		_vsnprintf(str, (sizeof(str) - 1), format, vl);
		str[(sizeof(str) - 1)] = 0;
		va_end(vl);

    ::MessageBox(0,str,str,0);
		// Output to debugger channel
		OutputDebugString(str);
	}
}


// ****************************************************************************
// Func: AddressToMappedName()
// Desc: Returns base name to memory address if it's mapped in the target
//       process.
//
// ****************************************************************************
char *AddressToMappedName(HANDLE hOwner, PVOID pAddress, char *szBuffer, int iSize)
{
    if(szBuffer && (iSize > 0))
    {     
	ZeroMemory(szBuffer, iSize);
	
	char szFullPath[MAX_PATH];
	if(GetMappedFileName(hOwner, pAddress, szFullPath, (sizeof(szFullPath) - 1)))
	{
                // Remove the path, keeping just the base name
                // TODO: You might optionally want the full path
		szFullPath[sizeof(szFullPath) - 1] = 0;	
		char szFileName[_MAX_FNAME + _MAX_EXT], szExtension[_MAX_EXT];
		_splitpath(szFullPath, NULL, NULL, szFileName, szExtension);
		_snprintf(szBuffer, (iSize - 1), "%s%s", szFileName, szExtension);
		szBuffer[(iSize - 1)] = 0;		
		return(szBuffer);
	}	
	
	 // Try alternate way, since first failed    
        {
            HMODULE hModule = NULL;
            if(pAddress)
            {
                // Try it as a module handle first        
                GetModuleHandleEx((GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS), (LPCTSTR) pAddress, &hModule);
                if(!hModule) hModule = (HMODULE) pAddress;
            }
              
            if(GetModuleBaseName(hOwner, hModule, szBuffer, (iSize - 1)) > 0)
                return(szBuffer);                
            else
                // Fix weird bug where GetModuleBaseName() puts a random char in [0] on failure
                szBuffer[0] = 0;
        }
    }	

    return(NULL);
}

BOOL ReportException(LPCTSTR pszFunction, LPEXCEPTION_POINTERS pExceptionInfo)
{
	char szModule[MAX_PATH] = {"Unknown"};
	AddressToMappedName(GetCurrentProcess(), pExceptionInfo->ExceptionRecord->ExceptionAddress, szModule, sizeof(szModule));
	Trace("DLL: ** Exception: %08X, @  %08X, in \"%s\", from Module: \"%s\", Base: %08X **\n", pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress, pszFunction, szModule, GetModuleHandle(szModule));
	return(TRUE);
}

#define DEBUGSTART() __try {
#define DEBUGEND()   }__except(ReportException(__FUNCTION__, GetExceptionInformation())){}
#endif