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
{
   try {
      fdHdl = FdHandle(devicePath, O_RDWR | O_NOCTTY | O_SYNC);
   } catch (int err) {
      throw SerialException("Failed to open port '" + devicePath + "': " + std::string(strerror(err)));
   }

   if (tcgetattr(fdHdl.get(), &originalTTY) != 0) {
      throw TerminalException("Failed to get terminal attributes: " + std::string(strerror(errno)));
   }

   auto tty = originalTTY;
   cfmakeraw(&tty);

   speed_t baud;
   switch (baudRate) {
      case 9600: baud = B9600; break;
      case 19200: baud = B19200; break;
      case 38400: baud = B38400; break;
      case 57600: baud = B57600; break;
      case 115200: baud = B115200; break;
      default:
         throw SerialException("Unsupported baud rate: " + std::to_string(baudRate));
   }

   cfsetispeed(&tty, baud);
   cfsetospeed(&tty, baud);
   tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines

   if (tcsetattr(fdHdl.get(), TCSANOW, &tty) != 0) {
      throw TerminalException("Failed to set terminal attributes: " + std::string(strerror(errno)));
   }

   tcflush(fdHdl.get(), TCIOFLUSH);
}

Port::~Port()
{
   if (fdHdl.isValid()) {
      tcsetattr(fdHdl.get(), TCSANOW, &originalTTY);
      tcflush(fdHdl.get(), TCIOFLUSH);  // TODO: Is this necessary?
   }
}

}
