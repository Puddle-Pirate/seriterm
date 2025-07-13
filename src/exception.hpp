#pragma once

#include <stdexcept>
#include <string>

class SerialException : public std::runtime_error
{
public:
   explicit SerialException(const std::string& message)
      : std::runtime_error("Serial error: " + message) {}
};

class TerminalException : public std::runtime_error
{
public:
   explicit TerminalException(const std::string& message)
      : std::runtime_error("Terminal error: " + message) {}
};