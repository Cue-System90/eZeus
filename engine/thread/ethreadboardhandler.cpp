#include "ethreadboardhandler.h"

#include "engine/egameboard.h"
#include "buildings/ehousebase.h"
#include "buildings/eagorabase.h"

eThreadBoardHandler::eThreadBoardHandler() {}

void eThreadBoardHandler::initialize(const int w, const int h) {
    mBoard.initialize(w, h);
    mTmpBoard.initialize(w, h);
}

void eThreadBoardHandler::updateAll(eGameBoard& board, const eBoardCity& c) {
    //    using std::chrono::high_resolution_clock;
//    using std::chrono::duration_cast;
//    using std::chrono::duration;
//    using std::chrono::milliseconds;

//    const auto t1 = high_resolution_clock::now();

//    std::printf("Update tmp board %p to %d\n", this, board.state());

    std::lock_guard l(mTmpBoardMutex);
    mTmpChanged = true;
    for(int i = 0; i < board.width(); i++) {
        for(int j = 0; j < board.height(); j++) {
            const auto src = board.dtile(i, j);
            const auto dst = mTmpBoard.dtile(i, j);
            if(!src || !dst) continue;
            dst->loadInitial(src);
        }
    }
    mTmpBoard.setState(board.state());
    updateFinishedMonuments(c);
    updateAgoras(c);

    const int bState = board.state();
    const int cABState = c.allBuildingsState();
    const int cTState = c.terrainState();
    const int cSState = c.sanctuariesState();
    const int cMState = c.monumentsState();

    mTmpBoard.setTerrainState(cTState);
    mTmpBoard.setForestsState(bState);
    mTmpBoard.setAllBuildingsState(cABState);
    mTmpBoard.setResourcesInBuildingsState(bState);
    mTmpBoard.setHouseVacanciesState(bState);
    mTmpBoard.setTreesAndVinesState(bState);
    mTmpBoard.setDomesticatedAnimalsState(bState);
    mTmpBoard.setSanctuariesState(cSState);
    mTmpBoard.setResourcesState(bState);
    mTmpBoard.setSanctBuildingsState(bState);
    mTmpBoard.setHuntingGroundsState(bState);
    mTmpBoard.setMonumentsState(cMState);

//    const auto t2 = high_resolution_clock::now();

//    const duration<double, std::milli> ms = t2 - t1;
    //    printf("update board: %f ms\n", ms.count());
}

void eThreadBoardHandler::updateFinishedMonuments(const eBoardCity& c) {
    auto& fms = mTmpBoard.finishedMonuments();
    fms.clear();
    const auto& ms = c.monuments();
    for(const auto m : ms) {
        const bool f = m->finished();
        if(!f) continue;
        auto& b = fms.emplace_back();
        b.load(m);
    }
}

void eThreadBoardHandler::updateAgoras(const eBoardCity& c) {
    auto& as = mTmpBoard.agoras();
    as.clear();
    for(const auto a : c.agoras()) {
        auto& b = as.emplace_back();
        b.load(a);
    }
}

void eThreadBoardHandler::update(eGameBoard& board, const eCityId cid,
                                 const eStateRelevance rel) {
    const auto c = board.boardCityWithId(cid);

//    std::printf("Update tmp board %p to %d\n", this, board.state());
    if(!mInitialized) {
        updateAll(board, *c);
        updateBoard();
        updateAll(board, *c);
        mInitialized = true;
        return;
    }

    const int bState = board.state();
    if(mTmpBoard.state() == bState) return;

    std::lock_guard l(mTmpBoardMutex);
    mTmpChanged = true;

    const int cABState = c->allBuildingsState();
    const int cTState = c->terrainState();
    const int cSState = c->sanctuariesState();
    const int cMState = c->monumentsState();
    const int tABState = mTmpBoard.allBuildingsState();
    const int tTState = mTmpBoard.terrainState();
    const int tSState = mTmpBoard.sanctuariesState();
    const int tMState = mTmpBoard.monumentsState();

    if(cMState != tMState) {
        updateFinishedMonuments(*c);
        mTmpBoard.setMonumentsState(cMState);
    }
    if(rel == eStateRelevance::all ||
       (static_cast<bool>(rel & eStateRelevance::buildings) &&
        cABState != tABState) ||
       (static_cast<bool>(rel & eStateRelevance::terrain) &&
        cTState != tTState)) {
        const auto& tiles = c->tiles();
        if(cSState != tSState) updateSanctuary(tiles);
        else update(tiles);
        updateAgoras(*c);
        mTmpBoard.setState(bState);
        mTmpBoard.setTerrainState(cTState);
        mTmpBoard.setForestsState(bState);
        mTmpBoard.setAllBuildingsState(cABState);
        mTmpBoard.setResourcesInBuildingsState(bState);
        mTmpBoard.setHouseVacanciesState(bState);
        mTmpBoard.setTreesAndVinesState(bState);
        mTmpBoard.setDomesticatedAnimalsState(bState);
        mTmpBoard.setSanctuariesState(cSState);
        mTmpBoard.setResourcesState(bState);
        mTmpBoard.setSanctBuildingsState(bState);
        mTmpBoard.setHuntingGroundsState(bState);
    } else {
        if(static_cast<bool>(rel & eStateRelevance::resourcesInBuildings) &&
           bState != mTmpBoard.resourcesInBuildingsState()) {
            const auto& bs = c->buildingsWithResource();
            for(const auto b : bs) {
                const auto& tiles = b->tilesUnder();
                update(tiles);
            }
            mTmpBoard.setResourcesInBuildingsState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::houseVacancies) &&
           bState != mTmpBoard.houseVacanciesState()) {
            const auto& bs = c->houses();
            for(const auto b : bs) {
                const auto& tiles = b->tilesUnder();
                update(tiles);
            }
            mTmpBoard.setHouseVacanciesState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::treesAndVines) &&
           bState != mTmpBoard.treesAndVinesState()) {
            const auto& bs = c->treesAndVines();
            for(const auto b : bs) {
                const auto& tiles = b->tilesUnder();
                update(tiles);
            }
            mTmpBoard.setTreesAndVinesState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::domesticatedAnimals) &&
           bState != mTmpBoard.domesticatedAnimalsState()) {
            const auto& tiles = c->animalBuildingsTiles();
            update(tiles);
            mTmpBoard.setDomesticatedAnimalsState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::huntingGrounds) &&
           bState != mTmpBoard.huntingGroundsState()) {
            const auto& tiles = c->huntingTiles();
            update(tiles);
            mTmpBoard.setHuntingGroundsState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::resources) &&
           bState != mTmpBoard.resourcesState()) {
            const auto& tiles = c->resourceTiles();
            update(tiles);
            mTmpBoard.setResourcesState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::forests) &&
           bState != mTmpBoard.forestsState()) {
            const auto& tiles = c->forestTiles();
            update(tiles);
            mTmpBoard.setForestsState(bState);
        }

        if(static_cast<bool>(rel & eStateRelevance::sanctBuildings) &&
           bState != mTmpBoard.sanctBuildingsState()) {
            const auto& bs = c->sanctBuildings();
            for(const auto b : bs) {
                const auto& tiles = b->tilesUnder();
                update(tiles);
            }
            mTmpBoard.setSanctBuildingsState(bState);
        }
    }
}

void eThreadBoardHandler::updateBoard() {
    if(mTmpChanged) {
//        std::printf("Swap boards %p from %d to %d\n", this, mBoard.state(), mTmpBoard.state());
        std::lock_guard l(mTmpBoardMutex);
        std::swap(mBoard, mTmpBoard);
        mTmpChanged = false;
    }
}

void eThreadBoardHandler::update(const std::vector<eTile*>& tiles) {
    for(const auto src : tiles) {
        const int dx = src->dx();
        const int dy = src->dy();
        const auto dst = mTmpBoard.dtile(dx, dy);
        if(!dst) continue;
        dst->load(src);
    }
}

void eThreadBoardHandler::updateSanctuary(const std::vector<eTile*>& tiles) {
    for(const auto src : tiles) {
        const int dx = src->dx();
        const int dy = src->dy();
        const auto dst = mTmpBoard.dtile(dx, dy);
        if(!dst) continue;
        dst->loadSanctuary(src);
    }
}
