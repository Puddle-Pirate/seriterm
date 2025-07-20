#pragma once

#include "linenoise.h"
#include <string>
#include <optional>
#include <vector>

namespace terminal
{

class LineEditor
{
private:
   int stdinFd;
   int stdoutFd;
   std::vector<char> buf;
   std::string prompt;
   linenoiseState lnState{};

   void start()
   {
      if (linenoiseEditStart(&lnState, stdinFd, stdoutFd,
                           buf.data(), buf.size(), prompt.c_str()) != 0)
      {
         throw std::runtime_error("linenoiseEditStart failed");
      }
   }

   void stop()
   {
      linenoiseEditStop(&lnState);
   }

public:
   LineEditor(int stdinFd, int stdoutFd, size_t bufSize = 1024, std::string const& prompt = "> ")
      : stdinFd(stdinFd), stdoutFd(stdoutFd), buf(bufSize, '\0'), prompt(prompt)
   {
      start();
   }

   ~LineEditor()
   {
      stop();
   }

   std::optional<std::string> feed()
   {
      auto* lnResult = linenoiseEditFeed(&lnState);

      if (lnResult == linenoiseEditMore || lnResult == nullptr) {
         return std::nullopt;
      }

      std::string line(lnResult);
      linenoiseFree(lnResult);
      return line;
   }

   void restart()
   {
      stop();
      start();
   }
};

}
