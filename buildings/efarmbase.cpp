#include "efarmbase.h"

#include "textures/egametextures.h"
#include "enumbers.h"

#include <algorithm>
#include <cmath>

eFarmBase::eFarmBase(eGameBoard& board,
                     const eBuildingType type,
                     const int sw, const int sh,
                     const eResourceType resType,
                     const eCityId cid) :
    eResourceBuildingBase(board, type, sw, sh, 10, resType, cid),
    mTextures(eGameTextures::buildings())  {
    eGameTextures::loadPlantation();
}

std::shared_ptr<eTexture> eFarmBase::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return mTextures[sizeId].fPlantation;
}

std::vector<eOverlay> eFarmBase::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& texs = mTextures[sizeId];
    const std::pair<int, int> xy[5] = {{-1, -1},
                                       {0, -1},
                                       {1, -1},
                                       {1, -2},
                                       {1, -3}};
    const double e = effectiveness();
    const int usedFields = std::clamp((int)std::round(1 + e*4), 0, 5);
    for(int i = 0; i < 5; i++) {
        eOverlay& o = os.emplace_back();
        const auto& xxyy = xy[i];
        o.fX = xxyy.first;
        o.fY = xxyy.second;
        o.fAlignTop = true;
        const int texId = i >= usedFields ? 0 : std::clamp(mRipe, 0, 5);
        const auto type = resourceType();
        switch(type) {
        case eResourceType::onions:
            o.fTex = texs.fOnions.getTexture(texId);
            break;
        case eResourceType::carrots:
            o.fTex = texs.fCarrots.getTexture(texId);
            break;
        case eResourceType::wheat:
            o.fTex = texs.fWheat.getTexture(texId);
            break;
        default: break;
        }
    }
    return os;
}

void eFarmBase::timeChanged(const int by) {
    if(enabled()) {
        mNextRipe += by*effectiveness();
        if(mNextRipe > eNumbers::sFarmRipePeriod) {
            mNextRipe = 0;
            if(++mRipe == 5) {
                addProduced(resourceType(), 4);
                mRipe = 0;
            }
        }
    }
    eResourceBuildingBase::timeChanged(by);
}

void eFarmBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);

    src >> mNextRipe;
    src >> mRipe;
}

void eFarmBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);

    dst << mNextRipe;
    dst << mRipe;
}
