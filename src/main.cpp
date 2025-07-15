#include <iostream>
#include <string>
#include <cstdlib>

#include <cxxopts.hpp>
#include "port.hpp"

static void ProgramLoop(std::string, int);

int main(int argc, char* argv[])
{
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

      auto portArg = result["port"].as<std::string>();
      auto baudArg = result["baud"].as<int>();

      ProgramLoop(portArg, baudArg);

   } catch (cxxopts::exceptions::exception const& e) {
      std::cerr << "Error parsing options: " << e.what() << "\n";
      return 1;
   } catch (std::exception const& e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 2;
   } catch (...) {
      std::cerr << "Internal error: " << errno << "\n";
      return -1;
   }

   return 0;
}

static void ProgramLoop(std::string portArg, int baudArg)
{
   serial::Port port(portArg, baudArg);

   port.startBackgroundReader([](char c) {
      std::cout << c;
   });

   std::this_thread::sleep_for(std::chrono::seconds(10));
   port.stopBackgroundReader();

}
