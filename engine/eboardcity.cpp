#include "eboardcity.h"

#include "buildings/ebuilding.h"
#include "buildings/eemployingbuilding.h"
#include "buildings/estoragebuilding.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "buildings/eheroshall.h"
#include "buildings/eagorabase.h"
#include "buildings/etradepost.h"
#include "buildings/eurchinquay.h"
#include "buildings/efishery.h"
#include "buildings/epier.h"
#include "buildings/eaestheticsbuilding.h"
#include "buildings/ehorseranch.h"
#include "buildings/ehorseranchenclosure.h"
#include "buildings/etriremewharf.h"
#include "buildings/pyramids/epyramid.h"

#include "evectorhelpers.h"

#include "gameEvents/einvasionevent.h"
#include "gameEvents/eplayerconquestevent.h"
#include "gameEvents/emonsterinvasioneventbase.h"
#include "gameEvents/egodvisitevent.h"
#include "gameEvents/earmyreturnevent.h"
#include "engine/emilitaryaid.h"
#include "spawners/ebanner.h"
#include "einvasionhandler.h"

#include "elanguage.h"
#include "emessages.h"
#include "eeventdata.h"

#include "eplague.h"

#include "engine/eepisode.h"
#include "egameboard.h"

#include "eiteratesquare.h"
#include "etilehelper.h"

#include "spawners/espawner.h"
#include "buildings/ehippodrome.h"
#include "epathfinder.h"
#include "eevent.h"

#include <algorithm>
#include <cmath>

eBoardCity::eBoardCity(const eCityId cid, eGameBoard& board) :
    mBoard(board),
    mId(cid),
    mCityEvents(cid, board),
    mCityPlan(cid),
    mHusbData(mPopData, *this, board),
    mEmplData(mPopData, *this, board),
    mEmplDistributor(mEmplData) {}

eBoardCity::~eBoardCity() {
    mCityEvents.clear();
}

void eBoardCity::updateTiles() {
    mTiles.clear();
    bool first = true;
    mBoard.iterateOverAllTiles([&](eTile* const tile) {
        const auto cid = tile->cityId();
        if(cid != mId) return;
        mTiles.push_back(tile);
        SDL_Rect tRect;
        tRect.x = tile->dx();
        tRect.y = tile->dy();
        tRect.w = 1;
        tRect.h = 1;
        if(first) {
            first = false;
            mTileBRect = tRect;
        } else {
            SDL_Rect result;
            SDL_UnionRect(&mTileBRect, &tRect, &result);
            mTileBRect = result;
        }
    });
    if(mTiles.empty()) clearTiles();
}

void eBoardCity::clearTiles() {
    mTiles.clear();
    mTileBRect = SDL_Rect{0, 0, 0, 0};
}

void eBoardCity::incTime(const int by) {
    mCoverageUpdate += by;
    const int cup = 2000;
    if(mCoverageUpdate > cup) {
        mCoverageUpdate -= cup;
        updateCoverage();
    }

    const auto& msgs = &eMessages::instance;
    eEventData ed(mId);
    const int pop = mPopData.population();
    if(pop >= 100 && !mPop100) {
        mBoard.showMessage(ed, msgs->fPop100);
        mPop100 = true;
    } else if(pop >= 500 && !mPop500) {
        mBoard.showMessage(ed, msgs->fPop500);
        mPop500 = true;
    } else if(pop >= 1000 && !mPop1000) {
        mBoard.showMessage(ed, msgs->fPop1000);
        allow(eBuildingType::commemorative, 0);
        mPop1000 = true;
    } else if(pop >= 2000 && !mPop2000) {
        mBoard.showMessage(ed, msgs->fPop2000);
        allow(eBuildingType::commemorative, 0);
        mPop2000 = true;
    } else if(pop >= 3000 && !mPop3000) {
        mBoard.showMessage(ed, msgs->fPop3000);
        mPop3000 = true;
    } else if(pop >= 5000 && !mPop5000) {
        mBoard.showMessage(ed, msgs->fPop5000);
        allow(eBuildingType::commemorative, 0);
        mPop5000 = true;
    } else if(pop >= 10000 && !mPop10000) {
        mBoard.showMessage(ed, msgs->fPop10000);
        allow(eBuildingType::commemorative, 0);
        mPop10000 = true;
    } else if(pop >= 15000 && !mPop15000) {
        mBoard.showMessage(ed, msgs->fPop15000);
        allow(eBuildingType::commemorative, 0);
        mPop15000 = true;
    } else if(pop >= 20000 && !mPop20000) {
        mBoard.showMessage(ed, msgs->fPop20000);
        allow(eBuildingType::commemorative, 0);
        mPop20000 = true;
    } else if(pop >= 25000 && !mPop25000) {
        mBoard.showMessage(ed, msgs->fPop25000);
        allow(eBuildingType::commemorative, 0);
        mPop25000 = true;
    }

    const int updateResWait = 1000;
    mUpdateResources += by;
    if(mUpdateResources > updateResWait) {
        mUpdateResources -= updateResWait;
        updateResources();
    }

    const int food = resourceCount(eResourceType::food);
    bool prolongedNoFood = false;
    const auto date = mBoard.date();
    if(mNoFood && food < 1 && pop > 10) {
        prolongedNoFood = date.year() - mNoFoodSince.year() > 1;
    } else if(food < 1 && pop > 10) {
        mNoFood = true;
        mNoFoodSince = date;
    } else {
        mNoFood = false;
    }
    const int u = mEmplData.unemployed();
    const int e = mEmplData.employable();
    const int v = mPopData.vacancies();
    const auto oldLimit = mImmigrationLimit;
    const auto p = owningPlayer();
    int drachmas = 0;
    eDate inDebtSince = date;
    if(p) {
        drachmas = p->drachmas();
        inDebtSince = p->inDebtSince();
    }
    if(prolongedNoFood) {
        mImmigrationLimit = eILB::lackOfFood;
    } else if(drachmas < 0 && date.year() - inDebtSince.year() > 1) {
        mImmigrationLimit = eILB::prolongedDebt;
    } else if(static_cast<int>(mWageRate) < static_cast<int>(eWageRate::low)) {
        mImmigrationLimit = eILB::lowWages;
    } else if(e < 10 ? false : (1.*u/e > 0.25 && u > 50)) {
        mImmigrationLimit = eILB::unemployment;
    } else if(static_cast<int>(mTaxRate) > static_cast<int>(eTaxRate::high)) {
        mImmigrationLimit = eILB::highTaxes;
    } else if(mExcessiveMilitaryServiceCount > 0) {
        mImmigrationLimit = eILB::excessiveMilitaryService;
    } else if(v <= 0) {
        mImmigrationLimit = eILB::lackOfVacancies;
    } else {
        mImmigrationLimit = eILB::none;
    }
    if(oldLimit != mImmigrationLimit) {
        switch(mImmigrationLimit) {
        case eILB::lackOfFood:
            mBoard.showTip(mId, eLanguage::zeusText(19, 112));
            break;
        case eILB::prolongedDebt:
            mBoard.showTip(mId, eLanguage::zeusText(19, 116));
            break;
        case eILB::lowWages:
            mBoard.showTip(mId, eLanguage::zeusText(19, 115));
            break;
        case eILB::unemployment:
            mBoard.showTip(mId, eLanguage::zeusText(19, 113));
            break;
        case eILB::highTaxes:
            mBoard.showTip(mId, eLanguage::zeusText(19, 114));
            break;
        case eILB::excessiveMilitaryService:
            mBoard.showTip(mId, eLanguage::zeusText(19, 117));
            break;
        case eILB::lackOfVacancies:
            mBoard.showTip(mId, eLanguage::zeusText(19, 111));
            break;
        case eILB::none:
            mBoard.showTip(mId, eLanguage::zeusText(19, 124));
        }
    }

    for(const auto i : mInvasionHandlers) {
        i->incTime(by);
    }

    mCityEvents.handleNewDate(date);

    if(p && !p->isPerson()) {
        const int lookForCityDefenseCheck = 5000;
        mLookForCityDefense += by;
        if(mLookForCityDefense > lookForCityDefenseCheck) {
            mLookForCityDefense -= lookForCityDefenseCheck;
            updateCityDefense();
        }
    }

    if(mCutOffCheckBuildingsState != mAllBuildingsState) {
        mCutOffCheckBuildingsState = mAllBuildingsState;
        const auto entry = entryPoint();
        const auto exit = exitPoint();
        if(entry && exit) {
            const int eX = exit->x();
            const int eY = exit->y();
            ePathFinder p([](eTileBase* const t) {
                return t->walkableTerrain();
            }, [&](eTileBase* const t) {
                return t->x() == eX && t->y() == eY;
            });
            const auto distance = [](eTileBase* const t) {
                const auto type = t->underBuildingType();
                const bool w = eBuilding::sWalkableBuilding(type);
                if(!w) return 200;
                return 1;
            };
            const int w = mBoard.width();
            const int h = mBoard.height();
            const bool r = p.findPath({0, 0, w, h}, entry, 1000, true, w, h, distance);
            if(r) {
                std::vector<eTile*> path;
                p.extractPath(path, mBoard);
                for(const auto t : path) {
                    const auto type = t->underBuildingType();
                    const bool w = eBuilding::sWalkableBuilding(type);
                    if(w) continue;
                    const auto ub = t->underBuilding();
                    ub->collapse();

                    eEventData ed;
                    ed.fTile = t;
                    mBoard.event(eEvent::areaCutOff, ed);
                }
            }
        }
    }
}

void eBoardCity::acquired() {
    mCityEvents.setupStartDate(mBoard.date());
}

void eBoardCity::updateCoverage() {
    int totalPeople = 0;
    int totalBuildings = 0;
    int commonPeople = 0;
    int sport = 0;
    int phil = 0;
    int drama = 0;
    int taxes = 0;
    double totalUnrest = 0;
    int totalSatisfaction = 0;
    int totalHygiene = 0;
    int totalMaintanance = 0;
    for(const auto b : mTimedBuildings) {
        if(const auto h = dynamic_cast<eHouseBase*>(b)) {
            const int p = h->people();
            if(h->athletesScholars() > 0) {
                sport += p;
            }
            if(h->philosophersInventors() > 0) {
                phil += p;
            }
            if(h->actorsAstronomers() > 0) {
                drama += p;
            }
            if(h->paidTaxes()) {
                taxes += p;
            }
            if(const auto ch = dynamic_cast<eSmallHouse*>(b)) {
                if(ch->disgruntled()) totalUnrest += p;
                totalSatisfaction += p*ch->satisfaction();
                totalHygiene += p*ch->hygiene();
                commonPeople += p;
            }
            totalPeople += p;
        }
        totalBuildings++;
        totalMaintanance += b->maintenance();
    }
    if(totalPeople <= 0) {
        mAthleticsCoverage = 0;
        mPhilosophyCoverage = 0;
        mDramaCoverage = 0;
        mTaxesCoverage = 0;
    } else {
        mAthleticsCoverage = std::round(100.*sport/totalPeople);
        mPhilosophyCoverage = std::round(100.*phil/totalPeople);
        mDramaCoverage = std::round(100.*drama/totalPeople);
        mTaxesCoverage = std::round(100.*taxes/totalPeople);
    }
    mAllDiscCoverage = (mAthleticsCoverage + mPhilosophyCoverage + mDramaCoverage)/3;

    if(commonPeople == 0) {
        mUnrest = 0;
        mPopularity = 100;
        mHealth = 100;
        mMaintanance = 100;
    } else {
        mUnrest = std::round(100.*totalUnrest/commonPeople);
        mPopularity = std::round(1.*totalSatisfaction/commonPeople);
        mHealth = std::round(1.*totalHygiene/commonPeople);
        mMaintanance = std::round(1.*totalMaintanance/totalBuildings);
    }
}

void eBoardCity::payTaxes(const int d, const int people) {
    mTaxesPaidThisYear += d;
    mPeoplePaidTaxesThisYear += people;
}

void eBoardCity::nextYear() {
    mTaxesPaidLastYear = mTaxesPaidThisYear;
    mTaxesPaidThisYear = 0;
    mPeoplePaidTaxesLastYear = mPeoplePaidTaxesThisYear;
    mPeoplePaidTaxesThisYear = 0;
}

void eBoardCity::payPensions() {
    const auto p = owningPlayer();
    const int d = std::ceil(mEmplData.pensions()/12.);
    if(p) p->incDrachmas(-d, eFinanceTarget::wages);
}

bool eBoardCity::replace3By3AestheticByCommemorative() {
    int id = -1;
    for(int i = 0; i <= 8; i++) {
        const bool a = mAvailableBuildings.available(
                           eBuildingType::commemorative, i);
        if(a) {
            id = i;
            break;
        }
    }
    if(id == -1) return false;
    double mostMissingAppeal = 0.;
    eBuilding* toReplace = nullptr;
    for(const auto b : m3x3AestheticBuildings) {
        const auto type = b->type();
        switch(type) {
        case eBuildingType::hedgeMaze:
        case eBuildingType::dolphinSculpture:
        case eBuildingType::orrery:
        case eBuildingType::spring:
        case eBuildingType::topiary: {
            const int did = b->districtId();
            if(did == -1) continue;
            const auto rect = b->tileRect();
            const int range = 3;
            for(int x = rect.x - range; x < rect.x + rect.w + range; x++) {
                for(int y = rect.y - range; y < rect.y + rect.h + range; y++) {
                    const SDL_Point p{x, y};
                    const bool in = SDL_PointInRect(&p, &rect);
                    if(in) continue;
                    int dx;
                    int dy;
                    eTileHelper::tileIdToDTileId(x, y, dx, dy);
                    const auto tile = mBoard.dtile(dx, dy);
                    if(!tile) continue;
                    const auto ub = tile->underBuilding();
                    if(!ub) continue;
                    if(const auto h = dynamic_cast<eHouseBase*>(ub)) {
                        const double goodAppeal =
                                type == eBuildingType::eliteHousing ?
                                    10.1 : 8.1;
                        const double a = h->appeal();
                        const double missing = goodAppeal - a;
                        if(missing > mostMissingAppeal) {
                            mostMissingAppeal = missing;
                            toReplace = b;
                        }
                    }
                }
            }
        } break;
        default:
            break;
        }
    }
    if(!toReplace) return false;
    const auto b = toReplace;
    const auto rect = b->tileRect();

    const int did = b->districtId();
    b->setDistrictId(-1);
    b->erase();
    const auto bc = [&]() {
        const auto c = e::make_shared<eCommemorative>(id, mBoard, mId);
        c->setDistrictId(did);
        return c;
    };

    const auto pid = mBoard.cityIdToPlayerId(mId);
    const bool r = mBoard.buildBase(rect.x, rect.y,
                                    rect.x + rect.w - 1,
                                    rect.y + rect.h - 1,
                                    bc, pid, mId, true);
    if(r) mAvailableBuildings.built(eBuildingType::commemorative, id);
    return r;
}

void eBoardCity::updateResourceTiles() {
    mResourceTiles.clear();
    for(const auto tile : mTiles) {
        if(tile->hasFish() || tile->hasUrchin()) {
            mResourceTiles.push_back(tile);
            continue;
        }
        const auto terr = tile->terrain();
        switch(terr) {
        case eTerrain::marble:
        case eTerrain::blackMarble:
        case eTerrain::copper:
        case eTerrain::silver:
        case eTerrain::orichalc:
        case eTerrain::forest:
        case eTerrain::choppedForest:
            mResourceTiles.push_back(tile);
            break;
        default:
            break;
        }
    }
}

void eBoardCity::updateForestTiles() {
    mForestTiles.clear();
    for(const auto tile : mTiles) {
        const auto terr = tile->terrain();
        switch(terr) {
        case eTerrain::forest:
        case eTerrain::choppedForest:
            mForestTiles.push_back(tile);
            break;
        default:
            break;
        }
    }
}

void eBoardCity::updateCityDefense() {
    eTile* defendTile = nullptr;
    if(!mInvasionHandlers.empty()) {
        const auto& inv = mInvasionHandlers[0];
        defendTile = inv->currentTile();
    } else {
        const auto i = mBoard.invasionToDefend(mId);
        if(i) defendTile = i->landInvasionTile();
    }
    if(defendTile) {
        const int tx = defendTile->x();
        const int ty = defendTile->y();
        std::vector<eSoldierBanner*> bs;
        for(const auto& b : mSoldierBanners) {
            if(b->isAbroad()) continue;
            b->backFromHome();
            bs.push_back(b.get());
        }
        eSoldierBanner::sPlace(bs, tx, ty, mBoard, 3, 3);
        mDefending = true;
    } else {
        for(const auto& b : mSoldierBanners) {
            if(b->isAbroad()) continue;
            b->moveToDefault();
            b->goHome();
        }
        mDefending = false;
    }
}

void eBoardCity::nextMonth() {
    payPensions();

    {
        int nTot = 0;
        int nCalled = 0;
        for(const auto& s : mPalaceSoldierBanners) {
            const bool a = s->isAbroad();
            if(a) continue;
            const int c = s->count();
            nTot += c;
            const bool h = s->isHome();
            if(h) continue;
            nCalled += c;
        }
        if(nTot == 0 || static_cast<double>(nCalled)/nTot < 0.25) {
            mExcessiveMilitaryServiceCount--;
            mMonthsOfMilitaryService = 0;
        } else {
            mMonthsOfMilitaryService++;
            if(mMonthsOfMilitaryService > 6) {
                mExcessiveMilitaryServiceCount = 2;
            }
        }
    }

    mPopData.nextMonth();
    for(const auto& h : mHippodromes) {
        h->nextMonth();
    }

    replace3By3AestheticByCommemorative();

    const auto date = mBoard.date();
    const bool pp = personPlayerOwner();
    if(!pp && !mDefending) {
        const int space = maxPalaceBannerCount();
        if(space == sPalaceTiles) {
            if(mNextAttackPlanned) {
                if(date < mNextAttackDate) {
                    return;
                }
            } else {
                mNextAttackPlanned = true;
                mNextAttackDate = date;
                mNextAttackDate.nextMonths(eNumbers::sAIInvasionMonthsBreak);
                return;
            }
            const int soldiers = mMaxHoplites + mMaxHorsemen;
            const int missingArmor = missingArmorFromEliteHouses();
            const auto pid = owningPlayerId();
            const auto tid = mBoard.playerIdToTeamId(pid);
            if(soldiers > 20 && missingArmor == 0) {
                auto enemyCids = mBoard.enemyCidsOnBoard(tid);
                if(!enemyCids.empty()) {
                    eRand::randomShuffle(enemyCids);
                    const auto targetCid = enemyCids[0];
                    const auto& wboard = mBoard.world();
                    const auto wc = wboard.cityWithId(targetCid);
                    eEnlistedForces forces;
                    for(const auto& b : mSoldierBanners) {
                        const auto type = b->type();
                        if(type == eBannerType::rockThrower) continue;
                        forces.fSoldiers.push_back(b);
                    }
                    for(const auto hh : mHeroHalls) {
                        const auto stage = hh->stage();
                        if(stage != eHeroSummoningStage::arrived) continue;
                        const bool abroad = hh->heroOnQuest();
                        if(abroad) continue;
                        const auto type = hh->heroType();
                        forces.fHeroes.push_back({mId, type});
                    }
                    mBoard.enlistForces(forces);
                    const auto e = e::make_shared<ePlayerConquestEvent>(
                                       mId, eGameEventBranch::root, mBoard);
                    const auto boardDate = mBoard.date();
                    const int period = eNumbers::sArmyTravelTime;
                    const auto date = boardDate + period;
                    e->setWarningMonths(period/31);
                    e->initializeDate(date, period, 1);
                    e->initialize(date, forces, wc);
                    mBoard.addRootGameEvent(e);
                    mNextAttackPlanned = false;
                }
            }
        }
    }
}

double eBoardCity::taxRateF() const {
    return eTaxRateHelpers::getRate(mTaxRate);
}

void eBoardCity::setTaxRate(const eTaxRate tr) {
    mTaxRate = tr;
}

void eBoardCity::setWageRate(const eWageRate wr) {
    mWageRate = wr;
    distributeEmployees();
}

void eBoardCity::saveEditorCityPlan() {
    mCityPlan = eAICityPlan(mId);
    for(const auto b : mAllBuildings) {
        const int did = b->districtId();
        while(did >= mCityPlan.districtCount()) {
            const int id = mCityPlan.districtCount();
            eAIDistrict d;
            d.fReadyConditions = mEditorDistrictConditions[id];
            mCityPlan.addDistrict(d);
        }
        auto& d = mCityPlan.district(did);
        eAIBuilding ab;
        ab.fType = b->type();
        ab.fRect = b->tileRect();
        if(const auto sb = dynamic_cast<eStorageBuilding*>(b)) {
            ab.fGet = sb->get();
            ab.fEmpty = sb->empties();
            ab.fAccept = sb->accepts();
            ab.fSpace = sb->maxCount();
            if(const auto p = dynamic_cast<eTradePost*>(b)) {
                ab.fO = p->orientation();
                ab.fTradePostType = p->tpType();
                const auto city = p->city();
                ab.fTradingPartner = city.cityId();
                if(ab.fTradePostType == eTradePostType::pier) {
                    const auto pier = p->unpackBuilding();
                    ab.fOtherRect = pier->tileRect();
                }
            }
        } else if(ab.fType == eBuildingType::pier) {
            continue;
        } else if(const auto hr = dynamic_cast<eHorseRanch*>(b)) {
            const auto e = hr->enclosure();
            ab.fOtherRect = e->tileRect();
        } else if(ab.fType == eBuildingType::horseRanchEnclosure) {
            continue;
        } else if(ab.fType == eBuildingType::agoraSpace) {
            continue;
        } else if(ab.fType == eBuildingType::foodVendor) {
            continue;
        } else if(ab.fType == eBuildingType::fleeceVendor) {
            continue;
        } else if(ab.fType == eBuildingType::oilVendor) {
            continue;
        } else if(ab.fType == eBuildingType::wineVendor) {
            continue;
        } else if(ab.fType == eBuildingType::armsVendor) {
            continue;
        } else if(ab.fType == eBuildingType::chariotVendor) {
            continue;
        } else if(ab.fType == eBuildingType::horseTrainer) {
            continue;
        } else if(ab.fType == eBuildingType::palaceTile) {
            continue;
        } else if(ab.fType == eBuildingType::templeTile) {
            continue;
        } else if(ab.fType == eBuildingType::templeAltar) {
            continue;
        } else if(ab.fType == eBuildingType::templeMonument) {
            continue;
        } else if(ab.fType == eBuildingType::templeStatue) {
            continue;
        } else if(ab.fType == eBuildingType::temple) {
            continue;
        } else if(ab.fType == eBuildingType::placeholder) {
            continue;
        } else if(const auto pb = dynamic_cast<ePatrolBuildingBase*>(b)) {
            ab.fGuides = pb->patrolGuides();
            ab.fGuidesBothDirections = pb->bothDirections();
            if(const auto f = dynamic_cast<eAgoraBase*>(b)) {
                ab.fO = f->diagonalOrientation();
            }
        } else if(const auto s = dynamic_cast<eSanctuary*>(b)) {
            const bool r = s->rotated();
            ab.fO = r ? eDiagonalOrientation::topLeft :
                        eDiagonalOrientation::topRight;
        } else if(const auto uq = dynamic_cast<eUrchinQuay*>(b)) {
            ab.fO = uq->orientation();
        } else if(const auto f = dynamic_cast<eFishery*>(b)) {
            ab.fO = f->orientation();
        }
        d.addBuilding(ab);
    }
}

void eBoardCity::editorClearBuildings() {
    for(const auto b : mAllBuildings) {
        const auto type = b->type();
        switch(type) {
        case eBuildingType::agoraSpace:
        case eBuildingType::foodVendor:
        case eBuildingType::fleeceVendor:
        case eBuildingType::oilVendor:
        case eBuildingType::wineVendor:
        case eBuildingType::armsVendor:
        case eBuildingType::horseTrainer:
        case eBuildingType::chariotVendor:
            continue;
        default:
            break;
        }

        b->erase();
    }
}

void eBoardCity::editorDisplayBuildings() {
    mCityPlan.editorDisplayBuildings(mBoard);
}

void eBoardCity::buildScheduled() {
    mCityPlan.buildScheduled(mBoard);
}

bool eBoardCity::previousDistrictFulfilled() {
    const int id = mCityPlan.lastBuiltDistrictId();
    if(id == -1) return true;
    const auto& d = mCityPlan.district(id);
    const auto& cs = d.fReadyConditions;
    if(cs.empty()) return true;
    for(const auto& c : cs) {
        using eType = eDistrictReadyCondition::eType;
        if(c.fType == eType::districtResourceCount ||
           c.fType == eType::totalResourceCount) {
            int total = 0;
            if(c.fResource == eResourceType::drachmas) {
                const auto pid = mBoard.cityIdToPlayerId(mId);
                total = mBoard.drachmas(pid);
            } else {
                const bool totalType = c.fType == eType::totalResourceCount;
                for(const auto b : mStorBuildings) {
                    if(!totalType) {
                        const int did = b->districtId();
                        if(did != id) continue;
                    }
                    const int cc = b->count(c.fResource);
                    total += cc;
                    if(total >= c.fValue) break;
                }
            }
            if(total < c.fValue) return false;
        } else if(c.fType == eType::districtPopulation) {
            int total = 0;
            for(const auto& b : mTimedBuildings) {
                if(const auto hb = dynamic_cast<eHouseBase*>(b)) {
                    const int did = b->districtId();
                    if(did != id) continue;
                    const int cc = hb->people();
                    total += cc;
                    if(total >= c.fValue) break;
                }
            }
            if(total < c.fValue) return false;
        } else if(c.fType == eType::totalPopulation) {
            const int p = population();
            if(p < c.fValue) return false;
        } else if(c.fType == eType::sanctuaryReady) {
            const auto s = sanctuary(c.fSanctuary);
            if(!s) return false;
            if(!s->finished()) return false;
        }
    }
    return true;
}

int eBoardCity::nextDistrictCost(int* const marble) {
    const int id = mCityPlan.nextDistrictId();
    if(id == -1) return 0;
    auto& c = mNextDistrictCost;
    if(c.fDrachmas == -1) {
        c.fDrachmas = mCityPlan.districtCost(mBoard, id, &c.fMarble);
    }
    if(marble) *marble = c.fMarble;
    return c.fDrachmas;
}

bool eBoardCity::buildNextDistrict(const int drachmas) {
    const bool pf = previousDistrictFulfilled();
    if(!pf) return false;
    const int id = mCityPlan.nextDistrictId();
    if(id == -1) return false;
    int marbleCost = 0;
    const int cost = nextDistrictCost(&marbleCost);
    if(cost > drachmas) return false;
    const int marble = resourceCount(eResourceType::marble);
    if(marbleCost > marble) return false;
    mCityPlan.buildNextDistrict(mBoard);
    mNextDistrictCost.fDrachmas = -1;
    return true;
}

std::vector<eBoardCity::eCondition> eBoardCity::getDistrictReadyConditions() {
    const auto& v = mEditorDistrictConditions[mCurrentDistrictId];
    return v;
    if(mCurrentDistrictId >= mCityPlan.districtCount()) return {};
    const auto& d = mCityPlan.district(mCurrentDistrictId);
    return d.fReadyConditions;
}

void eBoardCity::addDistrictReadyCondition(const eCondition& c) {
    auto& v = mEditorDistrictConditions[mCurrentDistrictId];
    v.push_back(c);
    if(mCurrentDistrictId >= mCityPlan.districtCount()) return;
    auto& d = mCityPlan.district(mCurrentDistrictId);
    d.fReadyConditions.push_back(c);
}

void eBoardCity::removeDistrictReadyCondition(const int id) {
    auto& v = mEditorDistrictConditions[mCurrentDistrictId];
    v.erase(v.begin() + id);
    if(mCurrentDistrictId >= mCityPlan.districtCount()) return;
    auto& d = mCityPlan.district(mCurrentDistrictId);
    d.fReadyConditions.erase(d.fReadyConditions.begin() + id);
}

void eBoardCity::setDistrictReadyCondition(const int id, const eCondition& c) {
    auto& v = mEditorDistrictConditions[mCurrentDistrictId];
    v[id] = c;
    if(mCurrentDistrictId >= mCityPlan.districtCount()) return;
    auto& d = mCityPlan.district(mCurrentDistrictId);
    d.fReadyConditions[id] = c;
}

void eBoardCity::registerBuilding(eBuilding* const b) {
    b->setDistrictId(mCurrentDistrictId);
    mAllBuildings.push_back(b);
    mAllBuildingsState++;
    const auto type = b->type();
    if(eBuilding::sTimedBuilding(type)) {
        mTimedBuildings.push_back(b);
    }
    switch(type) {
    case eBuildingType::road:
        mRoadState++;
        break;
    case eBuildingType::hedgeMaze:
    case eBuildingType::dolphinSculpture:
    case eBuildingType::orrery:
    case eBuildingType::spring:
    case eBuildingType::topiary:
        m3x3AestheticBuildings.push_back(b);
        break;
    case eBuildingType::granary:
    case eBuildingType::warehouse:
    case eBuildingType::tradePost:
    case eBuildingType::horseRanch:
    case eBuildingType::horseRanchEnclosure:
    case eBuildingType::chariotFactory:
    case eBuildingType::corral:
    case eBuildingType::carrotsFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::wheatFarm:
    case eBuildingType::fishery:
    case eBuildingType::olivePress:
    case eBuildingType::growersLodge:
    case eBuildingType::orangeTendersLodge:
    case eBuildingType::winery:
    case eBuildingType::timberMill:
    case eBuildingType::masonryShop:
    case eBuildingType::foundry:
    case eBuildingType::armory:
    case eBuildingType::refinery:
    case eBuildingType::blackMarbleWorkshop:
    case eBuildingType::sculptureStudio:
    case eBuildingType::dairy:
    case eBuildingType::urchinQuay:
    case eBuildingType::cardingShed:
    case eBuildingType::huntingLodge:
        mBuildingsWithResource.push_back(b);
        break;
    case eBuildingType::eliteHousing:
    case eBuildingType::commonHouse:
        mHouses.push_back(static_cast<eHouseBase*>(b));
        break;
    case eBuildingType::orangeTree:
    case eBuildingType::oliveTree:
    case eBuildingType::vine:
        mTreesAndVines.push_back(b);
        break;
    case eBuildingType::sheep:
    case eBuildingType::goat:
    case eBuildingType::cattle:
        mAnimalBuildings.push_back(b);
        mAnimalBuildingsSurroundingUpdate = true;
        break;
    case eBuildingType::templeAltar:
    case eBuildingType::temple:
    case eBuildingType::templeMonument:
    case eBuildingType::templeStatue:
    case eBuildingType::templeTile:

    case eBuildingType::pyramidPart:
    case eBuildingType::pyramidWall:
    case eBuildingType::pyramidTop:
    case eBuildingType::pyramidStatue:
    case eBuildingType::pyramidMonument:
    case eBuildingType::pyramidTile:
    case eBuildingType::pyramidAltar:
    case eBuildingType::pyramidTemple:
    case eBuildingType::pyramidObservatory:
    case eBuildingType::pyramidMuseum:
        mSanctBuildings.push_back(b);
        break;
    default:
        break;
    }
    if(type == eBuildingType::commemorative ||
       type == eBuildingType::godMonument) {
        mCommemorativeBuildings.push_back(b);
    }
}

bool eBoardCity::unregisterBuilding(eBuilding* const b) {
    if(!mBoard.editorMode()) {
        const int did = b->districtId();
        if(did != -1) {
            const auto bRect = b->tileRect();
            mCityPlan.addScheduledBuilding(did, bRect);
        }
    }
    const auto type = b->type();
    if(type == eBuildingType::road) {
        mRoadState++;
    }
    mAllBuildingsState++;
    eVectorHelpers::remove(mAllBuildings, b);
    eVectorHelpers::remove(mBuildingsWithResource, b);
    eVectorHelpers::remove(mHouses, static_cast<eHouseBase*>(b));
    eVectorHelpers::remove(m3x3AestheticBuildings, b);
    eVectorHelpers::remove(mTimedBuildings, b);
    eVectorHelpers::remove(mCommemorativeBuildings, b);
    eVectorHelpers::remove(mTreesAndVines, b);
    eVectorHelpers::remove(mSanctBuildings, b);
    const bool r = eVectorHelpers::remove(mAnimalBuildings, b);
    if(r) mAnimalBuildingsSurroundingUpdate = true;
    if(type == eBuildingType::hippodromePiece) {
        updateHippodromes();
    }
    return true;
}

void eBoardCity::registerEmplBuilding(eEmployingBuilding* const b) {
    mEmployingBuildings.push_back(b);
    const auto type = b->type();
    const bool sd = isShutDown(type);
    if(sd) b->setShutDown(true);
    eSector s;
    const bool r = eSectorHelpers::sBuildingSector(type, s);
    if(r) {
        mSectorBuildings[s].push_back(b);
    }
    if(!sd) {
        const int me = b->maxEmployees();
        mEmplData.incTotalJobVacancies(me);
        if(r) mEmplDistributor.incMaxEmployees(s, me);
    }
    distributeEmployees();
}

bool eBoardCity::unregisterEmplBuilding(eEmployingBuilding* const b) {
    const bool rr = eVectorHelpers::remove(mEmployingBuildings, b);
    const auto type = b->type();
    eSector s;
    const bool r = eSectorHelpers::sBuildingSector(type, s);
    if(r) {
        eVectorHelpers::remove(mSectorBuildings[s], b);
    }
    if(rr) {
        const bool sd = isShutDown(type);
        if(!sd) {
            const int me = b->maxEmployees();
            mEmplData.incTotalJobVacancies(-me);
            if(r) mEmplDistributor.incMaxEmployees(s, -me);
        }
    }
    distributeEmployees();
    return rr;
}

void eBoardCity::registerTradePost(eTradePost* const b) {
    mTradePosts.push_back(b);
}

bool eBoardCity::unregisterTradePost(eTradePost* const b) {
    return eVectorHelpers::remove(mTradePosts, b);
}

bool eBoardCity::hasTradePost(const eWorldCity& city) {
    for(const auto t : mTradePosts) {
        const bool r = &t->city() == &city;
        if(r) return true;
    }
    return false;
}

void eBoardCity::registerStadium(eStadium* const s) {
    mStadium = s;
}

void eBoardCity::unregisterStadium() {
    mStadium = nullptr;
}

void eBoardCity::registerMuseum(eMuseum* const s) {
    mMuseum = s;
}

void eBoardCity::unregisterMuseum() {
    mMuseum = nullptr;
}

void eBoardCity::registerStorBuilding(eStorageBuilding* const b) {
    mStorBuildings.push_back(b);
}

bool eBoardCity::unregisterStorBuilding(eStorageBuilding* const b) {
    return eVectorHelpers::remove(mStorBuildings, b);
}

void eBoardCity::registerSanctuary(eSanctuary* const b) {
    mSanctuariesState++;
    mSanctuaries.push_back(b);
}

bool eBoardCity::unregisterSanctuary(eSanctuary* const b) {
    mSanctuariesState++;
    return eVectorHelpers::remove(mSanctuaries, b);
}

void eBoardCity::registerMonument(eMonument* const b) {
    mMonumentsState++;
    mMonuments.push_back(b);
}

bool eBoardCity::unregisterMonument(eMonument* const b) {
    mMonumentsState++;
    return eVectorHelpers::remove(mMonuments, b);
}

void eBoardCity::monumentFinished() {
    mMonumentsState++;
}

void eBoardCity::registerHeroHall(eHerosHall* const b) {
    mHeroHalls.push_back(b);
}

bool eBoardCity::unregisterHeroHall(eHerosHall* const b) {
    return eVectorHelpers::remove(mHeroHalls, b);
}

bool eBoardCity::unregisterCommonHouse(eSmallHouse* const ch) {
    const auto p = plagueForHouse(ch);
    if(p) {
        p->removeHouse(ch);
        const int c = p->houseCount();
        if(c <= 0) healPlague(p);
    }
    return true;
}

void eBoardCity::registerPalace(ePalace* const p) {
    mPalace = p;
}

void eBoardCity::unregisterPalace() {
    mPalace = nullptr;
    soldierBannersUpdate();
}

bool eBoardCity::supportsBuilding(const eBuildingMode mode) const {
    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    int id = -1;
    switch(mode) {
    case eBuildingMode::populationMonument:
    case eBuildingMode::victoryMonument:
    case eBuildingMode::colonyMonument:
    case eBuildingMode::athleteMonument:
    case eBuildingMode::conquestMonument:
    case eBuildingMode::happinessMonument:
    case eBuildingMode::heroicFigureMonument:
    case eBuildingMode::diplomacyMonument:
    case eBuildingMode::scholarMonument:
        id = eBuildingModeHelpers::toCommemorativeId(mode);
        break;
    case eBuildingMode::aphroditeMonument:
    case eBuildingMode::apolloMonument:
    case eBuildingMode::aresMonument:
    case eBuildingMode::artemisMonument:
    case eBuildingMode::athenaMonument:
    case eBuildingMode::atlasMonument:
    case eBuildingMode::demeterMonument:
    case eBuildingMode::dionysusMonument:
    case eBuildingMode::hadesMonument:
    case eBuildingMode::hephaestusMonument:
    case eBuildingMode::heraMonument:
    case eBuildingMode::hermesMonument:
    case eBuildingMode::poseidonMonument:
    case eBuildingMode::zeusMonument: {
        const auto gt = eBuildingModeHelpers::toGodType(mode);
        id = static_cast<int>(gt);
    } break;
    default:
        break;
    }

    return availableBuilding(type, id);
}

bool eBoardCity::availableBuilding(const eBuildingType type,
                                   const int id) const {
    switch(type) {
    case eBuildingType::chariotVendor:
    case eBuildingType::chariotFactory: {
        if(!mAtlantean) return false;
    } break;
    case eBuildingType::horseTrainer: {
        if(mAtlantean) return false;
    } break;
    default:
        break;
    }

    return mAvailableBuildings.available(type, id);
}

void eBoardCity::built(const eBuildingType type,
                       const int id) {
    mAvailableBuildings.built(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::destroyed(const eBuildingType type,
                           const int id) {
    if(!mBoard.registerBuildingsEnabled()) return;
    mAvailableBuildings.destroyed(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::allow(const eBuildingType type,
                       const int id) {
    mAvailableBuildings.allow(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::disallow(const eBuildingType type,
                          const int id) {
    mAvailableBuildings.disallow(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::setManTowers(const bool m) {
    mManTowers = m;
}

void eBoardCity::distributeEmployees(const eSector s) {
    int e = mEmplDistributor.employees(s);
    const int maxE = mEmplDistributor.maxEmployees(s);
    const double frac = e/static_cast<double>(maxE);
    const auto& sb = mSectorBuildings[s];
    struct eSectorReminder {
        double fRem;
        eEmployingBuilding* fB;
    };

    std::vector<eSectorReminder> reminders;
    for(const auto b : sb) {
        const auto type = b->type();
        if(type == eBuildingType::triremeWharf) {
            const bool sd = b->shutDown();
            if(sd) continue;
        }
        const bool sd = isShutDown(type);
        if(sd) {
            b->setShutDown(true);
            continue;
        } else {
            b->setShutDown(false);
        }
        const int me = b->maxEmployees();
        const double eeF = frac*me;
        const int ee = std::floor(eeF);
        b->setEmployed(ee);
        e -= ee;
        reminders.push_back({(eeF - ee)/me, b});
    }

    const auto comp = [](const eSectorReminder& r1, const eSectorReminder& r2) {
        return r1.fRem > r2.fRem;
    };
    std::sort(reminders.begin(), reminders.end(), comp);

    while(e > 0) {
        bool changed = false;
        for(auto& r : reminders) {
            if(e <= 0) break;
            const auto b = r.fB;
            const int me = b->maxEmployees();
            const int ee = b->employed();
            if(ee >= me) continue;
            b->setEmployed(ee + 1);
            e--;
            changed = true;
        }
        if(!changed) break;
    }
}

void eBoardCity::distributeEmployees() {
    for(const auto& s : mSectorBuildings) {
        distributeEmployees(s.first);
    }
}

bool eBoardCity::isShutDown(const eResourceType type) const {
    return eVectorHelpers::contains(mShutDown, type);
}

bool eBoardCity::isShutDown(const eBuildingType type) const {
    if(type == eBuildingType::tower) return !mManTowers;
    const auto is = eIndustryHelpers::sIndustries(type);
    if(is.empty()) return false;
    for(const auto i : is) {
        const bool sd = isShutDown(i);
        if(!sd) return false;
    }
    return true;
}

void eBoardCity::incDistributeEmployees(const int by) {
    const int employmentUpdateWait = 5678;
    mEmploymentUpdateWait += by;
    if(mEmploymentUpdateWait > employmentUpdateWait) {
        if(mEmploymentUpdateScheduled) {
            mEmploymentUpdateWait = 0;
            distributeEmployees();
        }
    }
}

void eBoardCity::incPopulation(const int by) {
    mPopData.incPopulation(by);
    mEmploymentUpdateScheduled = true;
}

void eBoardCity::addShutDown(const eResourceType type) {
    mShutDown.push_back(type);
    for(const auto b : mEmployingBuildings) {
        const auto bt = b->type();
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) continue;
        const bool r = eVectorHelpers::contains(is, type);
        if(!r) continue;
        const bool sd = is.size() == 1 ? true : isShutDown(bt);
        if(!sd) continue;
        b->setShutDown(true);
        const int maxE = b->maxEmployees();
        mEmplData.incTotalJobVacancies(-maxE);
        eSector s;
        const bool ss = eSectorHelpers::sBuildingSector(bt, s);
        if(!ss) continue;
        mEmplDistributor.incMaxEmployees(s, -maxE);
    }
    distributeEmployees();
}

void eBoardCity::removeShutDown(const eResourceType type) {
    const auto oldShutDown = mShutDown;
    const auto wasShutDown = [&](const eBuildingType bt) {
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) return false;
        for(const auto i : is) {
            const bool sd = eVectorHelpers::contains(oldShutDown, i);
            if(!sd) return false;
        }
        return true;
    };
    const bool r = eVectorHelpers::remove(mShutDown, type);
    if(!r) return;
    for(const auto b : mEmployingBuildings) {
        const auto bt = b->type();
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) continue;
        const bool r = eVectorHelpers::contains(is, type);
        if(!r) continue;
        const bool sd = is.size() == 1 ? false : isShutDown(bt);
        if(sd) continue;
        const bool wasSD = wasShutDown(bt);
        if(!wasSD) continue;
        b->setShutDown(false);
        const int maxE = b->maxEmployees();
        mEmplData.incTotalJobVacancies(maxE);
        eSector s;
        const bool ss = eSectorHelpers::sBuildingSector(bt, s);
        if(!ss) continue;
        mEmplDistributor.incMaxEmployees(s, maxE);
    }
    distributeEmployees();
}

int eBoardCity::industryJobVacancies(const eResourceType type) const {
    const auto bs = eIndustryHelpers::sBuildings(type);
    int result = 0;
    for(const auto b : bs) {
        result += countBuildings(b);
    }
    return result;
}

std::vector<eBuilding*> eBoardCity::buildings(const eBuildingValidator& v) const {
    std::vector<eBuilding*> result;
    for(const auto b : mTimedBuildings) {
        const bool r = v(b);
        if(r) result.push_back(b);
    }
    return result;
}

std::vector<eBuilding*> eBoardCity::buildings(const eBuildingType type) const {
    return buildings([type](eBuilding* const b) {
        const auto bt = b->type();
        return type == bt;
    });
}

int eBoardCity::countBuildings(const eBuildingValidator& v) const {
    const auto vv = buildings(v);
    return vv.size();
}

int eBoardCity::countBuildings(const eBuildingType t) const {
    return countBuildings([t](eBuilding* const b) {
        const auto bt = b->type();
        return bt == t;
    });
}

bool eBoardCity::hasBuilding(const eBuildingType t) const {
    for(const auto b : mTimedBuildings) {
        const bool r = t == b->type();
        if(r) return true;
    }
    return false;
}

int eBoardCity::countAllowed(const eBuildingType t) const {
    eBuildingType parent;

    if(t == eBuildingType::sheep) {
        parent = eBuildingType::cardingShed;
    } else if(t == eBuildingType::goat) {
        parent = eBuildingType::dairy;
    } else if(t == eBuildingType::cattle) {
        parent = eBuildingType::corral;
    } else {
        return 0;
    }
    const int already = countBuildings(t);

    return 8*countBuildings(parent) - already;
}

eBuilding* eBoardCity::randomBuilding(const eBuildingValidator& v) const {
    auto blds = mTimedBuildings;
    eRand::randomShuffle(blds);
    for(const auto b : blds) {
        const bool r = v(b);
        if(r) return b;
    }
    return nullptr;
}

eTile* eBoardCity::randomTile() const {
    if(mTiles.empty()) return nullptr;
    const int id = eRand::rand() % mTiles.size();
    return mTiles[id];
}

std::vector<bool> eBoardCity::pyramidLevels(const eBuildingType type) const {
    return mAvailableBuildings.pyramidLevels(type);
}

double coverageMultiplier(const int pop) {
    if(pop < 250) return 0.125;
    else if(pop < 500) return 0.25;
    else if(pop < 1000) return 0.375;
    else return 0.5;
}

double eBoardCity::coverageMultiplier() const {
    const int pop = mPopData.population();
    return ::coverageMultiplier(pop);
}

double eBoardCity::winningChance(const eGames game) const {
    int coverage = 0;
    switch(game) {
    case eGames::isthmian:
        coverage = mPhilosophyCoverage;
        break;
    case eGames::nemean:
        coverage = mAthleticsCoverage;
        break;
    case eGames::pythian:
        coverage = mDramaCoverage;
        break;
    case eGames::olympian:
        coverage = mAllDiscCoverage;
        break;
    }

    const double mult = coverageMultiplier();

    if(mult*coverage < 15) {
        return 0.;
    } else {
        const double coveragef = coverage/100.;
        const double chance = mult*coveragef*coveragef;
        return chance;
    }
}

void eBoardCity::updateResources() {
    for(auto& r : mResources) {
        int& count = r.second;
        count = 0;
        const auto type = r.first;
        for(const auto s : mStorBuildings) {
            count += s->count(type);
        }
    }
}

int eBoardCity::resourceCount(const eResourceType type) const {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) return p->drachmas();
        return 0;
    }
    int result = 0;
    for(auto& r : mResources) {
        if(!static_cast<bool>(r.first & type)) continue;
        result += r.second;
    }
    return result;
}

int eBoardCity::takeResource(const eResourceType type, const int count) {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) p->incDrachmas(-count, eFinanceTarget::giftsAndAidGiven);
        return count;
    }
    int r = 0;
    using eValidator = std::function<bool(eStorageBuilding*)>;
    const auto takeFunc = [&](const eValidator& v) {
        if(r >= count) return;
        for(const auto s : mStorBuildings) {
            if(!v(s)) continue;
            if(r >= count) return;
            r += s->take(type, count - r);
        }
    };
    takeFunc([&](eStorageBuilding* const s) {
        return s->empties(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        return !s->accepts(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        return !s->get(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        (void)s;
        return true;
    });
    return r;
}

int eBoardCity::addResource(const eResourceType type, const int count) {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) p->incDrachmas(count, eFinanceTarget::giftsReceived);
        return count;
    }
    int rem = count;
    using eValidator = std::function<bool(eStorageBuilding*)>;
    const auto addFunc = [&](const eValidator& v) {
        if(rem <= 0) return;
        for(const auto s : mStorBuildings) {
            if(!v(s)) continue;
            const int c = s->add(type, rem);
            rem -= c;
            if(rem <= 0) break;
        }
    };
    addFunc([&](eStorageBuilding* const s) {
        return s->get(type);
    });
    addFunc([&](eStorageBuilding* const s) {
        (void)s;
        return true;
    });
    return count - rem;
}

int eBoardCity::spaceForResource(const eResourceType type) const {
    if(type == eResourceType::drachmas) {
        return __INT_MAX__/2;
    }
    int r = 0;
    for(const auto s : mStorBuildings) {
        r += s->spaceLeft(type);
    }
    return r;
}

int eBoardCity::maxSingleSpaceForResource(
        const eResourceType type,
        eStorageBuilding** b) const {
    *b = nullptr;
    int r = 0;
    for(const auto s : mStorBuildings) {
        const int ss = s->spaceLeft(type);
        if(ss > r) {
            *b = s;
            r = ss;
        }
    }
    return r;
}

int eBoardCity::waitingCount(const eResourceType type) const {
    const bool single = eResourceTypeHelpers::isSingleType(type);
    if(single) {
        const auto it = mWaiting.find(type);
        if(it == mWaiting.end()) return 0;
        return it->second;
    }
    int result = 0;
    for(auto& r : mWaiting) {
        if(!static_cast<bool>(r.first & type)) continue;
        result += r.second;
    }
    return result;
}

void eBoardCity::incWaitingCount(const eResourceType type, const int by) {
    mWaiting[type] += by;
}

int eBoardCity::maxMonumentSpaceForResource(
        eMonument** b) const {
    *b = nullptr;
    int r = 0;
    for(const auto s : mMonuments) {
        int ss = s->spaceLeft(eResourceType::wood);
        ss += s->spaceLeft(eResourceType::marble);
        ss += s->spaceLeft(eResourceType::sculpture);
        ss += s->spaceLeft(eResourceType::blackMarble);
        ss += s->spaceLeft(eResourceType::orichalc);
        if(ss > r) {
            *b = s;
            r = ss;
        }
    }
    return r;
}

void eBoardCity::killCommonFolks(int toKill) {
    auto bs = mTimedBuildings;
    eRand::randomShuffle(bs);
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::commonHouse) {
            const auto sh = static_cast<eSmallHouse*>(b);
            const int pop = sh->people();
            const int shk = std::min(toKill, pop);
            toKill -= shk;
            sh->kill(shk);
            if(toKill <= 0) break;
        }
    }
}

void eBoardCity::walkerKilled() {
    killCommonFolks(5);
}

void eBoardCity::rockThrowerKilled() {
    killCommonFolks(2*eNumbers::sRabbleDivisor);
}

void eBoardCity::hopliteKilled() {
    auto bs = mTimedBuildings;
    eRand::randomShuffle(bs);
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int pop = eh->people();
            const int shk = std::min(4, pop);
            eh->kill(shk);
            eh->removeArmor();
            return;
        }
    }
}

void eBoardCity::horsemanKilled() {
    auto bs = mTimedBuildings;
    eRand::randomShuffle(bs);
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int pop = eh->people();
            const int shk = std::min(4, pop);
            eh->kill(shk);
            eh->removeArmor();
            eh->removeHorse();
            return;
        }
    }
}

int eBoardCity::missingArmorFromEliteHouses() const {
    int result = 0;
    for(const auto b : mTimedBuildings) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int a = eh->arms();
            result += 4 - a;
        }
    }
    return result;
}

int eBoardCity::maxPalaceBannerCount() const {
    const auto pid = owningPlayerId();
    const auto ppid = mBoard.personPlayer();
    int nSpaces = sPalaceTiles;
    if(pid == ppid) return nSpaces;
    for(const auto& b : mPalaceSoldierBanners) {
        const bool a = b->isAbroad();
        if(a) nSpaces++;
    }
    return nSpaces;
}

void eBoardCity::soldierBannersUpdate() {
    updateMaxSoldiers();
    distributeSoldiers();
    consolidateSoldiers();
}

void eBoardCity::updateMaxSoldiers() {
    mMaxRabble = 0;
    mMaxHoplites = 0;
    mMaxHorsemen = 0;
    if(!mPalace) return;
    for(const auto b : mTimedBuildings) {
        const auto bt = b->type();
        if(bt == eBuildingType::commonHouse) {
            const auto ch = static_cast<eSmallHouse*>(b);
            const int l = ch->level();
            if(l < 2) continue;
            int lvlMax = 0;
            if(l == 2) lvlMax = 5;
            else if(l == 3) lvlMax = 6;
            else if(l == 4) lvlMax = 10;
            else if(l == 5) lvlMax = 12;
            else if(l == 6) lvlMax = 15;
            const int pop = ch->people();
            const int popMax = pop/4;
            mMaxRabble += std::min(lvlMax, popMax);
        } else if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int l = eh->level();
            if(l < 2) continue;
            const int a = eh->arms();
            if(l == 2) {
                mMaxHoplites += std::min(2, a);
            } else if(l == 3) {
                mMaxHoplites += std::min(4, a);
            } else if(l == 4) {
                const int h = eh->horses();
                const int hh = std::min(std::min(a, 4), h);
                mMaxHorsemen += hh;
                mMaxHoplites += std::min(4 - hh, a - hh);
            }
        }
    }
    mMaxRabble /= eNumbers::sRabbleDivisor;

    const int spb = eNumbers::sSoldiersPerBanner;
    const int nSpaces = maxPalaceBannerCount();
    mMaxHorsemen = std::min(spb*nSpaces, mMaxHorsemen);
    mMaxHorsemen = std::max(0, mMaxHorsemen);
    const int nHorsemenB = std::ceil(mMaxHorsemen/double(spb));
    mMaxHoplites = std::min(spb*nSpaces - spb*nHorsemenB, mMaxHoplites);
    mMaxHoplites = std::max(0, mMaxHoplites);
    const int nHoplitesB = std::ceil(mMaxHoplites/double(spb));
    mMaxRabble = std::min(spb*nSpaces - spb*nHorsemenB - spb*nHoplitesB, mMaxRabble);
    mMaxRabble = std::max(0, mMaxRabble);
}

void eBoardCity::distributeSoldiers() {
    int cRabble = 0;
    int cHoplites = 0;
    int cHorsemen = 0;
    for(const auto& b : mPalaceSoldierBanners) {
        const auto bt = b->type();
        const int c = b->count();
        if(bt == eBannerType::rockThrower) {
            cRabble += c;
        } else if(bt == eBannerType::hoplite) {
            cHoplites += c;
        } else if(bt == eBannerType::horseman) {
            cHorsemen += c;
        }
    }
    const int remRabble = mMaxRabble - cRabble;
    const int remHoplites = mMaxHoplites - cHoplites;
    const int remHorsemen = mMaxHorsemen - cHorsemen;

    for(int i = 0; i < -remRabble; i++) {
        removeSoldier(eCharacterType::rockThrower);
    }
    for(int i = 0; i < -remHoplites; i++) {
        removeSoldier(eCharacterType::hoplite);
    }
    for(int i = 0; i < -remHorsemen; i++) {
        removeSoldier(eCharacterType::horseman);
    }
    for(int i = 0; i < remHorsemen; i++) {
        addSoldier(eCharacterType::horseman);
    }
    for(int i = 0; i < remHoplites; i++) {
        addSoldier(eCharacterType::hoplite);
    }
    for(int i = 0; i < remRabble; i++) {
        addSoldier(eCharacterType::rockThrower);
    }
}

void eBoardCity::consolidateSoldiers() {
    using eSoldierBanners = std::vector<stdsptr<eSoldierBanner>>;
    eSoldierBanners rabble;
    eSoldierBanners hoplites;
    eSoldierBanners horsemen;
    for(const auto& s : mPalaceSoldierBanners) {
        if(s->isAbroad()) continue;
        const auto tile = s->tile();
        if(!tile) s->moveToDefault();
        if(!s->isHome()) continue;
        switch(s->type()) {
        case eBannerType::rockThrower:
            rabble.push_back(s);
            break;
        case eBannerType::hoplite:
            hoplites.push_back(s);
            break;
        case eBannerType::horseman:
            horsemen.push_back(s);
            break;
        default:
            break;
        }
    }
    const auto consolidator = [](const eSoldierBanners& banners) {
        for(int i = 0; i < static_cast<int>(banners.size()); i++) {
            const auto s = banners[i];
            const int sc = s->count();
            int sSpace = eNumbers::sSoldiersPerBanner - sc;
            if(sSpace <= 0) continue;
            for(int j = banners.size() - 1; j > i; j--) {
                const auto ss = banners[j];
                const int ssc = ss->count();
                const int kMax = std::min(sSpace, ssc);
                for(int k = 0; k < kMax; k++) {
                    ss->decCount();
                    s->incCount();
                    sSpace--;
                    if(sSpace <= 0) break;
                }
                if(sSpace <= 0) break;
            }
        }
    };
    consolidator(rabble);
    consolidator(hoplites);
    consolidator(horsemen);
}

void eBoardCity::addSoldier(const eCharacterType st) {
    bool found = false;
    for(const auto& b : mPalaceSoldierBanners) {
        if(b->isAbroad()) continue;
        const auto bt = b->type();
        const int c = b->count();
        if(c >= eNumbers::sSoldiersPerBanner) continue;
        if(bt == eBannerType::rockThrower &&
           st == eCharacterType::rockThrower) {
            found = true;
        } else if(bt == eBannerType::hoplite &&
                  st == eCharacterType::hoplite) {
            found = true;
        } else if(bt == eBannerType::horseman &&
                  st == eCharacterType::horseman) {
            found = true;
        }
        if(found) {
            b->incCount();
            break;
        }
    }
    if(found) return;
    const int nSpaces = maxPalaceBannerCount();
    const int nB = mPalaceSoldierBanners.size();
    if(nB >= nSpaces) return;
    eBannerType bt;
    if(st == eCharacterType::rockThrower) {
        bt = eBannerType::rockThrower;
    } else if(st == eCharacterType::hoplite) {
        bt = eBannerType::hoplite;
    } else if(st == eCharacterType::horseman) {
        bt = eBannerType::horseman;
    } else {
        return;
    }
    const auto b = e::make_shared<eSoldierBanner>(bt, mBoard);
    b->setBothCityIds(mId);
    registerSoldierBanner(b);
    b->incCount();
    b->moveToDefault();
}

void eBoardCity::removeSoldier(const eCharacterType st,
                               const bool skipNotHome) {
    for(const auto& b : mPalaceSoldierBanners) {
        if(b->isAbroad()) continue;
        if(skipNotHome && !b->isHome()) continue;
        const auto bt = b->type();
        const int c = b->count();
        if(c <= 0) continue;
        bool found = false;
        if(bt == eBannerType::rockThrower &&
           st == eCharacterType::rockThrower) {
            found = true;
        } else if(bt == eBannerType::hoplite &&
                  st == eCharacterType::hoplite) {
            found = true;
        } else if(bt == eBannerType::horseman &&
                  st == eCharacterType::horseman){
            found = true;
        }
        if(found) {
            b->decCount();
            return;
        }
    }
    if(skipNotHome) removeSoldier(st, false);
}

void eBoardCity::registerSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    if(b->cityId() != mId) return;
    switch(b->type()) {
    case eBannerType::enemy:
    case eBannerType::trireme:
        return;
    case eBannerType::rockThrower:
    case eBannerType::hoplite:
    case eBannerType::horseman:
        mPalaceSoldierBanners.push_back(b);
    default:
        mSoldierBanners.push_back(b);
    }
}

bool eBoardCity::unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    eVectorHelpers::remove(mPalaceSoldierBanners, b);
    return eVectorHelpers::remove(mSoldierBanners, b);
}

bool eBoardCity::personPlayerOwner() const {
    const auto ppid = mBoard.personPlayer();
    const auto pid = owningPlayerId();
    return ppid == pid;
}

ePlayerId eBoardCity::owningPlayerId() const {
    return mBoard.cityIdToPlayerId(mId);
}

eBoardPlayer* eBoardCity::owningPlayer() const {
    const auto pid = owningPlayerId();
    const auto p = mBoard.boardPlayerWithId(pid);
    return p;
}

bool eBoardCity::supportsResource(const eResourceType rt) const {
    const auto s = mSupportedResources & rt;
    return static_cast<bool>(s);
}

std::vector<eAgoraBase*> eBoardCity::agoras() const {
    std::vector<eAgoraBase*> r;
    for(const auto b : mAllBuildings) {
        const auto a = dynamic_cast<eAgoraBase*>(b);
        if(!a) continue;
        r.push_back(a);
    }
    return r;
}

int eBoardCity::eliteHouses() const {
    int r = 0;
    for(const auto b : mTimedBuildings) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) r++;
    }
    return r;
}

eSanctuary* eBoardCity::sanctuary(const eGodType god) const {
    for(const auto s : mSanctuaries) {
        if(s->godType() == god) return s;
    }
    return nullptr;
}

eHerosHall* eBoardCity::heroHall(const eHeroType hero) const {
    for(const auto h : mHeroHalls) {
        if(h->heroType() == hero) return h;
    }
    return nullptr;
}

int eBoardCity::countBanners(const eBannerType bt) const {
    int c = 0;
    for(const auto& bn : mSoldierBanners) {
        if(bn->type() != bt) continue;
        c++;
    }
    return c;
}

int eBoardCity::countSoldiers(const eBannerType bt) const {
    int c = 0;
    for(const auto& bn : mSoldierBanners) {
        if(bn->type() != bt) continue;
        c += bn->count();
    }
    return c;
}

int eBoardCity::countWorkingTriremes() const {
    int c = 0;
    for(const auto b : mTimedBuildings) {
        const auto type = b->type();
        if(type != eBuildingType::triremeWharf) continue;
        const auto tw = static_cast<eTriremeWharf*>(b);
        if(tw->hasTrireme()) c++;
    }
    return c;
}

ePyramid* eBoardCity::pyramid(const eBuildingType type) const {
    for(const auto b : mTimedBuildings) {
        const auto t = b->type();
        if(t == type) return static_cast<ePyramid*>(b);
    }
    return nullptr;
}

std::vector<ePyramid*> eBoardCity::pyramids() const {
    std::vector<ePyramid*> result;
    for(const auto b : mTimedBuildings) {
        if(const auto p = dynamic_cast<ePyramid*>(b)) {
            result.push_back(p);
        }
    }
    return result;
}

void eBoardCity::startPlague(eSmallHouse* const h) {
    const auto plague = std::make_shared<ePlague>(mId, mBoard);
    plague->spreadFrom(h);
    mPlagues.push_back(plague);
}

stdsptr<ePlague> eBoardCity::plagueForHouse(eSmallHouse* const h) {
    if(!h) return nullptr;
    for(const auto& p : mPlagues) {
        if(p->hasHouse(h)) return p;
    }
    return nullptr;
}

void eBoardCity::healPlague(const stdsptr<ePlague>& p) {
    p->healAll();
    eVectorHelpers::remove(mPlagues, p);
}

const eBoardCity::ePlagues& eBoardCity::plagues() const {
    return mPlagues;
}

stdsptr<ePlague> eBoardCity::nearestPlague(
        const int tx, const int ty, int& dist) const {
    dist = __INT_MAX__/2;
    stdsptr<ePlague> result;
    for(const auto& p : mPlagues) {
        const auto& hs = p->houses();
        for(const auto h : hs) {
            const auto tt = h->centerTile();
            const int ttx = tt->x();
            const int tty = tt->y();
            const int d = sqrt((ttx - tx)*(ttx - tx) +
                               (tty - ty)*(tty - ty));
            if(d < dist) {
                dist = d;
                result = p;
            }
        }
    }
    return result;
}

eBanner* eBoardCity::banner(const eBannerTypeS type, const int id) const {
    for(const auto& b : mBanners) {
        const int bid = b->id();
        if(bid != id) continue;
        const auto btype = b->type();
        if(btype != type) continue;
        return b;
    }
    return nullptr;
}

eTile* eBoardCity::entryPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::entryPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

eTile* eBoardCity::exitPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::exitPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

eTile* eBoardCity::riverEntryPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::riverEntryPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

eTile* eBoardCity::riverExitPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::riverExitPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

void eBoardCity::registerBanner(eBanner* const b) {
    const int id = b->id();
    const auto type = b->type();
    const auto bb = banner(type, id);
    const auto t = bb ? bb->tile() : nullptr;
    if(t) t->removeBanner(bb);
    mBanners.push_back(b);
}

void eBoardCity::unregisterBanner(eBanner* const b) {
    eVectorHelpers::remove(mBanners, b);
}

eTile* eBoardCity::monsterTile(const int id) const {
    const auto b = banner(eBannerTypeS::monsterPoint, id);
    return b ? b->tile() : nullptr;
}

eTile* eBoardCity::landInvasionTile(const int id) const {
    const auto b = banner(eBannerTypeS::landInvasion, id);
    return b ? b->tile() : nullptr;
}

eTile* eBoardCity::seaInvasionTile(const int id) const {
    const auto b = banner(eBannerTypeS::seaInvasion, id);
    return b ? b->tile() : nullptr;
}

eTile* eBoardCity::disasterTile(const int id) const {
    const auto b = banner(eBannerTypeS::disasterPoint, id);
    return b ? b->tile() : nullptr;
}

eTile* eBoardCity::landSlideTile(const int id) const {
    const auto b = banner(eBannerTypeS::landSlidePoint, id);
    return b ? b->tile() : nullptr;
}

void eBoardCity::musterAllSoldiers() {
    for(const auto& s : mSoldierBanners) {
        s->backFromHome();
    }
}

void eBoardCity::sendAllSoldiersHome() {
    for(const auto& s : mSoldierBanners) {
        s->goHome();
    }
}

eEnlistedForces eBoardCity::getEnlistableForces() const {
    eEnlistedForces result;

    for(const auto& s : mSoldierBanners) {
        if(s->count() <= 0) continue;
        switch(s->type()) {
        case eBannerType::hoplite:
        case eBannerType::horseman:
        case eBannerType::amazon:
        case eBannerType::aresWarrior:
            result.fSoldiers.push_back(s);
            break;
        default:
            break;
        }
    }

    for(const auto& h : mHeroHalls) {
        const auto s = h->stage();
        if(s != eHeroSummoningStage::arrived) continue;
        const auto ht = h->heroType();
        result.fHeroes.push_back({mId, ht});
    }

    return result;
}

void eBoardCity::addMonsterEvent(const eMonsterType type, eMonsterInvasionEventBase * const e) {
    mMonsterEvents[type] = e;
}

void eBoardCity::removeMonsterEvent(eMonsterInvasionEventBase * const e) {
    for(const auto& m : mMonsterEvents) {
        if(m.second != e) continue;
        mMonsterEvents.erase(m.first);
        break;
    }
}

void eBoardCity::addInvasionHandler(eInvasionHandler* const i) {
    mInvasionHandlers.push_back(i);
}

void eBoardCity::removeInvasionHandler(eInvasionHandler* const i) {
    eVectorHelpers::remove(mInvasionHandlers, i);
}

bool eBoardCity::hasActiveInvasions() const {
    return !mInvasionHandlers.empty();
}

const std::vector<eMonster*>& eBoardCity::monsters() const {
    return mMonsters;
}

void eBoardCity::registerMonster(eMonster* const m) {
    mMonsters.push_back(m);
}

void eBoardCity::unregisterMonster(eMonster* const m) {
    eVectorHelpers::remove(mMonsters, m);
}

void eBoardCity::monsterSlayed(const eMonsterType m) {
    const auto it = mMonsterEvents.find(m);
    if(it == mMonsterEvents.end()) return;
    const auto e = it->second;
    e->killed(m);
    mMonsterEvents.erase(m);
}

const eBoardCity::eChars& eBoardCity::attackingGods() const {
    return mAttackingGods;
}

void eBoardCity::registerAttackingGod(eCharacter* const c) {
    mAttackingGods.push_back(c);
}

bool eBoardCity::unregisterAttackingGod(eCharacter* const c) {
    return eVectorHelpers::remove(mAttackingGods, c);
}

bool eBoardCity::nearestEnemySoldier(const eTeamId tid,
                                     const int tx, const int ty,
                                     int& nX, int& nY) const {
    bool found = false;
    bool kNoTles = true;
    const auto prcs = [&](const int dx, const int dy) {
        const int x = tx + dx;
        const int y = ty + dy;
        const auto tile = mBoard.tile(x, y);
        if(!tile) return false;
        const auto tcid = tile->cityId();
        if(tcid != mId) return false;
        kNoTles = false;
        const auto& chars = tile->characters();
        for(const auto& c : chars) {
            const auto ctid = c->teamId();
            const bool e = eTeamIdHelpers::isEnemy(tid, ctid);
            if(e) {
                found = true;
                nX = x;
                nY = y;
                return true;
            }
        }
        return false;
    };
    for(int k = 0;; k++) {
        kNoTles = true;
        eIterateSquare::iterateSquare(k, prcs);
        if(kNoTles) return false;
        if(found) return true;
    }
    return false;
}

eMilitaryAid* eBoardCity::militaryAid(const stdsptr<eWorldCity>& c) const {
    for(const auto& m : mMilitaryAid) {
        if(m->fCity == c) return m.get();
    }
    return nullptr;
}

void eBoardCity::removeMilitaryAid(const stdsptr<eWorldCity>& c) {
    const int iMax = mMilitaryAid.size();
    for(int i = 0; i < iMax; i++) {
        const auto& m = mMilitaryAid[i];
        if(m->fCity != c) continue;
        mMilitaryAid.erase(mMilitaryAid.begin() + i);
        break;
    }
}

void eBoardCity::addMilitaryAid(const stdsptr<eMilitaryAid>& a) {
    mMilitaryAid.push_back(a);
}

bool eBoardCity::wasHeroSummoned(const eHeroType hero) const {
    return eVectorHelpers::contains(mSummonedHeroes, hero);
}

void eBoardCity::heroSummoned(const eHeroType hero) {
    mSummonedHeroes.push_back(hero);
}

void eBoardCity::addRootGameEvent(const stdsptr<eGameEvent>& e) {
    mCityEvents.addEvent(e);
}

void eBoardCity::removeRootGameEvent(const stdsptr<eGameEvent>& e) {
    mCityEvents.removeEvent(e);
}

void eBoardCity::addExported(const eCityId cid,
                             const eResourceType type,
                             const int count) {
    auto& map = mExported[cid];
    const auto it = map.find(type);
    if(it == map.end()) {
        map[type] = count;
    } else {
        map[type] += count;
    }
}

void eBoardCity::removeExported(const eCityId cid,
                                const eResourceType type,
                                const int count) {
    auto& map = mExported[cid];
    const auto it = map.find(type);
    if(it != map.end()) {
        map[type] -= count;
    }
}

int eBoardCity::exported(const eCityId cid, const eResourceType type) {
    auto& map = mExported[cid];
    const auto it = map.find(type);
    if(it != map.end()) {
        return map[type];
    } else {
        return 0;
    }
}

std::map<eResourceType, int> eBoardCity::exported(const eCityId cid) const {
    const auto it = mExported.find(cid);
    if(it == mExported.end()) return {};
    return it->second;
}

void eBoardCity::incTerrainState() {
    mTerrainState++;
    mResourceTilesUpdate = true;
}

void eBoardCity::incForestsState() {
    mForestsState++;
    mForestTilesUpdate = true;
}

const std::vector<eTile*>& eBoardCity::animalBuildingsTiles() {
    if(mAnimalBuildingsSurroundingUpdate) {
        mAnimalBuildingsSurrounding.clear();
        const int range = eNumbers::sAnimalMoveRange + 2;
        for(const auto b : mAnimalBuildings) {
            const auto center = b->centerTile();
            const int x = center->x();
            const int y = center->y();
            for(int i = x - range; i <= x + range; i++) {
                for(int j = y - range; j <= y + range; j++) {
                    const auto tile = mBoard.tile(i, j);
                    if(!tile) continue;
                    const auto cid = tile->cityId();
                    if(cid != mId) continue;
                    const bool r = eVectorHelpers::contains(
                                       mAnimalBuildingsSurrounding, tile);
                    if(r) continue;
                    mAnimalBuildingsSurrounding.push_back(tile);
                }
            }
        }
        mAnimalBuildingsSurroundingUpdate = false;
    }
    return mAnimalBuildingsSurrounding;
}

const std::vector<eTile*>& eBoardCity::huntingTiles() {
    if(mHuntingTilesUpdate) {
        mHuntingTiles.clear();
        const int range = eNumbers::sAnimalMoveRange + 2;
        for(const auto s : mSpawners) {
            const auto type = s->type();
            if(type != eBannerTypeS::boar &&
               type != eBannerTypeS::deer) continue;
            const auto tile = s->tile();
            const int x = tile->x();
            const int y = tile->y();
            for(int i = x - range; i <= x + range; i++) {
                for(int j = y - range; j <= y + range; j++) {
                    const auto tile = mBoard.tile(i, j);
                    if(!tile) continue;
                    const auto cid = tile->cityId();
                    if(cid != mId) continue;
                    const bool r = eVectorHelpers::contains(
                                       mHuntingTiles, tile);
                    if(r) continue;
                    mHuntingTiles.push_back(tile);
                }
            }
        }
        mHuntingTilesUpdate = false;
    }
    return mHuntingTiles;
}

const std::vector<eTile*>& eBoardCity::resourceTiles() {
    if(mResourceTilesUpdate) {
        updateResourceTiles();
        mResourceTilesUpdate = false;
    }
    return mResourceTiles;
}

const std::vector<eTile*>& eBoardCity::forestTiles() {
    if(mForestTilesUpdate) {
        updateForestTiles();
        mForestTilesUpdate = false;
    }
    return mForestTiles;
}

int eBoardCity::workingHippodrome() const {
    int result = 0;
    for(const auto& h : mHippodromes) {
        const bool w = h->working();
        if(!w) continue;
        const int r = h->length();
        if(r > result) result = r;
    }
    return result;
}

void eBoardCity::setInvasionHandlersIOIDs(int& id) {
    for(const auto i : mInvasionHandlers) {
        i->setIOID(id++);
    }
}

eInvasionHandler* eBoardCity::invasionHandlerWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto i : mInvasionHandlers) {
        const int iio = i->ioID();
        if(iio == id) return i;
    }
    return nullptr;
}

bool eBoardCity::handleEpisodeCompleteEvents() {
    return mCityEvents.handleEpisodeCompleteEvents();
}

void eBoardCity::updateHippodromes() {
    for(int i = 0; i < (int)mHippodromes.size(); i++) {
        const auto& h = mHippodromes[i];
        if(!h->closed()) {
            h->clear();
            mHippodromes.erase(mHippodromes.begin() + i);
            i--;
        }
    }

    const auto hps = buildings(eBuildingType::hippodromePiece);
    for(const auto b : hps) {
        const auto hp = static_cast<eHippodromePiece*>(b);
        {
            const auto h = hp->hippodrome();
            if(h) continue;
        }
        const auto h = std::make_shared<eHippodrome>(mId, mBoard);
        h->addPieces(hp);
        mHippodromes.push_back(h);
    }
}

void eBoardCity::clearHippodromes() {
    for(const auto& h : mHippodromes) {
        h->clear();
    }
    mHippodromes.clear();
}

void eBoardCity::addReinforcements(const eCityId fromCid,
                                   const eEnlistedForces& forces) {
    mReinforcements.emplace_back(eReinforcements{forces, fromCid});
}

void eBoardCity::reinforcementsGoHome(const stdsptr<eSoldierBanner>& b) {
    int i = -1;
    for(auto& r : mReinforcements) {
        i++;
        const bool result = r.remove(b);
        if(!result) continue;
        const bool e = r.checkEmpty();
        if(e) {
            mReinforcements.erase(mReinforcements.begin() + i);
        }
        break;
    }
}

void eBoardCity::sendAllReinforcementsHome() {
    const auto& world = mBoard.world();
    for(const auto& r : mReinforcements) {
        const auto fromCid = r.fromCid();
        const auto c = mBoard.boardCityWithId(fromCid);
        if(!c) continue;
        const auto& forces = r.forces();
        if(forces.fSoldiers.empty()) return;
        const auto fromC = world.cityWithId(fromCid);
        const auto e = e::make_shared<eArmyReturnEvent>(
            fromCid, eGameEventBranch::root, mBoard);
        const auto boardDate = mBoard.date();
        const int period = eNumbers::sReinforcementsTravelTime;
        const auto date = boardDate + period;
        e->initializeDate(date, period, 1);
        e->initialize(forces, fromC);
        c->addRootGameEvent(e);
    }
    mReinforcements.clear();
}

void eBoardCity::clearAfterLastEpisode() {
    mCityEvents.clearAfterLastEpisode();
}

void eBoardCity::startEpisode(eEpisode* const e) {
    const auto& es = e->fEvents[mId];
    const auto date = mBoard.date();
    for(const auto& ee : es) {
        const auto eee = ee->makeCopy();
        eee->setupStartDate(date);
        mCityEvents.addEvent(eee);
        eee->loadResources();
    }

    const auto& ab = e->fAvailableBuildings[mId];
    mAvailableBuildings.startEpisode(ab);

    const auto it = e->fMaxSanctuaries.find(mId);
    if(it != e->fMaxSanctuaries.end()) {
        mMaxSanctuaries = it->second;
    }

    mNextAttackDate = date;
    mNextAttackDate.nextYears(5);
    mNextAttackPlanned = true;

    setFriendlyGods(e->fFriendlyGods[mId]);
}

void eBoardCity::loadResources() {
    mCityEvents.loadResources();
}

void eBoardCity::setFriendlyGods(const std::vector<eGodType>& gods) {
    for(const auto g : gods) {
        eBuildingType bt;
        switch(g) {
        case eGodType::aphrodite:
            bt = eBuildingType::templeAphrodite;
            break;
        case eGodType::apollo:
            bt = eBuildingType::templeApollo;
            break;
        case eGodType::ares:
            bt = eBuildingType::templeAres;
            break;
        case eGodType::artemis:
            bt = eBuildingType::templeArtemis;
            break;
        case eGodType::athena:
            bt = eBuildingType::templeAthena;
            break;
        case eGodType::atlas:
            bt = eBuildingType::templeAtlas;
            break;
        case eGodType::demeter:
            bt = eBuildingType::templeDemeter;
            break;
        case eGodType::dionysus:
            bt = eBuildingType::templeDionysus;
            break;
        case eGodType::hades:
            bt = eBuildingType::templeHades;
            break;
        case eGodType::hephaestus:
            bt = eBuildingType::templeHephaestus;
            break;
        case eGodType::hera:
            bt = eBuildingType::templeHera;
            break;
        case eGodType::hermes:
            bt = eBuildingType::templeHermes;
            break;
        case eGodType::poseidon:
            bt = eBuildingType::templePoseidon;
            break;
        case eGodType::zeus:
            bt = eBuildingType::templeZeus;
            break;
        default:
            bt = eBuildingType::none;
            break;
        }

        allow(bt);
    }

    const auto e = e::make_shared<eGodVisitEvent>(
        mId, eGameEventBranch::root, mBoard);
    e->setIsEpisodeEvent(true);
    eDate date = mBoard.date();
    const int period = eNumbers::sFriendlyGodVisitPeriod;
    date += period;
    date += eRand::rand() % 60;
    e->initializeDate(date, period, 10000);
    e->setTypes(gods);
    addRootGameEvent(e);
    e->loadResources();
}
