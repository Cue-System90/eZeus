#ifndef ELANGUAGEID_H
#define ELANGUAGEID_H

#include <string>
#include <vector>

#include "etextencoding.h"

// Language of eZeus' own interface texts. The texts taken from the
// original game files always follow the language of the installation
// they were extracted from.
enum class eLanguageId {
    english,
    german,
    polish
};

namespace eLanguageIds {
    // two letter code, used for settings.txt and the Text/language_XX.txt
    // file name suffix
    std::string sCode(const eLanguageId id);
    eLanguageId sFromCode(const std::string& code);

    // name of the language in that language, for the settings menu
    std::string sNativeName(const eLanguageId id);

    // code page the original game uses for this language's release
    eTextEncoding sLegacyEncoding(const eLanguageId id);

    std::vector<eLanguageId> sAll();
    eLanguageId sNext(const eLanguageId id);
};

#endif // ELANGUAGEID_H
