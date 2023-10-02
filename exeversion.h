///////////////////////////////////////////////////////////////////////////////
///
/// Written by Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
/// Defines for the version information in the resource file
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __EXEVERSION_H_VER__
#define __EXEVERSION_H_VER__ 2017091421

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#    pragma once
#endif

#include "hgid.h"

// ---------------------------------------------------------------------------
// Several defines have to be given before including this file. These are:
// ---------------------------------------------------------------------------
#define TEXT_AUTHOR        Oliver Schneider  // author (optional value)
#define PRD_MAJVER         1                 // major product version
#define PRD_MINVER         1                 // minor product version
#define PRD_PATCH          0                 // patch number
#define PRD_BUILD          HG_REV_NO         // build number for product
#define PRD_BUILD_NUMERIC  HG_REV_NO_NUMERIC // build number for product
#define FILE_MAJVER        PRD_MAJVER        // major file version
#define FILE_MINVER        PRD_MINVER        // minor file version
#define FILE_PATCH         PRD_PATCH         // patch number
#define FILE_BUILD         PRD_BUILD         // build number
#define FILE_BUILD_NUMERIC PRD_BUILD_NUMERIC // build number for product
#define EXE_YEAR           2010 - 2013, 2017 // current year or timespan (e.g. 2003-2007)
#define TEXT_WEBSITE \
https:               \
    / ## / assarbad.net                              // website
#define TEXT_PRODUCTNAME List Alternate Data Streams // product's name
#define TEXT_FILEDESC                          \
    Allows to list alternate data streams(ADS) \
    on files                                             // component description
#define TEXT_COMPANY      Oliver Schneider(assarbad.net) // company
#define TEXT_MODULE       lsads                          // module name
#define TEXT_COPYRIGHT    Written EXE_YEAR by TEXT_COMPANY(PUBLIC DOMAIN / CC0)
#define TEXT_INTERNALNAME lsads.exe
#define HG_REPOSITORY     "https://sourceforge.net/p/lsads/code/"

#define _ANSISTRING(text) #text
#define ANSISTRING(text)  _ANSISTRING(text)

#define _WIDESTRING(text) L##text
#define WIDESTRING(text)  _WIDESTRING(text)

#define PRESET_UNICODE_STRING(symbol, buffer) \
    UNICODE_STRING symbol = {sizeof(WIDESTRING(buffer)) - sizeof(WCHAR), sizeof(WIDESTRING(buffer)), WIDESTRING(buffer)};

#define CREATE_XVER(maj, min, patch, build) maj##, ##min##, ##patch##, ##build
#define CREATE_FVER(maj, min, patch, build) maj##.##min##.##patch##.##build
#define CREATE_PVER(maj, min, patch, build) maj##.##min##.##patch

#endif // __EXEVERSION_H_VER__
