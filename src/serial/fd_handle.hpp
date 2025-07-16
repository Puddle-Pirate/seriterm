#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <system_error>

namespace serial
{

// RAII-styled wrapper to ensure fd's are open/closed properly.
struct FdHandle
{
   int const fd;

   explicit FdHandle(const std::string& path, int flags)
      : fd(open(path.c_str(), flags))
   {
      if (fd < 0) {
         throw std::system_error(errno, std::generic_category(), "Failed to open " + path);
      }
   }

   ~FdHandle() { close(fd); }

   operator int() const { return fd; }

   // Non-copyable/non-movable
   FdHandle(const FdHandle&) = delete;
   FdHandle& operator=(const FdHandle&) = delete;
   FdHandle(FdHandle&& other) = delete;
   FdHandle& operator=(FdHandle&& other) = delete;
};

}
