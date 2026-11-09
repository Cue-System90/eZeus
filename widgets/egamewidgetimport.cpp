#include "egamewidget.h"

#include "engine/egameboard.h"
#include "buildings/allbuildings.h"
#include "pak/zeusfile.h"
#include "pak/zeussavefile.h"

#include <algorithm>
#include <climits>
#include <map>
#include <set>

namespace {

// The original game numbers its building types like the building names
// in Zeus_Text group 28, see eBuilding::sNameForBuilding
eBuildingMode sModeForType(const int type) {
    switch(type) {
    case 15: return eBuildingMode::flowerGarden;
    case 16: return eBuildingMode::gazebo;
    case 17: return eBuildingMode::hedgeMaze;
    case 18: return eBuildingMode::fishPond;
    case 19: return eBuildingMode::tallObelisk;
    case 20: return eBuildingMode::sundial;
    case 21: return eBuildingMode::topiary;
    case 22: return eBuildingMode::spring;
    case 23: return eBuildingMode::stoneCircle;
    case 24: return eBuildingMode::shortObelisk;
    case 25: return eBuildingMode::waterPark;
    case 31: return eBuildingMode::wheatFarm;
    case 32: return eBuildingMode::onionFarm;
    case 33: return eBuildingMode::carrotFarm;
    case 37: return eBuildingMode::growersLodge;
    case 41: return eBuildingMode::cardingShed;
    case 42: return eBuildingMode::dairy;
    case 44: return eBuildingMode::fishery;
    case 45: return eBuildingMode::urchinQuay;
    case 46: return eBuildingMode::huntingLodge;
    case 48: return eBuildingMode::mint;
    case 49: return eBuildingMode::masonryShop;
    case 50: return eBuildingMode::foundry;
    case 51: return eBuildingMode::timberMill;
    case 53: return eBuildingMode::winery;
    case 54: return eBuildingMode::olivePress;
    case 55: return eBuildingMode::sculptureStudio;
    case 56: return eBuildingMode::artisansGuild;
    case 57: return eBuildingMode::granary;
    case 58: return eBuildingMode::warehouse;
    case 63: return eBuildingMode::commonAgora;
    case 64: return eBuildingMode::grandAgora;
    case 68: return eBuildingMode::foodVendor;
    case 69: return eBuildingMode::fleeceVendor;
    case 70: return eBuildingMode::oilVendor;
    case 71: return eBuildingMode::wineVendor;
    case 72: return eBuildingMode::armsVendor;
    case 73: return eBuildingMode::horseTrainer;
    case 74: return eBuildingMode::fountain;
    case 76: return eBuildingMode::hospital;
    case 77: return eBuildingMode::college;
    case 78: return eBuildingMode::dramaSchool;
    case 79: return eBuildingMode::gymnasium;
    case 80: return eBuildingMode::stadium;
    case 81: return eBuildingMode::podium;
    case 82: return eBuildingMode::theater;
    case 84: return eBuildingMode::templeZeus;
    case 85: return eBuildingMode::templePoseidon;
    case 86: return eBuildingMode::templeDemeter;
    case 87: return eBuildingMode::templeApollo;
    case 88: return eBuildingMode::templeArtemis;
    case 89: return eBuildingMode::templeAres;
    case 90: return eBuildingMode::templeAphrodite;
    case 91: return eBuildingMode::templeHermes;
    case 92: return eBuildingMode::templeAthena;
    case 93: return eBuildingMode::templeHephaestus;
    case 94: return eBuildingMode::templeDionysus;
    case 95: return eBuildingMode::templeHades;
    case 96: return eBuildingMode::templeHera;
    case 97: return eBuildingMode::templeAtlas;
    case 117: return eBuildingMode::palace;
    case 121: return eBuildingMode::maintenanceOffice;
    case 122: return eBuildingMode::taxOffice;
    case 123: return eBuildingMode::roadblock;
    case 124: return eBuildingMode::watchpost;
    case 127: return eBuildingMode::bench;
    case 129: return eBuildingMode::doricColumn;
    case 135: return eBuildingMode::armory;
    case 136: return eBuildingMode::triremeWharf;
    case 145: return eBuildingMode::ionicColumn;
    case 146: return eBuildingMode::corinthianColumn;
    case 148: return eBuildingMode::dolphinSculpture;
    case 150: return eBuildingMode::shellGarden;
    case 152: return eBuildingMode::birdBath;
    default: return eBuildingMode::none;
    }
}

// eGameBoard::sBuildTiles turns the tile a building is placed at into the
// building's rectangle, this goes the other way
void sAnchor(const SDL_Rect& r, int& tx, int& ty) {
    tx = r.x;
    ty = r.y;
    if(r.w == 2 && r.h == 2) {
        ty += 1;
    } else if(r.w == 3 && r.h == 3) {
        tx += 1;
        ty += 1;
    } else if(r.w == 4 || r.h == 4) {
        tx += 1;
        ty += 2;
    } else if(r.w == 5 || r.h == 5 || r.w == 6 || r.h == 6) {
        tx += 2;
        ty += 2;
    }
}

}

std::string eGameWidget::importOriginalSave(const ZeusSaveFile& save) {
    auto& board = *mBoard;
    board.waitUntilFinished();
    // the viewed city is only known after the first frame, building goes
    // through it, so it is set ahead of that from the map's territory
    auto cid = eCityId::neutralFriendly;
    board.iterateOverAllTiles([&](eTile* const t) {
        if(cid == eCityId::neutralFriendly) cid = t->cityId();
    });
    mViewedCityId = cid;
    const auto pid = board.personPlayer();
    const auto city = board.boardCityWithId(cid);
    const bool atlantean = city && city->atlantean();

    // the board comes from the map of the adventure, which knows nothing
    // of the city built on it since
    {
        std::set<eBuilding*> bs;
        board.iterateOverAllTiles([&](eTile* const t) {
            if(const auto b = t->underBuilding()) bs.insert(b);
        });
        for(const auto b : bs) b->erase();
    }

    auto tiles = ZeusFile::sApplyTerrain(board, save.mapsize(), atlantean,
                                         save.terrain(), save.scrub(),
                                         save.elevation());
    // tiles under buildings carry the building flag in front of everything
    // else, so the road through an agora or under a roadblock and the
    // meadow under farms and orchards have to be brought back
    const int border = (228 - save.mapsize())/2;
    for(auto& row : tiles) {
        for(auto& col : row.second) {
            const auto t = col.second.fTile;
            if(!t) continue;
            const uint32_t terr = save.terrain().get(col.first + border,
                                                     row.first + border);
            if(!(terr & 0x8)) continue;
            if(terr & 0x40) {
                const auto road = e::make_shared<eRoad>(board, t->cityId());
                t->setUnderBuilding(road);
                road->addUnderBuilding(t);
                road->setCenterTile(t);
                road->setTileRect({t->x(), t->y(), 1, 1});
            } else if(terr & 0x800) {
                t->setTerrain(eTerrain::fertile);
            }
        }
    }

    std::map<int, const eZeusSaveBuilding*> byId;
    for(const auto& b : save.buildings()) {
        byId[b.fId] = &b;
    }
    const auto chain = [&](const eZeusSaveBuilding& head) {
        std::vector<const eZeusSaveBuilding*> result{&head};
        int next = head.fNext;
        while(next && byId.count(next) && result.size() < 200) {
            const auto b = byId[next];
            result.push_back(b);
            next = b->fNext;
        }
        return result;
    };
    const auto rectOf = [&](const std::vector<const eZeusSaveBuilding*>& parts,
                            SDL_Rect& r) {
        int minX = INT_MAX;
        int minY = INT_MAX;
        int maxX = INT_MIN;
        int maxY = INT_MIN;
        for(const auto b : parts) {
            for(int i = 0; i < b->fSize; i++) {
                for(int j = 0; j < b->fSize; j++) {
                    const auto t = tiles[b->fY + j][b->fX + i].fTile;
                    if(!t) return false;
                    minX = std::min(minX, t->x());
                    minY = std::min(minY, t->y());
                    maxX = std::max(maxX, t->x());
                    maxY = std::max(maxY, t->y());
                }
            }
        }
        r = SDL_Rect{minX, minY, maxX - minX + 1, maxY - minY + 1};
        return true;
    };
    const auto placedAt = [&](const int x, const int y,
                              const eBuildingType type) {
        const auto t = board.tile(x, y);
        if(!t) return false;
        const auto b = t->underBuilding();
        return b && b->type() == type;
    };

    const bool editorMode = mEditorMode;
    const bool rotate = mRotate;
    mEditorMode = true;
    mImporting = true;
    const auto buildWithMode = [&](const eBuildingMode mode,
                                   const int tx, const int ty) {
        mGm->setMode(mode);
        mPressedTX = mHoverTX = tx;
        mPressedTY = mHoverTY = ty;
        buildMouseRelease();
    };

    int placed = 0;
    std::map<int, int> failed;
    std::map<int, int> skipped;

    const auto placeHouse = [&](const eZeusSaveBuilding& b,
                                const SDL_Rect& r) {
        const bool elite = b.fType >= 9;
        int tx;
        int ty;
        sAnchor(r, tx, ty);
        stdsptr<eHouseBase> house;
        const bool built = board.build(tx, ty, r.w, r.h, cid, pid, true,
                                       [&]() -> stdsptr<eBuilding> {
            if(elite) house = e::make_shared<eEliteHousing>(board, cid);
            else house = e::make_shared<eSmallHouse>(board, cid);
            return house;
        });
        if(!built || !house) return false;
        // The supplies of a house are not decoded yet, so it starts well
        // supplied and the walkers take over from there. Every supply
        // recomputes the level and a level too low for the people evicts
        // them, so one moves in first, the level rises with the supplies
        // and the rest follow once there is room.
        house->moveIn(1);
        house->provide(eProvide::water, 100);
        house->provide(eProvide::food, 1000);
        house->provide(eProvide::fleece, 100);
        house->provide(eProvide::oil, 100);
        house->provide(eProvide::philosopherInventor, 100);
        house->provide(eProvide::actorAstronomer, 100);
        house->provide(eProvide::gymnastScholar, 100);
        house->provide(eProvide::competitorCurator, 100);
        if(elite) {
            house->provide(eProvide::wine, 100);
            house->provide(eProvide::arms, 100);
            house->provide(eProvide::horses, 100);
        }
        house->moveIn(b.fPeople - house->people());
        // a house stores food for its people, who were one so far
        house->provide(eProvide::food, 1000);
        return true;
    };

    const auto placeTree = [&](const eZeusSaveBuilding& b,
                               const SDL_Rect& r) {
        const auto type = b.fType == 35 ? eResourceBuildingType::vine :
                                          eResourceBuildingType::oliveTree;
        return board.build(r.x, r.y, 1, 1, cid, pid, true, [&]() {
            return e::make_shared<eResourceBuilding>(board, type, cid);
        }, false, true);
    };

    const auto placeAgora = [&](const eZeusSaveBuilding& head,
                                const std::vector<const eZeusSaveBuilding*>& parts) {
        std::vector<const eZeusSaveBuilding*> vendors;
        for(const auto p : parts) {
            if(p != &head) vendors.push_back(p);
        }
        SDL_Rect r;
        if(vendors.empty() || !rectOf(vendors, r)) return false;
        std::set<eTile*> stalls;
        for(const auto v : vendors) {
            for(int i = 0; i < v->fSize; i++) {
                for(int j = 0; j < v->fSize; j++) {
                    stalls.insert(tiles[v->fY + j][v->fX + i].fTile);
                }
            }
        }
        // an agora takes the place along the road the cursor points at,
        // so look for the cursor position that gives the original stalls
        const bool grand = head.fType == 64;
        const auto mode = sModeForType(head.fType);
        bool done = false;
        for(int x = r.x - 6; x <= r.x + r.w + 6 && !done; x++) {
            for(int y = r.y - 6; y <= r.y + r.h + 6 && !done; y++) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                eAgoraOrientation bt;
                const auto p = agoraBuildPlaceIter(t, grand, bt, cid, pid);
                std::set<eTile*> lobes;
                for(const auto pt : p) {
                    if(pt && !pt->hasRoad()) lobes.insert(pt);
                }
                if(lobes != stalls) continue;
                buildWithMode(mode, x, y);
                done = true;
            }
        }
        if(!done) return false;
        for(const auto v : vendors) {
            const auto vmode = sModeForType(v->fType);
            if(vmode == eBuildingMode::none) continue; // empty stall
            SDL_Rect vr;
            if(!rectOf({v}, vr)) continue;
            const auto t = board.tile(vr.x, vr.y);
            const auto space = t ? t->underBuilding() : nullptr;
            if(!space || space->type() != eBuildingType::agoraSpace) {
                failed[v->fType]++;
                continue;
            }
            const auto ct = space->centerTile();
            buildWithMode(vmode, ct->x(), ct->y());
            const auto vb = t->underBuilding();
            if(vb && vb->type() != eBuildingType::agoraSpace) placed++;
            else failed[v->fType]++;
        }
        return true;
    };

    std::vector<const eZeusSaveBuilding*> houses;
    for(const auto& b : save.buildings()) {
        if(b.fPrev) continue; // a part, handled with its first part
        const int type = b.fType;
        const auto parts = chain(b);
        SDL_Rect r;
        bool ok = false;
        if(type >= 2 && type <= 14) {
            houses.push_back(&b);
            continue;
        } else if(type == 35 || type == 36) {
            ok = rectOf({&b}, r) && placeTree(b, r);
        } else if(type == 63 || type == 64) {
            ok = placeAgora(b, parts);
        } else if(type == 119) {
            // the kind of commemorative monument, counted from 1
            const int id = std::clamp(b.fSubtype - 1, 0, 8);
            ok = rectOf({&b}, r);
            if(ok) {
                int tx;
                int ty;
                sAnchor(r, tx, ty);
                ok = board.build(tx, ty, r.w, r.h, cid, pid, true, [&]() {
                    return e::make_shared<eCommemorative>(id, board, cid);
                });
            }
        } else if(type >= 84 && type <= 97) {
            // eZeus lays a sanctuary out from a blueprint of its own, which
            // differs in size from the original, centered at the cursor
            const auto mode = sModeForType(type);
            ok = rectOf(parts, r);
            if(ok) {
                const auto before = board.sanctuaries(cid).size();
                const bool wide = r.w >= r.h;
                for(const bool rot : {!wide, wide}) {
                    mRotate = rot;
                    buildWithMode(mode, r.x + r.w/2, r.y + r.h/2);
                    if(board.sanctuaries(cid).size() > before) break;
                }
                ok = board.sanctuaries(cid).size() > before;
            }
        } else if(type == 117) {
            // two 4x4 halves, the tiles around them come with eZeus' palace
            std::vector<const eZeusSaveBuilding*> halves;
            for(const auto p : parts) {
                if(p->fSize == 4) halves.push_back(p);
            }
            ok = rectOf(halves, r);
            if(ok) {
                mRotate = r.h > r.w;
                int tx;
                int ty;
                sAnchor(r, tx, ty);
                buildWithMode(eBuildingMode::palace, tx, ty);
                ok = placedAt(r.x, r.y, eBuildingType::palace);
            }
        } else {
            const auto mode = sModeForType(type);
            if(mode == eBuildingMode::none) {
                skipped[type]++;
                continue;
            }
            ok = rectOf(parts, r);
            if(ok) {
                mRotate = r.h > r.w;
                int tx;
                int ty;
                sAnchor(r, tx, ty);
                if(r.w == 1 && r.h == 1) {
                    tx = r.x;
                    ty = r.y;
                }
                buildWithMode(mode, tx, ty);
                const auto btype = eBuildingModeHelpers::toBuildingType(mode);
                if(mode == eBuildingMode::roadblock) {
                    const auto t = board.tile(r.x, r.y);
                    ok = t && t->hasRoad();
                } else {
                    ok = placedAt(r.x, r.y, btype) ||
                         placedAt(r.x + r.w - 1, r.y + r.h - 1, btype);
                }
                if(ok && (type == 57 || type == 58)) {
                    const auto t = board.tile(r.x + r.w - 1, r.y + r.h - 1);
                    const auto s = static_cast<eStorageBuilding*>(
                                       t->underBuilding());
                    for(const auto p : parts) {
                        for(const auto& g : p->fGoods) {
                            s->addNotAccept(g.first, g.second);
                        }
                    }
                }
            }
        }
        if(ok) placed++;
        else failed[type]++;
    }

    // the level of a house depends on the appeal around it, which comes
    // from the buildings placed so far, computed on the threads' copy of
    // the board
    board.scheduleDataUpdate();
    board.scheduleAppealMapUpdate(cid);
    board.updateAppealMapIfNeeded();
    board.waitUntilFinished();
    int people = 0;
    int originalPeople = 0;
    // original level -> level reached, per common (c) and elite (e) house
    std::map<std::string, int> levels;
    for(const auto b : houses) {
        originalPeople += b->fPeople;
        SDL_Rect r;
        const bool ok = rectOf({b}, r) && placeHouse(*b, r);
        if(ok) placed++;
        else failed[b->fType]++;
        const auto t = ok ? board.tile(r.x, r.y) : nullptr;
        const auto h = t ? dynamic_cast<eHouseBase*>(t->underBuilding()) : nullptr;
        if(!h) continue;
        people += h->people();
        const bool elite = b->fType >= 9;
        const int level = elite ? std::max(0, b->fType - 10) : b->fType - 2;
        levels[std::string(elite ? "e" : "c") + std::to_string(level) +
               "->" + std::to_string(h->level())]++;
    }

    mGm->clearMode();
    mRotate = rotate;
    mEditorMode = editorMode;
    mImporting = false;

    board.updateMarbleTiles();
    board.scheduleTerrainUpdate();
    updateMaps(true);

    std::string report = "placed " + std::to_string(placed) +
                         ", people " + std::to_string(people) +
                         " of " + std::to_string(originalPeople);
    const auto list = [&](const std::string& what,
                          const std::map<int, int>& types) {
        if(types.empty()) return;
        report += ", " + what + ":";
        for(const auto& t : types) {
            report += " " + std::to_string(t.first) +
                      "x" + std::to_string(t.second);
        }
    };
    list("failed", failed);
    list("not supported", skipped);
    report += ", house levels:";
    for(const auto& l : levels) {
        report += " " + l.first + "x" + std::to_string(l.second);
    }
    return report;
}
