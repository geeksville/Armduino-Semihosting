#include <semihosting_syscalls.h>
#include <SemihostingStream.h>

/* 
 * Arduino class to write to and read from semihosting console (stdin/stdout, buffered)
 * 2020 Koen De Vleeschauwer
 * No copyright asserted
 */

SemihostingStream::SemihostingStream() {
  inBuffered = 0;
  inReadPos = 0;
};

SemihostingStream::~SemihostingStream() {
  flush();
  semihosting::sys_close(stdin_handle);
  semihosting::sys_close(stdout_handle);
}

void SemihostingStream::open() {
  const char *stdio_fname=":tt";
  stdin_handle = semihosting::sys_open(stdio_fname, semihosting::OPEN_MODE_R, strlen(stdio_fname));
  stdout_handle = semihosting::sys_open(stdio_fname, semihosting::OPEN_MODE_W, strlen(stdio_fname));
};


/* print on gdb console */

size_t SemihostingStream::write(uint8_t ch) {
  outBuffer[outBuffered++] = ch;
  if ((outBuffered == outBufferSize) || (ch == '\n')) flush();
  return 1;
}

void SemihostingStream::flush() {
  if (outBuffered > 0) semihosting::sys_write(stdout_handle, (void *)outBuffer, outBuffered);
  outBuffered = 0;
  return;
}

/* read from gdb console */

int SemihostingStream::read() {
  fillBuffer();
  if (inBuffered == 0) return -1;
  inBuffered--;
  return inBuffer[inReadPos++];
}

int SemihostingStream::peek() {
  fillBuffer();
  if (inBuffered == 0) return -1;
  else return inBuffer[inReadPos];
}

int SemihostingStream::available() {
  fillBuffer();
  return inBuffered;
}

void SemihostingStream::fillBuffer() {
  if (inBuffered > 0) return;
  flush();
  int rc = semihosting::sys_read(stdin_handle, inBuffer, inBufferSize);
  if (rc < 0) inBuffered = 0;
  else inBuffered = inBufferSize - rc;
  inReadPos = 0;
  return;
}

/* not truncated */
