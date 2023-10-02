///////////////////////////////////////////////////////////////////////////////
///
/// Written 2010, Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
/// Original filename: lsads.cpp
/// Project          : lads
/// Date of creation : 2010-01-26
/// Author(s)        : Oliver Schneider
///
/// Purpose          : List Alternate Data Streams
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// #define UNICODE
// #define _UNICODE
// These two defines are given implicitly through the settings of C_DEFINES in
// the SOURCES file of the project. Hence change them there and there only.
///////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#ifdef _DEBUG
#    include <crtdbg.h>
#endif // _DEBUG
#include <tchar.h>
#include "lsads.h"
#define __USE_VERYSIMPLEBUF__
#include "VerySimpleBuf.hpp"
#include "VersionInfo.hpp"
#pragma comment(lib, "delayimp")

namespace
{
    BOOL isCommandLineSwitch(_TCHAR* arg, const _TCHAR* switchName = NULL)
    {
        if (arg && wcslen(arg) > 2 && arg[0] == L'-' && arg[1] == L'-')
        {
            if (!switchName)
            {
                return TRUE;
            }
            if (0 == wcscmp(&arg[2], switchName))
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    HMODULE GetInstanceHandle()
    {
        MEMORY_BASIC_INFORMATION mbi;
        static int iDummy;
        ::VirtualQuery(&iDummy, &mbi, sizeof(mbi));
        return HMODULE(mbi.AllocationBase);
    }
} // namespace

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF);
#endif // _DEBUG
    if (2 <= argc)
    {
        bool show_logo = true;
        int fname_arg = 1;
        while (isCommandLineSwitch(argv[fname_arg]))
        {
            if (isCommandLineSwitch(argv[fname_arg], L"nologo"))
                show_logo = false;
            fname_arg++;
        }

        if (show_logo)
        {
            CVersionInfo verinfo(GetInstanceHandle());
            _tprintf(_T("%s %s written by %s\n"), verinfo[_T("OriginalFilename")], verinfo[_T("FileVersion")], verinfo[_T("CompanyName")]);
#ifdef HG_REV_ID
            _tprintf(_T("\tRevision: %s\n"), verinfo[_T("Mercurial revision")]);
#endif
            _tprintf(_T("\n"));
        }

        CAlternateDataStreams ads(argv[fname_arg]);
        _tprintf(_T("%s (%u)\n"), ads.getPath(), ads.getStreamCount());
        for (size_t i = 0; ads[i]; i++)
        {
            CAlternateDataStreams::CWideString ws(ads.getPath());
            ws += ads[i];
            _tprintf(_T("\t%s\n"), ws.getBuf());
        }
        return 0;
    }
    else
    {
        _ftprintf(stderr, _T("Syntax:\n\tlsads [--nologo] <path>\n"));
    }
    return 1;
}
