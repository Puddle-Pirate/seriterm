#include <iostream>
#include <string>
#include <cstdlib>

#include <cxxopts.hpp>


int main(int argc, char* argv[]) {
   try {
      cxxopts::Options options("seriterm", "A modern serial terminal with line editing and history");

      options.add_options()
         ("p,port", "Serial port (e.g. /dev/ttyUSB0 or COM3)", cxxopts::value<std::string>())
         ("b,baud", "Baud rate (e.g. 9600, 115200)", cxxopts::value<int>()->default_value("115200"))
         ("h,help", "Print usage");

      auto result = options.parse(argc, argv);

      if (result.count("help") || !result.count("port")) {
         std::cout << options.help() << "\n";
         return 0;
      }

      std::string port = result["port"].as<std::string>();
      int baud = result["baud"].as<int>();

      std::cout << "Opening serial port: " << port << " at " << baud << " baud\n";

      // Later: Initialize terminal, line editor, and event loop
      // seriterm::run(port, baud);

   } catch (const cxxopts::exceptions::exception& e) {
      std::cerr << "Error parsing options: " << e.what() << "\n";
      return 1;
   }




   return 0;
}
