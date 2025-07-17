#pragma once

#include <string>
#include <optional>

namespace terminal
{

class LineBuffer
{
private:
   std::string historyFilePath;
public:
   explicit LineBuffer(const std::string& historyFile = ".seriterm_history");
   ~LineBuffer();

   std::optional<std::string> getLine(const std::string& prompt = "seriterm> ");
};

}
