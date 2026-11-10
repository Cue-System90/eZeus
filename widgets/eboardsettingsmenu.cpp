#include "eboardsettingsmenu.h"

#include "eframedbutton.h"
#include "elanguage.h"

#include "engine/emapgenerator.h"

#include "eacceptbutton.h"
#include "ecancelbutton.h"
#include "enumlineedit.h"

#include "engine/egameboard.h"
#include "emainwindow.h"
#include "egamewidget.h"
#include "eswitchbutton.h"

#include "estringhelpers.h"

#include "ecityonboardselectionwidget.h"

void eBoardSettingsMenu::initialize(
        eGameWidget* const gw, eGameBoard& board) {
    setType(eFrameType::message);

    const auto boardPtr = &board;

    const auto fogButt = new eSwitchButton(window());
    fogButt->setUnderline(false);
    fogButt->addValue(eLanguage::text("no_fog_of_war"));
    fogButt->addValue(eLanguage::text("fog_of_war"));
    fogButt->fitValidContent();
    fogButt->setValue(board.fogOfWar() ? 1 : 0);
    fogButt->setSwitchAction([boardPtr](const int val) {
        boardPtr->setFogOfWar(val);
    });
    addWidget(fogButt);
    fogButt->align(eAlignment::hcenter);

    const auto citiesButt = new eFramedButton(window());
    citiesButt->setUnderline(false);
    citiesButt->setText(eLanguage::text("cities_on_board"));
    citiesButt->fitContent();
    citiesButt->setPressAction([this, gw, boardPtr]() {
        const auto citiesMenu = new eCityOnBoardSelectionWidget(window());
        citiesMenu->resize(width(), height());

        const auto get = [boardPtr]() {
            return boardPtr->citiesOnBoard();
        };

        const auto wboard = &boardPtr->world();

        const auto add = [boardPtr, wboard, gw](const eCityId cid) {
            const auto c = boardPtr->addCityToBoard(cid);
            const auto wc = wboard->cityWithId(cid);
            const auto nat = wc->nationality();
            c->setAtlantean(nat == eNationality::atlantean);
            boardPtr->updatePlayersOnBoard();
            gw->updateMaps(true);
            gw->updateCitiesOnBoard();
        };

        const auto remove = [boardPtr, gw](const eCityId cid) {
            boardPtr->removeCityFromBoard(cid);
            boardPtr->updatePlayersOnBoard();
            gw->updateMaps(true);
            gw->updateCitiesOnBoard();
        };

        citiesMenu->initialize(get, add, remove, boardPtr, wboard);

        window()->execDialog(citiesMenu);
        citiesMenu->align(eAlignment::center);
    });
    addWidget(citiesButt);
    citiesButt->align(eAlignment::hcenter);

    const auto resizeButt = new eFramedButton(window());
    resizeButt->setUnderline(false);
    resizeButt->setText(eLanguage::text("resize"));
    resizeButt->fitContent();
    resizeButt->setPressAction([this, gw, boardPtr]() {
        const auto resizeMenu = new eFramedWidget(window());
        resizeMenu->setType(eFrameType::message);
        resizeMenu->resize(width(), height());

        const auto res = resolution();
        const int p = res.largePadding();

        const auto iw = new eWidget(window());
        iw->setNoPadding();
        resizeMenu->addWidget(iw);
        iw->move(p, p);

        const auto editW = new eWidget(window());
        editW->setNoPadding();
        iw->addWidget(editW);

        const auto widthL = new eLabel(window());
        widthL->setText(eLanguage::text("width"));
        widthL->fitContent();
        editW->addWidget(widthL);

        const auto width = new eNumLineEdit(window());
        width->setRenderBg(true);
        width->setRange(10, 999);
        width->setValue(999);
        width->fitContent();
        width->setValue(boardPtr->width());
        editW->addWidget(width);

        const auto heightL = new eLabel(window());
        heightL->setText(eLanguage::text("height"));
        heightL->fitContent();
        editW->addWidget(heightL);

        const auto height = new eNumLineEdit(window());
        height->setRenderBg(true);
        height->setRange(10, 999);
        height->setValue(999);
        height->fitContent();
        height->setValue(boardPtr->height());
        editW->addWidget(height);

        editW->stackHorizontally();
        editW->fitContent();

        widthL->align(eAlignment::vcenter);
        width->align(eAlignment::vcenter);
        heightL->align(eAlignment::vcenter);
        height->align(eAlignment::vcenter);

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();

        const auto accept = new eAcceptButton(window());
        const auto cancel = new eCancelButton(window());
        accept->setPressAction([resizeMenu, width, height, boardPtr, gw]() {
            int w = width->value();
            if(w % 2) w++;
            int h = height->value();
            if(h % 2) h++;
            boardPtr->resize(w, h);
            gw->updateViewBoxSize();
            gw->updateTopBottomAltitude();
            gw->updateMinMaxAltitude();
            gw->updateMaps(true);
            resizeMenu->deleteLater();
        });
        cancel->setPressAction([resizeMenu]() {
            resizeMenu->deleteLater();
        });

        buttonsW->addWidget(accept);
        buttonsW->addWidget(cancel);
        buttonsW->fitHeight();
        buttonsW->setWidth(editW->width());
        buttonsW->layoutHorizontallyWithoutSpaces();
        iw->addWidget(buttonsW);

        iw->stackVertically();
        iw->fitContent();
        resizeMenu->resize(2*p + iw->width(), 2*p + iw->height());

        window()->execDialog(resizeMenu);
        resizeMenu->align(eAlignment::center);
    });
    addWidget(resizeButt);
    resizeButt->align(eAlignment::hcenter);

    const auto generateButt = new eFramedButton(window());
    generateButt->setUnderline(false);
    generateButt->setText(eLanguage::text("generate"));
    generateButt->fitContent();
    generateButt->setPressAction([this, gw, boardPtr]() {
        const auto generateMenu = new eFramedWidget(window());
        generateMenu->setType(eFrameType::message);
        generateMenu->resize(width(), height());

        const auto res = resolution();
        const int p = res.largePadding();

        const auto iw = new eWidget(window());
        iw->setNoPadding();
        generateMenu->addWidget(iw);
        iw->move(p, p);

        // the size of the board: the current one, the sizes of the
        // original editor, and multiples of its largest map
        const auto sizeW = new eWidget(window());
        sizeW->setNoPadding();
        const auto sizeL = new eLabel(window());
        sizeL->setText(eLanguage::text("size"));
        sizeL->fitContent();
        sizeW->addWidget(sizeL);
        const auto sizeB = new eSwitchButton(window());
        sizeB->setUnderline(false);
        struct eSize { int fW; int fH; const char* fMult; };
        const std::vector<eSize> sizes = {
            {0, 0, ""},
            {42, 86, ""}, {56, 114, ""}, {70, 142, ""},
            {86, 172, ""}, {114, 228, ""},
            {162, 324, " (2x)"}, {228, 456, " (4x)"},
            {324, 648, " (8x)"}, {456, 912, " (16x)"}};
        for(const auto& sz : sizes) {
            if(sz.fW == 0) {
                sizeB->addValue(eLanguage::text("keep_size"));
            } else {
                sizeB->addValue(std::to_string(sz.fW) + " x " +
                                std::to_string(sz.fH) + sz.fMult);
            }
        }
        sizeB->fitValidContent();
        sizeB->setValue(0);
        sizeW->addWidget(sizeB);
        sizeW->stackHorizontally();
        sizeW->fitContent();
        iw->addWidget(sizeW);

        const auto editW = new eWidget(window());
        editW->setNoPadding();
        iw->addWidget(editW);

        const auto col1 = new eWidget(window());
        col1->setNoPadding();
        editW->addWidget(col1);

        const auto col2 = new eWidget(window());
        col2->setNoPadding();
        editW->addWidget(col2);

        using eNumLineEditPtr = eNumLineEdit*;
        const auto createW = [&](const std::string& name,
                                 const int value,
                                 eNumLineEditPtr& numEdit) {
            const auto w = new eWidget(window());
            w->setNoPadding();

            const auto label = new eLabel(window());
            label->setText(eLanguage::text(name));
            label->fitContent();
            w->addWidget(label);

            numEdit = new eNumLineEdit(window());
            numEdit->setRenderBg(true);
            numEdit->setRange(0, 999);
            numEdit->setValue(value);
            numEdit->fitContent();
            w->addWidget(numEdit);

            w->stackHorizontally();
            w->fitContent();
            return w;
        };

        using eMGS = eMapGeneratorSettings;
        const auto& last = eMGS::sLast;

        // shares of the map in percent
        eNumLineEdit* waterE;
        col1->addWidget(createW("water", last.fWater, waterE));
        eNumLineEdit* forestE;
        col1->addWidget(createW("forest", last.fForest, forestE));
        eNumLineEdit* fertileE;
        col1->addWidget(createW("fertile", last.fFertile, fertileE));
        eNumLineEdit* stonesE;
        col1->addWidget(createW("stones", last.fStones, stonesE));
        eNumLineEdit* elevationE;
        col1->addWidget(createW("elevation", last.fElevation, elevationE));
        eNumLineEdit* scrubE;
        col1->addWidget(createW("scrub", last.fScrub, scrubE));
        eNumLineEdit* seedE;
        col1->addWidget(createW("seed", last.fSeed, seedE));
        seedE->setRange(0, 999999);
        eNumLineEdit* cityE;
        col1->addWidget(createW("city", static_cast<int>(last.fCid), cityE));

        // counts per 10000 tiles
        const auto perL = new eLabel(window());
        perL->setText(eLanguage::text("per_26000_tiles"));
        perL->fitContent();
        col2->addWidget(perL);
        eNumLineEdit* lakesE;
        col2->addWidget(createW("lakes", last.fLakes, lakesE));
        eNumLineEdit* riversE;
        col2->addWidget(createW("rivers", last.fRivers, riversE));
        eNumLineEdit* copperE;
        col2->addWidget(createW("copper", last.fCopper, copperE));
        eNumLineEdit* silverE;
        col2->addWidget(createW("silver", last.fSilver, silverE));
        eNumLineEdit* marbleE;
        col2->addWidget(createW("marble", last.fMarble, marbleE));
        eNumLineEdit* orichalcE;
        col2->addWidget(createW("orichalc", last.fOrichalc, orichalcE));
        eNumLineEdit* animalsE;
        col2->addWidget(createW("animals", last.fAnimals, animalsE));
        eNumLineEdit* fishE;
        col2->addWidget(createW("fish_points", last.fFish, fishE));

        col1->stackVertically();
        col1->fitContent();
        col2->stackVertically();
        col2->fitContent();

        editW->stackHorizontally();
        editW->fitContent();

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();

        const auto accept = new eAcceptButton(window());
        const auto cancel = new eCancelButton(window());
        accept->setPressAction([generateMenu, boardPtr, gw,
                               sizeB, sizes,
                               waterE, forestE, fertileE, stonesE,
                               elevationE, scrubE, seedE, cityE,
                               lakesE, riversE, copperE, silverE,
                               marbleE, orichalcE, animalsE, fishE]() {
            const auto& sz = sizes[sizeB->currentValue()];
            if(sz.fW > 0 && (sz.fW != boardPtr->width() ||
                             sz.fH != boardPtr->height())) {
                boardPtr->resize(sz.fW, sz.fH);
                gw->updateViewBoxSize();
            }
            eMapGenerator g(*boardPtr);
            eMapGeneratorSettings sett;
            sett.fWater = waterE->value();
            sett.fForest = forestE->value();
            sett.fFertile = fertileE->value();
            sett.fStones = stonesE->value();
            sett.fElevation = elevationE->value();
            sett.fScrub = scrubE->value();
            sett.fSeed = seedE->value();
            sett.fCid = static_cast<eCityId>(cityE->value());
            sett.fLakes = lakesE->value();
            sett.fRivers = riversE->value();
            sett.fCopper = copperE->value();
            sett.fSilver = silverE->value();
            sett.fMarble = marbleE->value();
            sett.fOrichalc = orichalcE->value();
            sett.fAnimals = animalsE->value();
            sett.fFish = fishE->value();
            g.generate(sett);
            gw->updateTopBottomAltitude();
            gw->updateMinMaxAltitude();
            gw->updateMaps(true);
            generateMenu->deleteLater();
        });
        cancel->setPressAction([generateMenu]() {
            generateMenu->deleteLater();
        });

        buttonsW->addWidget(accept);
        buttonsW->addWidget(cancel);
        buttonsW->fitHeight();
        buttonsW->setWidth(editW->width());
        buttonsW->layoutHorizontallyWithoutSpaces();
        iw->addWidget(buttonsW);

        iw->stackVertically();
        iw->fitContent();
        generateMenu->resize(2*p + iw->width(), 2*p + iw->height());

        window()->execDialog(generateMenu);
        generateMenu->align(eAlignment::center);
    });
    addWidget(generateButt);
    generateButt->align(eAlignment::hcenter);

    const auto clearButt = new eFramedButton(window());
    clearButt->setUnderline(false);
    clearButt->setText(eLanguage::text("clear"));
    clearButt->fitContent();
    clearButt->setPressAction([gw, boardPtr]() {
        boardPtr->initialize(boardPtr->width(), boardPtr->height());
        gw->updateTopBottomAltitude();
        gw->updateMaps(true);
    });
    addWidget(clearButt);
    clearButt->align(eAlignment::hcenter);

    layoutVertically();
}
