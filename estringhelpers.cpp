#include "estringhelpers.h"
#include "efspath.h"

#include <regex>

bool eStringHelpers::replace(std::string& str, const std::string& from,
                             const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void eStringHelpers::replaceAll(std::string& source,
                                const std::string& from,
                                const std::string& to) {
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}

std::string eStringHelpers::pathToName(const std::string& path) {
    // Splitting on '/' by hand would miss the '\' that std::filesystem
    // inserts on Windows, so the name would still carry a folder in front
    // of it. On Linux '\' is a legal character in a file name, so let
    // std::filesystem decide what a separator is.
    return eFsPath::sUtf8(eFsPath::sPath(path).filename());
}

void eStringHelpers::replaceSpecial(std::string& value) {
    value = std::regex_replace(value, std::regex("^@L"), "");
    value = std::regex_replace(value, std::regex("@L"), "\n");
    value = std::regex_replace(value, std::regex("^@P"), "   ");
    value = std::regex_replace(value, std::regex("@P"), "\n\n   ");
}
