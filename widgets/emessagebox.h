#ifndef EMESSAGEBOX_H
#define EMESSAGEBOX_H

#include "eframedwidget.h"

#include "emessage.h"
#include "engine/edate.h"

class eWorldCity;
struct eEventData;
enum class eResourceType;
enum class eCityId;
class eLabel;
class eGameWidget;

class eMessageBox : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(eGameBoard &board,
                    const eEventData& ed,
                    const eAction& viewTile,
                    const eAction& closeFunc,
                    eMessage msg);

    void close();
    bool closable() const { return mClosable; }
    const std::string& title() const { return mTitle; }

    eWidget* createTributeWidget(const eResourceType type,
                                 const int count, const int space,
                                 const int months = -1,
                                 eLabel** spaceLabelPtr = nullptr);
protected:
//    void paintEvent(ePainter& p);
    bool keyPressEvent(const eKeyPressEvent& e);
    bool mousePressEvent(const eMouseEvent& e);
private:
    bool mClosable = false;
    std::string mTitle;
    eAction mCloseFunc;
//    eAction mDone;
};

#endif // EMESSAGEBOX_H
