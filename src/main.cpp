#include <iostream>
#include <string>
#include <cstdlib>

#include <cxxopts.hpp>
#include "echostream.hpp"
#include "port.hpp"
#include "line_edit.hpp"

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
   } catch (GracefulException) {
      std::cerr << "Exiting." << "\n";
      return 0;
   } catch (std::exception const& e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 2;
   } catch (...) {
      std::cerr << "Internal error: " << std::string(strerror(errno)) << "\n";
      return -1;
   }

   return 0;
}

static void ProgramLoop(std::string portArg, int baudArg)
{
   serial::Port port(portArg, baudArg);
   serial::EchoStream echo;
   terminal::LineEditor editor(STDIN_FILENO, STDOUT_FILENO);

   while (true)
   {
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(port.fd, &readfds);
      FD_SET(STDIN_FILENO, &readfds);

      int maxfd = std::max<int>(port.fd, STDIN_FILENO) + 1;
      int selectResult;
      do {
         selectResult = select(maxfd, &readfds, nullptr, nullptr, nullptr);
      } while (selectResult == -1 && errno == EINTR);
      if (selectResult < 0) {
         throw std::runtime_error("select() failed: " + cErrStr());
      }

      if (FD_ISSET(port.fd, &readfds)) {
         editor.hide();
         echo.feed(port.read());
         editor.show();
      }

      if (FD_ISSET(STDIN_FILENO, &readfds)) {
         auto gotLine = editor.feed();
         if (gotLine) {
            port.write(*gotLine);
         }
      }
   }

}
