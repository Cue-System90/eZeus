#include "efspath.h"

std::filesystem::path eFsPath::sPath(const std::string& utf8) {
    // u8path is the C++17 way of saying "these bytes are UTF-8". It is
    // deprecated in C++20, where the same job is done by constructing the
    // path from a std::u8string.
    return std::filesystem::u8path(utf8);
}

std::string eFsPath::sUtf8(const std::filesystem::path& path) {
    const auto str = path.u8string();
    return std::string(str.begin(), str.end());
}
