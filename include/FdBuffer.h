#ifndef BTRFCOMM_FDBUFFER_H
#define BTRFCOMM_FDBUFFER_H


#include <algorithm>
#include <iostream>
#include <iterator>
#include <streambuf>

namespace nonstd {

    class FdBuffer : public std::streambuf {
    private:
        static const size_t bufferSize = 1024;
        char mOutBuffer[bufferSize];
        char mInBuffer[bufferSize + 16 - sizeof(int)];
        int mFd;
    public:
        typedef std::streambuf::traits_type traits_type;

        explicit FdBuffer(int fd);
        ~FdBuffer();

        void open(int fd);
        void close();

    protected:
        int overflow(int c);
        int underflow();
        int sync();
    };

}

#endif //BTRFCOMM_FDBUFFER_H
