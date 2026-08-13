#include "esanctuaryinfowidget.h"

#include "elanguage.h"
#include "widgets/eframedbutton.h"
#include "characters/gods/egod.h"
#include "estringhelpers.h"
#include "buildings/sanctuaries/esanctuary.h"
#include "buildings/pyramids/epyramid.h"
#include "engine/egameboard.h"
#include "widgets/echoosecitydialog.h"
#include "evectorhelpers.h"
#include "widgets/egamewidget.h"
#include "widgets/eprogressbar.h"

#include <algorithm>
#include <cmath>

eSanctuaryInfoWidget::eSanctuaryInfoWidget(
        eMainWindow* const window,
        eMainWidget* const mw) :
    eEmployingBuildingInfoWidget(window, mw, false, false) {}

int sTextGodId(const eGodType god) {
    switch(god) {
    case eGodType::zeus:
        return 0;
    case eGodType::poseidon:
        return 1;
    case eGodType::demeter:
        return 2;
    case eGodType::apollo:
        return 3;
    case eGodType::artemis:
        return 4;
    case eGodType::ares:
        return 5;
    case eGodType::aphrodite:
        return 6;
    case eGodType::hermes:
        return 7;
    case eGodType::athena:
        return 8;
    case eGodType::hephaestus:
        return 9;
    case eGodType::dionysus:
        return 10;
    case eGodType::hades:
        return 11;
    case eGodType::hera:
        return 12;
    case eGodType::atlas:
        return 13;
    }
    return 0;
}

void eSanctuaryInfoWidget::initialize(eMonument* const m) {
    assert(m);
    const int p = resolution().largePadding();
    eSanctuary* const s = dynamic_cast<eSanctuary*>(m);
    if(s && s->finished()) {
        const auto title = eBuilding::sNameForBuilding(m);
        eInfoWidget::initialize(title);
        std::string employmentInfo;
        {
            std::string title;
            std::string info;
            std::string additionalInfo;
            eBuilding::sInfoText(m, title, info,
                                 employmentInfo,
                                 additionalInfo);
        }
        addText(employmentInfo);
        const auto cw = addCentralWidget();
        addEmploymentWidget(m);
        const auto gt = s->godType();

        const int cww = cw->width();
        const auto descLabel = new eLabel(window());
        descLabel->setNoPadding();
        descLabel->setSmallFontSize();
        descLabel->setWrapWidth(cww);
        const int godId = sTextGodId(gt);
        std::string desc;
        {
            const int string = 66 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        {
            const int string = 80 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        {
            const int string = 94 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        descLabel->setText(desc);
        descLabel->fitContent();
        cw->addWidget(descLabel);

        const auto buttonReasonW = new eWidget(window());
        buttonReasonW->setNoPadding();
        cw->addWidget(buttonReasonW);
        buttonReasonW->setWidth(cww);

        const auto reasonLabel = new eLabel(window());
        reasonLabel->setNoPadding();
        reasonLabel->setSmallFontSize();
        reasonLabel->setWrapWidth(cww);
        buttonReasonW->addWidget(reasonLabel);

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();

        {
            const auto bw = new eWidget(window());
            bw->setNoPadding();
            buttonsW->addWidget(bw);
            const int string = 10 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            const auto pb = new eFramedButton(txt, window());
            pb->setUnderline(false);
            pb->fitContent();
            bw->addWidget(pb);
            pb->setPressAction([s, godId, buttonReasonW, reasonLabel]() {
                eHelpDenialReason reason;
                const bool r = s->askForHelp(reason);
                int string;
                if(!r) {
                    switch(reason) {
                    case eHelpDenialReason::tooSoon:
                        string = 52 + godId;
                        break;
                    case eHelpDenialReason::noTarget:
                        string = 38 + godId;
                        break;
                    case eHelpDenialReason::error:
                        string = -1;
                        break;
                    }
                    reasonLabel->setYellowFontColor();
                } else {
                    string = 24 + godId;
                    reasonLabel->setLightFontColor();
                }
                const auto txt = eLanguage::zeusText(132, string);
                reasonLabel->setText(txt);
                reasonLabel->fitContent();

                buttonReasonW->stackVertically();
                buttonReasonW->fitHeight();
                buttonReasonW->align(eAlignment::bottom);
            });
            const auto bar = new eProgressBar(window());
            bar->setRange(0, 100);
            const double frac = s->helpTimeFraction();
            bar->setValue(std::clamp(int(std::floor(100*frac)), 5, 100));
            bw->addWidget(bar);
            const int w = pb->width();
            bar->resize(w, p);

            bw->stackVertically(p);
            bw->fitContent();
        }
        const auto& board = m->getBoard();
        const auto cids = board.citiesOnBoard();
        const auto pid = m->playerId();
        const auto ptid = board.playerIdToTeamId(pid);
        const auto enemyCids = board.enemyCidsOnBoard(ptid);
        if(!enemyCids.empty()) {
            const auto bw = new eWidget(window());
            bw->setNoPadding();
            buttonsW->addWidget(bw);
            const auto txt = eLanguage::zeusText(156, 27);
            const auto pb = new eFramedButton(txt, window());
            pb->setUnderline(false);
            pb->fitContent();
            bw->addWidget(pb);
            const auto wboard = &board.world();
            pb->setPressAction([this, wboard, s, buttonReasonW, reasonLabel, enemyCids]() {
                const auto askForAttack = [s, buttonReasonW, reasonLabel](const eCityId cid) {
                    eHelpDenialReason reason;
                    const bool r = s->askForAttack(cid, reason);
                    int string;
                    if(!r) {
                        switch(reason) {
                        case eHelpDenialReason::tooSoon: {
                            const double frac = s->helpAttackTimeFraction();
                            const int per = std::clamp(int(std::floor(100*frac)), 0, 100);
                            string = 19 + per/17;
                        } break;
                        case eHelpDenialReason::noTarget:
                        case eHelpDenialReason::error:
                            string = -1;
                            break;
                        }
                        reasonLabel->setYellowFontColor();
                    } else {
                        string = 25;
                        reasonLabel->setLightFontColor();
                    }
                    const auto godType = s->godType();
                    const auto godName = eGod::sGodName(godType);
                    const auto txt = godName + " " + eLanguage::zeusText(59, string);
                    reasonLabel->setText(txt);
                    reasonLabel->fitContent();

                    buttonReasonW->stackVertically();
                    buttonReasonW->fitHeight();
                    buttonReasonW->align(eAlignment::bottom);
                };
                if(enemyCids.size() == 1) {
                    askForAttack(enemyCids[0]);
                } else {
                    const auto choose = new eChooseCityDialog(window());
                    choose->setValidator([enemyCids](const stdsptr<eWorldCity>& c) {
                        const auto cid = c->cityId();
                        return eVectorHelpers::contains(enemyCids, cid);
                    });
                    const auto act = [askForAttack](const stdsptr<eWorldCity>& c) {
                        const auto cid = c->cityId();
                        askForAttack(cid);
                    };
                    choose->initialize(wboard, act);

                    const auto mw = mainWidget();
                    mw->openDialog(choose);
                }
            });
            const auto bar = new eProgressBar(window());
            bar->setRange(0, 100);
            const double frac = s->helpAttackTimeFraction();
            bar->setValue(std::clamp(int(std::floor(100*frac)), 5, 100));
            bw->addWidget(bar);
            const int w = pb->width();
            bar->resize(w, p);

            bw->stackVertically(p);
            bw->fitContent();
        }

        buttonsW->stackHorizontally(p);
        buttonsW->fitContent();

        buttonReasonW->addWidget(buttonsW);
        buttonsW->align(eAlignment::hcenter);

        buttonReasonW->stackVertically();
        buttonReasonW->fitHeight();
        buttonReasonW->align(eAlignment::bottom);
    } else if(m->finished()) {
        const auto title = eBuilding::sNameForBuilding(m);
        eInfoWidget::initialize(title);
        const auto type = m->type();
        int string = -1;
        switch(type) {
        case eBuildingType::modestPyramid:
            string = 114;
            break;
        case eBuildingType::pyramid:
            string = 115;
            break;
        case eBuildingType::greatPyramid:
            string = 116;
            break;
        case eBuildingType::majesticPyramid:
            string = 117;
            break;

        case eBuildingType::smallMonumentToTheSky:
            string = 118;
            break;
        case eBuildingType::monumentToTheSky:
            string = 119;
            break;
        case eBuildingType::grandMonumentToTheSky:
            string = 120;
            break;

        case eBuildingType::minorShrineAphrodite:
        case eBuildingType::minorShrineApollo:
        case eBuildingType::minorShrineAres:
        case eBuildingType::minorShrineArtemis:
        case eBuildingType::minorShrineAthena:
        case eBuildingType::minorShrineAtlas:
        case eBuildingType::minorShrineDemeter:
        case eBuildingType::minorShrineDionysus:
        case eBuildingType::minorShrineHades:
        case eBuildingType::minorShrineHephaestus:
        case eBuildingType::minorShrineHera:
        case eBuildingType::minorShrineHermes:
        case eBuildingType::minorShrinePoseidon:
        case eBuildingType::minorShrineZeus:
            string = 121;
            break;
        case eBuildingType::shrineAphrodite:
        case eBuildingType::shrineApollo:
        case eBuildingType::shrineAres:
        case eBuildingType::shrineArtemis:
        case eBuildingType::shrineAthena:
        case eBuildingType::shrineAtlas:
        case eBuildingType::shrineDemeter:
        case eBuildingType::shrineDionysus:
        case eBuildingType::shrineHades:
        case eBuildingType::shrineHephaestus:
        case eBuildingType::shrineHera:
        case eBuildingType::shrineHermes:
        case eBuildingType::shrinePoseidon:
        case eBuildingType::shrineZeus:
            string = 122;
            break;
        case eBuildingType::majorShrineAphrodite:
        case eBuildingType::majorShrineApollo:
        case eBuildingType::majorShrineAres:
        case eBuildingType::majorShrineArtemis:
        case eBuildingType::majorShrineAthena:
        case eBuildingType::majorShrineAtlas:
        case eBuildingType::majorShrineDemeter:
        case eBuildingType::majorShrineDionysus:
        case eBuildingType::majorShrineHades:
        case eBuildingType::majorShrineHephaestus:
        case eBuildingType::majorShrineHera:
        case eBuildingType::majorShrineHermes:
        case eBuildingType::majorShrinePoseidon:
        case eBuildingType::majorShrineZeus:
            string = 123;
            break;

        case eBuildingType::pyramidOfThePantheon:
            string = 124;
            break;
        case eBuildingType::altarOfOlympus:
            string = 125;
            break;
        case eBuildingType::templeOfOlympus:
            string = 126;
            break;
        case eBuildingType::observatoryKosmika:
            string = 127;
            break;
        case eBuildingType::museumAtlantika:
            string = 128;
            break;
        default:
            break;
        }

        auto text = eLanguage::zeusText(132, string);
        const auto god = ePyramid::sGod(type);
        const auto name = eGod::sGodName(god);
        eStringHelpers::replace(text, "[god]", name);
        eStringHelpers::replace(text, "[god]", name);
        addText(text);
    } else {
        const auto name = eBuilding::sNameForBuilding(m);
        auto title = eLanguage::zeusText(178, 2);
        eStringHelpers::replace(title, "[monument]", name);
        eInfoWidget::initialize(title);

        const bool r = m->accessToRoad();
        if(!r) {
            addText(eLanguage::zeusText(69, 4));
        }
        const bool h = m->constructionHalted();
        if(h) {
            addText(eLanguage::zeusText(132, 130));
        }
        auto& board = m->getBoard();
        const auto cid = m->cityId();
        const int na = board.countBuildings(cid, eBuildingType::artisansGuild);
        if(na == 0) {
            addText(eLanguage::zeusText(178, 0));
        }

        const int p = m->progress();
        const auto pStr = std::to_string(p);
        if(s) {
            const auto god = s->godType();
            const auto godStr = eGod::sGodName(god);
            auto complete = eLanguage::zeusText(178, 23);
            eStringHelpers::replace(complete, "[god]", godStr);
            eStringHelpers::replace(complete, "[percent_complete]", pStr + "%");
            addText(complete);
        } else {
            auto complete = name + " " + eLanguage::zeusText(178, 24);
            eStringHelpers::replace(complete, "[percent_complete]", pStr + "%");
            addText(complete);
        }

        const auto cost = m->cost();
        const auto stored = m->stored();
        const auto used = m->used();
        const auto needed = cost - stored - used;
        const int nm = needed.fMarble;
        const auto nmStr = std::to_string(nm);
        const int nw = needed.fWood;
        const auto nwStr = std::to_string(nw);
        const int ns = needed.fSculpture;
        const auto nsStr = std::to_string(ns);
        const int no = needed.fOrichalc;
        const auto noStr = std::to_string(no);
        const int nbm = needed.fBlackMarble;
        const auto nbmStr = std::to_string(nbm);
        if(nm > 0 || nw > 0 || ns > 0 || no > 0 || nbm > 0) {
            auto rem = eLanguage::zeusText(178, 25);
            if(nm == 1) {
                auto remM = eLanguage::zeusText(178, 26);
                eStringHelpers::replace(remM, "[amount]", nmStr);
                rem += "\n" + remM;
            } else if(nm > 1) {
                auto remM = eLanguage::zeusText(178, 27);
                eStringHelpers::replace(remM, "[amount]", nmStr);
                rem += "\n" + remM;
            }
            if(nw == 1) {
                auto remW = eLanguage::zeusText(178, 30);
                eStringHelpers::replace(remW, "[amount]", nwStr);
                rem += "\n" + remW;
            } else if(nw > 1) {
                auto remW = eLanguage::zeusText(178, 31);
                eStringHelpers::replace(remW, "[amount]", nwStr);
                rem += "\n" + remW;
            }
            if(ns == 1) {
                auto remS = eLanguage::zeusText(178, 32);
                eStringHelpers::replace(remS, "[amount]", nsStr);
                rem += "\n" + remS;
            } else if(ns > 1) {
                auto remS = eLanguage::zeusText(178, 33);
                eStringHelpers::replace(remS, "[amount]", nsStr);
                rem += "\n" + remS;
            }
            if(no == 1) {
                auto remO = eLanguage::zeusText(178, 34);
                eStringHelpers::replace(remO, "[amount]", noStr);
                rem += "\n" + remO;
            } else if(no > 1) {
                auto remO = eLanguage::zeusText(178, 35);
                eStringHelpers::replace(remO, "[amount]", noStr);
                rem += "\n" + remO;
            }
            if(nbm == 1) {
                auto remNbm = eLanguage::zeusText(178, 28);
                eStringHelpers::replace(remNbm, "[amount]", nbmStr);
                rem += "\n" + remNbm;
            } else if(nbm > 1) {
                auto remNbm = eLanguage::zeusText(178, 29);
                eStringHelpers::replace(remNbm, "[amount]", nbmStr);
                rem += "\n" + remNbm;
            }
            addText(rem);
        } else {
            const auto all = eLanguage::zeusText(178, 36);
            addText(all);
        }

        const auto cw = addCentralWidget();
        const auto haltB = new eFramedButton(window());
        haltB->setUnderline(false);
        haltB->setText(h ? eLanguage::zeusText(132, 113) :
                           eLanguage::zeusText(132, 112));
        haltB->fitContent();
        haltB->setPressAction([m, haltB]() {
            bool h = m->constructionHalted();
            h = !h;
            m->setConstructionHalted(h);
            haltB->setText(h ? eLanguage::zeusText(132, 113) :
                               eLanguage::zeusText(132, 112));
            haltB->fitContent();
            haltB->align(eAlignment::hcenter);
        });
        cw->addWidget(haltB);
        haltB->align(eAlignment::hcenter | eAlignment::bottom);
    }
}
