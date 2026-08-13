#include "etrireme.h"

#include "textures/egametextures.h"

#include "buildings/etriremewharf.h"
#include "actions/efightingaction.h"
#include "actions/etriremeaction.h"
#include "engine/egameboard.h"
#include "eiteratesquare.h"
#include "engine/epathfinder.h"
#include <cmath>

eTrireme::eTrireme(eGameBoard& board) :
    eBoatBase(board, &eCharacterTextures::fTrireme,
              eCharacterType::trireme),
    eFightingCharacter(this) {
    setRange(5);
    eGameTextures::loadTrireme();
    setHasSecondaryTexture(true);
}

void eTrireme::sPlace(std::vector<eTrireme*> bs,
                      const int ctx, const int cty,
                      eGameBoard& board, const int dist,
                      const int minDistFromEdge) {
    if(bs.empty()) return;

    eTile* startTile = nullptr;

    const auto bsFirst = bs[0];
    const auto onCid = bsFirst->onCityId();
    // const auto cid = bsFirst->cityId();
    // const auto onTid = board.cityIdToTeamId(onCid);
    // const auto tid = board.cityIdToTeamId(cid);
    // const bool isEnemy = onTid != tid;

    const int bw = board.width();
    const int bh = board.height();

    const auto prcsTile = [&](const int i, const int j) {
        const int tx = ctx + i;
        const int ty = cty + j;
        const auto tt = board.tile(tx, ty);
        if(!tt) return false;
        const auto ttCid = tt->cityId();
        if(ttCid != onCid) return false;
        const int dtx = tt->dx();
        const int dty = tt->dy();
        if(dtx < minDistFromEdge) return false;
        if(dty < 2*minDistFromEdge) return false;
        if(dtx > bw - minDistFromEdge) return false;
        if(dty > bh - 2*minDistFromEdge) return false;
        if(!tt->hasDeepWater()) return false;
        startTile = tt;
        return true;
    };

    for(int k = 0; k < 9; k++) {
        eIterateSquare::iterateSquare(k, prcsTile, dist);
        if(startTile) break;
    }

    if(!startTile) return;

    const auto rect = board.boardCityTileBRect(onCid);
    std::vector<SDL_Point> placed;
    for(const auto b : bs) {
        eTile* final = nullptr;
        ePathFinder p([&](eTileBase* const t) {
            return t->hasDeepWater();
        }, [&](eTileBase* const t) {
            const bool walkable = t->hasDeepWater();
            if(!walkable) return false;

            const auto tt = static_cast<eTile*>(t);
            const int tx = tt->x();
            const int ty = tt->y();
            for(const auto& p : placed) {
                const double d = std::sqrt((tx - p.x)*(tx - p.x) +
                                           (ty - p.y)*(ty - p.y));
                if(d < dist) return false;
            }
            final = tt;
            return true;
        });
        const int w = board.width();
        const int h = board.height();
        const bool r = p.findPath(rect, startTile, 100, true, w, h);
        if(!r) break;
        const int tx = final->x();
        const int ty = final->y();
        placed.push_back({tx, ty});
        const auto fa = b->fightingAction();
        if(fa) fa->goTo(tx, ty);
    }
}

eTriremeAction *eTrireme::triremeAction() const {
    const auto a = action();
    return dynamic_cast<eTriremeAction*>(a);
}

eOverlay eTrireme::getSecondaryTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = eGameTextures::characters();
    const auto& colls = texs[id];
    const auto a = actionType();
    const bool die = a == eCharacterActionType::die;
    const std::vector<eTextureCollection>* charTexs;
    if(a == eCharacterActionType::die) {
        charTexs = &colls.fTriremeDieOverlay;
    } else if(a == eCharacterActionType::fight ||
              a == eCharacterActionType::fight2) {
        charTexs = &colls.fTriremeFightOverlay;
    } else {
        charTexs = &colls.fTriremeOverlay;
    }
    const int oid = static_cast<int>(rotatedOrientation());
    const auto coll = &(*charTexs)[oid];
    const bool wrap = !die;
    const bool disappear = die;
    const auto tex = eCharacter::getTexture(coll, wrap, disappear);
    return eOverlay{0., 0., tex, false};
}

bool eTrireme::selectable() const {
    const auto ta = triremeAction();
    if(!ta) return false;
    const auto h = ta->home();
    if(!h) return false;
    return !h->abroad() && !h->shutDown();
}

void eTrireme::goHome() {
    const auto fa = fightingAction();
    if(!fa) return;
    fa->goHome();
}
