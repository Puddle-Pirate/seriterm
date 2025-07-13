#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace serial
{


// RAII-styled wrapper to ensure fd's are open/closed properly.
class FdHandle
{
private:
   int fd;

public:
   FdHandle() = default;

   explicit FdHandle(const std::string& path, int flags)
      : fd(open(path.c_str(), flags))
   {
      if (fd < 0) throw errno;
   }

   ~FdHandle()
   {
      if (fd >= 0) close(fd);
   }

   // Non-copyable
   FdHandle(const FdHandle&) = delete;
   FdHandle& operator=(const FdHandle&) = delete;

   // Movable
   FdHandle(FdHandle&& other) noexcept
      : fd(other.fd)
   {
      other.fd = -1;
   }

   FdHandle& operator=(FdHandle&& other) noexcept
   {
      if (this != &other) {
         if (fd >= 0) close(fd);
         fd = other.fd;
         other.fd = -1;
      }
      return *this;
   }

   int get() const { return fd; }
   bool isValid() const { return fd >= 0; }
};

}
