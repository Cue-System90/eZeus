#include "esoldierbanner.h"

#include <algorithm>

#include "engine/etile.h"
#include "engine/egameboard.h"
#include "characters/esoldier.h"
#include "characters/actions/esoldieraction.h"
#include "eiteratesquare.h"

#include "evectorhelpers.h"
#include "ewalkablehelpers.h"

#include "buildings/epalace.h"
#include "buildings/epalacetile.h"

#include "eiteratesquare.h"
#include "engine/epathfinder.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include <cmath>

int gNextId = 0;

eSoldierBanner::eSoldierBanner(const eBannerType type,
                               eGameBoard& board) :
    eObject(board),
    mType(type), mId(gNextId++), mBoard(board) {
    mBoard.registerAllSoldierBanner(this);
    const int nameId = mId % 30;
    auto name = eLanguage::zeusText(138, nameId);
    eStringHelpers::replaceAll(name, "&quot;", "\"");
    setName(name);
}

eSoldierBanner::~eSoldierBanner() {
    killAll();
    mBoard.unregisterAllSoldierBanner(this);
}

eCharacterType eSoldierBanner::characterType() const {
    if(atlantean()) {
        switch(mType) {
        case eBannerType::hoplite:
            return eCharacterType::hoplitePoseidon;
        case eBannerType::horseman:
            return eCharacterType::chariotPoseidon;
        case eBannerType::rockThrower:
            return eCharacterType::archerPoseidon;
        case eBannerType::amazon:
            return eCharacterType::amazon;
        case eBannerType::aresWarrior:
            return eCharacterType::aresWarrior;
        case eBannerType::enemy:
        case eBannerType::trireme:
            return eCharacterType::none;
        }
    } else {
        switch(mType) {
        case eBannerType::hoplite:
            return eCharacterType::hoplite;
        case eBannerType::horseman:
            return eCharacterType::horseman;
        case eBannerType::rockThrower:
            return eCharacterType::rockThrower;
        case eBannerType::amazon:
            return eCharacterType::amazon;
        case eBannerType::aresWarrior:
            return eCharacterType::aresWarrior;
        case eBannerType::enemy:
        case eBannerType::trireme:
            return eCharacterType::none;
        }
    }
    return eCharacterType::none;
}

stdsptr<eSoldier> eSoldierBanner::createSoldier(eTile* const t) {
    const auto ct = characterType();
    const auto c = eCharacter::sCreate(ct, mBoard);
    c->setCityId(cityId());
    c->setOnCityId(onCityId());
    const auto s = c->ref<eSoldier>();
    s->setBanner(this);
    const auto a = e::make_shared<eSoldierAction>(s.get());
    c->setAction(a);
    c->changeTile(t);
    c->setActionType(eCharacterActionType::stand);
    if(mHome) a->goHome();
    else a->goBackToBanner();
    return s;
}

void eSoldierBanner::moveTo(const int x, const int y) {
    const auto t = mBoard.tile(x, y);
    if(!t || t == mTile) return;

    const bool visible = visibleOnTile();

    if(visible && mTile) {
        mTile->setSoldierBanner(nullptr);
    }
    if(visible && t) {
        t->setSoldierBanner(this);
    }
    mTile = t;

    updatePlaces();
    if(!mHome) callSoldiers();
}

void eSoldierBanner::moveToDefault() {
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    switch(mType) {
    case eBannerType::rockThrower:
    case eBannerType::hoplite:
    case eBannerType::horseman: {
        const auto palace = mBoard.palace(cid);
        if(!palace) return;
        const auto ts = palace->tiles();
        for(const auto t : ts) {
            if(t->other()) continue;
            const auto tt = t->centerTile();
            if(!tt) continue;
            const auto bb = tt->soldierBanner();
            if(bb) continue;
            moveTo(tt->x(), tt->y());
            break;
        }
    } break;
    case eBannerType::amazon:
    case eBannerType::aresWarrior: {
        const auto s = mType == eBannerType::amazon ?
                            mBoard.sanctuary(cid, eGodType::artemis) :
                            mBoard.sanctuary(cid, eGodType::ares);
        if(!s) return;
        const auto ts = s->warriorTiles();
        for(const auto t : ts) {
            const auto bb = t->soldierBanner();
            if(bb) continue;
            moveTo(t->x(), t->y());
            break;
        }
    } break;
    case eBannerType::enemy:
    case eBannerType::trireme:
        break;
    }
}

void eSoldierBanner::goHome() {
    if(mAbroad && mMilitaryAid) {
        const auto onCid = onCityId();
        const auto c = mBoard.boardCityWithId(onCid);
        if(c) {
            const auto ptr = ref<eSoldierBanner>();
            c->reinforcementsGoHome(ptr);
        }
        goAbroad();
        mMilitaryAid = false;
        return;
    }
    if(mAbroad) return;
    if(mMilitaryAid) return;
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    if(mHome) return;
    mHome = true;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goHome();
    }
}

void eSoldierBanner::goAbroad() {
    if(mAbroad && !mMilitaryAid) return;
    if(mSelected) mBoard.deselectBanner(this);
    if(mHome) backFromHome();
    mAbroad = true;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goAbroad();
    }
    if(mTile) {
        mTile->setSoldierBanner(nullptr);
        mTile = nullptr;
    }
}

void eSoldierBanner::backFromAbroad(int& wait) {
    if(!mAbroad) return;
    if(mCount <= 0) {
        const auto tptr = ref<eSoldierBanner>();
        mBoard.unregisterSoldierBanner(tptr);
        return;
    }
    mAbroad = false;
    moveToDefault();
    const auto cid = cityId();
    const auto entryPoint = mBoard.entryPoint(cid);
    if(entryPoint) {
        while((int)mSoldiers.size() < mCount) {
            const auto s = createSoldier(entryPoint);
            const auto a = s->soldierAction();
            if(!a) continue;
            a->waitAndGoHome(wait);
            wait += 150;
        }
        mHome = true;
    } else {
        goHome();
    }
}

void eSoldierBanner::backFromHome() {
    if(mAbroad && !mMilitaryAid) return;
    if(!mHome) return;
    mHome = false;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goBackToBanner();
    }
    updateCount();
}

void eSoldierBanner::callSoldiers() {
    for(const auto s : mSoldiers) {
        callSoldier(s);
    }
}

bool eSoldierBanner::isGoingHome() const {
    if(!mHome) return false;
    return !mSoldiers.empty();
}

void eSoldierBanner::addSoldier(eSoldier* const s) {
    mSoldiers.push_back(s);
    updatePlaces();
    if(!mHome) callSoldier(s);
}

void eSoldierBanner::removeSoldier(eSoldier* const s) {
    mPlaces.erase(s);
    const bool r = eVectorHelpers::remove(mSoldiers, s);
    if(r) updatePlaces();
}

eTile* eSoldierBanner::place(eSoldier* const s) const {
    const auto it = mPlaces.find(s);
    if(it == mPlaces.end()) return nullptr;
    return it->second;
}

void eSoldierBanner::killAll() {
    for(const auto s : mSoldiers) {
        s->kill();
    }
    mSoldiers.clear();
}

void eSoldierBanner::killAllWithCorpse() {
    for(const auto s : mSoldiers) {
        s->killWithCorpse();
    }
    mSoldiers.clear();
}

void eSoldierBanner::setBothCityIds(const eCityId cid) {
    mCityId = cid;
    mOnCityId = cid;
}

bool eSoldierBanner::atlantean() const {
    return mBoard.atlantean(mCityId);
}

ePlayerId eSoldierBanner::playerId() const {
    const auto cid = cityId();
    auto& board = getBoard();
    return board.cityIdToPlayerId(cid);
}

eTeamId eSoldierBanner::teamId() const {
    const auto pid = playerId();
    auto& board = getBoard();
    return board.playerIdToTeamId(pid);
}

void eSoldierBanner::incCount() {
    mCount++;
    updateCount();
}

void eSoldierBanner::decCount() {
    if(mCount > 0) mCount--;
    updateCount();
}

bool eSoldierBanner::stationary() const {
    for(const auto s : mSoldiers) {
        const auto at = s->actionType();
        if(at != eCharacterActionType::stand) return false;
    }
    return true;
}

bool eSoldierBanner::fighting() const {
    for(const auto s : mSoldiers) {
        const auto at = s->actionType();
        if(at == eCharacterActionType::fight ||
           at == eCharacterActionType::fight2) return true;
    }
    return false;
}

void eSoldierBanner::read(eReadStream& src) {
    src >> mIOID;
    src >> mMilitaryAid;
    src >> mHome;
    src >> mAbroad;
    mTile = src.readTile(mBoard);
    src >> mCount;
    src >> mCityId;
    src >> mOnCityId;

    if(visibleOnTile() && mTile) {
        mTile->setSoldierBanner(this);
    }

    int np;
    src >> np;
    for(int i = 0; i < np; i++) {
        const auto t = src.readTile(mBoard);
        src.readCharacter(&mBoard, [this, t](eCharacter* const c) {
            const auto s = static_cast<eSoldier*>(c);
            mPlaces[s] = t;
        });
    }

    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        src.readCharacter(&mBoard, [this](eCharacter* const c) {
            const auto s = static_cast<eSoldier*>(c);
            mSoldiers.push_back(s);
        });
    }
}

void eSoldierBanner::write(eWriteStream& dst) const {
    dst << mIOID;
    dst << mMilitaryAid;
    dst << mHome;
    dst << mAbroad;
    dst.writeTile(mTile);
    dst << mCount;
    dst << mCityId;
    dst << mOnCityId;

    dst << mPlaces.size();
    for(const auto& p : mPlaces) {
        dst.writeTile(p.second);
        dst.writeCharacter(p.first);
    }

    dst << mSoldiers.size();
    for(const auto s : mSoldiers) {
        dst.writeCharacter(s);
    }
}

void eSoldierBanner::sPlaceDefault(std::vector<eSoldierBanner*>& bs,
                                   const int ctx, const int cty,
                                   eGameBoard& board) {
    if(bs.empty()) return;
    const auto bsFirst = bs[0];
    const auto cid = bsFirst->cityId();
    const auto onCid = bsFirst->onCityId();
    const auto tt = board.tile(ctx, cty);
    const auto ttCid = tt->cityId();
    if(tt && ttCid == onCid && ttCid == cid) {
        const auto b = tt->underBuilding();
        if(b) {
            const auto bt = b->type();
            if(bt == eBuildingType::palace ||
               bt == eBuildingType::palaceTile) {
                for(int i = 0; i < (int)bs.size(); i++) {
                    const auto bb = bs[i];
                    const auto bbt = bb->type();
                    if(bbt == eBannerType::hoplite ||
                       bbt == eBannerType::rockThrower ||
                       bbt == eBannerType::horseman) {
                        bb->moveToDefault();
                        eVectorHelpers::remove(bs, bb);
                        i--;
                    }
                }
            } else if(const auto sb = dynamic_cast<eSanctBuilding*>(b)) {
                const auto s = dynamic_cast<eSanctuary*>(sb->monument());
                if(!s) return;
                const auto gt = s->godType();
                for(int i = 0; i < (int)bs.size(); i++) {
                    const auto bb = bs[i];
                    const auto bbt = bb->type();
                    if((bbt == eBannerType::amazon && gt == eGodType::artemis) ||
                       (bbt == eBannerType::aresWarrior && gt == eGodType::ares)) {
                        bb->moveToDefault();
                        eVectorHelpers::remove(bs, bb);
                        i--;
                    }
                }
            }
        }
    }
}

void eSoldierBanner::sPlaceNoPathTrace(std::vector<eSoldierBanner*> bs,
                                       const int ctx, const int cty,
                                       eGameBoard& board, const int dist,
                                       const int minDistFromEdge) {
    sPlaceDefault(bs, ctx, cty, board);
    if(bs.empty()) return;

    const auto bsFirst = bs[0];
    const auto onCid = bsFirst->onCityId();
    const auto cid = bsFirst->cityId();
    const auto onTid = board.cityIdToTeamId(onCid);
    const auto tid = board.cityIdToTeamId(cid);
    const bool isEnemy = onTid != tid;

    int isld = 0;
    const int slds = bs.size();

    const int bw = board.width();
    const int bh = board.height();

    const auto prcsTile = [&](const int i, const int j) {
        if(isld >= slds) return false;
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
        if(!isEnemy && !tt->walkable()) return false;
        if(isEnemy && !tt->walkableTerrain()) return false;
        if(!isEnemy && tt->soldierBanner()) return false;

        const auto s = bs[isld++];
        s->moveTo(tx, ty);
        return false;
    };

    const int kinc = slds == 1 ? 1 : dist;
    for(int k = 0; isld < slds; k += kinc) {
        (void)isld;
        eIterateSquare::iterateSquare(k, prcsTile, dist);
    }
}

void eSoldierBanner::sPlace(std::vector<eSoldierBanner*> bs,
                            const int ctx, const int cty,
                            eGameBoard& board, const int dist,
                            const int minDistFromEdge) {
    sPlaceDefault(bs, ctx, cty, board);
    if(bs.empty()) return;

    eTile* startTile = nullptr;

    const auto bsFirst = bs[0];
    const auto onCid = bsFirst->onCityId();
    const auto cid = bsFirst->cityId();
    const auto onTid = board.cityIdToTeamId(onCid);
    const auto tid = board.cityIdToTeamId(cid);
    const bool isEnemy = onTid != tid;

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
        if(!isEnemy && !tt->walkable()) return false;
        if(isEnemy && !tt->walkableTerrain()) return false;
        if(!isEnemy && tt->soldierBanner()) return false;
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
            if(isEnemy) return t->walkableTerrain();
            else return t->walkable();
        }, [&](eTileBase* const t) {
            const bool walkable = isEnemy ? t->walkableTerrain() :
                                            t->walkable();
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
        b->moveTo(tx, ty);
    }
}

std::string eSoldierBanner::sName(
        const eBannerType type,
        const bool atlantean) {
    if(atlantean) {
        switch(type) {
        case eBannerType::hoplite:
            return eLanguage::zeusText(138, 79);
        case eBannerType::rockThrower:
            return eLanguage::zeusText(138, 77);
        case eBannerType::horseman:
            return eLanguage::zeusText(138, 80);
        case eBannerType::trireme:
            return eLanguage::zeusText(138, 76);
        default:
            break;
        }
    } else {
        switch(type) {
        case eBannerType::hoplite:
            return eLanguage::zeusText(138, 72);
        case eBannerType::rockThrower:
            return eLanguage::zeusText(138, 74);
        case eBannerType::horseman:
            return eLanguage::zeusText(138, 71);
        case eBannerType::trireme:
            return eLanguage::zeusText(138, 73);
        default:
            break;
        }
    }
    return "";
}

void eSoldierBanner::updatePlaces() {
    if(!mTile) return;
    purgeDead();
    if(mSoldiers.empty()) return;

    int isld = 0;
    const int slds = mSoldiers.size();

    const auto onCid = onCityId();
    const auto cid = cityId();
    const auto onTid = mBoard.cityIdToTeamId(onCid);
    const auto tid = mBoard.cityIdToTeamId(cid);

    const auto prcsTile = [&](const int i, const int j) {
        if(isld >= slds) return false;
        if(!mTile) return false;
        const int tx = mTile->x();
        const int ty = mTile->y();
        const auto tt = mBoard.tile(tx + i, ty + j);
        if(!tt) return false;
        const auto cid = tt->cityId();
        if(cid != onCid) return false;
        if(onTid == tid) {
            if(!eWalkableHelpers::sDefaultWalkable(tt)) return false;
        } else {
            if(!eWalkableHelpers::sBuildingsWalkable(tt)) return false;
        }

        const auto s = mSoldiers[isld++];
        mPlaces[s] = tt;
        return false;
    };

    for(int k = 0; isld < slds; k++) {
        (void)isld;
        eIterateSquare::iterateSquare(k, prcsTile);
    }
}

void eSoldierBanner::updateCount() {
    if(mMilitaryAid) return;
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    purgeDead();
    const int n = mSoldiers.size();
    if(!mHome && !mAbroad) {
        for(int i = n; i < mCount; i++) {
            eCharacterType cht;
            switch(mType) {
            case eBannerType::rockThrower:
                cht = eCharacterType::rockThrower;
                break;
            case eBannerType::hoplite:
                cht = eCharacterType::hoplite;
                break;
            case eBannerType::horseman:
                cht = eCharacterType::horseman;
                break;
            case eBannerType::amazon:
                cht = eCharacterType::amazon;
                break;
            case eBannerType::aresWarrior:
                cht = eCharacterType::aresWarrior;
                break;
            case eBannerType::enemy:
            case eBannerType::trireme:
                return;
            }
            const auto home = eSoldierAction::sFindHome(cht, cid, mBoard);
            if(!home) break;
            createSoldier(home->centerTile());
        }
    }

    while((int)mSoldiers.size() > std::max(0, mCount)) {
        const auto s = mSoldiers.back();
        const auto a = s->soldierAction();
        if(a) a->goHome();
        s->setBanner(nullptr);
    }
    const auto tptr = ref<eSoldierBanner>();
    if(mCount <= 0 && mSoldiers.size() == 0) {
        switch(mType) {
        case eBannerType::rockThrower:
        case eBannerType::hoplite:
        case eBannerType::horseman:
            mBoard.unregisterSoldierBanner(tptr);
            break;
        case eBannerType::amazon:
        case eBannerType::aresWarrior:
            break;
        case eBannerType::enemy:
        case eBannerType::trireme:
            break;
        }
        return;
    }
    updatePlaces();
}

void eSoldierBanner::callSoldier(eSoldier* const s) {
    if(s->dead()) return;
    const auto tt = place(s);
    if(!tt) return;

    const int ttx = tt->x();
    const int tty = tt->y();

    const auto a = s->soldierAction();
    if(a) {
        a->setOverwrittableAction(mType == eBannerType::enemy);
        a->goTo(ttx, tty);
    }
}

void eSoldierBanner::purgeDead() {
    for(int i = 0; i < (int)mSoldiers.size(); i++) {
        const auto s = mSoldiers[i];
        const bool dead = s->dead();
        if(dead) {
            removeSoldier(s);
            i--;
        }
    }
}

bool eSoldierBanner::nearestSoldier(const int fromX, const int fromY,
                                    int& toX, int& toY) const {
    bool found = false;
    int minDist = 99999;
    for(const auto& s : mSoldiers) {
        const auto t = s->tile();
        if(!t) continue;
        const int tx = t->x();
        const int ty = t->y();
        const int dx = fromX - tx;
        const int dy = fromY - ty;
        const int dist = sqrt(dx*dx + dy*dy);
        if(dist > minDist) continue;
        found = true;
        toX = tx;
        toY = ty;
        minDist = dist;
    }
    return found;
}

bool eSoldierBanner::visibleOnTile() const {
    const auto onCid = onCityId();
    const auto onPid = mBoard.cityIdToPlayerId(onCid);
    const auto ppid = mBoard.personPlayer();
    if(onPid != ppid) return false;
    const auto pid = playerId();
    if(pid == ppid) return true;
    return mMilitaryAid;
}

void eSoldierBanner::teleportSoldiersToPlaces() {
    for(const auto s : mSoldiers) {
        const auto tile = place(s);
        if(!tile) continue;
        s->changeTile(tile);
        const auto a = s->soldierAction();
        if(a) a->setCurrentAction(nullptr);
    }
}
