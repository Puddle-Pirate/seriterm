#pragma once

#include <functional>
#include <atomic>
#include <string>
#include <termios.h>
#include <thread>
#include "fd_handle.hpp"
#include "exception.hpp"

namespace serial
{

// Perhaps a bit overkill, but also a neat way of having a 'Baud' object
// that _must_ be valid on construction.
struct Baud
{
   speed_t const baud;
   Baud(int baud) : baud(baud)
   {
      switch (baud) {
         case B9600:
         case B19200:
         case B38400:
         case B57600:
         case B115200:
            break;
         default:
            throw SerialException("Unsupported baud rate: " + std::to_string(baud));
      }
   }
   constexpr operator speed_t() const { return baud; }
};

class Port
{
private:
   static constexpr size_t READ_BUF_SIZE = 256;
   Baud const baud;
   termios originalTTY; // Capture and restore outer terminal settings.

public:
   FdHandle const fd;
   Port(std::string const& devicePath, int baudRate)
      : baud(baudRate),
        fd(devicePath, O_RDWR | O_NOCTTY | O_SYNC)
   {
      if (tcgetattr(fd, &originalTTY) != 0) {
         throw TerminalException("Failed to get terminal attributes: " + cErrStr());
      }

      auto tty = originalTTY;
      cfmakeraw(&tty);

      cfsetispeed(&tty, baud);
      cfsetospeed(&tty, baud);
      tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines

      if (tcsetattr(fd, TCSANOW, &tty) != 0) {
         throw TerminalException("Failed to set terminal attributes: " + cErrStr());
      }

      tcflush(fd, TCIOFLUSH);
   }

   ~Port()
   {
      tcsetattr(fd, TCSANOW, &originalTTY);
      tcflush(fd, TCIOFLUSH);  // TODO: Is this necessary?
   }

   std::string read()
   {
      std::array<char, READ_BUF_SIZE> buffer;

      auto bytesRead = ::read(fd, buffer.data(), buffer.size());
      if (bytesRead < 0)
      {
         if (errno == EAGAIN || errno == EWOULDBLOCK) return {}; // non-fatal, no data ready
         throw SerialException("Serial read error: " + cErrStr());
      }

      return std::string(buffer.data(), static_cast<size_t>(bytesRead));
   }

   void write(std::string_view data)
   {
      size_t totalWritten = 0;
      while (totalWritten < data.size()) {
         auto written = ::write(fd, data.data() + totalWritten, data.size() - totalWritten);
         if (written < 0) {
            if (errno == EINTR) continue; // retry on interrupt
            throw SerialException("Serial write error: " + cErrStr());
         }
         totalWritten += static_cast<size_t>(written);
      }
   }

};

}
