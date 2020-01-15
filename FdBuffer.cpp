#include "FdBuffer.h"

#include <cstddef>
#include <unistd.h>

namespace nonstd {

    FdBuffer::FdBuffer(int fd): mFd(-1) {
        open(fd);
    }

    FdBuffer::~FdBuffer() {
        close();
    }

    void FdBuffer::open(int fd) {
        close();
        mFd = fd;
        setg(mInBuffer, mInBuffer, mInBuffer);
        setp(mOutBuffer, mOutBuffer + bufferSize - 1);
    }

    void FdBuffer::close() {
        if (mFd >= 0) {
            sync();
            ::close(mFd);
        }
    }

    int FdBuffer::sync() {
        if (pbase() != pptr()) {
            std::streamsize size(pptr() - pbase());
            std::streamsize done(::write(mFd, mOutBuffer, size));
            if (0 < done) {
                std::copy(pbase() + done, pptr(), pbase());
                setp(pbase(), epptr());
                pbump(size - done);
            }
        }
        return pptr() != epptr() ? 0 : -1;
    }

    int FdBuffer::overflow(int c) {
        if (!traits_type::eq_int_type(c, traits_type::eof())) {
            *pptr() = traits_type::to_char_type(c);
            pbump(1);
        }
        return sync() == -1 ? traits_type::eof() : traits_type::not_eof(c);
    }

    int FdBuffer::underflow() {
        if (gptr() == egptr()) {
            std::streamsize pback(std::min(gptr() - eback(), std::ptrdiff_t(16 - sizeof(int))));
            std::copy(egptr() - pback, egptr(), eback());
            int done(::read(mFd, eback() + pback, bufferSize));
            setg(eback(), eback() + pback, eback() + pback + std::max(0, done));
        }
        return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
    }

}