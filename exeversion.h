///////////////////////////////////////////////////////////////////////////////
///
/// Written 2010, Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
/// Defines for the version information in the resource file
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __EXEVERSION_H_VERSION__
#define __EXEVERSION_H_VERSION__ 100

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "buildnumber.h"

// ---------------------------------------------------------------------------
// Several defines have to be given before including this file. These are:
// ---------------------------------------------------------------------------
#define TEXT_AUTHOR            Oliver Schneider // author (optional value)
#define PRD_MAJVER             1 // major product version
#define PRD_MINVER             0 // minor product version
#define PRD_BUILD              0 // build number for product
#define FILE_MAJVER            1 // major file version
#define FILE_MINVER            0 // minor file version
#define FILE_BUILD             _FILE_VERSION_BUILD // build number
#define EXE_YEAR               2010 // current year or timespan (e.g. 2003-2009)
#define TEXT_WEBSITE           http:/##/assarbad.net // website
#define TEXT_PRODUCTNAME       List Alternate Data Streams // product's name
#define TEXT_FILEDESC          Allows to list alternate data streams (ADS) on files // component description
#define TEXT_COMPANY           Oliver Schneider (assarbad.net) // company
#define TEXT_MODULE            lsads // module name
#define TEXT_COPYRIGHT         Written  EXE_YEAR TEXT_COMPANY (PUBLIC DOMAIN/CC0) // copyright information
#define TEXT_INTERNALNAME      lsads.exe // copyright information

#endif // __EXEVERSION_H_VERSION__
