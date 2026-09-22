#include "elabelbase.h"

#include "emainwindow.h"

#include <algorithm>

eLabelBase::eLabelBase(eMainWindow* const window) :
    mWindow(window) {

}

bool eLabelBase::setFont(const eFont& font) {
    const auto ttf = eFonts::requestFont(font);
    mPtSize = font.fPtSize;
    return setFont(ttf);
}

bool eLabelBase::setTinyFontSize() {
    const int s = res().tinyFontSize();
    return setFontSize(s);
}

bool eLabelBase::setVerySmallFontSize() {
    const int s = res().verySmallFontSize();
    return setFontSize(s);
}

bool eLabelBase::setSmallFontSize() {
    const int s = res().smallFontSize();
    return setFontSize(s);
}

bool eLabelBase::setHugeFontSize() {
    const int s = res().hugeFontSize();
    return setFontSize(s);
}

bool eLabelBase::setFontSize(const int s) {
    const auto font = eFonts::defaultFont(s);
    mPtSize = s;
    return setFont(font);
}

bool eLabelBase::fitFontToWidth(const int w) {
    if(w <= 0 || mText.empty()) return true;
    // the original game is English only, its translations are wider and
    // do not always fit the width a menu reserves for them
    const int minSize = std::max(1, res().tinyFontSize());
    int size = ptSize();
    while(true) {
        int tw, th;
        textureSize(tw, th);
        if(tw <= w) return true;
        if(size <= minSize) return false;
        size = std::max(minSize, size - 1);
        setFontSize(size);
    }
}

bool eLabelBase::setFont(TTF_Font* const font) {
    mFont = font;
    return updateTextTexture();
}

bool eLabelBase::updateTexture() {
    if(!mText.empty()) {
        updateTextTexture();
        return true;
    } else {
        return false;
    }
}

bool eLabelBase::setText(const std::string& text) {
    if(text == mText) return true;
    mText = text;
    return updateTextTexture();
}

bool eLabelBase::setTexture(const std::shared_ptr<eTexture>& tex) {
    mTexture = tex;
    return true;
}

bool eLabelBase::setFontColor(const eFontColor color) {
    mFontColor = color;
    return updateTextTexture();
}

void eLabelBase::setLightFontColor() {
    setFontColor(eFontColor::light);
}

void eLabelBase::setDarkFontColor() {
    setFontColor(eFontColor::dark);
}

void eLabelBase::setYellowFontColor() {
    setFontColor(eFontColor::yellow);
}

int eLabelBase::fontSize() const {
    if(!mFont) return 0;
    return TTF_FontHeight(mFont);
}

int eLabelBase::ptSize() const {
    return mPtSize > 0 ? mPtSize : res().largeFontSize();
}

void eLabelBase::setWrapWidth(const int w) {
    mWidth = w;
    updateTextTexture();
}

bool eLabelBase::updateTextTexture() {
    if(mText.empty()) {
        mTexture.reset();
        return true;
    }
    if(!mFont) return false;
    mTexture = std::make_shared<eTexture>();
    const auto r = mWindow->renderer();
    const bool v = mTexture->loadText(r, mText, mFontColor, *mFont,
                                      mWidth, mWrapAlign);
    if(!v) {
        mTexture.reset();
        mUpdateTextTextureFailed = true;
    }
    return true;
}

const eResolution& eLabelBase::res() const {
    return mWindow->resolution();
}

int eLabelBase::lineWidth() const {
    return std::max(1, fontSize()/15);
}

void eLabelBase::textureSize(int& w, int& h) const {
    if(!mTexture) {
        w = 0;
        h = 0;
    } else {
        w = mTexture->width();
        h = mTexture->height();
    }
}

const std::shared_ptr<eTexture>& eLabelBase::texture() {
    if(mUpdateTextTextureFailed) {
        mUpdateTextTextureFailed = false;
        updateTextTexture();
    }
    return mTexture;
}
