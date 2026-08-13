#include "egodmonsteraction.h"

#include "ewaitaction.h"
#include "emovearoundaction.h"
#include "emovetoaction.h"
#include "engine/egameboard.h"
#include "engine/boardData/eheatmap.h"
#include "engine/boardData/eheatmapdivisor.h"
#include "engine/boardData/eheatmaptask.h"

#include "etilehelper.h"
#include "missiles/egodmissile.h"

#include "egodattackaction.h"
#include "emonsteraction.h"
#include "epatrolmoveaction.h"
#include <cmath>


void eGodMonsterAction::appear() {
    const auto c = character();
    const auto ct = c->type();
    bool isGod;
    const auto gt = eGod::sCharacterToGodType(ct, &isGod);
    if(isGod) {
        if(gt == eGodType::hermes) {
            hermesRun(true);
        } else {
            const auto c = character();
            c->setActionType(eCharacterActionType::appear);
            const auto a = e::make_shared<eWaitAction>(c);
            const int time = eGod::sGodAppearTime(gt);
            a->setTime(time);
            setCurrentAction(a);
            playAppearSound();
        }
    }
}

void eGodMonsterAction::disappear(
        const bool die,
        const stdsptr<eCharActFunc>& finish) {
    const auto c = character();
    const auto ct = c->type();
    bool isGod;
    const auto gt = eGod::sCharacterToGodType(ct, &isGod);
    if(isGod) {
        if(gt == eGodType::hermes) {
            hermesRun(false);
        } else {
            const auto c = character();
            c->setActionType(die ? eCharacterActionType::die :
                                   eCharacterActionType::disappear);
            const auto a = e::make_shared<eWaitAction>(c);
            a->setFinishAction(finish);
            a->setFailAction(finish);
            a->setTime(500);
            setCurrentAction(a);
            playDisappearSound();
        }
    }
}

void eGodMonsterAction::teleport(eTile* const tile) {
    using eGA_TF = eGA_teleportFinish;
    const auto finish = std::make_shared<eGA_TF>(
                            board(), this, tile);
    disappear(false, finish);
}

void eGodMonsterAction::playAppearSound() {
    const auto c = character();
    auto& board = c->getBoard();
    const auto ct = c->type();
    bool isGod;
    const auto gt = eGod::sCharacterToGodType(ct, &isGod);
    if(!isGod) return;
    board.ifVisible(c->tile(), [gt]() {
        eSounds::playGodSound(gt, eGodSound::appear);
    });
}

void eGodMonsterAction::playDisappearSound() {
    const auto c = character();
    auto& board = c->getBoard();
    const auto ct = c->type();
    bool isGod;
    const auto gt = eGod::sCharacterToGodType(ct, &isGod);
    if(!isGod) return;
    board.ifVisible(c->tile(), [gt]() {
        eSounds::playGodSound(gt, eGodSound::disappear);
    });
}

void eGodMonsterAction::hermesRun(const bool appear) {
    const auto c = character();
    c->setSpeed(2.0);
    const auto tile = c->tile();
    const stdptr<eGodMonsterAction> tptr(this);
    using eGA_HRF = eGA_hermesRunFinish;
    const auto finish = std::make_shared<eGA_HRF>(
                            board(), this, c, appear);
    if(tile->hasRoad()) {
        patrol(finish, 20);
    } else {
        moveAround(finish, 1000);
    }
    if(appear) c->setActionType(eCharacterActionType::appear);
    else c->setActionType(eCharacterActionType::disappear);
    if(appear) playAppearSound();
}

void eGodMonsterAction::randomPlaceOnBoard() {
    const auto c = character();
    auto& board = c->getBoard();
    const auto cid = c->onCityId();
    const auto city = board.boardCityWithId(cid);
    const auto rtile = city->randomTile();
    const int tx = rtile->x();
    const int ty = rtile->y();
    const auto tile = eTileHelper::closestRoad(tx, ty, board);
    if(!tile) return;
    c->changeTile(tile);
}

void eGodMonsterAction::moveAround(
        const stdsptr<eCharActFunc>& finishAct,
        const int time,
        const stdsptr<eWalkableObject>& walkable) {
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    const auto t = c->tile();
    const int tx = t->x();
    const int ty = t->y();
    const auto a = e::make_shared<eMoveAroundAction>(
                       c, tx, ty, walkable);
    a->setFinishAction(finishAct);
    a->setFailAction(finishAct);
    a->setMaxDistance(10);
    a->setTime(time);
    setCurrentAction(a);
}

void eGodMonsterAction::goToTarget(const eHeatGetters::eHeatGetter hg,
                                   const stdsptr<eFindFailFunc>& findFailFunc,
                                   const stdsptr<eObsticleHandler>& oh,
                                   const eTileDistance& tileDistance,
                                   const stdsptr<eWalkableObject>& pathFindWalkable,
                                   const stdsptr<eWalkableObject>& moveWalkable) {
    const auto c = character();
    const stdptr<eGodMonsterAction> tptr(this);
    const stdptr<eCharacter> cptr(c);
    const auto hmFinish = [tptr, this, cptr,
                          c, findFailFunc, oh,
                          tileDistance,
                          pathFindWalkable,
                          moveWalkable](eHeatMap& map) {
        if(!tptr || !cptr) return;
        eHeatMapDivisor divisor(map);
        divisor.divide(10);
        int dtx;
        int dty;
        const bool r = divisor.randomHeatTile(dtx, dty);
        if(r) {
            auto& board = c->getBoard();
            int tx;
            int ty;
            eTileHelper::dtileIdToTileId(dtx, dty, tx, ty);
            const auto tile = eTileHelper::closestRoad(tx, ty, board);
            if(tile) {
                goToTile(tile, findFailFunc, oh, tileDistance,
                         pathFindWalkable, moveWalkable);
            } else {
                setCurrentAction(nullptr);
            }
        } else {
            setCurrentAction(nullptr);
        }
    };
    const auto cid = onCityId();
    auto& board = c->getBoard();
    const auto cc = board.boardCityWithId(cid);
    const auto rect = cc->tileBRect();
    const auto task = new eHeatMapTask(cid, rect, hg, hmFinish);
    auto& tp = board.threadPool();
    tp.queueTask(task);
    wait();
}

void eGodMonsterAction::goToTile(
        eTile* const tile,
        const stdsptr<eFindFailFunc>& findFailFunc,
        const stdsptr<eObsticleHandler>& oh,
        const eTileDistance& tileDistance,
        const stdsptr<eWalkableObject>& pathFindWalkable,
        const stdsptr<eWalkableObject>& moveWalkable) {
    const auto c = character();
    if(!tile || !c) return setCurrentAction(nullptr);
    auto& board = c->getBoard();
    const auto ff = std::make_shared<eGMA_goToTargetFail>(
                        board, tile, findFailFunc);
    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(ff);
    a->setFindFailAction([findFailFunc, tile]() {
        if(findFailFunc) findFailFunc->call(tile);
    });
    a->setObsticleHandler(oh);
    a->setTileDistance(tileDistance);
    a->start(tile, pathFindWalkable, moveWalkable);
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
}

void eGodMonsterAction::spawnMissile(const eCharacterActionType at,
                                     const eCharacterType chart,
                                     const int attackTime,
                                     const eMissileTarget& target,
                                     const stdsptr<eCharActFunc>& playSound,
                                     const stdsptr<eGodAct>& hitAct,
                                     const stdsptr<eCharActFunc>& finishAttackA) {
    const auto c = character();
    c->setActionType(at);
    {
        const auto ct = c->tile();
        const int tx = ct->x();
        const int ty = ct->y();
        const auto tt = target.target();
        if(!tt) return;
        const int ttx = tt->x();
        const int tty = tt->y();
        const double dx = ttx - tx;
        const double dy = tty - ty;

        const vec2d angleLine{dx, dy};
        const double a = angleLine.angle();
        const auto o = sAngleOrientation(a);
        c->setOrientation(o);
    }
    auto& board = this->board();
    board.ifVisible(c->tile(), [&]() {
        if(playSound) {
            playSound->call();
        } else {
            eSounds::playAttackSound(c);
        }
    });

    const auto a = e::make_shared<eWaitAction>(c);
    const stdptr<eGodMonsterAction> tptr(this);
    const auto finish = std::make_shared<eGMA_spawnMissileFinish>(
                            board, character(),
                            at, chart, target.target(),
                            hitAct, finishAttackA);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    a->setTime(attackTime);
    setCurrentAction(a);
}

void eGodMonsterAction::spawnMultipleMissiles(
          const eCharacterActionType at,
          const eCharacterType ct,
          const int attackTime,
          const eMissileTarget& target,
          const stdsptr<eCharActFunc>& playSound,
          const stdsptr<eGodAct>& playHitSound,
          const stdsptr<eCharActFunc>& finishA,
          const int nMissiles) {
    if(nMissiles <= 0) {
        if(finishA) finishA->call();
        return;
    }
    stdsptr<eCharActFunc> finishAA;
    if(nMissiles == 1) {
        finishAA = finishA;
    } else {
        finishAA = std::make_shared<eGMA_spawnMultipleMissilesFinish>(
                       board(), this, at, ct, attackTime,
                       target, playSound, playHitSound,
                       finishA, nMissiles);
    }
    spawnMissile(at, ct, attackTime, target.target(),
                 playSound, playHitSound, finishAA);
}

void eGodMonsterAction::spawnTimedMissiles(
        const eCharacterActionType at,
        const eCharacterType ct,
        const int attackTime,
        const eMissileTarget& target,
        const stdsptr<eCharActFunc>& playSound,
        const stdsptr<eGodAct>& playHitSound,
        const stdsptr<eCharActFunc>& finishA,
        const int time) {
    const int n = std::round(double(time)/attackTime);
    spawnMultipleMissiles(at, ct, attackTime, target,
                          playSound, playHitSound,
                          finishA, n);
}

bool eGodMonsterAction::lookForGodAttack(const int dtime, int& time,
                                         const int freq, const int range) {
    const auto c = character();
    const auto cType = c->type();
    const auto cat = c->actionType();
    bool isGod = false;
    eGod::sCharacterToGodType(cType, &isGod);
    if(!isGod) return false;
    const bool walking = cat == eCharacterActionType::walk;
    if(!walking) return false;
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return false;
    const int tx = ct->x();
    const int ty = ct->y();

    time += dtime;
    if(time > freq) {
        time -= freq;
        eGod* otherGod = nullptr;
        for(int i = -range; i <= range && !otherGod; i++) {
            for(int j = -range; j <= range; j++) {
                const int ttx = tx + i;
                const int tty = ty + j;
                const auto t = brd.tile(ttx, tty);
                if(!t) continue;
                const auto& chars = t->characters();
                for(const auto& ch : chars) {
                    if(const auto god = dynamic_cast<eGod*>(ch.get())) {
                        const auto att = god->attitude();
                        const auto at = god->actionType();
                        if(at != eCharacterActionType::walk) {
                            continue;
                        }
                        if(att == eGodAttitude::friendly ||
                           att == eGodAttitude::worshipped) {
                            otherGod = god;
                            break;
                        }
                    }
                }
            }
        }

        if(otherGod) {
            const auto aa = otherGod->action();
            if(const auto otherAction = dynamic_cast<eGodMonsterAction*>(aa)) {
                const auto thisGod = static_cast<eGod*>(c);
                const stdptr<eGod> g1ptr(thisGod);
                const stdptr<eGodMonsterAction> g1Aptr(this);
                const stdptr<eGod> g2ptr(otherGod);
                const stdptr<eGodMonsterAction> g2Aptr(otherAction);
                const auto g1t = g1ptr->type();
                const auto g2t = g2ptr->type();
                const auto wt = eGod::sFightWinner(g1t, g2t);
                const auto lt = g1t == wt ? g2t : g1t;
                const auto winnerA = g1t == wt ? g1Aptr : g2Aptr;
                const auto loserA = g1t == wt ? g2Aptr : g1Aptr;
                auto& board = thisGod->getBoard();
                const auto finishA = std::make_shared<eGAA_fightFinish>(
                                         board, winnerA, loserA, wt, lt);
                pauseAction();
                fightGod(otherGod, finishA);
                otherAction->pauseAction();
                otherAction->fightGod(thisGod, finishA);
            }
        }
    }
    return false;
}

void eGodMonsterAction::fightGod(
        eGod* const g,
        const stdsptr<eCharActFunc>& finishAttackA) {
    const auto at = eCharacterActionType::fight2;
    const auto s = eGodSound::attack;
    using ePFGHSGA = ePlayFightGodHitSoundGodAct;
    const auto playHitSound = std::make_shared<ePFGHSGA>(
                                  board(), g);
    const auto c = character();
    spawnGodTimedMissiles(at, c->type(), g->tile(), s,
                          playHitSound, finishAttackA, 6000);
}

void eGodMonsterAction::spawnGodMultipleMissiles(
        const eCharacterActionType at,
        const eCharacterType chart,
        const eMissileTarget& target,
        const eGodSound sound,
        const stdsptr<eGodAct>& playHitSound,
        const stdsptr<eCharActFunc>& finishA,
        const int nMissiles) {
    const auto c = character();
    const auto cType = c->type();
    const auto godType = eGod::sCharacterToGodType(cType);
    const int time = eGod::sGodAttackTime(godType);
    using eGA_SGMPS = eGA_spawnGodMissilePlaySound;
    const auto playSound = std::make_shared<eGA_SGMPS>(
                               board(), sound, c);
    spawnMultipleMissiles(at, chart, time, target,
                          playSound, playHitSound,
                          finishA, nMissiles);
}

void eGodMonsterAction::spawnGodTimedMissiles(
        const eCharacterActionType at,
        const eCharacterType chart,
        eTile* const target,
        const eGodSound sound,
        const stdsptr<eGodAct>& playHitSound,
        const stdsptr<eCharActFunc>& finishA,
        const int time) {
    const auto c = character();
    const auto cType = c->type();
    const auto godType = eGod::sCharacterToGodType(cType);
    const int atime = eGod::sGodAttackTime(godType);
    const int n = std::round(double(time)/atime);
    spawnGodMultipleMissiles(at, chart, target,
                             sound, playHitSound,
                             finishA, n);
}

void eGodMonsterAction::patrol(
        const stdsptr<eCharActFunc>& finishAct,
        const int dist) {
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    const auto failAct = std::make_shared<eGMA_patrolFail>(
                             board(), this, finishAct, dist);
    const auto a = e::make_shared<ePatrolMoveAction>(c);
    a->setFailAction(failAct);
    a->setFinishAction(finishAct);
    a->setMaxWalkDistance(dist);
    setCurrentAction(a);
}

void eGodMonsterAction::goToNearestRoad(
        const stdsptr<eCharActFunc>& finishAct,
        const int dist) {
    const auto c = character();
    const auto t = c->tile();
    if(t->roadLength(5) >= 5) {
        patrol(finishAct, dist);
        return;
    }
    const auto cr = eTileHelper::closestRoad(t->x(), t->y(), board(), 5);
    if(cr && cr->roadLength(5) >= 5) {
        const auto fail = std::make_shared<eGMA_patrolFailFail>(
                              board(), this, finishAct);
        const auto finish = std::make_shared<eGMA_patrolFailFinish>(
                                board(), this, finishAct, dist);
        const auto a = e::make_shared<eMoveToAction>(c);
        a->setStateRelevance(eStateRelevance::buildings |
                             eStateRelevance::terrain);
        a->setFailAction(fail);
        a->setFinishAction(finish);
        a->start(cr);
        setCurrentAction(a);
    } else {
        moveAround(finishAct, 25000);
    }
}

void eGodMonsterAction::pauseAction() {
    const auto ca = currentAction();
    if(!ca) return;
    auto& p = mPausedActions.emplace_back();
    p.fA = ca->ref<eCharacterAction>();
    const auto c = character();
    p.fAt = c->actionType();
    p.fO = c->orientation();
}

void eGodMonsterAction::resumeAction() {
    if(mPausedActions.empty()) return;
    const auto p = mPausedActions.back();
    mPausedActions.pop_back();
    setCurrentAction(p.fA);
    const auto c = character();
    c->setActionType(p.fAt);
}

void eGodMonsterAction::read(eReadStream& src) {
    eComplexAction::read(src);
    int s;
    src >> s;
    for(int i = 0; i < s; i++) {
        auto& a = mPausedActions.emplace_back();
        src >> a.fAt;
        bool hasAction;
        src >> hasAction;
        if(hasAction) {
            eCharActionType type;
            src >> type;
            a.fA = eCharacterAction::sCreate(character(),
                                             type);
            a.fA->read(src);
        }
        src >> a.fO;
    }
}

void eGodMonsterAction::write(eWriteStream& dst) const {
    eComplexAction::write(dst);
    dst << mPausedActions.size();
    for(const auto& a : mPausedActions) {
        dst << a.fAt;
        dst << (a.fA != nullptr);
        if(a.fA) {
            dst << a.fA->type();
            a.fA->write(dst);
        }
        dst << a.fO;
    }
}

stdsptr<eFindFailFunc> eFindFailFunc::sCreate(
        eGameBoard& board,
        const eFindFailFuncType type) {
    switch(type) {
    case eFindFailFuncType::teleport:
        return std::make_shared<eTeleportFindFailFunc>(board);
    case eFindFailFuncType::teleport2:
        return std::make_shared<eGoToTargetTeleport>(board);
    case eFindFailFuncType::tryAgain:
        return std::make_shared<eGoToTargetTryAgain>(board);
    }
    return nullptr;
}

void eGMA_spawnMissileFinish::call() {
    if(!mCptr) return;
    const auto c = mCptr;
    const auto charType = c->type();
    const auto ct = c->tile();
    const int tx = ct->x();
    const int ty = ct->y();
    const int ttx = mTarget->x();
    const int tty = mTarget->y();
    const auto finishAttackA = mFinishAttackA;
    if(tx == ttx && ty == tty) {
        const auto hitAct = mHitAct;
        if(finishAttackA) finishAttackA->call();
        if(hitAct) hitAct->act();
    } else {
        auto& brd = c->getBoard();
        double h;
        if(mAt == eCharacterActionType::fight ||
           mAt == eCharacterActionType::fight2) {
            switch(charType) {
            case eCharacterType::apollo:
            case eCharacterType::atalanta:
                h = -0.5;
                break;
            case eCharacterType::calydonianBoar:
                h = -1;
                break;
            default:
                h = 0;
                break;
            }
        } else {
            h = 0;
        }

        const auto m = eMissile::sCreate<eGodMissile>(
                    brd, tx, ty, h,
                    ttx, tty, h, 0);

        if(m) {
            m->setTexture(mChart, mAt);
            m->setFinishAction(mHitAct);
        }

        if(finishAttackA) finishAttackA->call();
    }
}

void eGAA_fightFinish::call() {
    auto& board = this->board();
    ePlayerCityTarget target;
    const auto winnerPtr = mWinnerPtr;
    const auto loserPtr = mLoserPtr;
    const auto tip = eGod::sFightResultString(mWt, mLt);
    if(winnerPtr) winnerPtr->resumeAction();
    if(loserPtr) {
        const auto loser = loserPtr->character();
        target = ePlayerCityTarget(loser->onCityId());
        const auto loserGod = static_cast<eGod*>(loser);
        const auto att = loserGod->attitude();
        if(att == eGodAttitude::worshipped) {
            const auto type = loserGod->type();
            const auto cid = loserGod->cityId();
            const auto s = board.sanctuary(cid, type);
            if(s) s->setSpawnWait(40000);
        }
        const auto finish = std::make_shared<eGAA_loserDisappearFinish>(
                                board, loserPtr);
        loserPtr->disappear(true, finish);
    }
    board.showTip(target, tip);
}
