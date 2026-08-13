#ifndef ELANGUAGE_H
#define ELANGUAGE_H

#include "exmlparser.h"

#include "elanguageid.h"

class eLanguage {
public:
    eLanguage() {}

    static eLanguage instance;

    static bool load();
    static bool loaded();
    // drops everything and loads again, used after a language change
    static void reload();

    // the language of eZeus' own interface texts
    static void setLanguage(const eLanguageId id);
    static eLanguageId language();

    static const std::string& text(const std::string& key);

    static const std::string& zeusText(const int g, const int s);
    static const eMM& zeusMM(const int id);

    std::map<std::string, std::string> fText;

    std::map<int, std::map<int, std::string>> fZeusText;
    std::map<int, eMM> fZeusMM;
private:
    bool loadImpl();
    bool mLoaded = false;
    eLanguageId mLanguage = eLanguageId::english;
};

#endif // ELANGUAGE_H
