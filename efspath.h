#ifndef EFSPATH_H
#define EFSPATH_H

#include <filesystem>
#include <string>

// eZeus keeps every path as a UTF-8 std::string, and SDL_GetBasePath hands
// back UTF-8 as well. Windows however reads a narrow string given to
// std::filesystem or to the streams in the active ANSI code page, so a path
// holding anything outside ASCII, an umlaut in the user's name being the
// common case, silently fails to resolve. Every path that reaches the file
// system has to go through here.
namespace eFsPath {
    std::filesystem::path sPath(const std::string& utf8);
    // the other direction, for names read back from the file system
    std::string sUtf8(const std::filesystem::path& path);
};

#endif // EFSPATH_H
