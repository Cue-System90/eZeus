#include "exmlparser.h"

#include <vector>
#include <cctype>

#include "etextencoding.h"

bool match(const std::string& str, const std::string& line,
           const int oldIndex, int& newIndex) {
    const int ls = line.size();
    const int iMax = str.size();
    newIndex = oldIndex;
    for(int i = 0; i < iMax; i++, newIndex++) {
        const int lineIndex = oldIndex + i;
        if(lineIndex >= ls) return false;
        const auto strC = str[i];
        const auto lineC = line[lineIndex];
        if(strC != lineC) return false;
    }
    return true;
}

void skipSpaces(const std::string& line, int& index) {
    const int ls = line.size();
    if(index >= ls) return;
    while(line[index] == ' ') {
        index++;
        if(index >= ls) break;
    }
}

void skipTo(const std::string& line, const char c, int& index) {
    const int ls = line.size();
    if(index >= ls) return;
    while(line[index] != c) {
        index++;
        if(index >= ls) break;
    }
}

bool readId(const std::string& line,
            const int oldIndex,
            int& newIndex,
            int& id) {
    const int ls = line.size();
    newIndex = oldIndex;
    if(newIndex >= ls) return false;
    {
        const bool q = line[newIndex++] == '"';
        if(!q) return false;
    }
    if(newIndex >= ls) return false;
    std::string numberStr;
    while(newIndex < ls) {
        const auto c = line[newIndex];
        if(c == '"') break;
        if(!std::isdigit(c)) return false;
        numberStr = numberStr + c;
        newIndex++;
    }
    id = std::stoi(numberStr);
    {
        if(newIndex >= ls) return false;
        const bool q = line[newIndex++] == '"';
        if(!q) return false;
    }
    return true;
}

bool eXmlParser::sParse(eTextStrings& strings,
                        const std::string& filePath) {
    std::vector<std::string> lines;
    if(!eTextEncodings::sReadLines(filePath, lines)) return false;
    eTextGroup* group = nullptr;
    for(const auto& line : lines) {
        if(line.empty()) continue;
        const int ls = line.size();
        int index = 0;
        {
            skipSpaces(line, index);
            if(index >= ls) continue;
        }
        {
            int newIndex;
            const bool isGroup = match("<group id=", line, index, newIndex);
            if(isGroup) {
                index = newIndex;
                if(index >= ls) continue;
                int id;
                const bool r = readId(line, index, newIndex, id);
                if(!r) continue;
                index = newIndex;
                if(index >= ls) continue;
                group = &strings[id];
                continue;
            }
        }
        {
            int newIndex;
            const bool isString = match("<string id=", line, index, newIndex);
            if(isString) {
                index = newIndex;
                if(index >= ls) continue;
                int id;
                const bool r = readId(line, index, newIndex, id);
                if(!r) continue;
                index = newIndex + 1;
                if(index >= ls) continue;
                if(!group) continue;
                auto& str = (*group)[id];
                while(index < ls && line[index] != '<') {
                    str.push_back(line[index++]);
                }
                continue;
            }
        }
    }
    return true;
}

bool eXmlParser::sParse(eMMStrings& strings,
                        const std::string& filePath) {
    std::vector<std::string> lines;
    if(!eTextEncodings::sReadLines(filePath, lines)) return false;
    eMM* group = nullptr;
    for(const auto& line : lines) {
        if(line.empty()) continue;
        const int ls = line.size();
        int index = 0;
        {
            skipSpaces(line, index);
            if(index >= ls) continue;
        }
        {
            int newIndex;
            const bool isMessage = match("<message id=", line, index, newIndex);
            if(isMessage) {
                index = newIndex;
                if(index >= ls) continue;
                int id;
                const bool r = readId(line, index, newIndex, id);
                if(!r) continue;
                index = newIndex;
                if(index >= ls) continue;
                group = &strings[id];
                continue;
            }
        }
        {
            int newIndex;
            const bool isTitle = match("<title ", line, index, newIndex);
            if(isTitle) {
                index = newIndex;
                if(index >= ls) continue;
                skipTo(line, '>', newIndex);
                index = newIndex + 1;
                if(index >= ls) continue;
                if(!group) continue;
                auto& str = (*group).fTitle;
                while(index < ls && line[index] != '<') {
                    str.push_back(line[index++]);
                }
                continue;
            }
        }
        {
            int newIndex;
            const bool isContent = match("<content>", line, index, newIndex);
            if(isContent) {
                index = newIndex;
                if(index >= ls) continue;
                if(!group) continue;
                auto& str = (*group).fContent;
                while(index < ls && line[index] != '<') {
                    str.push_back(line[index++]);
                }
                continue;
            }
        }
        {
            int newIndex;
            const bool isSubtitle = match("<subtitle ", line, index, newIndex);
            if(isSubtitle) {
                index = newIndex;
                if(index >= ls) continue;
                skipTo(line, '>', newIndex);
                index = newIndex + 1;
                if(index >= ls) continue;
                if(!group) continue;
                auto& str = (*group).fSubtitle;
                while(index < ls && line[index] != '<') {
                    str.push_back(line[index++]);
                }
                continue;
            }
        }
    }
    return true;
}
