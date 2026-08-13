#include "emonument.h"

#include "engine/egameboard.h"

#include <algorithm>
#include <cmath>

eMonument::eMonument(eGameBoard& board,
                       const eBuildingType type,
                       const int sw, const int sh,
                       const int maxEmployees,
                       const eCityId cid) :
    eEmployingBuilding(board, type, sw, sh,
                       maxEmployees, cid) {
    setStashable(eResourceType::marble |
                 eResourceType::wood |
                 eResourceType::sculpture |
                 eResourceType::orichalc |
                 eResourceType::blackMarble);
    board.registerMonument(this);
}

eMonument::~eMonument() {
    if(mCart) mCart->kill();
    auto& board = getBoard();
    board.unregisterMonument(this);
    board.destroyed(cityId(), type());
}

void eMonument::erase() {
    const auto& board = getBoard();
    const auto rect = tileRect();
    for(int x = rect.x; x < rect.x + rect.w; x++) {
        for(int y = rect.y; y < rect.y + rect.h; y++) {
            const auto tile = board.tile(x, y);
            tile->setAltitude(mAltitude);
            tile->setWalkableElev(false);
            const auto ub = tile->underBuilding();
            if(ub && ub != this && !dynamic_cast<eSanctBuilding*>(ub)) {
                ub->erase();
            }
            const auto t = tile->terrain();
            const bool r = static_cast<bool>(t & eTerrain::stones);
            if(r) tile->setTerrain(eTerrain::dry);
        }
    }
    for(const auto& e : mElements) {
        e->eBuilding::erase();
    }
    eBuilding::erase();
}

int eMonument::spaceLeft(const eResourceType type) const {
    const auto c = cost();
    if(type == eResourceType::marble) {
        return c.fMarble - mStored.fMarble - mUsed.fMarble;
    } else if(type == eResourceType::wood) {
        return c.fWood - mStored.fWood - mUsed.fWood;
    } else if(type == eResourceType::sculpture) {
        return c.fSculpture - mStored.fSculpture - mUsed.fSculpture;
    } else if(type == eResourceType::orichalc) {
        return c.fOrichalc - mStored.fOrichalc - mUsed.fOrichalc;
    } else if(type == eResourceType::blackMarble) {
        return c.fBlackMarble - mStored.fBlackMarble - mUsed.fBlackMarble;
    }
    return 0;
}

int eMonument::add(const eResourceType type, const int count) {
    const int space = spaceLeft(type);
    const int add = std::min(count, space);
    if(type == eResourceType::marble) {
        mStored.fMarble += add;
    } else if(type == eResourceType::wood) {
        mStored.fWood += add;
    } else if(type == eResourceType::sculpture) {
        mStored.fSculpture += add;
    } else if(type == eResourceType::orichalc) {
        mStored.fOrichalc += add;
    } else if(type == eResourceType::blackMarble) {
        mStored.fBlackMarble += add;
    }
    return add;
}


std::vector<eCartTask> eMonument::cartTasks() const {
    if(mHaltConstruction) return {};
    std::vector<eCartTask> tasks;

    const int m = spaceLeft(eResourceType::marble);
    const int w = spaceLeft(eResourceType::wood);
    const int s = spaceLeft(eResourceType::sculpture);
    const int o = spaceLeft(eResourceType::orichalc);
    const int bm = spaceLeft(eResourceType::blackMarble);

    if(m > 0) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::marble;
        task.fMaxCount = m;
        tasks.push_back(task);
    }

    if(w > 0) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::wood;
        task.fMaxCount = w;
        tasks.push_back(task);
    }

    if(s > 0) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::sculpture;
        task.fMaxCount = s;
        tasks.push_back(task);
    }

    if(o > 0) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::orichalc;
        task.fMaxCount = o;
        tasks.push_back(task);
    }

    if(bm > 0) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::blackMarble;
        task.fMaxCount = bm;
        tasks.push_back(task);
    }

    return tasks;
}

void eMonument::read(eReadStream& src) {
    eEmployingBuilding::read(src);

    src >> mRotated;

    src >> mHaltConstruction;

    mStored.read(src);
    mUsed.read(src);

    src >> mAltitude;

    auto& board = getBoard();

    src.readCharacter(&board, [this](eCharacter* const c) {
        mCart = static_cast<eCartTransporter*>(c);
    });
}

void eMonument::write(eWriteStream& dst) const {
    eEmployingBuilding::write(dst);

    dst << mRotated;

    dst << mHaltConstruction;

    mStored.write(dst);
    mUsed.write(dst);

    dst << mAltitude;

    dst.writeCharacter(mCart);
}

eSanctCost eMonument::cost() const {
    eSanctCost c{0, 0, 0};
    for(const auto& e : mElements) {
        c += e->cost();
    }
    return c;
}

void eMonument::useResources(const eSanctCost& r) {
    mStored -= r;
    mUsed += r;
}

void eMonument::setConstructionHalted(const bool h) {
    mHaltConstruction = h;
}
void eMonument::timeChanged(const int by) {
    if(!mCart) mCart = spawnCart(eCartActionTypeSupport::take);
    eEmployingBuilding::timeChanged(by);
}

void eMonument::registerElement(const stdsptr<eSanctBuilding>& e) {
    mElements.push_back(e);
}

int eMonument::progress() const {
    double did = 0;
    double max = 0;
    for(const auto& e : mElements) {
        did += e->progress();
        max += e->maxProgress();
    }
    return std::round(100*did/max);
}

bool eMonument::finished() const {
    return progress() >= 100;
}
