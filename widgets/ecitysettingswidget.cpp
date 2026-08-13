#include "ecitysettingswidget.h"

#include "eframedbutton.h"
#include "elanguage.h"
#include "etradeeditwidget.h"
#include "emainwindow.h"
#include "eresourcebutton.h"
#include "evaluebutton.h"
#include "elineedit.h"
#include "echoosebutton.h"
#include "enamewidget.h"
#include "engine/eworldboard.h"
#include "engine/egameboard.h"
#include "ewatertradewidget.h"
#include "eswitchbutton.h"

#include <algorithm>

class eTributeSettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const stdsptr<eWorldCity>& c,
                    const bool rec) {
        setType(eFrameType::message);
        const auto innerWidget = new eWidget(window());
        innerWidget->setNoPadding();
        const int p = 2*padding();
        innerWidget->move(p, p);
        addWidget(innerWidget);

        const auto countButton = new eValueButton(window());
        countButton->initialize(1, 9999);
        countButton->setValue(rec ? c->recTributeCount() : c->tributeCount());
        countButton->setValueChangeAction([c, rec](const int v) {
            if(rec) c->setRecTributeCount(v);
            else c->setTributeCount(v);
        });
        innerWidget->addWidget(countButton);

        const auto resButton = new eResourceButton(window());
        resButton->initialize([this, p, innerWidget, c, rec](
                              const eResourceType r) {
            if(rec) c->setRecTributeType(r);
            else c->setTributeType(r);
            innerWidget->fitContent();
            resize(innerWidget->x() + innerWidget->width() + p,
                   innerWidget->y() + innerWidget->height() + p);
        }, eResourceType::allBasic | eResourceType::drachmas);
        resButton->setResource(rec ? c->recTributeType() : c->tributeType());
        innerWidget->addWidget(resButton);

        innerWidget->stackHorizontally();
        innerWidget->fitContent();
        resize(innerWidget->x() + innerWidget->width() + p,
               innerWidget->y() + innerWidget->height() + p);
    }
};

void eCitySettingsWidget::initialize(const stdsptr<eWorldCity>& c,
                                     eGameBoard* const board,
                                     eWorldBoard* const wb) {
    if(!c) return;
    const auto ppid = wb->personPlayer();
    setType(eFrameType::message);
    const auto res = resolution();
    const int w = res.centralWidgetLargeWidth();
    const int h = res.centralWidgetLargeHeight();
    resize(w, h);

    const int p = padding();

    const auto buttonsW1 = new eWidget(window());
    buttonsW1->setNoPadding();
    addWidget(buttonsW1);
    buttonsW1->move(2*p, 2*p);
    const int ww = (w - 8*p)/3;
    const int hh = h - 4*p;
    buttonsW1->resize(ww, hh);

    const auto buttonsW2 = new eWidget(window());
    buttonsW2->setNoPadding();
    addWidget(buttonsW2);
    buttonsW2->move(ww + 4*p, 2*p);
    buttonsW2->resize(ww, hh);

    const auto buttonsW3 = new eWidget(window());
    buttonsW3->setNoPadding();
    addWidget(buttonsW3);
    buttonsW3->move(2*ww + 6*p, 2*p);
    buttonsW3->resize(ww, hh);

    const auto nameButton = new eFramedButton(window());
    nameButton->setUnderline(false);
    const auto n = c->name();
    nameButton->setText(n.empty() ? eLanguage::text("name") : n);
    nameButton->fitContent();
    nameButton->setPressAction([this, nameButton, c]() {
        const auto d = new eNameWidget(window());
        d->initialize(c->name(), eWorldCity::sNames(),
                      [c, nameButton](const std::string& name) {
            c->setName(name);
            nameButton->setText(name);
            nameButton->fitContent();
            nameButton->align(eAlignment::hcenter);
        });

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(nameButton);
    nameButton->align(eAlignment::hcenter);

    const auto leaderButton = new eFramedButton(window());
    leaderButton->setUnderline(false);
    const auto l = c->leader();
    leaderButton->setText(l.empty() ? eLanguage::text("leader") : l);
    leaderButton->fitContent();
    leaderButton->setPressAction([this, leaderButton, c]() {
        const auto d = new eNameWidget(window());
        d->initialize(c->leader(), eWorldCity::sLeaders(),
                      [c, leaderButton](const std::string& name) {
            c->setLeader(name);
            leaderButton->setText(name);
            leaderButton->fitContent();
            leaderButton->align(eAlignment::hcenter);
        });

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(leaderButton);
    leaderButton->align(eAlignment::hcenter);

    const auto relationshipButton = new eFramedButton(window());
    const auto nationalityButton = new eFramedButton(window());
    const auto stateButton = new eSwitchButton(window());
    const auto visibleButton = new eSwitchButton(window());
    const auto attitudeButton = new eFramedButton(window());
    const auto directionButton = new eFramedButton(window());
    const auto teamButton = new eFramedButton(window());
    const auto playerButton = new eFramedButton(window());
    const auto capitalButton = new eFramedButton(window());
    const auto type = c->type();
    relationshipButton->setVisible(type == eCityType::foreignCity);
    nationalityButton->setVisible(type == eCityType::foreignCity ||
                                  type == eCityType::colony ||
                                  type == eCityType::parentCity);
    attitudeButton->setVisible(type == eCityType::foreignCity ||
                               type == eCityType::colony);
    directionButton->setVisible(type == eCityType::distantCity);

    const auto teamIdToName = [](const eTeamId id) {
        if(id == eTeamId::neutralAggresive) {
            return std::string("Neutral unfriendly");
        } else if(id == eTeamId::neutralFriendly) {
            return std::string("Neutral friendly");
        } else {
            const int i = static_cast<int>(id);
            return "Team " + std::to_string(i);
        }
    };

    const auto playerIdToName = [](const ePlayerId id) {
        if(id == ePlayerId::neutralAggresive) {
            return std::string("Neutral unfriendly");
        } else if(id == ePlayerId::neutralFriendly) {
            return std::string("Neutral friendly");
        } else {
            const int i = static_cast<int>(id);
            return "Player " + std::to_string(i);
        }
    };

    const auto playerCaptialIdToName = [](const ePlayerId id) {
        if(id == ePlayerId::neutralFriendly ||
           id == ePlayerId::neutralAggresive) {
            return std::string("None");
        } else {
            const int i = static_cast<int>(id);
            return std::to_string(i);
        }
    };

    const auto typeButton = new eFramedButton(window());
    typeButton->setUnderline(false);
    const auto typeName = eWorldCity::sTypeName(type);
    typeButton->setText(typeName);
    typeButton->fitContent();
    typeButton->setPressAction([this, board, ppid, p,
                               relationshipButton, typeButton,
                               directionButton,
                               nationalityButton,
                               attitudeButton,
                               c, buttonsW1]() {
        const std::vector<eCityType> types =
            {eCityType::parentCity,
             eCityType::colony,
             eCityType::foreignCity,
             eCityType::distantCity,
             eCityType::enchantedPlace,
             eCityType::destroyedCity};
        std::vector<std::string> typeNames;
        for(const auto t : types) {
            const auto name = eWorldCity::sTypeName(t);
            typeNames.push_back(name);
        }
        const auto act = [board, ppid, p,
                          relationshipButton, nationalityButton,
                          directionButton,
                          attitudeButton, c, buttonsW1,
                          types, typeNames, typeButton](const int val) {
            const auto type = types[val];
            c->setType(type);
            if(type == eCityType::colony ||
               type == eCityType::parentCity) {
                const auto cid = c->cityId();
                if(c->nationality() == eNationality::atlantean) {
                    if(board) board->setAtlantean(cid, true);
                } else {
                    c->setNationality(eNationality::greek);
                    if(board) board->setAtlantean(cid, false);
                }
            }
            const auto name = typeNames[val];
            typeButton->setText(name);
            typeButton->fitContent();
            typeButton->align(eAlignment::hcenter);

            relationshipButton->setVisible(type == eCityType::foreignCity);
            nationalityButton->setVisible(type == eCityType::foreignCity ||
                                          type == eCityType::colony ||
                                          type == eCityType::parentCity);
            attitudeButton->setVisible(type == eCityType::foreignCity ||
                                       type == eCityType::colony);
            directionButton->setVisible(type == eCityType::distantCity);
            buttonsW1->stackVertically(p, true);

            const auto attitude = c->attitudeClass(ppid);
            attitudeButton->setText(eWorldCity::sAttitudeName(attitude));
            attitudeButton->fitContent();
            attitudeButton->align(eAlignment::hcenter);
        };
        const auto d = new eChooseButton(window());
        d->initialize(3, typeNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(typeButton);
    typeButton->align(eAlignment::hcenter);

    relationshipButton->setUnderline(false);
    const auto relationship = c->relationship();
    relationshipButton->setText(eWorldCity::sRelationshipName(relationship));
    relationshipButton->fitContent();
    relationshipButton->setPressAction([this, attitudeButton, ppid,
                                       relationshipButton, c]() {
        const auto d = new eChooseButton(window());
        const std::vector<eForeignCityRelationship> relationships =
            {eForeignCityRelationship::vassal,
             eForeignCityRelationship::ally,
             eForeignCityRelationship::rival};
        std::vector<std::string> relationshipNames;
        for(const auto r : relationships) {
            const auto name = eWorldCity::sRelationshipName(r);
            relationshipNames.push_back(name);
        }
        const auto act = [attitudeButton, ppid,
                          relationships, relationshipNames,
                          c, relationshipButton](const int val) {
            const auto rel = relationships[val];
            c->setRelationship(rel);
            const auto name = relationshipNames[val];
            relationshipButton->setText(name);
            relationshipButton->fitContent();
            relationshipButton->align(eAlignment::hcenter);

            const auto attitude = c->attitudeClass(ppid);
            attitudeButton->setText(eWorldCity::sAttitudeName(attitude));
            attitudeButton->fitContent();
            attitudeButton->align(eAlignment::hcenter);
        };
        d->initialize(5, relationshipNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(relationshipButton);
    relationshipButton->align(eAlignment::hcenter);

    stateButton->setUnderline(false);
    stateButton->addValue(eWorldCity::sStateName(eCityState::active));
    stateButton->addValue(eWorldCity::sStateName(eCityState::inactive));
    stateButton->fitValidContent();
    const auto state = c->state();
    stateButton->setValue(state == eCityState::active ? 0 : 1);
    stateButton->setSwitchAction([c](const int v) {
        c->setState(v == 0 ? eCityState::active : eCityState::inactive);
    });
    buttonsW1->addWidget(stateButton);
    stateButton->align(eAlignment::hcenter);

    visibleButton->setUnderline(false);
    visibleButton->addValue(eLanguage::zeusText(44, 306)); // invisible
    visibleButton->addValue(eLanguage::zeusText(44, 307)); // visible
    visibleButton->fitValidContent();
    const bool vis = c->visible();
    visibleButton->setValue(vis ? 1 : 0);
    visibleButton->setSwitchAction([c](const int v) {
        c->setVisible(v);
    });
    buttonsW1->addWidget(visibleButton);
    visibleButton->align(eAlignment::hcenter);

    attitudeButton->setUnderline(false);
    const auto attitude = c->attitudeClass(ppid);
    attitudeButton->setText(eWorldCity::sAttitudeName(attitude));
    attitudeButton->fitContent();
    attitudeButton->setPressAction([this, attitudeButton, c, ppid]() {
        const auto d = new eChooseButton(window());
        std::vector<eCityAttitude> attitudes;
        if(c->isAlly()) {
            attitudes.push_back(eCityAttitude::annoyed);
            attitudes.push_back(eCityAttitude::apatheticA);
            attitudes.push_back(eCityAttitude::sympathetic);
            attitudes.push_back(eCityAttitude::congenial);
            attitudes.push_back(eCityAttitude::helpful);
        } else if(c->isVassal() || c->isColony()) {
            attitudes.push_back(eCityAttitude::angry);
            attitudes.push_back(eCityAttitude::bitter);
            attitudes.push_back(eCityAttitude::loyal);
            attitudes.push_back(eCityAttitude::dedicated);
            attitudes.push_back(eCityAttitude::devoted);
        } else { // rival
            attitudes.push_back(eCityAttitude::furious);
            attitudes.push_back(eCityAttitude::displeased);
            attitudes.push_back(eCityAttitude::apatheticR);
            attitudes.push_back(eCityAttitude::respectful);
            attitudes.push_back(eCityAttitude::admiring);
        }
        std::vector<std::string> attitudeNames;
        for(const auto r : attitudes) {
            const auto name = eWorldCity::sAttitudeName(r);
            attitudeNames.push_back(name);
        }
        const auto act = [attitudes, attitudeNames, ppid,
                          c, attitudeButton](const int val) {
            c->setAttitude(10 + val*20, ppid);
            const auto name = attitudeNames[val];
            attitudeButton->setText(name);
            attitudeButton->fitContent();
            attitudeButton->align(eAlignment::hcenter);
        };
        d->initialize(8, attitudeNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(attitudeButton);
    attitudeButton->align(eAlignment::hcenter);

    nationalityButton->setUnderline(false);
    const auto nationality = c->nationality();
    nationalityButton->setText(eWorldCity::sNationalityName(nationality));
    nationalityButton->fitContent();
    nationalityButton->setPressAction([this, board, type,
                                      nationalityButton, c]() {
        const auto d = new eChooseButton(window());
        std::vector<eNationality> nationalities;
        if(type == eCityType::parentCity ||
           type == eCityType::colony) {
            nationalities =
            {eNationality::greek,
             eNationality::atlantean};
        } else {
            nationalities =
            {eNationality::greek,
             eNationality::trojan,
             eNationality::persian,
             eNationality::centaur,
             eNationality::amazon,

             eNationality::egyptian,
             eNationality::mayan,
             eNationality::phoenician,
             eNationality::oceanid,
             eNationality::atlantean};
        }
        std::vector<std::string> nationalityNames;
        for(const auto r : nationalities) {
            const auto name = eWorldCity::sNationalityName(r);
            nationalityNames.push_back(name);
        }
        const auto act = [board,
                          nationalities, nationalityNames,
                          c, nationalityButton](const int val) {
            const auto nat = nationalities[val];
            c->setNationality(nat);
            const auto cid = c->cityId();
            if(board) board->setAtlantean(cid, nat == eNationality::atlantean);
            const auto name = nationalityNames[val];
            nationalityButton->setText(name);
            nationalityButton->fitContent();
            nationalityButton->align(eAlignment::hcenter);
        };
        d->initialize(5, nationalityNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(nationalityButton);
    nationalityButton->align(eAlignment::hcenter);

    playerButton->setUnderline(false);
    const auto cid = c->cityId();
    const auto pid = wb->cityIdToPlayerId(cid);
    playerButton->setText(playerIdToName(pid));
    playerButton->fitContent();
    playerButton->setPressAction([this, c, wb,
                                  playerButton, playerIdToName,
                                  teamButton, teamIdToName]() {
        const auto d = new eChooseButton(window());
        std::vector<std::string> playerNames;
        for(int i = -2; i <= 11; i++) {
            const auto id = static_cast<ePlayerId>(i);
            const auto name = playerIdToName(id);
            playerNames.push_back(name);
        }
        const auto act = [c, wb,
                         playerButton, playerIdToName,
                         teamButton, teamIdToName](const int val) {
            const auto pid = static_cast<ePlayerId>(val - 2);
            const auto cid = c->cityId();
            wb->moveCityToPlayer(cid, pid);
            playerButton->setText(playerIdToName(pid));
            playerButton->fitContent();

            const auto tid = wb->playerIdToTeamId(pid);
            teamButton->setText(teamIdToName(tid));
            teamButton->fitContent();
        };
        d->initialize(8, playerNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW3->addWidget(playerButton);
    playerButton->align(eAlignment::hcenter);

    teamButton->setUnderline(false);
    const auto tid = wb->cityIdToTeamId(cid);
    teamButton->setText(teamIdToName(tid));
    teamButton->fitContent();
    teamButton->setPressAction([this, c, wb, teamButton, teamIdToName]() {
        const auto d = new eChooseButton(window());
        std::vector<std::string> teamNames;
        for(int i = -2; i <= 11; i++) {
            const auto id = static_cast<eTeamId>(i);
            const auto name = teamIdToName(id);
            teamNames.push_back(name);
        }
        const auto act = [c, wb, teamButton, teamIdToName](const int val) {
            const auto pid = c->playerId();
            const auto tid = static_cast<eTeamId>(val - 2);
            wb->setPlayerTeam(pid, tid);
            teamButton->setText(teamIdToName(tid));
            teamButton->fitContent();
        };
        d->initialize(8, teamNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW3->addWidget(teamButton);
    teamButton->align(eAlignment::hcenter);

    capitalButton->setUnderline(false);
    capitalButton->setText(eLanguage::text("capital") + " " + playerCaptialIdToName(pid));
    capitalButton->fitContent();
    capitalButton->setPressAction([this, c, capitalButton, playerCaptialIdToName]() {
        const auto d = new eChooseButton(window());
        std::vector<std::string> playerNames;
        for(int i = -1; i <= 11; i++) {
            const auto id = static_cast<ePlayerId>(i);
            const auto name = playerCaptialIdToName(id);
            playerNames.push_back(name);
        }
        const auto act = [c, capitalButton, playerCaptialIdToName](const int val) {
            const auto pid = static_cast<ePlayerId>(val - 1);
            c->setCapitalOf(pid);
            capitalButton->setText(eLanguage::text("capital") + " " + playerCaptialIdToName(pid));
            capitalButton->fitContent();
        };
        d->initialize(8, playerNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW3->addWidget(capitalButton);
    capitalButton->align(eAlignment::hcenter);

    directionButton->setUnderline(false);
    const auto dir = c->direction();
    directionButton->setText(eWorldCity::sDirectionName(dir));
    directionButton->fitContent();
    directionButton->setPressAction([this, directionButton, c]() {
        const auto d = new eChooseButton(window());
        const std::vector<eDistantDirection> directions =
            {eDistantDirection::none,
             eDistantDirection::N,
             eDistantDirection::NE,
             eDistantDirection::E,
             eDistantDirection::SE,
             eDistantDirection::S,
             eDistantDirection::SW,
             eDistantDirection::W,
             eDistantDirection::NW};
        std::vector<std::string> directionNames;
        for(const auto r : directions) {
            const auto name = eWorldCity::sDirectionName(r);
            directionNames.push_back(name);
        }
        const auto act = [directions, directionNames,
                          c, directionButton](const int val) {
            const auto dir = directions[val];
            c->setDirection(dir);
            const auto name = directionNames[val];
            directionButton->setText(name);
            directionButton->fitContent();
            directionButton->align(eAlignment::hcenter);
        };
        d->initialize(5, directionNames, act);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW1->addWidget(directionButton);
    directionButton->align(eAlignment::hcenter);

    const auto buysButton = new eFramedButton(window());
    buysButton->setUnderline(false);
    buysButton->setText(eLanguage::zeusText(47, 1));
    buysButton->fitContent();
    buysButton->setPressAction([this, c]() {
        const auto d = new eTradeEditWidget(window());
        const auto res = resolution();
        const int w = res.centralWidgetSmallWidth();
        const int h = res.centralWidgetSmallHeight();
        d->resize(w, h);
        d->initialize(&c->buys());

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW2->addWidget(buysButton);
    buysButton->align(eAlignment::hcenter);

    const auto sellsButton = new eFramedButton(window());
    sellsButton->setUnderline(false);
    sellsButton->setText(eLanguage::zeusText(47, 2));
    sellsButton->fitContent();
    sellsButton->setPressAction([this, c]() {
        const auto d = new eTradeEditWidget(window());
        const auto res = resolution();
        const int w = res.centralWidgetSmallWidth();
        const int h = res.centralWidgetSmallHeight();
        d->resize(w, h);
        d->initialize(&c->sells());

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW2->addWidget(sellsButton);
    sellsButton->align(eAlignment::hcenter);

    const auto tributeButton = new eFramedButton(window());
    tributeButton->setUnderline(false);
    tributeButton->setText(eLanguage::text("tribute"));
    tributeButton->fitContent();
    tributeButton->setPressAction([this, c]() {
        const auto d = new eTributeSettingsWidget(window());
        d->initialize(c, false);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW2->addWidget(tributeButton);
    tributeButton->align(eAlignment::hcenter);

    const auto recTributeButton = new eFramedButton(window());
    recTributeButton->setUnderline(false);
    recTributeButton->setText(eLanguage::text("rec_tribute"));
    recTributeButton->fitContent();
    recTributeButton->setPressAction([this, c]() {
        const auto d = new eTributeSettingsWidget(window());
        d->initialize(c, true);

        window()->execDialog(d);
        d->align(eAlignment::center);
    });
    buttonsW2->addWidget(recTributeButton);
    recTributeButton->align(eAlignment::hcenter);

    const auto waterTradeButton = new eFramedButton(window());
    waterTradeButton->setUnderline(false);
    waterTradeButton->setText(eLanguage::text("water_trade"));
    waterTradeButton->fitContent();
    waterTradeButton->setPressAction([this, c, wb]() {
        const auto w = new eWaterTradeWidget(window());
        w->resize(width(), height());

        const auto get = [c]() {
            const auto& cids = c->waterTrades();
            std::vector<eCityId> result;
            for(const auto cid : cids) {
                result.push_back(cid);
            }
            return result;
        };

        const auto add = [c](const eCityId cid) {
            c->setWaterTrade(true, cid);
        };

        const auto remove = [c](const eCityId cid) {
            c->setWaterTrade(false, cid);
        };

        w->initialize(get, add, remove, wb);

        window()->execDialog(w);
        w->align(eAlignment::center);
    });
    buttonsW2->addWidget(waterTradeButton);
    waterTradeButton->align(eAlignment::hcenter);

    const auto mStr = new eValueButton(window());
    mStr->initialize(1, 5);
    mStr->setValue(c->militaryStrength());
    const auto mStrStr = eLanguage::zeusText(44, 349);
    mStr->setText(mStrStr + " " + mStr->text());
    mStr->fitContent();
    mStr->setValueChangeAction([c, mStr, mStrStr](const int v) {
        const int vv = std::clamp(v, 1, 6);
        c->setMilitaryStrength(vv);
        mStr->setValue(vv);
        mStr->setText(mStrStr + " " + mStr->text());
    });
    buttonsW2->addWidget(mStr);
    mStr->align(eAlignment::hcenter);

    const auto eStr = new eValueButton(window());
    eStr->initialize(1, 5);
    eStr->setValue(c->wealth());
    const auto eStrStr = eLanguage::zeusText(44, 350);
    eStr->setText(eStrStr + " " + eStr->text());
    eStr->fitContent();
    eStr->setValueChangeAction([c, eStr, eStrStr](const int v) {
        const int vv = std::clamp(v, 1, 5);
        c->setWealth(vv);
        eStr->setValue(vv);
        eStr->setText(eStrStr + " " + eStr->text());
    });
    buttonsW2->addWidget(eStr);
    eStr->align(eAlignment::hcenter);

    buttonsW1->stackVertically(p, true);
    buttonsW2->stackVertically(p, true);
    buttonsW3->stackVertically(p, true);
}
