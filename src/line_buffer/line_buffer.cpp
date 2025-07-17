#include "line_buffer.hpp"
#include "linenoise.h"

namespace
{

class LineNoiseInput
{
private:
   char* input;
public:
   LineNoiseInput(std::string const& prompt)
      : input(linenoise(prompt.c_str())) {}

   ~LineNoiseInput()
   {
      if (input != nullptr) delete input;
   }

   char operator[](size_t i) const { return input[i]; }
   char* data() const { return input; } // For C interopt
   bool empty() const { return input == nullptr; }
   std::string toString() const { return {input}; }
};

}

namespace terminal
{

LineBuffer::LineBuffer(const std::string& historyFile)
   : historyFilePath(historyFile)
{
   linenoiseHistoryLoad(historyFilePath.c_str());
   linenoiseSetMultiLine(1); // TODO: maybe?
}

LineBuffer::~LineBuffer()
{
   linenoiseHistorySave(historyFilePath.c_str());
}

std::optional<std::string> LineBuffer::getLine(std::string const& prompt)
{
   LineNoiseInput input(prompt);

   if (input.empty()) return std::nullopt; // Likely Ctrl+D (EOF)

   if (input[0] != '\0') {
      linenoiseHistoryAdd(input.data());
      return input.toString();
   }

   // TODO: Error?
   return std::nullopt;
}

} // namespace terminal
