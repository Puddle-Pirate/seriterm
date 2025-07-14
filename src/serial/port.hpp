#pragma once

#include <string>
#include <termios.h>
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

public:
   Port(std::string const& devicePath, int baudRate);
   ~Port();

   void startReader();
   void stopReader();

};

}
