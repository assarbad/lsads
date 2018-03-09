///////////////////////////////////////////////////////////////////////////////
///
/// Written by Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
/// Purpose          : Class to list ADS on files (NTFS volumes)
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __ALTERNATEDATASTREAMS_H_VER__
#define __ALTERNATEDATASTREAMS_H_VER__ 2018030923
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // Check for "#pragma once" support

#include <tchar.h>
#include <Windows.h>
#ifndef __NTNATIVE_H_VER__
#pragma warning(disable:4005)
#include <ntstatus.h>
#pragma warning(default:4005)
#endif // __NTNATIVE_H_VER__

#define WIN32_UNICODE_PREFIX L"\\\\?\\"
#define DATA_TAG_NAME        L":$DATA"

class CAlternateDataStreams
{
    /* hide some default stuff */
    CAlternateDataStreams& operator=(CAlternateDataStreams&);
    CAlternateDataStreams();
    CAlternateDataStreams(CAlternateDataStreams&);
public:
    // Allow the header to be included again outside of this file (and class)
    // Still this header needs to be included before anyone else who requires VerySimpleBuf.hpp
#   define __VERYSIMPLEBUF_MULTI_INC__
#   include "VerySimpleBuf.hpp"
#   undef __VERYSIMPLEBUF_HPP_VER__
    // Just make it more readable
    typedef CVerySimpleBuf<WCHAR> CWideString, *PWideString;
    typedef CVerySimpleBuf<CHAR>  CAnsiString, *PAnsiString;
    typedef CVerySimpleBuf<TCHAR> CTString, *PTString;
    typedef CVerySimpleBuf<BYTE>  ByteBuf;
private:
#ifndef __NTNATIVE_H_VER__
    // Stuff we need to talk to NTDLL
    typedef LONG NTSTATUS;

    typedef struct _IO_STATUS_BLOCK
    {
        union
        {
            NTSTATUS Status;
            PVOID Pointer;
        };
        ULONG_PTR Information;
    } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

    typedef enum _NT_FILE_INFORMATION_CLASS
    {
        FileInformationDirectory = 1,
        FileInformationFullDirectory, //  2
        FileInformationBothDirectory, //  3
        FileInformationBasic, //  4
        FileInformationStandard, //  5
        FileInformationInternal, //  6
        FileInformationEa, //  7
        FileInformationAccess, //  8
        FileInformationName, //  9
        FileInformationRename, //  10
        FileInformationLink, //  11
        FileInformationNames, //  12
        FileInformationDisposition, //  13
        FileInformationPosition, //  14
        FileInformationFullEa, //  15
        FileInformationMode, //  16
        FileInformationAlignment, //  17
        FileInformationAll, //  18
        FileInformationAllocation, //  19
        FileInformationEndOfFile, //  20
        FileInformationAlternateName, //  21
        FileInformationStream, //  22
        FileInformationPipe, //  23
        FileInformationPipeLocal, //  24
        FileInformationPipeRemote, //  25
        FileInformationMailslotQuery, //  26
        FileInformationMailslotSet, //  27
        FileInformationCompression, //  28
        FileInformationObjectId, //  29
        FileInformationCompletion, //  30
        FileInformationMoveCluster, //  31
        FileInformationQuota, //  32
        FileInformationReparsePoint, //  33
        FileInformationNetworkOpen, //  34
        FileInformationAttributeTag, //  35
        FileInformationTracking, //  36
        FileInformationIdBothDirectory, //  37
        FileInformationIdFullDirectory, //  38
        FileInformationValidDataLength, //  39
        FileInformationShortName, //  40
        FileInformationIoCompletionNotification, //  41
        FileInformationIoStatusBlockRange, //  42
        FileInformationIoPriorityHint, //  43
        FileInformationSfioReserve, //  44
        FileInformationSfioVolume, //  45
        FileInformationHardLink, //  46
        FileInformationProcessIdsUsingFile, //  47
        FileInformationNormalizedName, //  48
        FileInformationNetworkPhysicalName, //  49
        FileInformationIdGlobalTxDirectory, //  50
        FileInformationIsRemoteDevice, //  51
        FileInformationAttributeCache, //  52
        FileInformationMaximum,
    } NT_FILE_INFORMATION_CLASS, *PNT_FILE_INFORMATION_CLASS;

#pragma pack(push, 4)
    typedef struct _FILE_STREAM_INFORMATION // Information Class 22
    {
        ULONG NextEntryOffset;
        ULONG StreamNameLength;
        LARGE_INTEGER EndOfStream;
        LARGE_INTEGER AllocationSize;
        WCHAR StreamName[1];
    } FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;
#pragma pack(pop)

    typedef NTSTATUS (WINAPI *NtQueryInformationFile_t)
        (
        HANDLE hFile,
        PIO_STATUS_BLOCK IoStatusBlock,
        PVOID FileInformation,
        ULONG Length,
        NT_FILE_INFORMATION_CLASS FileInformationClass
        );

#endif // __NTNATIVE_H_VER__

    static const DWORD dwAllocIncrement = 0x400;

    static NtQueryInformationFile_t getNtQueryInformationFileAddress()
    {
        HMODULE hNtdll = ::GetModuleHandle(_T("ntdll.dll"));
        if(hNtdll)
            return reinterpret_cast<NtQueryInformationFile_t>(::GetProcAddress(hNtdll, "NtQueryInformationFile"));
        else
            return reinterpret_cast<NtQueryInformationFile_t>(0);
    }

public:
    CAlternateDataStreams(WCHAR const* Path)
        : m_Path(normalizePath_(Path))
        , m_Attr(::GetFileAttributesW(m_Path.getBuf()))
        , m_OpenFlags(((m_Attr & FILE_ATTRIBUTE_DIRECTORY) ? FILE_FLAG_BACKUP_SEMANTICS : 0))
        , m_NtQueryInformationFile(getNtQueryInformationFileAddress())
        , m_StreamCount(0)
        , m_BufSize(dwAllocIncrement)
        , m_StreamNames(0)
    {
        if(0 != (m_StreamCount = cacheStreams_()))
        {
            retrieveNames_();
        }
    }

    ~CAlternateDataStreams()
    {
        emptyStreamNames_();
    }

    inline bool isDirectory() const
    {
        return (0 != (m_Attr & FILE_ATTRIBUTE_DIRECTORY));
    }

    // Number of streams found
    DWORD getStreamCount() const
    {
        return m_StreamCount;
    }

    // getPath of the file/directory
    WCHAR const* getPath() const
    {
        return m_Path.getBuf();
    }

    WCHAR const* operator[](size_t idx) const
    {
        if((idx < m_StreamCount) && (m_StreamNames[idx]))
        {
            return m_StreamNames[idx]->getBuf();
        }
        return 0;
    }
private:
    CWideString normalizePath_(WCHAR const* Path)
    {
        if(Path)
        {
            WCHAR const Win32Prefix[] = WIN32_UNICODE_PREFIX;
            CVerySimpleBuf<WCHAR> sPath((0 != wcsncmp(Win32Prefix, Path, wcslen(Win32Prefix))) ? Win32Prefix : L"");
            if(sPath.getCountZ()) // Only do anything if the path doesn't have the prefix, yet
            {
                // The following handles the special case where Path == "." ... any other (including "..") seem to be handled fine by GetFullPathName()
                if(1 == wcslen(Path) && 0 == wcsncmp(L".", Path, 1))
                {
                    DWORD dwNeeded = ::GetCurrentDirectoryW(0, NULL);
                    if(dwNeeded)
                    {
                        if(sPath.reAlloc(1 + dwNeeded + sPath.getCount()))
                        {
                            if(0 < ::GetCurrentDirectoryW(static_cast<DWORD>(sPath.getCount() - sPath.getCountZ()), sPath.getBuf() + sPath.getCountZ()))
                            {
                                return sPath;
                            }
                        }
                        return Path; // fallback method
                    }
                }
                LPWSTR filePart = 0;
                // dummy call to evaluate required length
                DWORD dwNeeded = ::GetFullPathNameW(Path, 0, sPath.getBuf(), &filePart);
                if(sPath.reAlloc(1 + dwNeeded + wcslen(Path)))
                {
                    if(0 < ::GetFullPathNameW(Path, static_cast<DWORD>(sPath.getCount() - sPath.getCountZ()), sPath.getBuf() + sPath.getCountZ(), &filePart))
                    {
                        return sPath;
                    }
                }
            }
        }
        return Path;
    }

    DWORD cacheStreams_()
    {
        DWORD dwReturn = 0;
        if(m_NtQueryInformationFile)
        {
            // We only cache stream names for files
            HANDLE hFile = ::CreateFile(m_Path.getBuf(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, m_Attr | m_OpenFlags, NULL);
            if(INVALID_HANDLE_VALUE != hFile)
            {
                IO_STATUS_BLOCK iostat;
                NTSTATUS status = -1;
                do
                {
                    m_BufSize += dwAllocIncrement;
                    if(!m_Buffer.reAlloc(m_BufSize))
                    {
                        break;
                    }
                    reinterpret_cast<PFILE_STREAM_INFORMATION>(m_Buffer.getBuf())->StreamNameLength = 0;
                    status = m_NtQueryInformationFile(
                        hFile
                        , &iostat
                        , reinterpret_cast<PVOID>(m_Buffer.getBuf())
                        , static_cast<ULONG>(m_Buffer.getByteCount())
                        , FileInformationStream
                        );
                } while(STATUS_BUFFER_OVERFLOW == status);
                // No errors? Well, then count the elements
                if(status >= 0)
                {
                    BYTE* curr = m_Buffer.getBuf();
                    PFILE_STREAM_INFORMATION psi = reinterpret_cast<PFILE_STREAM_INFORMATION>(curr);
                    if(psi->StreamNameLength)
                    {
                        do
                        {
                            dwReturn++;
                            curr += psi->NextEntryOffset;
                            psi = reinterpret_cast<PFILE_STREAM_INFORMATION>((psi->NextEntryOffset) ? curr : 0);
                        } while (psi);
                    }
                }
                // We handle errors by simply returning a count of zero
                // and nulling the buffer
                else
                {
                    m_Buffer.reAlloc(0); // free the buffer
                    dwReturn = 0;
                }
                ::CloseHandle(hFile);
            }
        }
        return dwReturn;
    }

    void retrieveNames_()
    {
        try
        {
            BYTE* curr = m_Buffer.getBuf();
            PFILE_STREAM_INFORMATION psi = reinterpret_cast<PFILE_STREAM_INFORMATION>(curr);
            // Allocate an array
            m_StreamNames = new PWideString[m_StreamCount]; // may throw
            // Clear it
            memset(m_StreamNames, 0, m_StreamCount * sizeof(PWideString));
            // Directories may have no streams at all
            if(psi->StreamNameLength)
            {
                // Walk through our buffer with the stream data
                size_t i = 0;
                do 
                {
                    CWideString sName;
                    size_t const sLen = (psi->StreamNameLength / sizeof(WCHAR));
                    if(sName.reAlloc(sLen + 1))
                    {
                        memcpy(sName.getBuf(), psi->StreamName, psi->StreamNameLength);
                        sName.getBuf()[sLen] = 0;
                        m_StreamNames[i] = new CWideString(sName);
                        ++i;
                    } // TODO: else store error
                    ULONG NextEntryOffset = psi->NextEntryOffset;
                    curr += NextEntryOffset;
                    psi = reinterpret_cast<PFILE_STREAM_INFORMATION>(NextEntryOffset ? curr : 0);
                } while (psi);
            }
        }
        catch(...)
        {
            emptyStreamNames_(); // TODO: also make sure to somehow relay that info to the outside world
        }
    }

    void emptyStreamNames_()
    {
        if(m_StreamNames)
        {
            for(DWORD i = 0; i < m_StreamCount; i++)
            {
                delete m_StreamNames[i];
            }
        }
        delete[] m_StreamNames;
        m_StreamNames = 0;
        m_StreamCount = 0;
    }

    CWideString                     m_Path;
    DWORD const                     m_Attr;
    DWORD const                     m_OpenFlags;
    NtQueryInformationFile_t        m_NtQueryInformationFile;
    DWORD mutable                   m_StreamCount;
    CVerySimpleBuf<BYTE> mutable    m_Buffer;
    DWORD mutable                   m_BufSize;
    PWideString*                    m_StreamNames;
};

#endif // __ALTERNATEDATASTREAMS_H_VER__
