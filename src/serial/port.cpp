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

void Port::startBackgroundReader(std::function<void(char)> onByteRx)
{
   if (doBgRead) throw std::logic_error("Tried to start bg reader twice.");
   doBgRead = true;

   bgReaderThread = std::thread([this, onByteRx]()
   {
      while (doBgRead) {
         fd_set readfds;
         FD_ZERO(&readfds);
         FD_SET(fdHdl.fd, &readfds);

         timeval tv = { .tv_sec = 0, .tv_usec = 500000 }; // 0.5 sec poll
         int ret = select(fdHdl.fd + 1, &readfds, nullptr, nullptr, &tv);
         if (ret > 0 && FD_ISSET(fdHdl.fd, &readfds)) {
            char byte;
            if (read(fdHdl.fd, &byte, 1) == 1) onByteRx(byte);
         }
      }
   });

}

void Port::stopBackgroundReader()
{
   doBgRead = true;
   if (bgReaderThread.joinable()) bgReaderThread.join();
}

}
