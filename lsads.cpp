///////////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2010 - Oliver Schneider (assarbad.net)
///
/// Original filename: lsads.cpp
/// Project          : lads
/// Date of creation : 2010-01-26
/// Author(s)        : Oliver Schneider
///
/// Purpose          : List Alternate Data Streams
///
///////////////////////////////////////////////////////////////////////////////

// $Id$

///////////////////////////////////////////////////////////////////////////////
// #define UNICODE
// #define _UNICODE
// These two defines are given implicitly through the settings of C_DEFINES in
// the SOURCES file of the project. Hence change them there and there only.
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <tchar.h>
#include "lsads.h"

BOOL isCommandLineSwitch(_TCHAR *arg, const _TCHAR *switchName = NULL)
{
    if(arg && wcslen(arg) > 2 && arg[0] == L'-' && arg[1] == L'-')
    {
        if(!switchName)
        {
            return TRUE;
        }
        if(0 == wcscmp(&arg[2], switchName))
        {
            return TRUE;
        }
    }
    return FALSE;
}

int __cdecl _tmain(int argc, _TCHAR *argv[])
{
    if(2 != argc)
    {
        _tprintf(_T("Syntax:\n\tlsads <filename>\n"));
        return 1;
    }
    CAlternateDataStreams ads(argv[1]);
    _tprintf(_T("%s (%u)\n"), ads.getPath(), ads.getStreamCount());
    for(size_t i = 0; ads[i]; i++)
    {
        CAlternateDataStreams::CWideString ws(ads.getPath());
        ws += ads[i];
        _tprintf(_T("\t%s\n"), ws.getBuf());
    }
    return 0;
}
