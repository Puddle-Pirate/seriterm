#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <errno.h>

#include "port.hpp"
#include "exception.hpp"

namespace serial
{

Port::Port(std::string const& devicePath, int baudRate)
   : fdHdl(devicePath, O_RDWR | O_NOCTTY | O_SYNC), baud(baudRate)
{
   if (tcgetattr(fdHdl.fd, &originalTTY) != 0) {
      throw TerminalException("Failed to get terminal attributes: " + std::string(strerror(errno)));
   }

   auto tty = originalTTY;
   cfmakeraw(&tty);

   cfsetispeed(&tty, baud);
   cfsetospeed(&tty, baud);
   tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines

   if (tcsetattr(fdHdl.fd, TCSANOW, &tty) != 0) {
      throw TerminalException("Failed to set terminal attributes: " + std::string(strerror(errno)));
   }

   tcflush(fdHdl.fd, TCIOFLUSH);
}

Port::~Port()
{
   tcsetattr(fdHdl.fd, TCSANOW, &originalTTY);
   tcflush(fdHdl.fd, TCIOFLUSH);  // TODO: Is this necessary?
}

}
