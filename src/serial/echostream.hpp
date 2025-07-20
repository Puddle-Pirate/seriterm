#pragma once

#include <string>
#include <string_view>
#include <iostream>

namespace serial
{

class EchoStream
{
private:
   std::string buffer;

public:
   void feed(std::string_view data)
   {
      buffer.append(data);

      std::size_t pos;
      while ((pos = buffer.find_first_of("\r\n")) != std::string::npos) {
         // Extract complete line (up to the newline)
         std::string line = buffer.substr(0, pos);
         buffer.erase(0, buffer.find_first_not_of("\r\n", pos + 1));

         // Echo the line above the input
         std::cout << "\r\n" << line << std::endl;
      }
   }

   void flush()
   {
      if (!buffer.empty()) {
         std::cout << "\r\n" << buffer << std::flush;
         buffer.clear();
      }
   }

   void clear() { buffer.clear(); }
};

}
