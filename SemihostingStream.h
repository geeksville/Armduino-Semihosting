
#ifndef SEMIHOSTINGSTREAM_H
#define SEMIHOSTINGSTREAM_H

/* 
 * Arduino class to write to and read from semihosting console (stdin/stdout, buffered)
 * 2020 Koen De Vleeschauwer
 * No copyright asserted
 */

#include <Stream.h>

class SemihostingStream : public Stream
{
  private:
    int stdin_handle;
    int stdout_handle;
    static const size_t inBufferSize = 256;
    static const size_t outBufferSize = 256;
    uint8_t inBuffered;
    uint8_t inReadPos;
    uint8_t inBuffer[inBufferSize];
    uint8_t outBuffered;
    uint8_t outBuffer[outBufferSize];
    void fillBuffer();
  public:
    SemihostingStream();
    ~SemihostingStream();

    // You must explicitly call this method to begin talking to the semihosting server
    void open();

    virtual size_t write(uint8_t ch);
    
    virtual void flush();
    virtual int read();
    virtual int available();
    virtual int peek();
};
#endif

/* not truncated */
