#include "eloadtexthelper.h"

#include <algorithm>
#include <vector>

#include "etextencoding.h"

bool eLoadTextHelper::load(const std::string& path, eMap& map) {
    std::vector<std::string> lines;
    if(!eTextEncodings::sReadLines(path, lines)) return false;
    for(const auto& str : lines) {
        if(str.empty()) continue;
        if(str.front() == '\t') continue;
        if(str.front() == ';') {
            if(str.size() < 7) continue;
            if(str.substr(1, 6) != "PHRASE") continue;
        }
        const auto keyEnd1 = str.find(' ');
        const auto keyEnd2 = str.find('\t');
        const auto keyEnd = std::min(keyEnd1, keyEnd2);
        if(keyEnd == std::string::npos) continue;
        const auto key = str.substr(0, keyEnd);

        const auto valueStart = str.find('"');
        if(valueStart == std::string::npos) continue;
        auto valueEnd = str.find('"', valueStart + 1);
        // an unterminated value runs to the end of the line
        if(valueEnd == std::string::npos) valueEnd = str.size();
        const auto value = str.substr(valueStart + 1,
                                      valueEnd - valueStart - 1);

        map[key] = value;
    }
    return true;
}
