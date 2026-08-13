#include "efightingaction.h"

#include <math.h>

#include "characters/actions/ewaitaction.h"
#include "characters/efightingcharacter.h"
#include "emovetoaction.h"
#include "buildings/ebuilding.h"
#include "enumbers.h"
#include "vec2.h"

#include "missiles/erockmissile.h"
#include "missiles/earrowmissile.h"
#include "missiles/espearmissile.h"

eAttackTarget::eAttackTarget() :
    mC(nullptr), mB(nullptr) {}

eAttackTarget::eAttackTarget(eCharacter* const c) :
    mC(c), mB(nullptr) {}

eAttackTarget::eAttackTarget(eBuilding* const b) :
    mC(nullptr), mB(b) {}

eTile* eAttackTarget::tile() const {
    if(mC) return mC->tile();
    if(mB) return mB->centerTile();
    return nullptr;
}

bool eAttackTarget::valid() const {
    return mC || mB;
}

bool eAttackTarget::defend(const double a) {
    if(mC) return mC->defend(a);
    if(mB) return mB->defend(a);
    return true;
}

bool eAttackTarget::dead() const {
    if(mC) return mC->dead();
    if(mB) return false;
    return true;
}

void eAttackTarget::clear() {
    mC = nullptr;
    mB = nullptr;
}

bool eAttackTarget::building() const {
    return mB;
}

double eAttackTarget::absX() const {
    if(mC) return mC->absX();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->x();
    }
    return 0.;
}

double eAttackTarget::absY() const {
    if(mC) return mC->absY();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->y();
    }
    return 0.;
}

void eAttackTarget::read(eGameBoard &board, eReadStream &src) {
    src.readCharacter(&board, [this](eCharacter* const c) {
        mC = c;
    });
    src.readBuilding(&board, [this](eBuilding* const b) {
        mB = b;
    });
}

void eAttackTarget::write(eWriteStream &dst) const {
    dst.writeCharacter(mC);
    dst.writeBuilding(mB);
}

void eFightingAction::sSignalBeingAttack(
    eCharacter* const attacked,
    eCharacter* const by,
    eGameBoard& brd) {
    const auto tt = by->tile();
    if(!tt) return;
    const int ttx = tt->x();
    const int tty = tt->y();
    sSignalBeingAttack(attacked, ttx, tty, brd);
}

void eFightingAction::sSignalBeingAttack(eCharacter * const attacked,
                                         const int ttx, const int tty,
                                         eGameBoard &brd) {
    const auto att = attacked->tile();
    if(!att) return;
    const int attx = att->x();
    const int atty = att->y();
    const auto atid = attacked->teamId();
    const int range = eNumbers::sSoldierBeingAttackedCallRange;
    for(int ii = -range; ii <= range; ii++) {
        for(int jj = -range; jj <= range; jj++) {
            const auto tt = brd.tile(attx + ii, atty + jj);
            if(!tt) continue;
            const auto& ccchars = tt->characters();
            for(const auto& ccc : ccchars) {
                const auto sss = dynamic_cast<eFightingCharacter*>(ccc.get());
                if(!sss) continue;
                if(ccc->teamId() != atid) continue;
                if(ccc->dead()) continue;

                const auto aaa = sss->fightingAction();
                if(aaa) aaa->beingAttacked(ttx, tty);
            }
        }
    }
}

eLookForEnemyState eFightingAction::lookForEnemy(const int by) {
    const int rangeAttackCheck = 500;
    const int lookForEnemyCheck = 500;
    const int missileCheck = 200;
    const int buildingCheck = 5000;

    const auto c = character();
    if(c->dead()) return eLookForEnemyState::dead;
    int range = 0;
    if(const auto s = dynamic_cast<eFightingCharacter*>(c)) {
        range = s->range();
    }
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return eLookForEnemyState::dead;
    const int tx = ct->x();
    const int ty = ct->y();
    const auto tid = c->teamId();

    if(mAttack) {
        const auto at = c->actionType();
        if(range > 0 && mAttackTarget.valid() &&
            at == eCharacterActionType::fight2) {
            mMissile += by;
            if(mMissile > missileCheck) {
                mMissile = mMissile - missileCheck;
                const auto tt = mAttackTarget.tile();
                const int ttx = tt ? tt->x() : 0;
                const int tty = tt ? tt->y() : 0;
                const double dx = tx - ttx;
                const double dy = ty - tty;
                const double dist = sqrt(dx*dx + dy*dy);
                const auto ct = c->type();
                if(ct == eCharacterType::amazon ||
                    ct == eCharacterType::centaurArcher ||
                    ct == eCharacterType::egyptianArcher ||
                    ct == eCharacterType::atlanteanArcher ||
                    ct == eCharacterType::phoenicianArcher ||
                    ct == eCharacterType::persianArcher ||
                    ct == eCharacterType::trireme) {
                    eMissile::sCreate<eArrowMissile>(brd, tx, ty, 0.5,
                                                     ttx, tty, 0.5, 0.25*dist);
                } else if(ct == eCharacterType::trojanSpearthrower ||
                          ct == eCharacterType::oceanidSpearthrower) {
                    eMissile::sCreate<eSpearMissile>(brd, tx, ty, 0.5,
                                                     ttx, tty, 0.5, 0.1*dist);
                } else {
                    eMissile::sCreate<eRockMissile>(brd, tx, ty, 0.5,
                                                    ttx, tty, 0.5, 0.5*dist);
                }
            }
        }
        mAttackTime += by;
        bool finishAttack = !mAttackTarget.valid() ||
                            mAttackTarget.dead() ||
                            (mAttackTime > 1000 && !mAttackTarget.building());
        if(mAttackTarget.valid() && !mAttackTarget.dead()) {
            const double att = by*c->attack();
            const bool d = mAttackTarget.defend(att);
            if(d) finishAttack = true;
        }
        if(finishAttack) {
            mAttack = false;
            mAttackTarget.clear();
            mAttackTime = 0;
            mRangeAttack = rangeAttackCheck;
            c->setActionType(mSavedAction);
            c->setPlayFightSound(false);
            mLookForEnemy = lookForEnemyCheck;
        } else {
            return eLookForEnemyState::attacking;
        }
    }
    const vec2d cpos{c->absX(), c->absY()};

    const auto setAttackTarget = [&](const stdsptr<eCharacter>& cc,
                                     const bool range) {
        const vec2d ccpos{cc->absX(), cc->absY()};
        const vec2d posdif = ccpos - cpos;
        mAttackTarget = eAttackTarget(cc.get());
        mAttack = true;
        c->setPlayFightSound(true);
        mAttackTime = 0;
        mSavedAction = c->actionType();
        c->setActionType(range ? eCharacterActionType::fight2 :
                             eCharacterActionType::fight);
        mAngle = posdif.angle();
        const auto o = sAngleOrientation(mAngle);
        c->setOrientation(o);
    };

    mBuildingAttack += by;
    const bool buildingAttack = mBuildingAttack > buildingCheck;
    if(buildingAttack) {
        mBuildingAttack -= buildingCheck;
    }
    stdsptr<eCharacter> secondOption;
    stdsptr<eCharacter> thirdOption;
    // wolves and the like, taken once no enemy soldier is in reach
    stdsptr<eCharacter> animalOption;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            const auto t = brd.tile(tx + i, ty + j);
            if(!t) continue;
            const auto& chars = t->characters();
            for(const auto& cc : chars) {
                const auto cctype = cc->type();
                const auto cctid = cc->teamId();
                if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                if(cc->dead()) continue;
                const vec2d ccpos{cc->absX(), cc->absY()};
                const vec2d posdif = ccpos - cpos;
                const double dist = posdif.length();
                if(dist > 1.) continue;
                if(!cc->isSoldier()) {
                    if(cc->isImmortal()) {
                        thirdOption = cc;
                    } else if(cctype == eCharacterType::enemyBoat ||
                              cctype == eCharacterType::trireme) {
                        secondOption = cc;
                    } else if(cc->isHostileAnimal()) {
                        animalOption = cc;
                    }
                    continue;
                }
                setAttackTarget(cc, false);
                return eLookForEnemyState::attacking;
            }
            if(buildingAttack) {
                const bool r = attackBuilding(t, false);
                if(r) return eLookForEnemyState::attacking;
            }
        }
    }
    if(animalOption) {
        setAttackTarget(animalOption, false);
        return eLookForEnemyState::attacking;
    }
    if(secondOption) {
        setAttackTarget(secondOption, false);
        return eLookForEnemyState::attacking;
    }
    if(thirdOption) {
        setAttackTarget(thirdOption, false);
        return eLookForEnemyState::attacking;
    }

    if(range > 0) {
        mRangeAttack += by;
        if(mRangeAttack > rangeAttackCheck) {
            mRangeAttack -= rangeAttackCheck;
            for(int i = -range; i <= range; i++) {
                for(int j = -range; j <= range; j++) {
                    const auto t = brd.tile(tx + i, ty + j);
                    if(!t) continue;
                    const auto& chars = t->characters();
                    for(const auto& cc : chars) {
                        const auto cctype = cc->type();
                        const auto cctid = cc->teamId();
                        if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                        if(cc->dead()) continue;
                        if(!cc->isSoldier()) {
                            if(cc->isImmortal()) {
                                thirdOption = cc;
                            } else if(cctype == eCharacterType::enemyBoat ||
                                      cctype == eCharacterType::trireme) {
                                secondOption = cc;
                            } else if(cc->isHostileAnimal()) {
                                animalOption = cc;
                            }
                            continue;
                        }
                        setAttackTarget(cc, true);
                        sSignalBeingAttack(cc.get(), c, brd);
                        return eLookForEnemyState::attacking;
                    }
                    if(buildingAttack) {
                        const bool r = attackBuilding(t, true);
                        if(r) return eLookForEnemyState::attacking;
                    }
                }
            }
        }

        if(animalOption) {
            setAttackTarget(animalOption, true);
            sSignalBeingAttack(animalOption.get(), c, brd);
            return eLookForEnemyState::attacking;
        }
        if(secondOption) {
            setAttackTarget(secondOption, true);
            return eLookForEnemyState::attacking;
        }
        if(thirdOption) {
            setAttackTarget(thirdOption, true);
            return eLookForEnemyState::attacking;
        }
    }

    if(!currentAction() || mOverwrittableAction) {
        mLookForEnemy += by;
        if(mLookForEnemy > lookForEnemyCheck) {
            mLookForEnemy -= lookForEnemyCheck;
            const int erange = 3 + range;
            for(int i = -erange; i <= erange; i++) {
                for(int j = -erange; j <= erange; j++) {
                    const int ttx = tx + i;
                    const int tty = ty + j;
                    const auto t = brd.tile(ttx, tty);
                    if(!t) continue;
                    const auto& chars = t->characters();
                    for(const auto& cc : chars) {
                        if(!cc->isFighter() && !cc->isHostileAnimal()) continue;
                        const auto cctid = cc->teamId();
                        if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                        if(cc->dead()) continue;
                        setOverwrittableAction(false);
                        goTo(ttx, tty, range);
                        return eLookForEnemyState::attacking;
                    }
                }
            }
        }
    }

    return eLookForEnemyState::none;
}

bool eFightingAction::attackBuilding(eTile* const t, const bool range) {
    const auto ub = t->underBuilding();
    if(!ub) return false;
    const auto c = character();
    const auto tid = c->teamId();
    if(!eTeamIdHelpers::isEnemy(ub->teamId(), tid)) return false;
    const vec2d cpos{c->absX(), c->absY()};
    const bool att = eBuilding::sAttackable(ub->type());
    if(!att) return false;
    mAttackTarget = eAttackTarget(ub);
    mAttack = true;
    c->setPlayFightSound(true);
    mAttackTime = 0;
    mSavedAction = c->actionType();
    c->setActionType(range ? eCharacterActionType::fight2 :
                         eCharacterActionType::fight);
    const vec2d ccpos{1.*t->x(), 1.*t->y()};
    const vec2d posdif = ccpos - cpos;
    mAngle = posdif.angle();
    const auto o = sAngleOrientation(mAngle);
    c->setOrientation(o);
    return true;
}

void eFightingAction::goTo(const int fx, const int fy,
                           const int dist,
                           const eAction& findFailAct,
                           const eAction& findFinishAct) {
    const auto c = character();
    const auto t = c->tile();
    const int sx = t->x();
    const int sy = t->y();
    if(abs(fx - sx) <= dist && abs(fy - sy) <= dist) return;

    const auto hha = [fx, fy, dist](eThreadTile* const t) {
        return abs(t->x() - fx) <= dist && abs(t->y() - fy) <= dist;
    };

    const auto finishAct = std::make_shared<eSA_goToFinish>(
        board(), c);

    const auto tcid = t->cityId();
    auto& board = this->board();
    const auto ttid = board.cityIdToTeamId(tcid);
    const auto tid = c->teamId();
    const bool attackBuildings = eTeamIdHelpers::isEnemy(tid, ttid);
    stdsptr<eWalkableObject> pathFindWalkable;
    stdsptr<eWalkableObject> moveWalkable;
    if(c->isBoat()) {
        pathFindWalkable =
            eWalkableObject::sCreateDeepWater();
    } else {
        if(attackBuildings) {
            pathFindWalkable = eWalkableObject::sCreateAttacker();
            moveWalkable = eWalkableObject::sCreateDefault();
        } else {
            pathFindWalkable =
                eWalkableObject::sCreateDefault();
        }
    }

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finishAct);
    a->setFinishAction(finishAct);
    a->setFindFailAction(findFailAct);
    if(attackBuildings) {
        a->setObsticleHandler(obsticleHandler());
    }

    const stdptr<eCharacter> cptr(character());
    const stdptr<eFightingAction> tptr(this);
    a->setFoundAction([cptr, tptr, findFinishAct]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
        if(!tptr) return;
        if(findFinishAct) findFinishAct();
    });
    a->start(hha, pathFindWalkable, moveWalkable,
             [fx, fy](eThreadBoard& board) {
                 return board.tile(fx, fy);
             });
    setCurrentAction(a);
}

void eFightingAction::beingAttacked(eCharacter* const ss) {
    const auto tt = ss->tile();
    const int ttx = tt->x();
    const int tty = tt->y();
    beingAttacked(ttx, tty);
}

void eFightingAction::beingAttacked(const int ttx, const int tty) {
    if(mAttack) return;
    if(!mOverwrittableAction && currentAction()) return;
    setOverwrittableAction(false);
    goTo(ttx, tty);
}

void eFightingAction::read(eReadStream& src) {
    eComplexAction::read(src);
    src >> mAngle;
    src >> mMissile;
    src >> mRangeAttack;
    src >> mBuildingAttack;
    src >> mLookForEnemy;
    src >> mAttackTime;
    src >> mAttack;
    mAttackTarget.read(board(), src);
    src >> mSavedAction;
    src >> mOverwrittableAction;
}

void eFightingAction::write(eWriteStream& dst) const {
    eComplexAction::write(dst);
    dst << mAngle;
    dst << mMissile;
    dst << mRangeAttack;
    dst << mBuildingAttack;
    dst << mLookForEnemy;
    dst << mAttackTime;
    dst << mAttack;
    mAttackTarget.write(dst);
    dst << mSavedAction;
    dst << mOverwrittableAction;
}

void eFightingAction::waitAndGoHome(const int w) {
    const auto c = character();
    c->setActionType(eCharacterActionType::none);
    const auto finish = std::make_shared<eSA_waitAndGoHomeFinish>(
        board(), this);
    const auto a = e::make_shared<eWaitAction>(c);
    a->setFinishAction(finish);
    a->setTime(w);
    setCurrentAction(a);
}
