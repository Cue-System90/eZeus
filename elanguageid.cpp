#include "elanguageid.h"

std::string eLanguageIds::sCode(const eLanguageId id) {
    switch(id) {
    case eLanguageId::german: return "de";
    case eLanguageId::polish: return "pl";
    case eLanguageId::english: break;
    }
    return "en";
}

eLanguageId eLanguageIds::sFromCode(const std::string& code) {
    if(code == "de") return eLanguageId::german;
    if(code == "pl") return eLanguageId::polish;
    return eLanguageId::english;
}

std::string eLanguageIds::sNativeName(const eLanguageId id) {
    switch(id) {
    case eLanguageId::german: return "Deutsch";
    case eLanguageId::polish: return "Polski";
    case eLanguageId::english: break;
    }
    return "English";
}

eTextEncoding eLanguageIds::sLegacyEncoding(const eLanguageId id) {
    switch(id) {
    case eLanguageId::polish: return eTextEncoding::cp1250;
    case eLanguageId::german:
    case eLanguageId::english: break;
    }
    return eTextEncoding::cp1252;
}

std::vector<eLanguageId> eLanguageIds::sAll() {
    return {eLanguageId::english,
            eLanguageId::german,
            eLanguageId::polish};
}

eLanguageId eLanguageIds::sNext(const eLanguageId id) {
    const auto all = sAll();
    const int iMax = all.size();
    for(int i = 0; i < iMax; i++) {
        if(all[i] == id) return all[(i + 1) % iMax];
    }
    return all.front();
}
