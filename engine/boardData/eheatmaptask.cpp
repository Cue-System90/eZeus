#include "eheatmaptask.h"

#include "../thread/ethreadboard.h"

eHeatMapTask::eHeatMapTask(const eCityId cid,
                           const SDL_Rect& bRect,
                           const eHeatGetter& heatGetter,
                           const eFunc& finish) :
    eTask(cid), mHeatGetter(heatGetter), mFinish(finish),
    mBRect(bRect) {
    setRelevance(eStateRelevance::buildings);
}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const eCityId cid,
                        const SDL_Rect& bRect,
                        const eHeatGetter& heatGetter,
                        eHeatMap& map) {
    map.initialize(bRect.x, bRect.y, bRect.w, bRect.h);
    for(int tx = bRect.x; tx < bRect.x + bRect.w; tx++) {
        for(int ty = bRect.y; ty < bRect.y + bRect.h; ty++) {
            const auto t = board.dtile(tx, ty);
            if(t->cityId() != cid) map.setOutsideRange(tx, ty);
            const auto& ub = t->underBuilding();
            const auto ubt = ub.type();
            switch(ubt) {
            case eBuildingType::none:

            case eBuildingType::templeAphrodite:
            case eBuildingType::templeApollo:
            case eBuildingType::templeAres:
            case eBuildingType::templeArtemis:
            case eBuildingType::templeAthena:
            case eBuildingType::templeAtlas:
            case eBuildingType::templeDemeter:
            case eBuildingType::templeDionysus:
            case eBuildingType::templeHades:
            case eBuildingType::templeHephaestus:
            case eBuildingType::templeHera:
            case eBuildingType::templeHermes:
            case eBuildingType::templePoseidon:
            case eBuildingType::templeZeus:

            case eBuildingType::modestPyramid: // 3x3
            case eBuildingType::pyramid: // 5x5
            case eBuildingType::greatPyramid: // 7x7
            case eBuildingType::majesticPyramid: // 9x9

            case eBuildingType::smallMonumentToTheSky: // 5x5
            case eBuildingType::monumentToTheSky: // 6x6
            case eBuildingType::grandMonumentToTheSky: // 8x8

            case eBuildingType::minorShrineAphrodite: // 3x3
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

            case eBuildingType::shrineAphrodite: // 6x6
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

            case eBuildingType::majorShrineAphrodite: // 8x8
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

            case eBuildingType::pyramidOfThePantheon: // 11x9
            case eBuildingType::altarOfOlympus: // 8x8
            case eBuildingType::templeOfOlympus: // 8x8
            case eBuildingType::observatoryKosmika: // 9x9
            case eBuildingType::museumAtlantika: // 8x8
                continue;
            default:
                break;
            }
            const auto& rect = ub.tileRect();
            const int ttx = t->x();
            const int tty = t->y();
            if(ttx != rect.x || tty != rect.y) continue;
            const auto a = heatGetter(ubt);
            map.addHeat(a, rect);
        }
    }

    const auto& fms = board.finishedMonuments();
    for(const auto& m : fms) {
        const auto ubt = m.type();
        const auto& rect = m.tileRect();
        const auto a = heatGetter(ubt);
        map.addHeat(a, rect);
    }
    for(const auto& ag : board.agoras()) {
        map.addHeat(heatGetter(ag.type()), ag.tileRect());
    }
}

void eHeatMapTask::run(eThreadBoard& board) {
    sRun(board, cid(), mBRect, mHeatGetter, mMap);
}

void eHeatMapTask::finish() {
    if(mFinish) mFinish(mMap);
}
