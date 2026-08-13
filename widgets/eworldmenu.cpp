#include "eworldmenu.h"

#include "textures/egametextures.h"

#include "ebutton.h"
#include "emainwindow.h"
#include "eworldgoodswidget.h"
#include "eworldtributewidget.h"
#include "egamewidget.h"
#include "engine/eworldboard.h"

#include "elanguage.h"
#include <cmath>

void eWorldMenu::initialize(const eAction& openRequest,
                            const eAction& openFulfill,
                            const eAction& openGift,
                            const eAction& openRaid,
                            const eAction& openConquer,
                            const bool showText) {
    int iRes;
    int mult;
    iResAndMult(iRes, mult);

    const auto& intrfc = eGameTextures::interface();
    const auto& coll = intrfc[iRes];
    const auto tex = coll.fWorldMenuBackground;
    setTexture(tex);
    setPadding(0);
    fitContent();

    {
        const auto wlab = eButton::sCreate(coll.fWorldLeftArrowButton, window(), this);
        const auto whb = eButton::sCreate(coll.fWorldHistoryButton, window(), this);
        const auto wrab = eButton::sCreate(coll.fWorldRightArrowButton, window(), this);

        const int xwlab = std::round(5.5*mult);
        const int xwhb = std::round(34.5*mult);
        const int xwrab = std::round(59.5*mult);

        const int ywlab = std::round(48.5*mult);
        const int ywhb = std::round(44.5*mult);
        const int ywrab = ywlab;

        wlab->setX(xwlab);
        wlab->setY(ywlab);

        whb->setX(xwhb);
        whb->setY(ywhb);

        wrab->setX(xwrab);
        wrab->setY(ywrab);
    }

    {
        mRequestButton = eButton::sCreate(coll.fRequestButton, window(), this);
        mRequestButton->setTooltip(eLanguage::zeusText(44, 308));
        mFulfillButton = eButton::sCreate(coll.fFulfillButton, window(), this);
        mFulfillButton->setTooltip(eLanguage::zeusText(44, 310));
        mGiftButton = eButton::sCreate(coll.fGiftButton, window(), this);
        mGiftButton->setTooltip(eLanguage::zeusText(44, 311));
        mRaidButton = eButton::sCreate(coll.fRaidButton, window(), this);
        mRaidButton->setTooltip(eLanguage::zeusText(44, 312));
        mConquerButton = eButton::sCreate(coll.fConquerButton, window(), this);
        mConquerButton->setTooltip(eLanguage::zeusText(44, 313));

        const int xwrb = std::round(6.5*mult);
        const int xwfb = 35*mult;
        const int xwgb = std::round(63.5*mult);
        const int xwrdb = xwrb;
        const int xwcb = 49*mult;

        const int ywrb = 230*mult;
        const int ywfb = ywrb;
        const int ywgb = ywrb;
        const int ywrdb = 259*mult;
        const int ywcb = ywrdb;

        mRequestButton->setX(xwrb);
        mRequestButton->setY(ywrb);
        mRequestButton->setPressAction([this, openRequest]() {
            const bool editor = mBoard && mBoard->editorMode();
            if(editor) return;
            if(openRequest) openRequest();
        });

        mFulfillButton->setX(xwfb);
        mFulfillButton->setY(ywfb);
        mFulfillButton->setPressAction([this, openFulfill]() {
            const bool editor = mBoard && mBoard->editorMode();
            if(editor) return;
            if(openFulfill) openFulfill();
        });

        mGiftButton->setX(xwgb);
        mGiftButton->setY(ywgb);
        mGiftButton->setPressAction([this, openGift]() {
            const bool editor = mBoard && mBoard->editorMode();
            if(editor) return;
            if(openGift) openGift();
        });

        mRaidButton->setX(xwrdb);
        mRaidButton->setY(ywrdb);
        mRaidButton->setPressAction([this, openRaid]() {
            const bool editor = mBoard && mBoard->editorMode();
            if(editor) return;
            if(openRaid) openRaid();
        });

        mConquerButton->setX(xwcb);
        mConquerButton->setY(ywcb);
        mConquerButton->setPressAction([this, openConquer]() {
            const bool editor = mBoard && mBoard->editorMode();
            if(editor) return;
            if(openConquer) openConquer();
        });

        const auto wh = eButton::sCreate(coll.fHelpButton, window(), this);
        const int whx = 6*mult;
        const int why = 286*mult;

        wh->setX(whx);
        wh->setY(why);

        if(showText) {
            const auto wgw = eButton::sCreate(coll.fWorldSmallButton, window(), this);
            wgw->setPressAction([this](){
                const bool editor = mBoard && mBoard->editorMode();
                if(editor) return;
                window()->showGame(static_cast<eGameBoard*>(nullptr),
                                   eGameWidgetSettings());
            });
            const int wgwx = 20*mult;
            const int wgwy = 285*mult;

            const auto backToCityStr = eLanguage::zeusText(47, 8);
            const auto wgwtxt = new eLabel(backToCityStr, window());
            wgwtxt->setSmallFontSize();
            wgwtxt->fitContent();
            wgw->addWidget(wgwtxt);
            wgwtxt->align(eAlignment::center);

            wgw->setX(wgwx);
            wgw->setY(wgwy);
        }

        const auto wat = eButton::sCreate(coll.fWorldBigButton, window(), this);

        mAttitudeLabel = new eLabel("unknown", window());
        mAttitudeLabel->setTooltip(eLanguage::zeusText(44, 333));
        mAttitudeLabel->setSmallFontSize();
        mAttitudeLabel->fitContent();
        wat->addWidget(mAttitudeLabel);
        mAttitudeLabel->align(eAlignment::center);

        const int watx = 4*mult;
        const int waty = 66*mult;

        wat->setX(watx);
        wat->setY(waty);
    }

    {
        mRelationshipLabel = new eLabel("a", window());
        mRelationshipLabel->setNoPadding();
        mRelationshipLabel->setSmallFontSize();
        mRelationshipLabel->fitContent();
        addWidget(mRelationshipLabel);
        mRelationshipLabel->align(eAlignment::hcenter);
        const int rly = 14*mult;
        mRelationshipLabel->setY(rly);

        mNameLabel = new eLabel("a", window());
        mNameLabel->setNoPadding();
        mNameLabel->setSmallFontSize();
        mNameLabel->fitContent();
        addWidget(mNameLabel);
        mNameLabel->align(eAlignment::hcenter);
        mNameLabel->setY(rly + mRelationshipLabel->height());

        mLeaderLabel = new eLabel("a", window());
        mLeaderLabel->setNoPadding();
        mLeaderLabel->setTinyFontSize();
        mLeaderLabel->fitContent();
        addWidget(mLeaderLabel);
        mLeaderLabel->align(eAlignment::hcenter);
        mLeaderLabel->setY(mNameLabel->y() + mNameLabel->height());
    }

    {
        mTextLabel = new eLabel(window());
        mTextLabel->setNoPadding();
        addWidget(mTextLabel);
        mTextLabel->setX(mult*10);
        mTextLabel->setY(mult*90);
        const int w = mult*75;
        mTextLabel->setWrapWidth(w);
        mTextLabel->setWrapAlignment(eAlignment::hcenter);
        mTextLabel->setWidth(w);
        mTextLabel->setHeight(mult*105);
        mTextLabel->setVisible(showText);
        mShowText = showText;
    }

    {
        mGoodsWidget = new eWorldGoodsWidget(window());
        addWidget(mGoodsWidget);
        mGoodsWidget->setX(mult*10);
        mGoodsWidget->setY(mult*90);

        mGoodsWidget->setWidth(mult*75);
        mGoodsWidget->setHeight(mult*105);

        mGoodsWidget->initialize();
    }

    {
        mTributeWidget = new eWorldTributeWidget(window());
        addWidget(mTributeWidget);

        mTributeWidget->setX(mult*7);
        mTributeWidget->setY(mult*205);

        mTributeWidget->setWidth(mult*84);
        mTributeWidget->setHeight(mult*21);

        mTributeWidget->initialize();
    }

    setCity(nullptr);
}

void eWorldMenu::setCity(const stdsptr<eWorldCity>& c) {
    if(!c && mShowText) {
        const auto text = eLanguage::zeusText(47, 5);
        mTextLabel->setText(text);
        mTextLabel->show();
    } else {
        mTextLabel->hide();
    }

    mCity = c;

    updateButtonsEnabled();
    updateLabels();

    if(mBoard) {
        const auto pid = mBoard->personPlayer();
        mGoodsWidget->setPlayerId(pid);
    }
    mGoodsWidget->setCity(c);
    mTributeWidget->setCity(c);
}

void eWorldMenu::setWorldBoard(eWorldBoard* const b) {
    mBoard = b;
}

void eWorldMenu::setText(const std::string& text) {
    setCity(nullptr);
    mTextLabel->setText(text);
    mTextLabel->show();
}

void eWorldMenu::updateLabels() const {
    if(!mCity) {
        mAttitudeLabel->setText("");
        mRelationshipLabel->setText("");
        mNameLabel->setText("");
        mLeaderLabel->setText("");
        return;
    }

    const auto rel = mCity->relationship();
    const auto type = mCity->type();
    const bool cc = mCity->isCurrentCity();
    const bool onBoardNeutral = mCity->isOnBoardNeutral();
    const bool onBoardColony = mCity->isOnBoardColony();
    mNameLabel->setText(mCity->name());
    const auto leader = eLanguage::zeusText(44, 328);
    if(onBoardNeutral) {
        mAttitudeLabel->setText("");
        mRelationshipLabel->setText("");
        mLeaderLabel->setText("");
    } else if(onBoardColony) {
        mAttitudeLabel->setText("");
        mLeaderLabel->setText("");
    } else {
        mLeaderLabel->setText(leader + " " + mCity->leader());
    }

    if(cc || onBoardColony) {
        mAttitudeLabel->setText("");
    } else {
        const auto ppid = mBoard->personPlayer();
        const auto at = mCity->attitudeClass(ppid);
        const auto atStr = eWorldCity::sAttitudeName(at);
        mAttitudeLabel->setText(atStr);
        mAttitudeLabel->fitContent();
        mAttitudeLabel->align(eAlignment::center);
    }

    {
        int group = -1;
        int string = -1;
        if(cc) {
            group = 47;
            string = 0;
        } else {
            switch(type) {
            case eCityType::parentCity:
                group = 39;
                string = 0;
                break;
            case eCityType::colony:
                group = 253;
                string = 3;
                break;
            case eCityType::foreignCity: {
                switch(rel) {
                case eForeignCityRelationship::vassal:
                    group = 253;
                    string = 2;
                    break;
                case eForeignCityRelationship::ally:
                    group = 253;
                    string = 0;
                    break;
                case eForeignCityRelationship::rival:
                    group = 253;
                    string = 1;
                    break;
                }
            } break;

            case eCityType::distantCity:
                group = 39;
                string = 4;
                break;
            case eCityType::enchantedPlace:
                group = 39;
                string = 5;
                break;
            case eCityType::destroyedCity:
                group = 39;
                string = 6;
                break;
            }
        }
        const auto relStr = eLanguage::zeusText(group, string);
        mRelationshipLabel->setText(relStr);
        mRelationshipLabel->fitContent();
        mRelationshipLabel->align(eAlignment::hcenter);
    }
}

void eWorldMenu::updateButtonsEnabled() const {
    if(!mCity) {
        mRequestButton->setEnabled(false);
        mFulfillButton->setEnabled(false);
        mGiftButton->setEnabled(false);
        mRaidButton->setEnabled(false);
        mConquerButton->setEnabled(false);
    } else {
        const bool cc = mCity->isCurrentCity();
        const auto rel = mCity->relationship();
        const auto type = mCity->type();
        const bool vassalOrColony = (type == eCityType::foreignCity &&
                                     rel == eForeignCityRelationship::vassal) ||
                                    type == eCityType::colony;
        const bool distant = type == eCityType::distantCity;
        const bool onBoard = mCity->isOnBoard();
        const bool onBoardNeutral = mCity->isOnBoardNeutral();
        const bool onBoardPlayerColony = mCity->isOnBoardColony();
        const auto ppid = mBoard->personPlayer();
        const bool ownedOnBoardColony = onBoardPlayerColony && mCity->playerId() == ppid;
        const bool onBoardEnemyColony = onBoardPlayerColony && !ownedOnBoardColony;

        mRequestButton->setEnabled(!distant && !cc &&
                                   !onBoardPlayerColony && !onBoardNeutral);
        mFulfillButton->setEnabled(!distant && !cc &&
                                   !onBoardPlayerColony && !onBoardNeutral);
        mGiftButton->setEnabled(!distant && !cc &&
                                !onBoardPlayerColony && !onBoardNeutral);
        mRaidButton->setEnabled(!vassalOrColony &&
                                !distant && !cc && !onBoard);

        const auto cids = mBoard->personPlayerCities();
        int nPlayerOnBoard = 0;
        for(const auto cid : cids) {
            const auto c = mBoard->cityWithId(cid);
            if(!c) continue;
            const bool ob = c->isOnBoard();
            if(!ob) continue;
            nPlayerOnBoard++;
        }
        const bool sendReinforcements = nPlayerOnBoard > 1 && (ownedOnBoardColony || cc);
        if(sendReinforcements) {
            mConquerButton->setEnabled(true);
            mConquerButton->setTooltip(eLanguage::zeusText(41, 7));
        } else {
            mConquerButton->setEnabled((!vassalOrColony || mCity->conqueredByRival() || onBoardEnemyColony) &&
                                       !distant && !cc && !ownedOnBoardColony &&
                                       !onBoardNeutral);
            mConquerButton->setTooltip(eLanguage::zeusText(44, 313));
        }
    }
}
