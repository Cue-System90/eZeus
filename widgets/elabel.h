#ifndef ELABEL_H
#define ELABEL_H

#include "ewidget.h"
#include "elabelbase.h"

class eLabel : public eWidget, public eLabelBase {
public:
    eLabel(eMainWindow* const window);
    eLabel(const std::string& text,
           eMainWindow* const window);

    void fitOptions(const std::vector<std::string>& options);
    // Gives every label the same width and, where a text does not fit it,
    // the same reduced font size.
    static void sSetRowWidth(const std::vector<eLabel*>& labels, const int w);
    void renderTargetsReset() override;
protected:
    void sizeHint(int& w, int& h) override;
    void paintEvent(ePainter& p) override;
};

#endif // ELABEL_H
