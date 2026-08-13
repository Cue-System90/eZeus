#include "enumlineedit.h"

#include <algorithm>
#include <stdexcept>

eNumLineEdit::eNumLineEdit(eMainWindow* const window) :
    eLineEdit(window) {
    setValue(0);
}

bool eNumLineEdit::keyPressEvent(const eKeyPressEvent& e) {
    const auto k = e.key();
    if(k != SDL_Scancode::SDL_SCANCODE_BACKSPACE) return false;
    eLineEdit::keyPressEvent(e);
    setValue(value());
    return true;
}

bool eNumLineEdit::acceptsInput(const std::string& character) const {
    if(character.size() != 1) return false;
    return character[0] >= '0' && character[0] <= '9';
}

bool eNumLineEdit::rejectedInput(const std::string& character) {
    // handled here rather than through a scan code, so that it also works
    // on layouts where minus does not sit on the US key
    if(character != "-") return false;
    setValue(-value());
    return true;
}

bool eNumLineEdit::textInputEvent(const std::string& text) {
    const bool r = eLineEdit::textInputEvent(text);
    setValue(value()); // strips leading zeros, as typing used to do
    return r;
}

int eNumLineEdit::value() const {
    const auto str = text();
    if(str == "" || str == "-") return 0;
    try {
        return std::stoi(str);
    } catch(const std::out_of_range&) {
        // typing past the range of an int used to throw out of main
        return str.front() == '-' ? (-__INT_MAX__ - 1) : __INT_MAX__;
    }
}

int eNumLineEdit::clampedValue() const {
    const int v = std::clamp(value(), mMin, mMax);
    return v;
}

void eNumLineEdit::setValue(const int value) {
    setText(std::to_string(value));
}

void eNumLineEdit::setRange(const int min, const int max) {
    mMin = min;
    mMax = max;
}

void eNumLineEdit::clamp() {
    const int v = clampedValue();
    setText(std::to_string(v));
}

bool eNumLineEdit::mouseWheelEvent(const eMouseWheelEvent& e) {
    setValue(value() + ((e.dy() > 0) ? 1 : -1));
    return true;
}
