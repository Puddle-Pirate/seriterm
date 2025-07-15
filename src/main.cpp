#include <iostream>
#include <string>
#include <cstdlib>

#include <cxxopts.hpp>
#include "port.hpp"


int main(int argc, char* argv[])
{
   std::string portArg;
   int baudArg;

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

      portArg = result["port"].as<std::string>();
      baudArg = result["baud"].as<int>();


      std::cout << "Opening serial port: " << portArg << " at " << baudArg << " baud\n";

      // Later: Initialize terminal, line editor, and event loop
      // seriterm::run(port, baud);

   } catch (const cxxopts::exceptions::exception& e) {
      std::cerr << "Error parsing options: " << e.what() << "\n";
      return 1;
   }

   try {
      serial::Port port(portArg, baudArg);

      port.startBackgroundReader([](char c) {
         std::cout << "Received: " << c << "\n";
      });

      // do other work...
      std::this_thread::sleep_for(std::chrono::seconds(10));
      port.stopBackgroundReader();
   } catch (std::exception& e) {
      std::cerr << "Internal seriterm error:  " << e.what() << "\n";
      return 1;
   }



   return 0;
}
