#ifndef ETEXTENCODING_H
#define ETEXTENCODING_H

#include <string>
#include <vector>

// Encoding of the plain text files eZeus reads. The original game ships
// its text in the ANSI code page of its release language, which is not
// valid UTF-8, while SDL_ttf only renders UTF-8. Everything read from
// disk therefore goes through this layer first.
enum class eTextEncoding {
    autoDetect, // keep valid UTF-8, treat the rest as the legacy code page
    utf8,
    cp1252,     // Western European - English, German, French, Spanish, Italian
    cp1250      // Central European - Polish, Czech, Hungarian
};

namespace eTextEncodings {
    // Encoding setting, read from settings.txt. Defaults to autoDetect.
    void sSetEncoding(const eTextEncoding e);
    eTextEncoding sEncoding();

    // Code page assumed for files that are not valid UTF-8. Set from the
    // active language, defaults to cp1252.
    void sSetLegacyEncoding(const eTextEncoding e);
    eTextEncoding sLegacyEncoding();

    std::string sName(const eTextEncoding e);
    eTextEncoding sFromName(const std::string& name);

    bool sIsValidUtf8(const std::string& str);
    // Single byte code page to UTF-8. utf8/autoDetect return str unchanged.
    std::string sToUtf8(const std::string& str, const eTextEncoding e);

    // Reads path, drops a leading byte order mark, converts the content to
    // UTF-8 and splits it into lines. Trailing carriage returns are removed,
    // so CRLF files behave the same on every platform.
    bool sReadLines(const std::string& path, std::vector<std::string>& lines);
};

#endif // ETEXTENCODING_H
