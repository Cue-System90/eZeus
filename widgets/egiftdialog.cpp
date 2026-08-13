#include "egiftdialog.h"

#include "elanguage.h"
#include "elabel.h"
#include "eframedbuttonwithicon.h"
#include "engine/egameboard.h"
#include "engine/egifthelpers.h"
#include "estringhelpers.h"

#include "eboardcityswitchbutton.h"
#include <cmath>

void eGiftDialog::initialize(const stdsptr<eWorldCity>& c,
                             const eRequestFunction& func,
                             const eGameBoard& board) {
    const auto r = resolution();
    const double mult = r.multiplier();

    setType(eFrameType::message);

    const auto innerWid = new eWidget(window());
    addWidget(innerWid);
    const int p = std::round(20*mult);
    innerWid->setNoPadding();
    innerWid->move(p, p/2);

    const auto name = c->name();
    auto rof = eLanguage::zeusText(41, 0); // give to
    eStringHelpers::replace(rof, "[city_name]", name);
    const auto rofLabel = new eLabel(window());
    rofLabel->setTinyFontSize();
    rofLabel->setSmallPadding();
    rofLabel->setText(rof);
    rofLabel->fitContent();

    innerWid->addWidget(rofLabel);

    const auto bw = new eWidget(window());
    bw->setNoPadding();

    const auto createCW = [&](const eCityId cid) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        std::vector<eResourceType> gifts;
        const auto all = eResourceTypeHelpers::extractResourceTypes(
                    eResourceType::allBasic | eResourceType::drachmas);
        for(const auto r : all) {
            const int count = eGiftHelpers::giftCount(r);
            const int c = board.resourceCount(cid, r);
            if(c >= count) gifts.push_back(r);
        }

        eWidget* ng = nullptr;
        if(gifts.empty()) {
            const auto rof = eLanguage::zeusText(41, 20); // no gifts possible
            const auto label = new eLabel(window());
            label->setTinyFontSize();
            label->setSmallPadding();
            label->setText(rof);
            label->fitContent();
            ng = label;

            w->addWidget(label);
        } else {
            for(const auto s : gifts) {
                const auto b = new eFramedButtonWithIcon(window());
                b->setPressAction([s, func, cid]() {
                    func(s, cid);
                });
                const auto typeName = eResourceTypeHelpers::typeLongName(s);
                const auto text = " " + typeName;
                b->initialize(s, text);
                w->addWidget(b);
            }
        }
        w->stackVertically();
        w->fitContent();
        if(ng) ng->align(eAlignment::hcenter);
        return w;
    };

    eWidget* cityButtonW = nullptr;

    std::map<eCityId, eWidget*> cws;
    const auto cids = board.personPlayerCitiesOnBoard();
    if(cids.size() > 1) {
        bool first = true;
        for(const auto cid : cids) {
            const auto w = createCW(cid);
            bw->addWidget(w);
            if(first) {
                first = false;
            } else {
                w->hide();
            }
            cws[cid] = w;
        }

        const auto ppid = board.personPlayer();
        const auto cityButton = new eBoardCitySwitchButton(window());
        cityButtonW = cityButton;
        const auto v = eBoardCitySwitchButton::sPlayerCityValidator(board, ppid);
        const auto cityAct = [cws](const eCityId cid) {
            for(const auto& w : cws) {
                w.second->setVisible(w.first == cid);
            }
        };
        cityButton->initialize(board, v, cityAct);
        innerWid->addWidget(cityButton);
    } else if(cids.size() == 1) {
        const auto cid = cids[0];
        const auto w = createCW(cid);
        cws[cid] = w;
        bw->addWidget(w);
    }

    bw->fitContent();
    for(const auto w : cws) {
        w.second->align(eAlignment::hcenter);
    }
    innerWid->addWidget(bw);

    innerWid->stackVertically();
    innerWid->fitContent();

    fitContent();
    rofLabel->align(eAlignment::hcenter);
    bw->align(eAlignment::hcenter);
    if(cityButtonW) cityButtonW->align(eAlignment::hcenter);
}
