#pragma once

#include <string>
#include <termios.h>
#include "fd_handle.hpp"

namespace serial
{

class Port
{
private:
   FdHandle fdHdl;
   termios originalTTY; // Capture and restore outer terminal settings.

public:
   Port(std::string const& devicePath, int baudRate);
   ~Port();
};

}
