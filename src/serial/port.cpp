#include <fcntl.h>
#include <functional>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <errno.h>

#include "port.hpp"
#include "exception.hpp"

namespace serial
{

Port::Port(std::string const& devicePath, int baudRate)
   : baud(baudRate),
     fd(devicePath, O_RDWR | O_NOCTTY | O_SYNC)
{
   if (tcgetattr(fd, &originalTTY) != 0) {
      throw TerminalException("Failed to get terminal attributes: " + std::string(strerror(errno)));
   }

   auto tty = originalTTY;
   cfmakeraw(&tty);

   cfsetispeed(&tty, baud);
   cfsetospeed(&tty, baud);
   tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines

   if (tcsetattr(fd, TCSANOW, &tty) != 0) {
      throw TerminalException("Failed to set terminal attributes: " + std::string(strerror(errno)));
   }

   tcflush(fd, TCIOFLUSH);
}

Port::~Port()
{
   tcsetattr(fd, TCSANOW, &originalTTY);
   tcflush(fd, TCIOFLUSH);  // TODO: Is this necessary?
}

}
