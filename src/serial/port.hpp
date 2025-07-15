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
   FdHandle const fdHdl;
   Baud const baud;
   termios originalTTY; // Capture and restore outer terminal settings.
   std::thread bgReaderThread{};
   std::atomic_bool doBgRead{false};

public:
   Port(std::string const& devicePath, int baudRate);
   ~Port();

   void startBackgroundReader(std::function<void(char)> onByteRx);
   void stopBackgroundReader();

};

}
