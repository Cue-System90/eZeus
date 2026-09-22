#include "elabel.h"

eLabel::eLabel(eMainWindow* const window) :
    eWidget(window), eLabelBase(window) {
    setFont(eFonts::defaultFont(resolution()));
}

eLabel::eLabel(const std::string& text,
               eMainWindow* const window) :
    eLabel(window) {
    setText(text);
}

void eLabel::fitOptions(const std::vector<std::string> &options) {
    int w = 0;
    const auto tmp = text();
    for(const auto& v : options) {
        setText(v);
        fitContent();
        const int wv = width();
        if(wv > w) w = wv;
    }
    setWidth(w);
    setText(tmp);
}

void eLabel::sSetRowWidth(const std::vector<eLabel*>& labels, const int w) {
    int size = 0;
    for(const auto l : labels) {
        l->fitFontToWidth(w - 2*l->padding());
        const int s = l->ptSize();
        if(size == 0 || s < size) size = s;
    }
    for(const auto l : labels) {
        l->setFontSize(size);
        l->setWidth(w);
    }
}

void eLabel::renderTargetsReset() {
    eWidget::renderTargetsReset();
    updateTexture();
}

void eLabel::sizeHint(int& w, int& h) {
    const auto& tex = texture();
    if(tex) {
        w = tex->width();
        h = tex->height();
    } else {
        eWidget::sizeHint(w, h);
    }
}

void eLabel::paintEvent(ePainter& p) {
    const auto& tex = texture();
    if(tex) {
        p.drawTexture(rect(), tex, textAlignment());
        //p.drawRect(rect(), {0, 0, 0, 255}, 1);
    }
}
