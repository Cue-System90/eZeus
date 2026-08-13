#include "elanguage.h"

#include "eloadtexthelper.h"

#include "exmlparser.h"
#include "egamedir.h"
#include "etextencoding.h"

#include <cstdio>

eLanguage eLanguage::instance;

const std::string& eLanguage::text(const std::string& key) {
    static const std::string sEmpty;
    const auto it = instance.fText.find(key);
    if(it == instance.fText.end()) {
        printf("Could not find text '%s'\n", key.c_str());
        return sEmpty;
    }
    return it->second;
}

const std::string& eLanguage::zeusText(const int g, const int s) {
    return instance.fZeusText[g][s];
}

const eMM& eLanguage::zeusMM(const int id) {
    return instance.fZeusMM[id];
}

bool eLanguage::load() {
    return instance.loadImpl();
}

bool eLanguage::loaded() {
    return instance.mLoaded;
}

void eLanguage::reload() {
    instance.mLoaded = false;
    instance.fText.clear();
    instance.fZeusText.clear();
    instance.fZeusMM.clear();
    instance.loadImpl();
}

void eLanguage::setLanguage(const eLanguageId id) {
    instance.mLanguage = id;
    // text taken straight from the original game files is stored in the
    // code page of that language's release
    eTextEncodings::sSetLegacyEncoding(eLanguageIds::sLegacyEncoding(id));
}

eLanguageId eLanguage::language() {
    return instance.mLanguage;
}

bool eLanguage::loadImpl() {
    if(mLoaded) return false;
    mLoaded = true;

    eXmlParser::sParse(fZeusText, eGameDir::exeDir() + "../Zeus_Text.xml");
    eXmlParser::sParse(fZeusMM, eGameDir::exeDir() + "../Zeus_MM.xml");

    // English is loaded first so that a translation may be incomplete
    // without leaving parts of the interface blank
    const std::string dir = eGameDir::exeDir() + "../Text/";
    const bool r = eLoadTextHelper::load(dir + "language.txt", fText);
    if(mLanguage != eLanguageId::english) {
        const auto code = eLanguageIds::sCode(mLanguage);
        eLoadTextHelper::load(dir + "language_" + code + ".txt", fText);
    }
    return r;
}
