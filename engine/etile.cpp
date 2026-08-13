#include "etile.h"

#include <algorithm>

#include "buildings/ebuilding.h"
#include "buildings/eroad.h"
#include "characters/echaracter.h"
#include "egameboard.h"

#include "evectorhelpers.h"
#include "spawners/ebanner.h"

#include "eiteratesquare.h"
#include <cmath>

eTile::eTile(const int x, const int y,
             const int dx, const int dy,
             eGameBoard &board) :
    mBoard(board) {
    setSeed(eRand::rand());
    setX(x);
    setY(y);
    setDX(dx);
    setDY(dy);
}

void eTile::setScrub(const double s) {
    mScrub = std::clamp(s, 0., 1.);
}

void eTile::incScrub(const double s) {
    setScrub(mScrub + s);
}

int eTile::scrubId(const int nLevels) const {
    const double div = 1./nLevels;
    const int i = std::round(scrub()/div);
    return std::clamp(i, 0, nLevels);
}

void eTile::setAltitude(const int a, const bool update) {
    setDoubleAltitude(2*a, update);
}

void eTile::setDoubleAltitude(const int da, const bool update) {
    mDoubleAltitude = da;
    if(!update) return;
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            const auto t = tileRel<eTile>(dx, dy);
            if(!t) continue;
            t->updateIsElevationTile();
        }
    }
}

int eTile::characterDoubleAltitude() const {
    if(!mUnderBuilding) return mDoubleAltitude;
    const auto type = mUnderBuilding->type();
    if(type != eBuildingType::road) return mDoubleAltitude;
    const auto r = static_cast<eRoad*>(mUnderBuilding.get());
    return mDoubleAltitude + r->characterAltitude();
}

void eTile::updateIsElevationTile() {
    mHalfSlope = false;
    setElevation(false);

    bool wholeSlope = false;

    const int a = doubleAltitude();
    for(int x = -1; x < 2; x++) {
        for(int y = -1; y < 2; y++) {
            if(x == 0 && y == 0) continue;
            const auto t = tileRel<eTile>(x, y);
            if(!t) continue;
            const int ta = t->doubleAltitude();
            if(ta > a) {
                if(ta - a == 1) {
                    if(x == 0 || y == 0) {
                        mHalfSlope = !wholeSlope;
                    }
                } else {
                    wholeSlope = true;
                    mHalfSlope = false;
                }
                setElevation(true);
            }
        }
    }
}

std::vector<eTile*> eTile::surroundingRoads() const {
    std::vector<eTile*> tiles;
    for(int i = 0; i < 8; i += 2) {
        const auto o = static_cast<eOrientation>(i);
        const auto n = neighbour(o);
        const auto nt = static_cast<eTile*>(n);
        if(n && nt->hasRoad()) tiles.push_back(nt);
    }
    return tiles;
}

eTile* eTile::nearestRoad() const {
    const auto tiles = surroundingRoads();
    if(tiles.empty()) return nullptr;
    return tiles[eRand::rand() % tiles.size()];
}

int eTile::roadLength(const int skipAfter) const {
    std::vector<const eTile*> visited;
    int result = 0;
    std::function<void(const eTile* const)> process;
    process = [&](const eTile* const tile) {
        if(result >= skipAfter) return;
        if(!tile) return;
        if(!tile->hasRoad()) return;
        const bool v = eVectorHelpers::contains(visited, tile);
        if(v) return;
        result++;
        visited.push_back(tile);
        process(tile->topRight<eTile>());
        process(tile->bottomRight<eTile>());
        process(tile->bottomLeft<eTile>());
        process(tile->topLeft<eTile>());
    };
    process(this);
    return result;
}

void eTile::surroundingTerrain(eTerrain& tlTerr,
                               eTerrain& trTerr,
                               eTerrain& brTerr,
                               eTerrain& blTerr,
                               eTerrain& tTerr,
                               eTerrain& rTerr,
                               eTerrain& bTerr,
                               eTerrain& lTerr,
                               const eWorldDirection dir) const {
    tlTerr = terrain();
    trTerr = tlTerr;
    brTerr = tlTerr;
    blTerr = tlTerr;

    tTerr = tlTerr;
    rTerr = tlTerr;
    bTerr = tlTerr;
    lTerr = tlTerr;

    eTile* tr = nullptr;
    eTile* r = nullptr;
    eTile* br = nullptr;
    eTile* b = nullptr;
    eTile* bl = nullptr;
    eTile* l = nullptr;
    eTile* tl = nullptr;
    eTile* t = nullptr;
    rotatedNeighbours(tr, r, br, b, bl, l, tl, t, dir);

    {
        if(tl) {
            tlTerr = tl->terrain();
        }
    }
    {
        if(tr) {
            trTerr = tr->terrain();
        }
    }
    {
        if(br) {
            brTerr = br->terrain();
        }
    }
    {
        if(bl) {
            blTerr = bl->terrain();
        }
    }

    {
        if(t) {
            tTerr = t->terrain();
        }
    }

    {
        if(r) {
            rTerr = r->terrain();
        }
    }

    {
        if(b) {
            bTerr = b->terrain();
        }
    }

    {
        if(l) {
            lTerr = l->terrain();
        }
    }
}

void eTile::neighboursWithTerrain(const eTerrain terr,
                                  bool& tl, bool& tr,
                                  bool& br, bool& bl,
                                  bool& t, bool& r,
                                  bool& b, bool& l,
                                  const eWorldDirection dir) const {
    eTerrain tlTerr;
    eTerrain trTerr;
    eTerrain brTerr;
    eTerrain blTerr;

    eTerrain tTerr;
    eTerrain rTerr;
    eTerrain bTerr;
    eTerrain lTerr;

    surroundingTerrain(tlTerr, trTerr, brTerr, blTerr,
                       tTerr, rTerr, bTerr, lTerr, dir);

    tl = tlTerr != terr;
    tr = trTerr != terr;
    br = brTerr != terr;
    bl = blTerr != terr;

    t = tTerr != terr;
    r = rTerr != terr;
    b = bTerr != terr;
    l = lTerr != terr;
}

void eTile::rotatedNeighbours(eTilePtr& tr,
                              eTilePtr& r,
                              eTilePtr& br,
                              eTilePtr& b,
                              eTilePtr& bl,
                              eTilePtr& l,
                              eTilePtr& tl,
                              eTilePtr& t,
                              const eWorldDirection dir) const {
    if(dir == eWorldDirection::N) {
        tr = topRight<eTile>();
        r = right<eTile>();
        br = bottomRight<eTile>();
        b = bottom<eTile>();
        bl = bottomLeft<eTile>();
        l = left<eTile>();
        tl = topLeft<eTile>();
        t = top<eTile>();
    } else if(dir == eWorldDirection::E) {
        tr = bottomRight<eTile>();
        r = bottom<eTile>();
        br = bottomLeft<eTile>();
        b = left<eTile>();
        bl = topLeft<eTile>();
        l = top<eTile>();
        tl = topRight<eTile>();
        t = right<eTile>();
    } else if(dir == eWorldDirection::S) {
        tr = bottomLeft<eTile>();
        r = left<eTile>();
        br = topLeft<eTile>();
        b = top<eTile>();
        bl = topRight<eTile>();
        l = right<eTile>();
        tl = bottomRight<eTile>();
        t = bottom<eTile>();
    } else if(dir == eWorldDirection::W) {
        tr = topLeft<eTile>();
        r = top<eTile>();
        br = topRight<eTile>();
        b = right<eTile>();
        bl = bottomRight<eTile>();
        l = bottom<eTile>();
        tl = bottomLeft<eTile>();
        t = left<eTile>();
    }
}

void eTile::updateTerritoryBorder() {
    const auto cid = cityId();
    const auto set = [cid](const eTile* const other, bool& border) {
        if(!other) {
            border = false;
            return;
        }
        const auto ocid = other->cityId();
        border = ocid != cid;
    };

    set(topRight<eTile>(), mBorder.fTR);
    set(bottomRight<eTile>(), mBorder.fBR);
    set(bottomLeft<eTile>(), mBorder.fBL);
    set(topLeft<eTile>(), mBorder.fTL);

    set(top<eTile>(), mBorder.fT);
    set(bottom<eTile>(), mBorder.fB);
    set(left<eTile>(), mBorder.fL);
    set(right<eTile>(), mBorder.fR);

    mDistanceToBorder = sMaxDistanceToBorder;
    bool found = false;
    for(int k = 0; k < sMaxDistanceToBorder; k++) {
        const auto prcs = [&](const int dx, const int dy) {
            const auto t = tileRel(dx, dy);
            if(!t) return false;
            const auto cid = t->cityId();
            if(cid != eCityId::neutralFriendly) {
                mDistanceToBorder = k;
                found = true;
                return true;
            }
            return false;
        };
        eIterateSquare::iterateSquare(k, prcs);
        if(found) break;
    }
}

void eTile::scheduleNeighboursTerrainUpdate(const int range) {
    for(int dx = -range; dx <= range; dx++) {
        for(int dy = -range; dy <= range; dy++) {
            const auto t = tileRel<eTile>(dx, dy);
            if(!t) continue;
            t->scheduleTerrainUpdate();
        }
    }
}

void eTile::setUnderTile(eTile* const tile,
                         const int dx, const int dy) {
    mUnderTile = tile;
    mUnderTileDX = dx;
    mUnderTileDY = dy;
}

bool eTile::onFire() const {
    const auto ub = underBuilding();
    if(!ub) return false;
    return ub->isOnFire();
}

void eTile::setOnFire(const bool f) {
    const auto ub = underBuilding();
    if(!ub) return;
    ub->setOnFire(f);
}

void eTile::setTerrain(const eTerrain terr) {
    eTileBase::setTerrain(terr);
    scheduleTerrainUpdate();
}

void eTile::setMarbleLevel(const int l) {
    eTileBase::setMarbleLevel(l);
    scheduleNeighboursTerrainUpdate();
}

void eTile::read(eReadStream& src) {
    eTileBase::read(src);
    src >> mDoubleAltitude;
    src >> mScrub;

    unsigned char bools;
    src >> bools;
    mTidalWaveZone = bools & 1 << 0;
    mLavaZone = bools & 1 << 1;
    mLandSlideZone = bools & 1 << 2;
    mRainforest = bools & 1 << 3;
    mHalfSlope = bools & 1 << 4;

    unsigned char nb;
    src >> nb;
    for(unsigned char i = 0; i < nb; i++) {
        eBannerTypeS type;
        src >> type;
        int id;
        src >> id;
        const auto b = eBanner::sCreate(id, this, mBoard, type);
        b->read(src);
    }
}

void eTile::write(eWriteStream& dst) const {
    eTileBase::write(dst);
    dst << mDoubleAltitude;
    dst << mScrub;

    unsigned char bools = 0;
    if(mTidalWaveZone) bools |= 1 << 0;
    if(mLavaZone) bools |= 1 << 1;
    if(mLandSlideZone) bools |= 1 << 2;
    if(mRainforest) bools |= 1 << 3;
    if(mHalfSlope) bools |= 1 << 4;
    dst << bools;

    const unsigned char nb = mBanners.size();
    dst << nb;
    for(const auto& b : mBanners) {
        dst << b->type();
        dst << b->id();
        b->write(dst);
    }
}

void eTile::addCharacter(const stdsptr<eCharacter>& c,
                         const bool prepend) {
    if(prepend) {
        mCharacters.insert(mCharacters.begin(), c);
    } else {
        mCharacters.push_back(c);
    }
}

bool eTile::removeCharacter(const stdsptr<eCharacter>& c) {
    return eVectorHelpers::remove(mCharacters, c);
}

void eTile::addMissile(const stdsptr<eMissile>& m) {
    mMissiles.push_back(m);
}

bool eTile::removeMissile(const stdsptr<eMissile>& m) {
    return eVectorHelpers::remove(mMissiles, m);
}

bool eTile::hasCharacter(const eHasChar& func) const {
    for(const auto& c : mCharacters) {
        if(func(*c)) return true;
    }
    return false;
}

bool eTile::hasRoad() const {
    if(!mUnderBuilding) return false;
    const auto t = mUnderBuilding->type();
    return t == eBuildingType::road;
}

bool eTile::hasAvenue() const {
    if(!mUnderBuilding) return false;
    const auto t = mUnderBuilding->type();
    return t == eBuildingType::avenue;
}

void eTile::setUnderBuilding(const stdsptr<eBuilding>& b) {
    mUnderBuilding = b;
    scheduleTerrainUpdate();
}

eBuildingType eTile::underBuildingType() const {
    if(!mUnderBuilding) return eBuildingType::none;
    else return mUnderBuilding->type();
}

void eTile::addBanner(const stdsptr<eBanner>& b) {
    mBanners.push_back(b);
}

void eTile::removeBanner(const stdsptr<eBanner>& b) {
    eVectorHelpers::remove(mBanners, b);
}

void eTile::removeAllBanners() {
    mBanners.clear();
}

void eTile::removeBanner(eBanner * const b) {
    const int iMax = mBanners.size();
    for(int i = 0; i < iMax; i++) {
        const auto& bb = mBanners[i];
        if(bb.get() == b) {
            mBanners.erase(mBanners.begin() + i);
            break;
        }
    }
}

bool eTile::hasPrey() const {
    for(const auto& b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::boar ||
           type == eBannerTypeS::deer) {
            return true;;
        }
    }
    return false;
}

void eTile::setSoldierBanner(eSoldierBanner* const b) {
    mSoldierBanner = b;
}

void eTile::setDrawDim(const int drawDim) {
    mTerrainPainter.fDrawDim = drawDim;
}
