#ifndef ELABELBASE_H
#define ELABELBASE_H

#include "ewidget.h"
#include "efonts.h"

class eLabelBase {
public:
    eLabelBase(eMainWindow* const window);

    bool setTinyFontSize();
    bool setVerySmallFontSize();
    bool setSmallFontSize();
    bool setHugeFontSize();

    bool setFont(const eFont& font);
    bool setFontSize(const int s);
    // Lowers the font size until the text is no wider than w. Returns
    // false when it does not fit even at the smallest size.
    bool fitFontToWidth(const int w);
    std::string text() const { return mText; }
    bool setText(const std::string& text);
    bool setTexture(const std::shared_ptr<eTexture>& tex);
    bool setFontColor(const eFontColor color);

    eFontColor fontColor() const
    { return mFontColor; }
    void setLightFontColor();
    void setDarkFontColor();
    void setYellowFontColor();

    int fontSize() const; // the height of the font, not the size asked for
    int ptSize() const;   // the size asked for

    void setWrapWidth(const int w);

    eAlignment textAlignment() { return mTextAlign; }
    void setTextAlignment(const eAlignment a) { mTextAlign = a; }

    void setWrapAlignment(const eAlignment a) { mWrapAlign = a; }

    int lineWidth() const;

    void textureSize(int& w, int& h) const;
protected:
    const std::shared_ptr<eTexture>& texture();
    bool setFont(TTF_Font* const font);
    bool updateTexture();
private:
    const eResolution& res() const;
    bool updateTextTexture();

    int mWidth = 0;
    // 0 means the default the constructor picked, the large size
    int mPtSize = 0;
    eMainWindow* const mWindow;
    TTF_Font* mFont = nullptr;
    std::string mText;
    std::shared_ptr<eTexture> mTexture;
    bool mUpdateTextTextureFailed = false;
    eFontColor mFontColor = eFontColor::light;
    eAlignment mTextAlign = eAlignment::center;
    eAlignment mWrapAlign = eAlignment::left;
};

#endif // ELABELBASE_H
