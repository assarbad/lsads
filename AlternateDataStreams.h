///////////////////////////////////////////////////////////////////////////////
///
/// Written 2010, 2012, Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
/// Original filename: ReparsePoint.h
/// Project          : lads
/// Date of creation : 2010-01-26
/// Author(s)        : Oliver Schneider
///
/// Purpose          : Class to list ADS on files (NTFS volumes)
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __ALTERNATEDATASTREAMS_H_VER__
#define __ALTERNATEDATASTREAMS_H_VER__ 2010012621
// $Id$
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // Check for "#pragma once" support

#include <tchar.h>
#include <Windows.h>

#define WIN32_UNICODE_PREFIX L"\\\\?\\"
#define DATA_TAG_NAME        L":$DATA"

class CAlternateDataStreams
{
    template <typename T> class CVerySimpleBuf
    {
    public:
        CVerySimpleBuf(size_t count = 0)
            : m_buf(0)
            , m_count(0)
        {
            reAlloc(count);
        }

        CVerySimpleBuf(const CVerySimpleBuf& rval)
            : m_buf(0)
            , m_count(0)
        {
            operator=(rval);
        }

        CVerySimpleBuf(const T* buf)
            : m_buf(0)
            , m_count(0)
        {
            if(buf)
            {
                operator=(buf);
            }
        }

        ~CVerySimpleBuf()
        {
            delete[] m_buf;
        }

        CVerySimpleBuf& operator=(const CVerySimpleBuf& rval)
        {
            if(&rval != this)
            {
                reAlloc(0);
                if(rval.getBuf() && reAlloc(rval.getCount()))
                {
                    memcpy(getBuf(), rval.getBuf(), getMin_(getByteCount(), rval.getByteCount()));
                }
            }
            return *this;
        }

        CVerySimpleBuf& operator=(const T* buf)
        {
            reAlloc(0);
            if(buf)
            {
                const size_t len = getBufLenZ_<const T*>(buf);
                if(!len)
                {
                    reAlloc(1);
                }
                else if(reAlloc(len))
                {
                    memcpy(getBuf(), buf, getMin_(sizeof(T) * len, getByteCount()));
                }
            }
            return *this;
        }

        CVerySimpleBuf& operator+=(const CVerySimpleBuf& rval)
        {
            if(rval.getCountZ() && reAlloc(getCountZ() + rval.getCountZ()))
            {
                memcpy(getBuf() + getCountZ(), rval.getBuf(), sizeof(T) * rval.getCountZ());
            }
            return *this;
        }

        CVerySimpleBuf& operator+=(const T* buf)
        {
            const size_t len = getBufLenZ_<const T*>(buf);
            if(len && reAlloc(getCountZ() + len))
            {
                memcpy(getBuf() + getCountZ(), buf, sizeof(T) * len);
            }
            return *this;
        }

        inline T* getBuf() const
        {
            return m_buf;
        };

        inline operator bool() const
        {
            return (0 != m_buf);
        }

        inline bool operator!() const
        {
            return (0 != m_buf);
        }

        void clear()
        {
            if(m_buf)
            {
                memset(m_buf, 0, m_count * sizeof(T));
            }
        }

        bool reAlloc(size_t count)
        {
            T* tempBuf = 0;
            size_t count_ = 0;
            if(count)
            {
                count_ = getCeil_(count+1);
                if(count_ <= m_count)
                {
                    memset(m_buf + count, 0, sizeof(T) * (m_count - count));
                    return true;
                }
                if(0 != (tempBuf = new T[count_]))
                {
                    memset(tempBuf, 0, sizeof(T) * count_);
                }
                if(tempBuf && m_buf)
                {
                    memcpy(tempBuf, m_buf, sizeof(T) * getMin_(count, m_count));
                }
            }
            delete[] m_buf;
            m_buf = tempBuf;
            m_count = count_;
            return (0 != m_buf);
        }

        inline size_t getCount() const
        {
            return m_count;
        }

        inline size_t getCountZ() const
        {
            return getBufLenZ_<const T*>(m_buf);
        }

        inline size_t getByteCount() const
        {
            return m_count * sizeof(T);
        }
    protected:
        T* m_buf;
        size_t m_count;

        inline size_t getCeil_(size_t count) const
        {
            const size_t align = sizeof(void*);
            return (((sizeof(T) * count) + (align - 1)) & (~(align - 1))) / sizeof(T);
        }

        inline size_t getMin_(size_t a, size_t b) const
        {
            return ((a < b) ? a : b);
        }

        template <typename T> static size_t getBufLenZ_(const char* val)
        {
            return (val) ? strlen(val) : 0;
        }

        template <typename T> static size_t getBufLenZ_(const wchar_t* val)
        {
            return (val) ? wcslen(val) : 0;
        }
    };

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

    typedef enum _FILE_INFORMATION_CLASS
    {
        FileDirectoryInformation       = 1,
        FileFullDirectoryInformation, // 2
        FileBothDirectoryInformation, // 3
        FileBasicInformation,         // 4
        FileStandardInformation,      // 5
        FileInternalInformation,      // 6
        FileEaInformation,            // 7
        FileAccessInformation,        // 8
        FileNameInformation,          // 9
        FileRenameInformation,        // 10
        FileLinkInformation,          // 11
        FileNamesInformation,         // 12
        FileDispositionInformation,   // 13
        FilePositionInformation,      // 14
        FileFullEaInformation,        // 15
        FileModeInformation,          // 16
        FileAlignmentInformation,     // 17
        FileAllInformation,           // 18
        FileAllocationInformation,    // 19
        FileEndOfFileInformation,     // 20
        FileAlternateNameInformation, // 21
        FileStreamInformation,        // 22
        FilePipeInformation,          // 23
        FilePipeLocalInformation,     // 24
        FilePipeRemoteInformation,    // 25
        FileMailslotQueryInformation, // 26
        FileMailslotSetInformation,   // 27
        FileCompressionInformation,   // 28
        FileObjectIdInformation,      // 29
        FileCompletionInformation,    // 30
        FileMoveClusterInformation,   // 31
        FileQuotaInformation,         // 32
        FileReparsePointInformation,  // 33
        FileNetworkOpenInformation,   // 34
        FileAttributeTagInformation,  // 35
        FileTrackingInformation,      // 36
        FileMaximumInformation
        // begin_wdm
    } FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

#pragma pack(push, 4)
    typedef struct _FILE_FULL_EA_INFORMATION
    {
        ULONG NextEntryOffset;
        UCHAR Flags;
        UCHAR EaNameLength;
        USHORT EaValueLength;
        CHAR EaName[1];
    } FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

    typedef struct _FILE_STREAM_INFORMATION // Information Class 22
    {
        ULONG NextEntryOffset;
        ULONG StreamNameLength;
        LARGE_INTEGER EndOfStream;
        LARGE_INTEGER AllocationSize;
        WCHAR StreamName[1];
    } FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;
#pragma pack(pop)

    typedef NTSTATUS (WINAPI *TFNZwQueryInformationFile)
        (
        HANDLE hFile,
        PIO_STATUS_BLOCK IoStatusBlock,
        PVOID FileInformation,
        ULONG Length,
        FILE_INFORMATION_CLASS FileInformationClass
        );

    static const NTSTATUS STATUS_BUFFER_OVERFLOW = 0x80000005;
    static const DWORD dwAllocIncrement = 0x400;

public:
    // Just make it more readable
    typedef CVerySimpleBuf<WCHAR> CWideString, *PWideString;

    CAlternateDataStreams(WCHAR const* Path)
        : m_Path(normalizePath_(Path))
        , m_Attr(::GetFileAttributesW(m_Path.getBuf()))
        , m_OpenFlags(((m_Attr & FILE_ATTRIBUTE_DIRECTORY) ? FILE_FLAG_BACKUP_SEMANTICS : 0))
        , m_ZwQueryInformationFile(reinterpret_cast<TFNZwQueryInformationFile>(::GetProcAddress(::GetModuleHandle(_T("ntdll.dll")), "ZwQueryInformationFile")))
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
        if(m_ZwQueryInformationFile)
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
                    status = m_ZwQueryInformationFile(
                        hFile
                        , &iostat
                        , reinterpret_cast<PVOID>(m_Buffer.getBuf())
                        , m_Buffer.getByteCount()
                        , FileStreamInformation
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
                for(DWORD i = 0; psi && (i < m_StreamCount);)
                {
                    CWideString sName;
                    if(sName.reAlloc((psi->StreamNameLength / sizeof(WCHAR)) + 1))
                    {
                        memcpy(sName.getBuf(), psi->StreamName, psi->StreamNameLength);
                        WCHAR* datatag = wcsstr(sName.getBuf(), DATA_TAG_NAME);
                        if(0 != datatag)
                        {
                            *datatag = 0; // zero-terminate at the tag name
                            // We don't store the default unnamed stream
                            if(0 != wcscmp(sName.getBuf(), L":"))
                            {
                                // if this throws we'll let it fall through ;)
                                m_StreamNames[i] = new CWideString(sName.getBuf());
                                ++i;
                            }
                        }
                    }
                    curr += psi->NextEntryOffset;
                    psi = reinterpret_cast<PFILE_STREAM_INFORMATION>((psi->NextEntryOffset) ? curr : 0);
                }
            }
            // Count the valid elements
            m_StreamCount = 0;
            for(DWORD i = 0; m_StreamNames[i]; i++, m_StreamCount++);
        }
        catch(...)
        {
            emptyStreamNames_();
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
    TFNZwQueryInformationFile       m_ZwQueryInformationFile;
    DWORD mutable                   m_StreamCount;
    CVerySimpleBuf<BYTE> mutable    m_Buffer;
    DWORD mutable                   m_BufSize;
    PWideString*                    m_StreamNames;
};

#endif // __ALTERNATEDATASTREAMS_H_VER__
