///////////////////////////////////////////////////////////////////////////////
///
/// Written 2012, Oliver Schneider (assarbad.net) - PUBLIC DOMAIN/CC0
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __VERYSIMPLEBUF_HPP_VER__
#define __VERYSIMPLEBUF_HPP_VER__ 2017091422
#if !defined(__VERYSIMPLEBUF_MULTI_INC__) && ((defined(_MSC_VER) && (_MSC_VER >= 1020)) || defined(__MCPP))
#pragma once
#endif // Check for "#pragma once" support

template <typename T> class CVerySimpleBuf
{
public:
    typedef T ElemType;

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
            if(rval.getBuf() && reAlloc(rval.getCount(), true))
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
            else if(reAlloc(len, true))
            {
                memcpy(getBuf(), buf, getByteCount());
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
        return (0 == m_buf);
    }

    inline size_t elemSize() const
    {
        return sizeof(ElemType);
    }

    void clear()
    {
        if(m_buf)
        {
            memset(m_buf, 0, m_count * sizeof(T));
        }
    }

    bool reAlloc(size_t count, bool exact = false)
    {
        T* tempBuf = 0;
        size_t count_ = 0;
        if(count)
        {
            count_ = (!exact) ? getCeil_(count+1) : count;
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
#endif // __VERYSIMPLEBUF_HPP_VER__
