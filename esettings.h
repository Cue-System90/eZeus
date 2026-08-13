#ifndef ESETTINGS_H
#define ESETTINGS_H

#include "widgets/eresolution.h"
#include "engine/etile.h"

#include "elanguageid.h"
#include "etextencoding.h"

struct eSettings {
    bool fTinyTextures = true;
    bool fSmallTextures = true;
    bool fMediumTextures = true;
    bool fLargeTextures = true;
    bool fFullscreen = false;
    eResolution fRes = eResolution(1280, 720);
    eLanguageId fLanguage = eLanguageId::english;
    // encoding of the original game's text files, autoDetect works for
    // every release seen so far
    eTextEncoding fEncoding = eTextEncoding::autoDetect;

    // applies the language and encoding to the text loading layer
    void apply() const;

    std::vector<eTileSize> availableSizes() const;

    void write() const;
    void read();
};

#endif // ESETTINGS_H
