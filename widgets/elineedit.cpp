#include "elineedit.h"

eLineEdit::eLineEdit(eMainWindow* const window) :
    eFramedLabel(window) {
    setType(eFrameType::inner);
    setTextAlignment(eAlignment::left | eAlignment::vcenter);
}

void eLineEdit::setChangeAction(const eAction a) {
    mChangeAction = a;
}

void eLineEdit::setRenderBg(const bool r) {
    mRenderBg = r;
}

bool eLineEdit::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool eLineEdit::mouseEnterEvent(const eMouseEvent& e) {
    (void)e;
    mHovered = true;
    return true;
}

bool eLineEdit::mouseLeaveEvent(const eMouseEvent& e) {
    (void)e;
    mHovered = false;
    return true;
}

bool eLineEdit::keyPressEvent(const eKeyPressEvent& e) {
    // characters arrive through textInputEvent, a key press only has to
    // deal with editing keys
    const auto k = e.key();
    if(k != SDL_Scancode::SDL_SCANCODE_BACKSPACE) return false;
    auto txt = text();
    if(txt.empty()) return true;
    // drop a whole UTF-8 character, not a single byte, or a deleted umlaut
    // would leave a stray continuation byte behind
    do {
        txt.pop_back();
    } while(!txt.empty() &&
            (static_cast<unsigned char>(txt.back()) & 0xC0) == 0x80);
    setText(txt);
    if(mChangeAction) mChangeAction();
    return true;
}

bool eLineEdit::acceptsInput(const std::string& character) const {
    (void)character;
    return true;
}

bool eLineEdit::rejectedInput(const std::string& character) {
    (void)character;
    return false;
}

bool eLineEdit::textInputEvent(const std::string& input) {
    bool changed = false;
    // SDL may deliver more than one character at once, from a dead key
    // sequence or a paste, so they are taken one character at a time
    for(size_t i = 0; i < input.size();) {
        const auto lead = static_cast<unsigned char>(input[i]);
        size_t len = 1;
        if((lead & 0xE0) == 0xC0) len = 2;
        else if((lead & 0xF0) == 0xE0) len = 3;
        else if((lead & 0xF8) == 0xF0) len = 4;
        if(i + len > input.size()) break;
        const auto character = input.substr(i, len);
        i += len;
        if(acceptsInput(character)) {
            // read back every time, a rejected character may have
            // rewritten the text on its own
            setText(text() + character);
            changed = true;
        } else if(rejectedInput(character)) {
            changed = true;
        }
    }
    if(changed && mChangeAction) mChangeAction();
    return true;
}

void eLineEdit::paintEvent(ePainter& p) {
    if(mRenderBg) eFramedWidget::paintEvent(p);
    const auto& tex = texture();
    if(tex) {
        p.drawTexture(rect(), tex, textAlignment());
    }
    if(!mHovered && !isKeyboardGrabber() && !isLastPressed()) return;
    mTime++;
    if(mTime > 10) {
        if(mTime > 20) {
            mTime = 0;
        }
        return;
    }

    int texw;
    int texh;
    const int fs = fontSize();
    if(tex) {
        texw = tex->width();
        texh = tex->height();
    } else {
        texw = 0;
        texh = fs;
    }
    const int lh = 2*texh/3;
    const int x = texw + fs/4;
    const int y = (height() + lh)/2;
    const int lw = lineWidth();
    const int w = 4*lw;
    const int h = 2*lw;
    SDL_Color col1;
    SDL_Color col2;
    eFontColorHelpers::colors(fontColor(), col1, col2);
    p.fillRect({x, y, w, h}, col1);
}
