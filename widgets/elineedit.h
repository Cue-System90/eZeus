#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include "eframedlabel.h"

class eLineEdit : public eFramedLabel {
public:
    eLineEdit(eMainWindow* const window);

    void setChangeAction(const eAction a);

    void setRenderBg(const bool r);
protected:
    bool mouseMoveEvent(const eMouseEvent& e);
    bool mouseEnterEvent(const eMouseEvent& e);
    bool mouseLeaveEvent(const eMouseEvent& e);
    bool keyPressEvent(const eKeyPressEvent& e);
    bool textInputEvent(const std::string& text);
    void paintEvent(ePainter& p);

    // decides whether a typed character is taken, one UTF-8 character per
    // call. Everything is accepted unless a subclass narrows it down.
    virtual bool acceptsInput(const std::string& character) const;
    // called for a character that acceptsInput rejected
    virtual bool rejectedInput(const std::string& character);
private:
    bool mHovered = false;
    bool mRenderBg = false;
    eAction mChangeAction;
    int mTime = 0;
};

#endif // ELINEEDIT_H
