#include "etopbarwidget.h"

#include "engine/egameboard.h"
#include "engine/boardData/epopulationdata.h"
#include "textures/egametextures.h"
#include "ebutton.h"
#include "edatewidget.h"
#include "egamewidget.h"

#include "emainwindow.h"

void eTopBarWidget::initialize() {
    const auto& intrfc = eGameTextures::interface();
    const auto uiScale = resolution().uiScale();
    const int icoll = static_cast<int>(uiScale);
    const int mult = icoll + 1;
    const auto& coll = intrfc[icoll];
    setPadding(0);

    const auto s0 = new eWidget(window());
    s0->setWidth(mult*20);

    mDrachmasWidget = new eTopWidget(window());
    mDrachmasWidget->initialize(coll.fDrachmasTopMenu, "-");

    const auto s1 = new eWidget(window());
    s1->setWidth(mult*20);

    mCityLabel = new eLabel("-", window());
    mCityLabel->setSmallFontSize();
    mCityLabel->setNoPadding();
    mCityLabel->fitContent();

    const auto s2 = new eWidget(window());
    s2->setWidth(mult*20);

    mPopulationWidget = new eTopWidget(window());
    mPopulationWidget->initialize(coll.fPopulationTopMenu, "-");

    const auto s3 = new eWidget(window());
    s3->setWidth(mult*20);

    mDateLabel = new eButton(window());
    mDateLabel->setPressAction([this]() {
        if(!mBoard) return;
        const auto dw = new eDateWidget(window());
        dw->initialize([this](const eDate& d) {
            mBoard->setDate(d);
            mDateLabel->setText(d.shortString());
        }, false);
        dw->setDate(mBoard->date());
        window()->execDialog(dw);
        dw->align(eAlignment::center);
    });
    const eDate date(30, eMonth::january, -1500);
    mDateLabel->setSmallFontSize();
    mDateLabel->setText(date.shortString());
    mDateLabel->fitContent();
    mDateLabel->setEnabled(false);

    const auto s4 = new eWidget(window());
    s4->setWidth(mult*15);

    addWidget(s0);
    addWidget(mCityLabel);
    addWidget(s1);
    addWidget(mDrachmasWidget);
    addWidget(s2);
    addWidget(mPopulationWidget);
    addWidget(s3);
    addWidget(mDateLabel);
    addWidget(s4);

    setHeight(12*mult);

    mCityLabel->align(eAlignment::vcenter);
    mDrachmasWidget->align(eAlignment::vcenter);
    mPopulationWidget->align(eAlignment::vcenter);
    mDateLabel->align(eAlignment::vcenter);

    // the spacers above are the intended gaps, spreading the row over the
    // whole width on top of them leaves the left of a wide bar empty
    stackHorizontally(0);
}

void eTopBarWidget::setBoard(eGameBoard* const board) {
    mBoard = board;
}

void eTopBarWidget::setGameWidget(eGameWidget* const gw) {
    mGW = gw;
}

void eTopBarWidget::paintEvent(ePainter& p) {
    // const bool update = (++mTime % 60) == 0;
    if(mBoard) {
        const auto cid = mGW->viewedCity();
        const auto pid = mBoard->personPlayer();
//        const auto pid = mBoard->cityIdToPlayerId(cid);
        const auto& wb = mBoard->world();
        const auto c = wb.cityWithId(cid);

        const auto label = c ? c->name() : "-";
        mCityLabel->setText(label);
        mCityLabel->fitContent();

        const auto popData = mBoard->populationData(cid);
        if(popData) {
            const int pop = popData->population();
            mPopulationWidget->setText(std::to_string(pop));
        } else {
            mPopulationWidget->setText("-");
        }

        const int d = mBoard->drachmas(pid);
        mDrachmasWidget->setText(std::to_string(d));

        mDateLabel->setText(mBoard->date().shortString());
        mDateLabel->fitContent();
        mDateLabel->setEnabled(mBoard->editorMode());

        // the texts above are wider than the placeholders initialize() laid
        // the row out with, so the row has to be laid out again
        stackHorizontally(0);
        mCityLabel->align(eAlignment::vcenter);
        mDrachmasWidget->align(eAlignment::vcenter);
        mPopulationWidget->align(eAlignment::vcenter);
        mDateLabel->align(eAlignment::vcenter);

        int iRes;
        int mult;
        iResAndMult(iRes, mult);
        const auto& intrfc = eGameTextures::interface()[iRes];
        const auto& tex = intrfc.fGameTopBar;
        const int texWidth = tex->width();
        const auto& rend = p.renderer();
        bool flip = false;
        for(int x = width() - texWidth; x > -texWidth; x -= texWidth) {
            tex->render(rend, x, 0, flip);
            flip = !flip;
        }
    } else {
        mDateLabel->setEnabled(false);
    }
}
