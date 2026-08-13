#include "egamewidget.h"

#include "eterraineditmenu.h"

#include "textures/etiletotexture.h"
#include "textures/egametextures.h"

#include "textures/eparktexture.h"
#include "textures/evaryingsizetex.h"

#include "buildings/allbuildings.h"
#include "buildings/pyramids/epyramid.h"

#include "missiles/emissile.h"
#include "characters/esoldierbanner.h"

#include "spawners/elandinvasionpoint.h"

#include "characters/esoldier.h"
#include "characters/actions/esoldieraction.h"

#include "evectorhelpers.h"
#include "etilehelper.h"
#include "emainwindow.h"
#include "eminimap.h"

#include "eiteratesquare.h"

#include <string>
#include <algorithm>
#include <cmath>

bool sDontDrawAppeal(const eTerrain terr) {
    return terr == eTerrain::stones ||
           terr == eTerrain::flatStones ||
           terr == eTerrain::tallStones ||
           terr == eTerrain::copper ||
           terr == eTerrain::silver ||
           terr == eTerrain::orichalc ||
           terr == eTerrain::water;
}

void drawColumn(eTilePainter& tp, const int n,
                const double rx, const double ry,
                const eTextureCollection& coll) {
    double y = 0;
    const auto top = coll.getTexture(0);
    const auto mid = coll.getTexture(1);
    const auto btm = coll.getTexture(2);

    tp.drawTexture(rx + 1 - y, ry - y, btm,
                   eAlignment::hcenter | eAlignment::top);
    y += 0.75;
    for(int i = 0; i < n; i++) {
        tp.drawTexture(rx + 1 - y, ry - y, mid,
                       eAlignment::hcenter | eAlignment::top);
        y += 0.33;
    }
    tp.drawTexture(rx + 1 - y, ry - y, top,
                   eAlignment::hcenter | eAlignment::top);
}

void eGameWidget::drawXY(int tx, int ty,
                         double& rx, double& ry,
                         const int wSpan, const int hSpan,
                         const int a) {
    if(mBoard) {
        const auto dir = mBoard->direction();
        if(dir != eWorldDirection::N) {
            const int width = mBoard->width();
            const int height = mBoard->height();
            eTileHelper::tileIdToRotatedTileId(tx, ty, tx, ty,
                                               dir, width, height);
        }
    }

    rx = tx + 0.5;
    ry = ty + 1.5;

    if(wSpan == 2 && hSpan == 2) {
        rx += 0.5;
        ry += 0.5;
    } else if(wSpan == 3 && hSpan == 3) {
        rx += 0.0;
        ry += 2.0;
    } else if(wSpan == 4 && hSpan == 4) {
        rx += 0.5;
        ry += 2.5;
    } else if(wSpan == 5 && hSpan == 5) {
        rx += 0.0;
        ry += 4.0;
    } else if(wSpan == 6 && hSpan == 6) {
        rx += 0.5;
        ry += 5.5;
    }
    rx -= a;
    ry -= a;
}

stdsptr<eTexture> eGameWidget::getBasementTexture(
        const int rtx, const int rty,
        eBuilding* const d,
        const eTerrainTextures& trrTexs,
        const eWorldDirection dir,
        const int boardw,
        const int boardh) {
    auto tr = d->tileRect();
    tr = eTileHelper::toRotatedRect(tr, dir, boardw, boardh);
    const int right = tr.x + tr.w - 1;
    const int bottom = tr.y + tr.h - 1;
    int id = 0;
    if(tr.w == 1 && tr.h == 1) {
        id = 0;
    } else if(rtx == tr.x) {
        if(rty == tr.y) {
            id = 2;
        } else if(rty == bottom) {
            id = 8;
        } else {
            id = 9;
        }
    } else if(rtx == right) {
        if(rty == tr.y) {
            id = 4;
        } else if(rty == bottom) {
            id = 6;
        } else {
            id = 5;
        }
    } else if(rty == tr.y) {
        id = 3;
    } else if(rty == bottom) {
        id = 7;
    } else {
        id = 1;
    }
    const eTextureCollection* coll = nullptr;
    const auto type = d->type();
    if(type == eBuildingType::commonHouse ||
       type == eBuildingType::eliteHousing) {
        coll = &trrTexs.fBuildingBase3;
    } else {
        coll = &trrTexs.fBuildingBase2;
    }
    return coll->getTexture(id);
}

std::vector<eTile*> eGameWidget::selectedTiles() const {
    std::vector<eTile*> result;
    const int x0 = mPressedX > mHoverX ? mHoverX : mPressedX;
    const int y0 = mPressedY > mHoverY ? mHoverY : mPressedY;
    const int x1 = mPressedX > mHoverX ? mPressedX : mHoverX;
    const int y1 = mPressedY > mHoverY ? mPressedY : mHoverY;
    int t0x;
    int t0y;
    int t1x;
    int t1y;
    pixToId(x0, y0, t0x, t0y);
    pixToId(x1, y1, t1x, t1y);

    int dt0x;
    int dt0y;
    eTileHelper::tileIdToDTileId(t0x, t0y, dt0x, dt0y);
    int dt1x;
    int dt1y;
    eTileHelper::tileIdToDTileId(t1x, t1y, dt1x, dt1y);

    const int xMin = std::min(dt0x, dt1x);
    const int xMax = std::max(dt0x, dt1x);
    const int yMin = std::min(dt0y, dt1y);
    const int yMax = std::max(dt0y, dt1y);
    for(int x = xMin; x < xMax; x++) {
        for(int y = yMin; y < yMax; y++) {
            const auto tile = mBoard->dtile(x, y);
            if(!tile) continue;
            const auto cid = tile->cityId();
            if(cid != mViewedCityId) continue;
            result.push_back(tile);
        }
    }
    return result;
}

class eGameBoardRegisterLock {
public:
    eGameBoardRegisterLock(eGameBoard& board) :
        mBoard(board) {
        mBoard.setRegisterBuildingsEnabled(false);
    }
    ~eGameBoardRegisterLock() {
        mBoard.setRegisterBuildingsEnabled(true);
    }
private:
    eGameBoard& mBoard;
};

void eGameWidget::setArmyMenuVisible(const bool v) {
    if(mAm->visible() == v) return;
    mAm->setVisible(v);
    if(v) {
        mGm->show();
        mTem->hide();
        const auto map = mAm->miniMap();
        map->scheduleUpdate();
    } else {
        mTem->setVisible(mTerrainEditMode);
        mGm->setVisible(!mTerrainEditMode);
    }
}

void eGameWidget::scheduleConnectedTerrainUpdate(eTile* const startTile) {
    std::vector<eTile*> tiles;
    std::function<bool(eTile*)> check;
    std::function<void(eTile*)> prcs;
    prcs = [&](eTile* const tile) {
        if(!tile) return;
        if(!check(tile)) return;
        if(eVectorHelpers::contains(tiles, tile)) return;
        tiles.push_back(tile);
        tile->scheduleTerrainUpdate();
        tile->setDrawDim(1);
        tile->setUnderTile(nullptr);
        const auto tr = tile->topRight<eTile>();
        prcs(tr);
        const auto br = tile->bottomRight<eTile>();
        prcs(br);
        const auto bl = tile->bottomLeft<eTile>();
        prcs(bl);
        const auto tl = tile->topLeft<eTile>();
        prcs(tl);
    };

    const auto terr = startTile->terrain();
    if(static_cast<bool>(terr & eTerrain::stones)) {
        check = [terr](eTile* const tile) {
            return tile->terrain() == terr;
        };
        prcs(startTile);
    } else if(startTile->underBuildingType() == eBuildingType::park) {
        check = [](eTile* const tile) {
            return tile->underBuildingType() == eBuildingType::park;
        };
        prcs(startTile);
    } else {
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                const auto t = startTile->tileRel<eTile>(dx, dy);
                if(!t) continue;
                t->scheduleTerrainUpdate();
            }
        }
    }
    std::sort(tiles.begin(), tiles.end(),
              [this](eTile* const t1, eTile* const t2) {
        const auto dir = mBoard->direction();
        const int t1dx = t1->dx();
        const int t1dy = t1->dy();
        const int t2dx = t2->dx();
        const int t2dy = t2->dy();
        switch(dir) {
        case eWorldDirection::N: {
            if(t1dy != t2dy) return t1dy < t2dy;
            return t1dx < t2dx;
        } break;
        case eWorldDirection::E: {
            if(t1dx != t2dx) return t1dx < t2dx;
            return t1dy < t2dy;
        } break;
        case eWorldDirection::S: {
            if(t1dy != t2dy) return t1dy > t2dy;
            return t1dx > t2dx;
        } break;
        case eWorldDirection::W: {
            if(t1dx != t2dx) return t1dx > t2dx;
            return t1dy > t2dy;
        } break;
        }
    });

    const int tid = static_cast<int>(mTileSize);
    const auto& trrTexs = eGameTextures::terrain().at(tid);
    const auto& builTexs = eGameTextures::buildings().at(tid);

    for(const auto tile : tiles) {
        updateTerrainTextures(tile, trrTexs, builTexs);
    }
}

void eGameWidget::updateTerrainTextures(eTile* const tile,
                                        const eTerrainTextures& trrTexs,
                                        const eBuildingTextures& builTexs) {
    tile->setUnderTile(nullptr);
    auto& painter = tile->terrainPainter();

    painter.fColl = nullptr;
    painter.fTex = eTileToTexture::get(tile, trrTexs, builTexs,
                                       mTileSize, mDrawElevation,
                                       painter.fDrawDim,
                                       &painter.fColl,
                                       mBoard->direction());
}

void eGameWidget::updateTerrainTextures() {
    const int tid = static_cast<int>(mTileSize);
    const auto& trrTexs = eGameTextures::terrain().at(tid);
    const auto& builTexs = eGameTextures::buildings().at(tid);

    mBoard->iterateOverAllTiles([&](eTile* const tile) {
        tile->setDrawDim(1);
        tile->setUnderTile(nullptr);
    });
    mBoard->iterateOverAllTiles([&](eTile* const tile) {
        updateTerrainTextures(tile, trrTexs, builTexs);
    });
}

void eGameWidget::paintEvent(ePainter& p) {
    if(mUpdateViewedTileScheduled) {
        mUpdateViewedTileScheduled = false;
        const auto oldC = mViewedTile ? mViewedTile->cityId() :
                                        eCityId::neutralFriendly;
        mViewedTile = viewedTile();
        const auto newC = mViewedTile ? mViewedTile->cityId() :
                                        eCityId::neutralFriendly;
        mViewedCityId = newC;
        if(oldC != newC) {
            mBoard->clearBannerSelection();
            mBoard->clearTriremeSelection();
            const auto c = mBoard->boardCityWithId(newC);
            if(!mEditorMode && c && c->owningPlayer() == nullptr) {
                showBuyCity(newC);
            } else {
                hideBuyCity();
            }
            mGm->viewedCityChanged();
        }
    }
    if(mBoard->duringEarthquake()) {
        mDX += (eRand::rand() % 11) - 5;
        mDY += (eRand::rand() % 11) - 5;
        clampViewBox();
    }
    {
        const auto& ss = mBoard->selectedSoldiers();
        const auto& ts = mBoard->selectedTriremes();
        const bool v = !ss.empty() || !ts.empty();
        setArmyMenuVisible(v);

        bool home = false;
        if(ss.empty()) {
            home = false;
        } else {
            for(const auto& s : ss) {
                const bool h = s->isHome();
                if(h) {
                    home = true;
                    break;
                }
            }
        }
        mAm->setSoldiersHome(home);
    }
    {
        const auto pbv = eViewMode::patrolBuilding;
        if(!mPatrolBuilding && (mViewMode == pbv || mPatrolPathWid)) {
            setPatrolBuilding(nullptr);
        }
    }
    mFrame++;
    mRotateFrame++;
    bool updateTips = false;
    for(int i = 0; i < int(mTips.size()); i++) {
        const auto& tip = mTips[i];
        if(mFrame > tip.fLastFrame) {
            tip.fWid->deleteLater();
            mTips.erase(mTips.begin() + i);
            updateTips = true;
            i--;
        }
    }
    if(updateTips) updateTipPositions();
    mBoard->incFrame();

    const bool iterate = mSpeedId == sMaxSpeedId;
    const int iMax = iterate ? 5 : 1;
    for(int i = 0; i < iMax; i++) {
        mBoard->scheduleDataUpdate();
        mBoard->updateAppealMapIfNeeded();
        mBoard->handleFinishedTasks();
        const int nc = children().size() - mTips.size();
        const bool incTime = !mPaused && !mLocked && !mMsgBox && !mInfoWidget && nc < 6;
        if(incTime) {
            const bool lost = mBoard->episodeLost();
            if(lost) {
                const auto w = window();
                w->episodeLost();
                return;
            } else {
                mTime += mSpeed;
                mBoard->incTime(mSpeed);
            }
        }
        mBoard->emptyRubbish();
        if(iterate) mBoard->waitUntilFinished();
        if(!incTime) break;
    }

    if(mHoverX == 0) {
        setDX(mDX + 35);
    } else if(mHoverX == width() - 1) {
        setDX(mDX - 35);
    }
    if(mHoverY == 0) {
        setDY(mDY + 35);
    } else if(mHoverY == height() - 1) {
        setDY(mDY - 35);
    }
    eGameBoardRegisterLock lock(*mBoard);

    p.setFont(eFonts::defaultFont(resolution()));
    p.translate(mDX, mDY);
    eTilePainter tp(p, mTileSize, mTileW, mTileH);
    const auto& numbers = mNumbers[mTileSize];

    const auto ppid = mBoard->personPlayer();

    const int tid = static_cast<int>(mTileSize);
    const auto& trrTexs = eGameTextures::terrain().at(tid);
    const auto& builTexs = eGameTextures::buildings().at(tid);
    const auto& destTexs = eGameTextures::destrution().at(tid);
    const auto& charTexs = eGameTextures::characters().at(tid);
    const auto dir = mBoard->direction();
    const int boardw = mBoard->width();
    const int boardh = mBoard->height();

    const auto mode = mGm->mode();

    const int sMinX = std::min(mPressedTX, mHoverTX);
    const int sMinY = std::min(mPressedTY, mHoverTY);
    const int sMaxX = std::max(mPressedTX, mHoverTX);
    const int sMaxY = std::max(mPressedTY, mHoverTY);

    const bool terrUpdated = mBoard->terrainUpdateScheduled();
    if(terrUpdated) {
        updateTerrainTextures();
        mBoard->afterTerrainUpdated();
    }

    const bool terrainEditing = mTem->visible();
    const bool fogOfWar = !terrainEditing && mBoard->fogOfWar();
    const auto drawTerrain = [&](eTile* const tile) {
        const int tx = tile->x();
        const int ty = tile->y();

        const auto terr = tile->terrain();

        auto border = tile->territoryBorder();
        int rtx;
        int rty;
        eTileHelper::tileIdToRotatedTileId(tx, ty,
                                           rtx, rty, dir,
                                           boardw, boardh);

        const int ta = tile->altitude();

        switch(dir) {
        case eWorldDirection::N: {
        } break;
        case eWorldDirection::E: {
            border.fT = border.fR;
            border.fTL = border.fTR;
            border.fTR = border.fBR;
        } break;
        case eWorldDirection::S: {
            border.fT = border.fB;
            border.fTR = border.fBL;
            border.fTL = border.fBR;
        } break;
        case eWorldDirection::W: {
            border.fT = border.fL;
            border.fTR = border.fTL;
            border.fTL = border.fBL;
        } break;
        }

        const auto cid = tile->cityId();
        const bool tileFogOfWar = fogOfWar &&
                                  cid == eCityId::neutralFriendly;

        if(tile->updateTerrain() || tile->hasRoad()) {
            if(!terrUpdated) {
                updateTerrainTextures(tile, trrTexs, builTexs);
            }
            tile->terrainUpdated();
        }
        const auto& painter = tile->terrainPainter();

        const int drawDim = painter.fDrawDim;

        double rx;
        double ry;
        const int a = mDrawElevation ? tile->altitude() : 0;
        drawXY(tx, ty, rx, ry, drawDim, drawDim, a);

        stdsptr<eTexture> tex;
        if(drawDim == 0) {
            const auto u = tile->underTile();
            if(u) {
                const auto& upainter = u->terrainPainter();
                const int dx = tile->underTileDX();
                const int dy = tile->underTileDY();
                const int uDrawDim = upainter.fDrawDim;
                const auto type = u->underBuildingType();
                const bool isPark = type == eBuildingType::park;
                if(isPark && !(dy == uDrawDim - 1 && dx == 0)) return;
                const bool fitX = dx == uDrawDim - 1;
                const bool fitY = dy == uDrawDim - 1;
                if(fitX || fitY) {
                    rx += 0.5*(uDrawDim - 1) - dx;
                    ry += 1.5*(uDrawDim - 1) - dy;
                    tex = upainter.getTexture(mFrame);
                    if(tex && !isPark) {
                        SDL_Rect clipRect;
                        clipRect.y = -10000;
                        clipRect.h = 20000;
                        const int d = fitY ? 1 : 0;
                        clipRect.x = mDX + (rtx - rty - d)*mTileW/2;
                        clipRect.w = fitX && fitY ? mTileW : mTileW/2;
                        SDL_RenderSetClipRect(p.renderer(), &clipRect);
                    }
                }
            }
        } else if(drawDim == 1) {
            tex = painter.getTexture(mFrame);
        }

        if(tex) {
            bool lavaCm = false;
            bool eraseCm = false;
            bool patrolCm = false;
            bool editorHover = false;
            if(terrainEditing) {
                editorHover = eVectorHelpers::contains(mHoverTiles, tile) ||
                              eVectorHelpers::contains(mInflTiles, tile);
                if(editorHover) {
                    tex->setColorMod(255, 175, 175);
                }
            }
            const bool lavaFlows = mBoard->duringLavaFlow();
            const bool hasLava = terr == eTerrain::lava;
            if(lavaFlows && hasLava) {
                lavaCm = true;
                tex->setColorMod(255, 0, 0);
            } else if(mPatrolBuilding && (!mPatrolPath.empty() || !mPatrolPath1.empty())) {
                patrolCm = eVectorHelpers::contains(mPatrolPath, tile) ||
                           eVectorHelpers::contains(mPatrolPath1, tile);
                if(patrolCm) {
                    tex->setColorMod(175, 255, 175);
                } else {
                    patrolCm = eVectorHelpers::contains(mExcessPatrolPath, tile) ||
                               eVectorHelpers::contains(mExcessPatrolPath1, tile);
                    if(patrolCm) {
                        tex->setColorMod(255, 175, 175);
                    }
                }
            } else if((!terrainEditing &&
                       !static_cast<bool>(terr & eTerrain::stones)) ||
                      (terrainEditing &&
                       mTem->brushType() == eBrushType::apply)) {
                const auto ub = tile->underBuilding();
                if(ub) {
                    eraseCm = inErase(ub);
                    const auto type = ub->type();
                    if(type == eBuildingType::park) {
                        eraseCm = eraseCm && drawDim == 1;
                    }
                } else {
                    eraseCm = inErase(tx, ty);
                }
                if(eraseCm) tex->setColorMod(255, 175, 175);
            }

            if(mEditorMode && !eraseCm && !patrolCm) {
                const auto ub = tile->underBuilding();
                if(ub) {
                    const int eid = ub->districtId();
                    const auto ecid = mBoard->currentDistrictId();
                    if(ecid == eid) {
                        tex->setColorMod(200, 255, 200);
                    } else {
                        tex->setColorMod(255, 200, 200);
                    }
                } else {
                    if(tile->tidalWaveZone()) {
                        tex->setColorMod(0, 0, 255);
                    } else if(tile->lavaZone()) {
                        tex->setColorMod(255, 0, 0);
                    } else if(tile->landSlideZone()) {
                        tex->setColorMod(255, 0, 255);
                    }
                }
            }

            if(tileFogOfWar) {
                const int maxDist = eTile::sMaxDistanceToBorder;
                const double dist = tile->distanceToBorder();
                const int val = std::round((maxDist - dist)*255/maxDist);
                tex->setColorMod(val, val, val);
            }
            if(hasLava) {
                const int seed = tile->seed();
                const auto& coll = trrTexs.fDryTerrainTexs;
                const int texId = seed % coll.size();
                const auto tex = coll.getTexture(texId);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
            }
            tp.drawTexture(rx, ry, tex, eAlignment::top);
            if(drawDim == 0) SDL_RenderSetClipRect(p.renderer(), nullptr);
            if(eraseCm || patrolCm || editorHover ||
               mEditorMode || tileFogOfWar || lavaCm) {
                tex->clearColorMod();
            }
        }

        {
            if(!tileFogOfWar || true) {
                const int dim = mTileW/10;
                const SDL_Color color{255, 255, 255, 255};
                if(border.fT) {
                    tp.fillRectCenter(rtx - ta, rty - ta, dim, dim, color);
                }
                if(border.fTR) {
                    tp.fillRectCenter(rtx + 0.5 - ta, rty - ta, dim, dim, color);
                }
                if(border.fTL) {
                    tp.fillRectCenter(rtx - ta, rty + 0.5 - ta, dim, dim, color);
                }
            }
        }
    };


    std::vector<eTile*> bridgetTs;
    bool bridgeValid = false;
    bool bridgeRot;
    if(mode == eBuildingMode::bridge) {
        const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
        bridgeValid = bridgeTiles(startTile, eTerrain::water, bridgetTs, bridgeRot);
        if(!bridgeValid) bridgeValid = bridgeTiles(startTile, eTerrain::quake, bridgetTs, bridgeRot);
    }

    const auto isPatrolSelected = [&](eBuilding* const ub) {
        bool selected = false;
        if(const auto v = dynamic_cast<eVendor*>(ub)) {
            selected = v->agora() == mPatrolBuilding;
        } else if(const auto s = dynamic_cast<eAgoraSpace*>(ub)) {
            selected = s->agora() == mPatrolBuilding;
        } else {
            selected = ub == mPatrolBuilding;
        }
        return selected;
    };

    const auto buildingDrawer = [&](eTile* const tile) {
        const int tx = tile->x();
        const int ty = tile->y();
        int rtx;
        int rty;
        eTileHelper::tileIdToRotatedTileId(tx, ty,
                                           rtx, rty, dir,
                                           boardw, boardh);
        const int a = mDrawElevation ? tile->altitude() : 0;
        const int da = mDrawElevation ? tile->doubleAltitude() : 0;

        auto ub = tile->underBuilding();
        if(ub && ub->type() == eBuildingType::road) {
            const auto r = static_cast<eRoad*>(ub);
            if(const auto h = r->aboveHippodrome()) {
                ub = h;
            }
        }
        const auto bt = ub ? ub->type() : eBuildingType::none;

        const bool bv = eViewModeHelpers::buildingVisible(mViewMode, ub);
        const bool v = ub && bv;

        double rx;
        double ry;
        drawXY(tx, ty, rx, ry, 1, 1, a);

        const auto drawBlessedCursed = [&](const double bx, const double by) {
            if(ub->blessed()) {
                eGameTextures::loadBlessed();
                const auto& blsd = destTexs.fBlessed;
                const auto tex = blsd.getTexture(ub->textureTime() % blsd.size());
                tp.drawTexture(bx, by, tex, eAlignment::bottom);
            } else if(ub->cursed()) {
                eGameTextures::loadCursed();
                const auto& blsd = destTexs.fCursed;
                const auto tex = blsd.getTexture(ub->textureTime() % blsd.size());
                tp.drawTexture(bx, by, tex, eAlignment::bottom);
            }
        };

        const auto drawFire = [&](eTile* const ubt) {
            const int tx = ubt->x();
            const int ty = ubt->y();
            double frx;
            double fry;
            drawXY(tx, ty, frx, fry, 1, 1, a);
            eGameTextures::loadFire();
            const int f = (tx + ty) % destTexs.fFire.size();
            const auto& ff = destTexs.fFire[f];
            const int dt = mBoard->frame() + std::abs(tx*ty);
            const auto tex = ff.getTexture(dt % ff.size());
            tp.drawTexture(frx + 1, fry, tex, eAlignment::hcenter | eAlignment::top);
        };

        const auto drawBuildingModes = [&]() {
            if(!ub) return;
            const double cdx = -0.65;
            const double cdy = -0.65;
            if(mViewMode == eViewMode::hazards) {
                const auto pid = mBoard->personPlayer();
                const auto diff = mBoard->difficulty(pid);
                const int fr = eDifficultyHelpers::fireRisk(diff, bt);
                const int dr = eDifficultyHelpers::damageRisk(diff, bt);
                if(const auto h = dynamic_cast<eHouseBase*>(ub)) {
                    if(h->people() == 0) return;
                }
                const int h = 100 - ub->maintenance();
                if((fr || dr) && h > 5) {
                    const int n = h/15;
                    const eTextureCollection* coll = nullptr;
                    if(n < 2) {
                        coll = &builTexs.fColumn1;
                    } else if(n < 3) {
                        coll = &builTexs.fColumn2;
                    } else if(n < 4) {
                        coll = &builTexs.fColumn3;
                    } else {
                        coll = &builTexs.fColumn4;
                    }

                    drawColumn(tp, n, rx + cdx, ry + cdy, *coll);
                }
            } else if(mViewMode == eViewMode::taxes) {
                if(const auto h = dynamic_cast<eHouseBase*>(ub)) {
                    if(h->people() == 0) return;
                    const bool paid = h->paidTaxes();
                    const int n = paid ? 4 : 0;
                    drawColumn(tp, n, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::water) {
                if(bt == eBuildingType::commonHouse) {
                    const auto ch = static_cast<eSmallHouse*>(ub);
                    if(ch->people() == 0) return;
                    const int w = ch->water()/2;
                    drawColumn(tp, w, rx + cdx, ry + cdy, builTexs.fColumn5);
                }
            } else if(mViewMode == eViewMode::hygiene) {
                if(bt == eBuildingType::commonHouse) {
                    const auto ch = static_cast<eSmallHouse*>(ub);
                    if(ch->people() == 0) return;
                    const int h = ch->hygiene();
                    const int n = h/15;
                    const eTextureCollection* coll = nullptr;
                    if(n < 2) {
                        coll = &builTexs.fColumn4;
                    } else if(n < 3) {
                        coll = &builTexs.fColumn3;
                    } else if(n < 4) {
                        coll = &builTexs.fColumn2;
                    } else {
                        coll = &builTexs.fColumn1;
                    }

                    drawColumn(tp, n, rx + cdx, ry + cdy, *coll);
                }
            } else if(mViewMode == eViewMode::unrest) {
                if(bt == eBuildingType::commonHouse) {
                    const auto ch = static_cast<eSmallHouse*>(ub);
                    if(ch->people() == 0) return;
                    const int h = 100 - ch->satisfaction();
                    const int n = h/15;
                    const eTextureCollection* coll = nullptr;
                    if(n < 2) {
                        coll = &builTexs.fColumn1;
                    } else if(n < 3) {
                        coll = &builTexs.fColumn2;
                    } else if(n < 4) {
                        coll = &builTexs.fColumn3;
                    } else {
                        coll = &builTexs.fColumn4;
                    }

                    drawColumn(tp, n, rx + cdx, ry + cdy, *coll);
                }
            } else if(mViewMode == eViewMode::actors ||
                      mViewMode == eViewMode::astronomers) {
                if(bt == eBuildingType::commonHouse ||
                   bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eHouseBase*>(ub);
                    if(ch->people() == 0) return;
                    const int a = ch->actorsAstronomers()/2;
                    drawColumn(tp, a, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::philosophers ||
                      mViewMode == eViewMode::inventors) {
                if(bt == eBuildingType::commonHouse ||
                   bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eHouseBase*>(ub);
                    if(ch->people() == 0) return;
                    const int a = ch->philosophersInventors()/2;
                    drawColumn(tp, a, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::athletes ||
                      mViewMode == eViewMode::scholars) {
                if(bt == eBuildingType::commonHouse ||
                   bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eHouseBase*>(ub);
                    if(ch->people() == 0) return;
                    const int a = ch->athletesScholars()/2;
                    drawColumn(tp, a, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::competitors ||
                      mViewMode == eViewMode::curators) {
                if(bt == eBuildingType::commonHouse ||
                   bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eHouseBase*>(ub);
                    if(ch->people() == 0) return;
                    const int a = ch->competitorsCurators()/2;
                    drawColumn(tp, a, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::allCulture ||
                      mViewMode == eViewMode::allScience) {
                if(bt == eBuildingType::commonHouse ||
                   bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eHouseBase*>(ub);
                    if(ch->people() == 0) return;
                    const int a = ch->allCultureScience();
                    drawColumn(tp, a, rx + cdx, ry + cdy, builTexs.fColumn1);
                }
            } else if(mViewMode == eViewMode::supplies) {
                if(bt == eBuildingType::commonHouse) {
                    const auto ch = static_cast<eSmallHouse*>(ub);
                    if(ch->people() == 0) return;
                    double rxx = rx - 2.5;
                    double ryy = ry - 2;
                    tp.scheduleDrawTexture(rxx, ryy, builTexs.fSuppliesBg);
                    rxx += 0.49;
                    ryy += 0.15;
                    const double inc = 0.43;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFood() ?
                                               builTexs.fNHasFood :
                                               builTexs.fHasFood);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFleece() ?
                                               builTexs.fNHasFleece :
                                               builTexs.fHasFleece);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowOil() ?
                                               builTexs.fNHasOil :
                                               builTexs.fHasOil);
                } else if(bt == eBuildingType::eliteHousing) {
                    const auto ch = static_cast<eEliteHousing*>(ub);
                    if(ch->people() == 0) return;
                    double rxx = rx - 3.5;
                    double ryy = ry - 1.5;
                    tp.scheduleDrawTexture(rxx, ryy, builTexs.fEliteSuppliesBg);
                    rxx += 0.49;
                    ryy += 0.15;
                    const double inc = 0.43;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFood() ?
                                               builTexs.fNHasFood :
                                               builTexs.fHasFood);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFleece() ?
                                               builTexs.fNHasFleece :
                                               builTexs.fHasFleece);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowOil() ?
                                               builTexs.fNHasOil :
                                               builTexs.fHasOil);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowWine() ?
                                               builTexs.fNHasWine :
                                               builTexs.fHasWine);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowArms() ?
                                               builTexs.fNHasArms :
                                               builTexs.fHasArms);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowHorses() ?
                                               builTexs.fNHasHorses :
                                               builTexs.fHasHorses);
                }
            }
        };

        if(ub && !v) {
            if(!isPatrolSelected(ub) && mViewMode != eViewMode::appeal) {
                const auto tex = getBasementTexture(rtx, rty, ub, trrTexs,
                                                    dir, boardw, boardh);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
            }
        } else if(ub && !eBuilding::sFlatBuilding(bt)) {
            const auto getDisplacement =
            [](const int w, const int h,
               double& dx, double& dy) {
                if(w == 1 && h == 1) {
                    dx = -0.5;
                    dy = 0.5;
                } else if(w == 2 && h == 2) {
                    dx = -1.;
                    dy = 1.;
                } else if(w == 3 && h == 3) {
                    dx = -1.5;
                    dy = 1.5;
                } else if(w == 4 && h == 4) {
                    dx = -2.;
                    dy = 2.;
                } else if(w == 5 && h == 5) {
                    dx = -2.5;
                    dy = 2.5;
                } else if(w == 6 && h == 6) {
                    dx = -3.0;
                    dy = 3.0;
                } else {
                    dx = 0.;
                    dy = 0.;
                }
            };
            const auto size = tp.size();
            const auto ts = ub->getTextureSpace(tx, ty, size);
            const auto& tsRect = ts.fRect;
            const auto rtsRect = eTileHelper::toRotatedRect(
                                     tsRect, dir, boardw, boardh);
            const int fitY = rtsRect.y + rtsRect.h - 1;
            const int fitX = rtsRect.x + rtsRect.w - 1;
            const bool fitXB = rtx == fitX;
            const bool fitYB = rty == fitY;
            double dx;
            double dy;
            getDisplacement(tsRect.w, tsRect.h, dx, dy);
            const double drawX = fitX + dx + 1 - da*0.5;
            const double drawY = fitY + dy + 1 - da*0.5;
            if(fitXB || fitYB) {
                const bool last = fitXB && fitYB;
                if(ts.fClamp) {
                    SDL_Rect clipRect;
                    clipRect.y = -10000;
                    clipRect.h = 20000;
                    const int d = fitYB ? 1 : 0;
                    clipRect.x = mDX + (rtx - rty - d)*mTileW/2;
                    clipRect.w = last ? mTileW : mTileW/2;
                    const int margin = 5*mTileW;
                    if(rtx == fitX && rty == rtsRect.y) {
                        if(dir == eWorldDirection::N ||
                           dir == eWorldDirection::S) {
                            clipRect.w += margin;
                        } else {
                            clipRect.x -= margin;
                            clipRect.w += margin;
                        }
                    }
                    if(rty == fitY && rtx == rtsRect.x) {
                        if(dir == eWorldDirection::N ||
                           dir == eWorldDirection::S) {
                            clipRect.x -= margin;
                            clipRect.w += margin;
                        } else {
                            clipRect.w += margin;
                        }
                    }
                    SDL_RenderSetClipRect(p.renderer(), &clipRect);
                }

                const bool erase = inErase(ub);
                const bool hover = inPatrolBuildingHover(ub);
                bool colorMod = false;
                int cred = 255;
                int cgreen = 255;
                int cblue = 255;
                if(erase) {
                    colorMod = true;
                    cred = 255;
                    cgreen = 175;
                    cblue = 175;
                } else if(hover) {
                    colorMod = true;
                    cred = 175;
                    cgreen = 255;
                    cblue = 255;
                } else if(mEditorMode) {
                    colorMod = true;
                    const int eid = ub->districtId();
                    const auto ecid = mBoard->currentDistrictId();
                    if(ecid == eid) {
                        cred = 200;
                        cgreen = 255;
                        cblue = 200;
                    } else {
                        cred = 255;
                        cgreen = 200;
                        cblue = 200;
                    }
                }
                const auto& tex = ts.fTex;
                if(tex) {
                    if(colorMod) tex->setColorMod(cred, cgreen, cblue);
                    tp.drawTexture(drawX + ts.fX, drawY + ts.fY,
                                   tex, eAlignment::top);
                    if(colorMod) tex->clearColorMod();
                }
                if(ub->overlayEnabled() && ts.fOvelays) {
                    const auto overlays = ub->getOverlays(size);
                    for(const auto& o : overlays) {
                        const auto& tex = o.fTex;
                        if(!tex) continue;
                        if(colorMod) tex->setColorMod(cred, cgreen, cblue);
                        if(o.fAlignTop) {
                            tp.drawTexture(drawX + ts.fX + o.fX, drawY + ts.fY + o.fY,
                                           tex, eAlignment::top);
                        } else {
                            tp.drawTexture(drawX + ts.fX + o.fX, drawY + ts.fY + o.fY, tex);
                        }
                        if(colorMod) tex->clearColorMod();
                    }
                }
                SDL_RenderSetClipRect(p.renderer(), nullptr);

                if(last) {
                    bool globalLast = true;
                    if(bt == eBuildingType::eliteHousing) {
                        const auto ubRect = ub->tileRect();
                        const auto rubRect = eTileHelper::toRotatedRect(
                                                 ubRect, dir, boardw, boardh);
                        const int globalFitY = rubRect.y + rubRect.h - 1;
                        const int globalFitX = rubRect.x + rubRect.w - 1;
                        globalLast = rtx == globalFitX && rty == globalFitY;
                    }
                    if(bt == eBuildingType::commonHouse) {
                        const auto ch = static_cast<eSmallHouse*>(ub);
                        const bool p = ch->plague();
                        if(p && ch->people()) {
                            eGameTextures::loadPlague();
                            const auto& blsd = destTexs.fPlague;
                            const int texId = ub->textureTime() % blsd.size();
                            const auto tex = blsd.getTexture(texId);

                            tp.drawTexture(drawX + 3, drawY + 1, tex, eAlignment::top);
                        }
                    }
                    if(ub->isOnFire()) {
                        const auto& ubts = ub->tilesUnder();
                        for(const auto& ubt : ubts) {
                            drawFire(ubt);
                        }
                    }
                    if(ts.fOvelays && tex) {
                        const int bx = drawX;
                        const int by = drawY - tsRect.h;
                        drawBlessedCursed(bx, by);
                    }
                    if(globalLast) drawBuildingModes();
                }
            }
        } else if(ub) {
            if(ub->isOnFire()) {
                const auto& ubts = ub->tilesUnder();
                for(const auto& ubt : ubts) {
                    drawFire(ubt);
                }
            }
            bool drawBlessed = true;
            if(eResourceBuilding::sIsResourceBuilding(bt)) {
                drawBlessed = tx % 2 && ty % 2;
            }
            if(drawBlessed) drawBlessedCursed(rx + 0.75, ry);
        }
    };
    iterateOverVisibleTiles([&](eTile* const tile) {
        const int tx = tile->x();
        const int ty = tile->y();
        int rtx;
        int rty;
        eTileHelper::tileIdToRotatedTileId(tx, ty,
                                           rtx, rty, dir,
                                           boardw, boardh);
        const int dtx = tile->dx();
        const int dty = tile->dy();
        const int a = mDrawElevation ? tile->altitude() : 0;

        const auto mode = mGm->mode();

        const auto ub = tile->underBuilding();
        const auto bt = tile->underBuildingType();

        const bool bv = eViewModeHelpers::buildingVisible(mViewMode, ub);
        const bool v = ub && bv;

        bool bd = false;

        double rx;
        double ry;
        drawXY(tx, ty, rx, ry, 1, 1, a);

        const auto drawSheepGoat = [&]() {
            if(mode == eBuildingMode::sheep ||
               mode == eBuildingMode::goat ||
               mode == eBuildingMode::cattle ||
               mode == eBuildingMode::erase) {
                if(bt == eBuildingType::sheep ||
                   bt == eBuildingType::goat ||
                   bt == eBuildingType::cattle) {
                    const auto tex = trrTexs.fBuildingBase;
                    const bool e = inErase(ub);
                    if(e) tex->setColorMod(255, 175, 175);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    if(e) tex->clearColorMod();
                    bd = true;
                }
            }
        };

        const auto drawPatrol = [&]() {
            if(mViewMode == eViewMode::patrolBuilding) {
                if(!ub || !mPatrolBuilding) return;
                const auto ubt = ub->type();
                if(eBuilding::sFlatBuilding(ubt)) return;
                std::shared_ptr<eTexture> tex;
                if(isPatrolSelected(ub)) {
                    tex = trrTexs.fSelectedBuildingBase;
                } else {
                    tex = getBasementTexture(rtx, rty, ub, trrTexs,
                                             dir, boardw, boardh);
                }
                tp.drawTexture(rx, ry, tex, eAlignment::top);
                bd = true;
            }
        };

        const auto drawAppeal = [&]() {
            const auto terr = tile->terrain();
            if(sDontDrawAppeal(terr)) return;
            if(tile->isElevationTile()) return;
            if(!v && mViewMode == eViewMode::appeal) {
                const auto& am = mBoard->appealMap();
                const auto ae = am.enabled(dtx, dty);
                const bool ch = bt == eBuildingType::commonHouse ||
                                bt == eBuildingType::eliteHousing;
                if(ae || ch || ub) {
                    const bool pyramid = eBuilding::sPyramidBuilding(bt);
                    int da = 0;
                    if(pyramid) {
                        const auto p = static_cast<ePyramidElement*>(ub);
                        da = 2*p->currentElevation();
                    }
                    const eTextureCollection* coll;
                    if(ch) {
                        coll = &trrTexs.fHouseAppeal;
                    } else {
                        coll = &trrTexs.fAppeal;
                    }
                    const double app = am.heat(dtx, dty);
                    const double mult = app > 0 ? 1 : -1;
                    const double appS = mult*pow(abs(app), 0.75);
                    int appId = (int)std::round(appS + 2.);
                    appId = std::clamp(appId, 0, 9);
                    const auto tex = coll->getTexture(appId);
                    tp.drawTexture(rx + da, ry + da, tex, eAlignment::top);
                    bd = true;
                }
            }
        };

        const auto drawCharacters = [&](eTile* const tile,
                                        const bool big,
                                        const bool crosswalk) {
            if(!tile) return;
            const int tx = tile->x();
            const int ty = tile->y();
            int rtx;
            int rty;
            eTileHelper::tileIdToRotatedTileId(tx, ty,
                                               rtx, rty, dir,
                                               boardw, boardh);
            const int da = tile->characterDoubleAltitude();
            const auto bttt = tile->underBuildingType();
            const bool flat = eBuilding::sFlatBuilding(bttt);
            const bool hover = tx == mHoverTX && ty == mHoverTY;
            const int hr = 200;
            const int hg = 200;
            const int hb = 255;
            const bool pyramid = eBuilding::sPyramidBuilding(bttt);
            if(flat || bttt == eBuildingType::wall || pyramid) {
                if(crosswalk) {
                    if(bttt != eBuildingType::road) return;
                    const auto b = tile->underBuilding();
                    const auto r = static_cast<eRoad*>(b);
                    const auto h = r->aboveHippodrome();
                    if(!h) return;
                } else {
                    if(bttt == eBuildingType::road) {
                        const auto b = tile->underBuilding();
                        const auto r = static_cast<eRoad*>(b);
                        const auto h = r->aboveHippodrome();
                        if(h) return;
                    }
                }
                const auto r = p.renderer();
                const auto& chars = tile->characters();
                for(const auto& c : chars) {
                    if(!c->visible()) continue;
                    const auto ct = c->type();
                    if(ct == eCharacterType::cartTransporter ||
                       ct == eCharacterType::ox ||
                       ct == eCharacterType::trailer) {
                        if(eBuilding::sSanctuaryBuilding(bttt)) {
                            continue;
                        }
                    }
                    const bool cbig = ct == eCharacterType::scylla ||
                                      ct == eCharacterType::kraken ||
                                      ct == eCharacterType::enemyBoat ||
                                      ct == eCharacterType::trireme ||
                                      ct == eCharacterType::tradeBoat;
                    if(big != cbig) continue;
                    const bool v = eViewModeHelpers::characterVisible(
                                       mViewMode, ct);
                    if(!v) continue;
                    const double cx = c->x();
                    const double cy = c->y();
                    double x;
                    double y;
                    if(dir == eWorldDirection::N) {
                        x = tx - da*0.5 + cx + 0.25;
                        y = ty - da*0.5 + cy + 0.25;
                    } else if(dir == eWorldDirection::E) {
                        x = rtx - da*0.5 + cy + 0.25;
                        y = rty - da*0.5 - cx + 1.25;
                    } else if(dir == eWorldDirection::S) {
                        x = rtx - da*0.5 - cx + 1.25;
                        y = rty - da*0.5 - cy + 1.25;
                    } else { // if(dir == eWorldDirection::W) {
                        x = rtx - da*0.5 - cy + 1.25;
                        y = rty - da*0.5 + cx + 0.25;
                    }
                    if(!pyramid) {
                        const auto t = tile->topRotated<eTile>(dir);
                        const auto l = tile->leftRotated<eTile>(dir);
                        const auto r = tile->rightRotated<eTile>(dir);
                        const auto b = tile->bottomRotated<eTile>(dir);
                        const auto tl = tile->topLeftRotated<eTile>(dir);
                        const auto tr = tile->topRightRotated<eTile>(dir);
                        const auto bl = tile->bottomLeftRotated<eTile>(dir);
                        const auto br = tile->bottomRightRotated<eTile>(dir);
                        if(tl && tl->characterDoubleAltitude() > da) {
                            const double mult = 1 - cx;
                            const int tla = tl->characterDoubleAltitude();
                            const double fa = mult*(tla - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(tr && tr->characterDoubleAltitude() > da) {
                            const double mult = 1 - cy;
                            const int tra = tr->characterDoubleAltitude();
                            const double fa = mult*(tra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(bl && bl->characterDoubleAltitude() > da) {
                            const double mult = cy;
                            const int bla = bl->characterDoubleAltitude();
                            const double fa = mult*(bla - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(br && br->characterDoubleAltitude() > da) {
                            const double mult = cx;
                            const int bra = br->characterDoubleAltitude();
                            const double fa = mult*(bra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(t && t->characterDoubleAltitude() > da) {
                            const double mult = (1 - cx)*(1 - cy);
                            const int ta = t->characterDoubleAltitude();
                            const double fa = mult*(ta - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(l && l->characterDoubleAltitude() > da) {
                            const double mult = (1 - cx)*cy;
                            const int la = l->characterDoubleAltitude();
                            const double fa = mult*(la - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(r && r->characterDoubleAltitude() > da) {
                            const double mult = cx*(1 - cy);
                            const int ra = r->characterDoubleAltitude();
                            const double fa = mult*(ra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(b && b->characterDoubleAltitude() > da) {
                            const double mult = cx*cy;
                            const int ba = b->characterDoubleAltitude();
                            const double fa = mult*(ba - da)*0.5;
                            x -= fa;
                            y -= fa;
                        }
                    }
                    const auto tex = c->getTexture(mTileSize);
                    if(tex) {
                        const double offX = mTileH*tex->offsetX()/30.;
                        const double offY = mTileH*tex->offsetY()/30.;
                        const double dstXf = 0.5*(x - y)*mTileW - offX;
                        const double dstYf = 0.5*(x + y)*mTileH - offY;
                        const int ddstXf = mDX + dstXf;
                        const int ddstYf = mDY + dstYf;
                        const int ddstX = std::round(ddstXf);
                        const int ddstY = std::round(ddstYf);
                        if(hover) {
                            tex->setColorMod(hr, hg, hb);
                        }
                        tex->render(r, ddstX, ddstY, false);
                        if(hover) {
                            tex->clearColorMod();
                        }
                    }
                    if(c->hasSecondaryTexture()) {
                        const auto stex = c->getSecondaryTexture(mTileSize);
                        const auto stexTex = stex.fTex;
                        if(stexTex) {
                            const double offX = mTileH*stexTex->offsetX()/30.;
                            const double offY = mTileH*stexTex->offsetY()/30.;
                            const double sdstXf = 0.5*(x + stex.fX - y - stex.fY)*mTileW - offX;
                            const double sdstYf = 0.5*(x + stex.fX + y + stex.fY)*mTileH - offY;
                            const double sddstXf = mDX + sdstXf;
                            const double sddstYf = mDY + sdstYf;
                            const int sddstX = std::round(sddstXf);
                            const int sddstY = std::round(sddstYf);
                            if(hover) {
                                stexTex->setColorMod(hr, hg, hb);
                            }
                            stexTex->render(r, sddstX, sddstY, false);
                            if(hover) {
                                stexTex->clearColorMod();
                            }
                        }
                    }
//                        tex->clearColorMod();
//                      tp.drawTexture(x, y, tex);
                }
            }
        };

        const auto drawNumber = [&](const int id) {
            const auto tex = numbers[id % 10];
            tp.drawTexture(rx - 1.65, ry - 2.60, tex,
                           eAlignment::hcenter | eAlignment::top);
        };

        const auto drawPatrolGuides = [&]() {
            if(mPatrolBuilding) {
                using ePatrolGuides = std::vector<ePatrolGuide>;
                const auto drawPGS = [&](const ePatrolGuides& pgs) {
                    int i = 0;
                    for(const auto& pg : pgs) {
                        if(pg.fX == tx && pg.fY == ty) {
                            const bool invalid = !eVectorHelpers::contains(mPatrolPath, tile) &&
                                                 !eVectorHelpers::contains(mPatrolPath1, tile);
                            const auto& coll = builTexs.fSpawner;
                            const int texId = mFrame % coll.size();
                            const auto& tex = coll.getTexture(texId);
                            if(invalid) tex->setColorMod(255, 125, 125);
                            //const auto& coll = builTexs.fPatrolGuides;
                            //const auto tex = coll.getTexture(14);
                            //tp.drawTexture(rx, ry, tex, eAlignment::top);
                            tp.drawTexture(rx, ry - 1, tex,
                                           eAlignment::hcenter | eAlignment::top);
                            drawNumber(i + 1);
                            if(invalid) tex->clearColorMod();
                            break;
                        }
                        i++;
                    }
                };
                const auto& pgs = mPatrolBuilding->patrolGuides();
                drawPGS(pgs);
            }
        };

        const auto drawSpawner = [&]() {
            if(mTem->visible()) {
                const auto& banners = tile->banners();
                for(const auto& b : banners) {
                    const auto& coll = builTexs.fSpawner;
                    const int texId = mFrame % coll.size();
                    const auto& tex = coll.getTexture(texId);
                    tp.drawTexture(rx, ry - 1, tex,
                                   eAlignment::hcenter | eAlignment::top);
                    const int id = b->id();
                    drawNumber(id);

                    std::shared_ptr<eTexture> topTex;
                    switch(b->type()) {
                    case eBannerTypeS::none:
                        break;
                    case eBannerTypeS::boar:
                        topTex = builTexs.fBoarPoint;
                        break;
                    case eBannerTypeS::deer:
                        topTex = builTexs.fDeerPoint;
                        break;
                    case eBannerTypeS::landInvasion:
                    case eBannerTypeS::seaInvasion:
                        topTex = builTexs.fLandInvasionPoint;
                        break;
                    case eBannerTypeS::disembarkPoint:
                        topTex = builTexs.fDisembarkPoint;
                        break;
                    case eBannerTypeS::entryPoint:
                        topTex = builTexs.fEntryPoint;
                        break;
                    case eBannerTypeS::riverEntryPoint:
                        topTex = builTexs.fRiverEntryPoint;
                        break;
                    case eBannerTypeS::exitPoint:
                        topTex = builTexs.fExitPoint;
                        break;
                    case eBannerTypeS::riverExitPoint:
                        topTex = builTexs.fRiverExitPoint;
                        break;
                    case eBannerTypeS::monsterPoint:
                        topTex = builTexs.fMonsterPoint;
                        break;
                    case eBannerTypeS::disasterPoint:
                    case eBannerTypeS::landSlidePoint:
                        topTex = builTexs.fDisasterPoint;
                        break;
                    case eBannerTypeS::wolf:
                        topTex = builTexs.fWolfPoint;
                        break;
                    }
                    if(topTex) {
                        tp.drawTexture(rx - 2.5, ry - 3.5, topTex,
                                       eAlignment::hcenter | eAlignment::top);
                    }
                }
            }
        };

        const auto drawMissiles = [&]() {
            const auto& mss = tile->missiles();
            for(const auto& m : mss) {
                const auto type = m->type();
                if(type == eMissileType::racingHorse) continue;
                const bool isWave = type == eMissileType::wave ||
                                    type == eMissileType::lava ||
                                    type == eMissileType::dust;
                if(isWave) continue;
                const double h = m->height();
                const double mx = m->x();
                const double my = m->y();
                double x;
                double y;
                if(dir == eWorldDirection::N) {
                    x = rtx + mx + 0.25 - h;
                    y = rty + my + 0.25 - h;
                } else if(dir == eWorldDirection::E) {
                    x = rtx + my + 0.25 - h;
                    y = rty - mx + 1.25 - h;
                } else if(dir == eWorldDirection::S) {
                    x = rtx - mx + 1.25 - h;
                    y = rty - my + 1.25 - h;
                } else { // if(dir == eWorldDirection::W) {
                    x = rtx - my + 1.25 - h;
                    y = rty + mx + 0.25 - h;
                }
                const auto tex = m->getTexture(mTileSize);
                tp.drawTexture(x, y, tex);
            }
        };

        const auto drawWaves = [this, dir, boardw, boardh, &tp](eTile* const tile) {
            const int tx = tile->x();
            const int ty = tile->y();
            int rtx;
            int rty;
            eTileHelper::tileIdToRotatedTileId(tx, ty,
                                               rtx, rty, dir,
                                               boardw, boardh);
            const auto& mss = tile->missiles();
            for(const auto& m : mss) {
                const auto type = m->type();
                if(type == eMissileType::racingHorse) continue;
                const bool isWave = type == eMissileType::wave ||
                                    type == eMissileType::lava ||
                                    type == eMissileType::dust;
                if(!isWave) continue;
                const double h = m->height();
                const double mx = m->x();
                const double my = m->y();
                double x;
                double y;
                if(dir == eWorldDirection::N) {
                    x = rtx + mx + 0.25 - h;
                    y = rty + my + 0.25 - h;
                } else if(dir == eWorldDirection::E) {
                    x = rtx + my + 0.25 - h;
                    y = rty - mx + 1.25 - h;
                } else if(dir == eWorldDirection::S) {
                    x = rtx - mx + 1.25 - h;
                    y = rty - my + 1.25 - h;
                } else { // if(dir == eWorldDirection::W) {
                    x = rtx - my + 1.25 - h;
                    y = rty + mx + 0.25 - h;
                }
                const auto tex = m->getTexture(mTileSize);
                tp.drawTexture(x, y, tex);
            }
        };

        const auto drawBanners = [&]() {
            const auto b = tile->soldierBanner();
            if(!b) return;
            const bool aid = b->militaryAid();
            bool hover;
            if(mLeftPressed && mMovedSincePress) {
                hover = false;
                const auto selected = selectedTiles();
                for(const auto t : selected) {
                   if(t == tile) {
                       hover = true;
                       break;
                   }
                }
            } else {
                hover = tx == mHoverTX && ty == mHoverTY;
            }

            {
                eGameTextures::loadBanners();
                const auto& rods = charTexs.fBannerRod;
                const auto& rod = rods.getTexture(0);
                if(hover) rod->setColorMod(175, 255, 255);
                else if(aid) rod->setColorMod(255, 125, 125);
                tp.drawTexture(rx, ry - 1, rod,
                               eAlignment::hcenter | eAlignment::top);
                if(hover || aid) rod->clearColorMod();
            }
            {
                const int id = b->id();
                const auto& bnrs = charTexs.fBanners;
                const auto& bnr = bnrs[id % bnrs.size()];
                int texId;
                if(b->selected()) {
                    texId = (mFrame/5) % 6;
                } else {
                    texId = 6;
                }
                const auto& tex = bnr.getTexture(texId);
                if(hover) tex->setColorMod(175, 255, 255);
                else if(aid) tex->setColorMod(255, 125, 125);
                tp.drawTexture(rx - 1, ry - 2.6, tex,
                               eAlignment::hcenter | eAlignment::top);
                if(hover || aid) tex->clearColorMod();
            }
            {
                const auto type = b->type();
                const auto& tps = charTexs.fBannerTops;
                const auto& pTps = charTexs.fPoseidonBannerTops;
                const bool p = b->atlantean();
                if(!p ||
                   type == eBannerType::aresWarrior ||
                   type == eBannerType::amazon) {
                    int itype = -1;
                    if(type == eBannerType::aresWarrior) {
                        itype = 0;
                    } else if(type == eBannerType::amazon) {
                        itype = -1;
                    } else if(type != eBannerType::enemy) {
                        itype = static_cast<int>(type);
                    }
                    if(itype != -1) {
                        const auto& top = tps.getTexture(itype);
                        if(hover) top->setColorMod(175, 255, 255);
                        else if(aid) top->setColorMod(255, 125, 125);
                        tp.drawTexture(rx - 2.5, ry -  3.5, top,
                                       eAlignment::hcenter | eAlignment::top);
                        if(hover || aid) top->clearColorMod();
                    }
                } else {
                    int itype = -1;
                    if(type == eBannerType::horseman) {
                        itype = 0;
                    } else if(type == eBannerType::rockThrower) {
                        itype = 1;
                    } else if(type == eBannerType::hoplite) {
                        itype = 2;
                    }
                    if(itype != -1) {
                        const auto& top = pTps.getTexture(itype);
                        if(hover) top->setColorMod(175, 255, 255);
                        else if(aid) top->setColorMod(255, 125, 125);
                        tp.drawTexture(rx - 2.5, ry -  3.5, top,
                                       eAlignment::hcenter | eAlignment::top);
                        if(hover || aid) top->clearColorMod();
                    }
                }
            }
        };

        if(tile) {
            const auto terrUb = tile->underBuilding();
            const auto terrBt = tile->underBuildingType();
            bool flatSanct = false;
            if(terrUb) {
                if(const auto sb = dynamic_cast<eSanctBuilding*>(terrUb)) {
                    const bool pyramid = eBuilding::sPyramidBuilding(terrBt);
                    if(!pyramid) flatSanct = sb->progress() <= 0;
                }
            }
            const auto terr = tile->terrain();
            if(!terrUb || flatSanct ||
               eBuilding::sFlatBuilding(terrBt)) {
                if(mViewMode == eViewMode::appeal && !terrUb &&
                   !sDontDrawAppeal(terr) && !tile->isElevationTile()) {
                    const auto& am = mBoard->appealMap();
                    const int ttdx = tile->dx();
                    const int ttdy = tile->dy();
                    const auto ae = am.enabled(ttdx, ttdy);
                    if(!ae) drawTerrain(tile);
                } else {
                    drawTerrain(tile);
                }
            }
        }
        //drawTerrain(tile);

        drawSheepGoat();
        drawPatrol();
        drawAppeal();

        if(tile->hasFish()) {
            const auto& fh = builTexs.fFish;
            const int t = mTime/30;
            const auto tex = fh.getTexture(t % fh.size());
            const auto a = eAlignment::right | eAlignment::top;
            tp.drawTexture(rx + 1, ry, tex, a);
        }
        if(tile->hasUrchin()) {
            const auto& fh = builTexs.fUrchin;
            const int t = mTime/30;
            const auto tex = fh.getTexture(t % fh.size());
            const auto a = eAlignment::bottom;
            tp.drawTexture(rx + 0.5, ry - 0.5, tex, a);
        }

        const auto drawBridge = [&]() {
            if(mode == eBuildingMode::bridge) {
                eGameTextures::loadBridge();
                const bool r = eVectorHelpers::contains(bridgetTs, tile);
                if(r) {
                    const int texId = bridgeRot ? 11 : 10;
                    const auto& tex = builTexs.fBridge.getTexture(texId);
                    if(bridgeValid) tex->setColorMod(0, 255, 0);
                    else tex->setColorMod(255, 0, 0);
                    tp.drawTexture(rx + 0.5, ry - 0.5, tex,
                                   eAlignment::hcenter | eAlignment::top);
                    tex->clearColorMod();
                }
            }
        };

        const auto drawCrosswalk = [&]() {
            if(mode == eBuildingMode::crosswalk) {
                const auto b = mBoard->buildingAt(mHoverTX, mHoverTY);
                if(b && b->type() == eBuildingType::hippodromePiece) {
                    bool red = false;
                    for(int dx = -1; dx <= 1; dx++) {
                        for(int dy = -1; dy <= 1; dy++) {
                            if(dx == 0 && dy == 0) continue;
                            const auto bb = mBoard->buildingAt(mHoverTX + dx, mHoverTY + dy);
                            if(bb && bb->type() == eBuildingType::road) {
                                const auto r = static_cast<eRoad*>(bb);
                                if(r->aboveHippodrome() == b) {
                                    red = true;
                                    break;
                                }
                            }
                        }
                    }
                    const auto h = static_cast<eHippodromePiece*>(b);
                    int id = h->id();
                    if(id == 0) {
                        id = 4;
                    } else if(id == 6) {
                        id = 2;
                    } else if(id != 2 && id != 4) {
                        return;
                    }
                    const auto& r = h->tileRect();
                    const auto rr = eTileHelper::toRotatedRect(
                        r, dir, boardw, boardh);
                    const int fitX = rr.x + rr.w - 1;
                    const int fitY = rr.y + rr.h - 1;
                    if(rtx != fitX || rty != fitY) return;

                    const int sizeId = static_cast<int>(mTileSize);
                    const auto& builTexs = eGameTextures::buildings()[sizeId];
                    const auto& coll = builTexs.fHippodrome;
                    stdsptr<eTexture> tex;
                    int hx;
                    int hy;
                    const auto draw = [&]() {
                        if(tex) {
                            double rx;
                            double ry;
                            drawXY(hx, hy, rx, ry, 1, 1, a);
                            if(red) tex->setColorMod(255, 0, 0);
                            else tex->setColorMod(0, 255, 0);
                            tp.drawTexture(rx + 0.5, ry - 0.5, tex,
                                           eAlignment::hcenter | eAlignment::top);
                            tex->clearColorMod();
                        }
                    };
                    if(id == 2) {
                        bool reverse = false;
                        int texId1;
                        int texId2;
                        int texId3;
                        switch(dir) {
                        case eWorldDirection::N: {
                            texId1 = 11;
                            texId2 = 12;
                            texId3 = 13;
                        } break;
                        case eWorldDirection::E: {
                            texId1 = 8;
                            texId2 = 9;
                            texId3 = 10;
                            reverse = true;
                        } break;
                        case eWorldDirection::S: {
                            texId1 = 13;
                            texId2 = 12;
                            texId3 = 11;
                            reverse = true;
                        } break;
                        case eWorldDirection::W: {
                            texId1 = 10;
                            texId2 = 9;
                            texId3 = 8;
                        } break;
                        }

                        for(int x = reverse ? r.x + r.w - 1 : r.x;
                            reverse ? x >= r.x : x < r.x + r.w;
                            reverse ? x-- : x++) {
                            hx = x;
                            hy = mHoverTY;
                            if(x == r.x) {
                                tex = coll.getTexture(texId1);
                            } else if(x == r.x + r.w - 1) {
                                tex = coll.getTexture(texId3);
                            } else {
                                tex = coll.getTexture(texId2);
                            }
                            draw();
                        }
                    } else if(id == 4) {
                        bool reverse = false;
                        int texId1;
                        int texId2;
                        int texId3;
                        switch(dir) {
                        case eWorldDirection::N: {
                            texId1 = 10;
                            texId2 = 9;
                            texId3 = 8;
                        } break;
                        case eWorldDirection::E: {
                            texId1 = 11;
                            texId2 = 12;
                            texId3 = 13;
                        } break;
                        case eWorldDirection::S: {
                            texId1 = 8;
                            texId2 = 9;
                            texId3 = 10;
                            reverse = true;
                        } break;
                        case eWorldDirection::W: {
                            texId1 = 13;
                            texId2 = 12;
                            texId3 = 11;
                            reverse = true;
                        } break;
                        }

                        for(int y = reverse ? r.y + r.h - 1 : r.y;
                            reverse ? y >= r.y : y < r.y + r.h;
                            reverse ? y-- : y++) {
                            hx = mHoverTX;
                            hy = y;
                            if(y == r.y) {
                                tex = coll.getTexture(texId1);
                            } else if(y == r.y + r.h - 1) {
                                tex = coll.getTexture(texId3);
                            } else {
                                tex = coll.getTexture(texId2);
                            }
                            draw();
                        }
                    } else {
                        return;
                    }
                }
            }
        };
        const auto drawCrosswalkCharacters = [&]() {
            auto b = ub;
            if(bt == eBuildingType::road) {
                const auto r = static_cast<eRoad*>(b);
                b = r->aboveHippodrome();
            }
            if(b && b->type() == eBuildingType::hippodromePiece) {
                const auto& r = b->tileRect();
                const auto rr = eTileHelper::toRotatedRect(
                    r, dir, boardw, boardh);
                const int fitX = rr.x + rr.w - 1;
                const int fitY = rr.y + rr.h - 1;
                if(rtx != fitX || rty != fitY) return;

                switch(dir) {
                case eWorldDirection::N: {
                    for(int y = r.y; y < r.y + r.h; y++) {
                        for(int x = r.x; x < r.x + r.w; x++) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::E: {
                    for(int x = r.x + r.w - 1; x >= r.x; x--) {
                        for(int y = r.y; y < r.y + r.h; y++) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::S: {
                    for(int y = r.y + r.h - 1; y >= r.y; y--) {
                        for(int x = r.x + r.w - 1; x >= r.x; x--) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::W: {
                    for(int x = r.x; x < r.x + r.w; x++) {
                        for(int y = r.y + r.h - 1; y >= r.y; y--) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                }
            }
        };

        drawBridge();
        drawPatrolGuides();
        drawSpawner();

        if(bt == eBuildingType::templeTile) {
            const auto t = static_cast<eTempleTileBuilding*>(ub);
            const int tid = t ? t->id() : 0;
            if(tid >= 10) {
                const auto s = t ? t->monument() : nullptr;
                const int f = s && s->finished();
                if(f) {
                    const auto& coll = builTexs.fSanctuaryFire;
                    const int textureTime = mFrame/4;
                    const int texId = textureTime % coll.size();
                    const auto& tex = coll.getTexture(texId);
                    tp.drawTexture(rx + 0.5, ry - 0.5, tex, eAlignment::bottom);
                }
            }
        }

        const auto r = p.renderer();
        enum class eTileClipSide {
            left, right
        };

        const auto clipTileRect = [&](const eTileClipSide side) {
            SDL_Rect clipRect;
            clipRect.y = -10000;
            clipRect.h = 20000;
            switch(side) {
            case eTileClipSide::left: {
                clipRect.x = mDX + (rtx - rty - 1)*mTileW/2;
                clipRect.w = 10000;
            } break;
            case eTileClipSide::right: {
                clipRect.x = mDX + (rtx - rty - 1)*mTileW/2 - 10000;
                clipRect.w = mTileW + 10000;
            } break;
            }
            SDL_RenderSetClipRect(r, &clipRect);
        };

        enum class eCharRenderOrder {
            x0y1x1y0,
            x1y1
        };

        const auto tileCharRenderOrder = [dir](const eTile* tile) {
            const auto tileFlat = [](const eTile* const tile) {
                const auto bt = tile->underBuildingType();
                const bool flat = eBuilding::sFlatBuilding(bt);
                return flat;
            };
            {
                const auto t_x0y1 = tile->bottomLeftRotated<eTile>(dir);
                if(t_x0y1) {
                    const bool flat = tileFlat(t_x0y1);
                    if(!flat) return eCharRenderOrder::x0y1x1y0;
                }
            }
            {
                const auto t_x1y0 = tile->bottomRightRotated<eTile>(dir);
                if(t_x1y0) {
                    const bool flat = tileFlat(t_x1y0);
                    if(!flat) return eCharRenderOrder::x0y1x1y0;
                }
            }
            return eCharRenderOrder::x1y1;
        };
        bool lastTile = false;
        if(dir == eWorldDirection::N) {
            lastTile = dty >= mBoard->height() - 2;
        } else if(dir == eWorldDirection::E) {
            lastTile = dtx <= 1;
        } else if(dir == eWorldDirection::S) {
            lastTile = dty <= 1;
        } else if(dir == eWorldDirection::W) {
            lastTile = dtx >= mBoard->width() - 2;
        }
        {
            const auto tt = tile->tileRelRotated<eTile>(-3, -3, dir);
            if(tt) {
                drawCharacters(tt, true, false);
            }
            if(lastTile) {
                for(int i = 0; i < 3; i++) {
                    const auto tt = tile->tileRelRotated<eTile>(-i, -i, dir);
                    if(tt) {
                        drawCharacters(tt, true, false);
                    }
                }
            }
        }
        {
            const auto t = tile->topRotated<eTile>(dir);
            if(t) {
                const auto order = tileCharRenderOrder(t);
                if(order == eCharRenderOrder::x1y1) {
                    drawCharacters(t, false, false);
                }
            }
        }
        {
            const auto tl = tile->topLeftRotated<eTile>(dir);
            if(tl) {
                const auto order = tileCharRenderOrder(tl);
                if(order == eCharRenderOrder::x0y1x1y0) {
                    clipTileRect(eTileClipSide::left);
                    drawCharacters(tl, false, false);
                    SDL_RenderSetClipRect(r, nullptr);
                }
            }
        }
        {
            const auto tr = tile->topRightRotated<eTile>(dir);
            if(tr) {
                const auto order = tileCharRenderOrder(tr);
                if(order == eCharRenderOrder::x0y1x1y0) {
                    clipTileRect(eTileClipSide::right);
                    drawCharacters(tr, false, false);
                    SDL_RenderSetClipRect(r, nullptr);
                }
            }
        }

        drawBanners();

        buildingDrawer(tile);
        drawCrosswalk();
        drawCrosswalkCharacters();

        drawMissiles();

        if(mBoard->duringTidalWave() ||
           mBoard->duringLavaFlow() ||
           mBoard->duringLandSlide()) {
            const auto tt = tile->tileRelRotated<eTile>(-2, -2, dir);
            if(tt) {
                drawWaves(tt);
            }
            if(lastTile) {
                for(int i = 0; i < 2; i++) {
                    const auto tt = tile->tileRelRotated<eTile>(-i, -i, dir);
                    if(tt) {
                        drawWaves(tt);
                    }
                }
            }
        }

        if(mLeftPressed && mMovedSincePress &&
           mGm->visible() && mGm->mode() == eBuildingMode::none) {
            const int x = mPressedX > mHoverX ? mHoverX : mPressedX;
            const int y = mPressedY > mHoverY ? mHoverY : mPressedY;
            const int w = abs(mPressedX - mHoverX);
            const int h = abs(mPressedY - mHoverY);
            SDL_Rect selRect{x - mDX, y - mDY, w, h};
            p.drawRect(selRect, SDL_Color{0, 255, 0, 255}, 1);
        }
    });

    tp.handleScheduledDraw();

    if(mPatrolBuilding) {
        const auto& pgs = mPatrolBuilding->patrolGuides();
        if(!pgs.empty()) {
            const auto t = mPatrolBuilding->centerTile();
            const int tx = t->x();
            const int ty = t->y();
            int rtx;
            int rty;
            eTileHelper::tileIdToRotatedTileId(tx, ty,
                                               rtx, rty, dir,
                                               boardw, boardh);
            const int ta = t->altitude();
            std::vector<SDL_Point> polygon;
            polygon.reserve(pgs.size() + 2);
            polygon.push_back({rtx - ta, rty - ta});
            for(const auto& pg : pgs) {
                const int tx = pg.fX;
                const int ty = pg.fY;
                int rtx;
                int rty;
                eTileHelper::tileIdToRotatedTileId(tx, ty,
                                                   rtx, rty, dir,
                                                   boardw, boardh);
                const auto t = mBoard->tile(tx, ty);
                const int ta = t->altitude();
                polygon.push_back({rtx - ta, rty - ta});
            }
            polygon.push_back({rtx - ta, rty - ta});
            tp.drawPolygon(polygon, {0, 0, 0, 255});
        }
    }

    const auto drawBuildText = [&](const std::string& text) {
        p.drawText(mHoverX - mDX + padding(), mHoverY - mDY + padding(), text, eFontColor::light);
    };

    const auto drawBuildDims = [&](const int buildW, const int buildH) {
        const auto text = std::to_string(buildW) + " x " + std::to_string(buildH);
        drawBuildText(text);
    };

    const auto drawBuildCount = [&](const int buildCount) {
        drawBuildText(std::to_string(buildCount));
    };

    if((mode == eBuildingMode::road ||
        mode == eBuildingMode::doricColumn ||
        mode == eBuildingMode::ionicColumn ||
        mode == eBuildingMode::corinthianColumn) && mLeftPressed) {
        int buildW = 0;
        int buildH = 0;
        const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
        std::vector<eOrientation> path;
        const bool r = mode == eBuildingMode::road ? roadPath(path) :
                                                     columnPath(path);
        if(r) {
            const auto drawBase = [&](eTile* const t) {
                const auto& tex = trrTexs.fBuildingBase;
                double rx;
                double ry;
                drawXY(t->x(), t->y(), rx, ry, 1, 1, t->altitude());
                tp.drawTexture(rx, ry, tex, eAlignment::top);

                buildW = std::max(buildW, 1 + std::abs(mHoverTX - t->x()));
                buildH = std::max(buildH, 1 + std::abs(mHoverTY - t->y()));
            };
            eTile* t = startTile;
            for(int i = path.size() - 1; i >= 0; i--) {
                if(!t) break;
                drawBase(t);
                t = t->neighbour<eTile>(path[i]);
            }
            if(t) drawBase(t);

            drawBuildDims(buildW, buildH);
            return;
        }
    }

    if(mode == eBuildingMode::bridge) {
        if(!bridgeValid && bridgetTs.empty()) {
            eGameTextures::loadBridge();
            const auto hoverTile = mBoard->tile(mHoverTX, mHoverTY);
            if(hoverTile) {
                const auto& tex = builTexs.fBridge.getTexture(10);
                tex->setColorMod(255, 0, 0);
                double rx;
                double ry;
                const int hx = hoverTile->x();
                const int hy = hoverTile->y();
                const int ha = hoverTile->altitude();
                drawXY(hx, hy, rx, ry, 1, 1, ha);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
                tex->clearColorMod();
            }
        }
    }

    if(mLeftPressed) {
        if(mode == eBuildingMode::vine ||
           mode == eBuildingMode::oliveTree ||
           mode == eBuildingMode::orangeTree) {
            int buildCount = 0;
            std::shared_ptr<eTexture> tex;
            if(mode == eBuildingMode::vine) {
                tex = builTexs.fVine.getTexture(0);
                tex->setColorMod(0, 255, 0);
            } else if(mode == eBuildingMode::oliveTree) {
                tex = builTexs.fOliveTree.getTexture(0);
                tex->setColorMod(0, 255, 0);
            } else if(mode == eBuildingMode::orangeTree) {
                tex = builTexs.fOrangeTree.getTexture(0);
                tex->setColorMod(0, 255, 0);
            } else {
                tex = trrTexs.fBuildingBase;
            }
            for(int x = sMinX; x <= sMaxX; x++) {
                for(int y = sMinY; y <= sMaxY; y++) {
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    if(t->terrain() != eTerrain::fertile) continue;
                    if(t->underBuilding()) continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);

                    buildCount++;
                }
            }
            tex->clearColorMod();
            drawBuildCount(buildCount);
            return;
        }

        if(mode == eBuildingMode::sheep ||
           mode == eBuildingMode::goat ||
           mode == eBuildingMode::cattle) {
            int buildCount = 0;
            const auto tex = trrTexs.fBuildingBase;
            tex->setColorMod(0, 255, 0);
            const auto bt = eBuildingModeHelpers::toBuildingType(mode);
            const int allowed = mBoard->countAllowed(mViewedCityId, bt);
            int n = 1;
            for(int x = sMinX; x <= sMaxX; x++) {
                for(int y = sMinY; y <= sMaxY; y++) {
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    if(t->underBuilding()) continue;
                    const auto t2 = mBoard->tile(x, y + 1);
                    if(!t2) continue;
                    if(t2->underBuilding()) continue;
                    if(t2->terrain() != eTerrain::fertile &&
                       t->terrain() != eTerrain::fertile) continue;
                    double rx;
                    double ry;
                    const int a = t->altitude();
                    const bool exccess = n > allowed;
                    if(exccess) {
                        tex->setColorMod(255, 0, 0);
                    }
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tp.drawTexture(rx, ry + 1, tex, eAlignment::top);
                    y++;
                    n++;

                    buildCount++;
                }
            }
            tex->clearColorMod();

            drawBuildCount(buildCount);
        }

        if(mode == eBuildingMode::park ||
           mode == eBuildingMode::avenue ||
           mode == eBuildingMode::wall) {
            int buildW = 0;
            int buildH = 0;

            const auto& tex = trrTexs.fBuildingBase;
            for(int x = sMinX; x <= sMaxX; x++) {
                for(int y = sMinY; y <= sMaxY; y++) {
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    const auto terr = t->terrain();
                    const bool b = static_cast<bool>(terr & eTerrain::buildable);
                    if(!b) continue;
                    if(t->underBuilding()) continue;
                    if(mode == eBuildingMode::avenue) {
                        const bool hr = canBuildAvenue(t, mViewedCityId, ppid,
                                                       mEditorMode);
                        if(!hr) continue;
                    }
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);

                    buildW = 1 + std::max(buildW, std::abs(mHoverTX - t->x()));
                    buildH = 1 + std::max(buildH, std::abs(mHoverTY - t->y()));
                }
            }
            drawBuildDims(buildW, buildH);
            return;
        }

        if(mode == eBuildingMode::commonHousing) {
            std::vector<SDL_Rect> rects;
            const auto& tex = trrTexs.fBuildingBase;
            std::set<int> xs;
            std::set<int> ys;
            for(int x = sMinX; x <= sMaxX; x++) {
                for(int y = sMinY - 1; y <= sMaxY; y++) {
                    const SDL_Rect rect{x, y, 2, 2};
                    bool cbr = true;
                    for(const auto& r : rects) {
                        const bool i = SDL_HasIntersection(&r, &rect);
                        if(i) {
                            cbr = false;
                            break;
                        }
                    }
                    if(!cbr) continue;
                    const bool cb = mBoard->canBuildBase(x, x + 2, y, y + 2,
                                                         mEditorMode,
                                                         mViewedCityId, ppid);
                    if(!cb) continue;
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tp.drawTexture(rx + 1, ry, tex, eAlignment::top);
                    tp.drawTexture(rx, ry + 1, tex, eAlignment::top);
                    tp.drawTexture(rx + 1, ry + 1, tex, eAlignment::top);
                    rects.emplace_back(rect);

                    xs.insert(x);
                    xs.insert(x + 1);
                    ys.insert(y);
                    ys.insert(y + 1);
                }
            }

            drawBuildDims(xs.size()/2, ys.size()/2);
            return;
        }
    }


    if(mode == eBuildingMode::sheep ||
       mode == eBuildingMode::goat ||
       mode == eBuildingMode::cattle) {
        if(mLeftPressed) return;
        const auto bt = eBuildingModeHelpers::toBuildingType(mode);
        const int allowed = mBoard->countAllowed(mViewedCityId, bt);
        double rx;
        double ry;
        const auto t = mBoard->tile(mHoverTX, mHoverTY);
        if(!t) return;
        const int tx = t->x();
        const int ty = t->y();
        const bool cb = allowed > 0 && mBoard->canBuild(
                            tx, ty, 1, 2,
                            mEditorMode,
                            mViewedCityId, ppid,
                            true, true);
        const auto& tex = trrTexs.fBuildingBase;
        tex->setColorMod(cb ? 0 : 255, cb ? 255 : 0, 0);
        const int a = t->altitude();
        drawXY(mHoverTX, mHoverTY, rx, ry, 1, 1, a);
        tp.drawTexture(rx, ry, tex, eAlignment::top);
        tp.drawTexture(rx, ry + 1, tex, eAlignment::top);
        tex->clearColorMod();
        return;
    }

    const auto t = mBoard->tile(mHoverTX, mHoverTY);
    const int tx = mHoverTX;
    const int ty = mHoverTY;
    const int a = t ? t->altitude() : 0;

    switch(mode) {
    case eBuildingMode::commonAgora: {
        eGameTextures::loadAgora();
        const auto& agr = builTexs.fAgora;
        const auto& agrr = builTexs.fAgoraRoad;

        eAgoraOrientation bt;
        const auto p = agoraBuildPlaceIter(t, false, bt, mViewedCityId, ppid);
        if(p.empty()) {
            const auto& tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for(int i = tx - 1; i < tx + 2; i++) {
                for(int j = ty - 3; j < ty + 3; j++) {
                    double rx;
                    double ry;
                    drawXY(i, j, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S) ? 0 : 1;
            const auto& road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for(int j = ty - 3; j < ty + 3; j++) {
                double rx;
                double ry;
                drawXY(tx - 1, j, rx, ry, 1, 1, a);
                tp.drawTexture(rx, ry, road, eAlignment::top);
            }
            road->clearColorMod();
        } else {
            if(bt == eAgoraOrientation::bottomRight) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                        if(i == 5) i++;
                    } else if(i < 12) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            } else if(bt == eAgoraOrientation::topLeft) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    } else if(i > 12) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            } else if(bt == eAgoraOrientation::bottomLeft) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    } else if(i > 11) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            } else if(bt == eAgoraOrientation::topRight) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    } else if(i < 12) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
        }
    } break;
    case eBuildingMode::grandAgora: {
        eGameTextures::loadAgora();
        const auto& agr = builTexs.fAgora;
        const auto& agrr = builTexs.fAgoraRoad;

        eAgoraOrientation bt;
        const auto p = agoraBuildPlaceIter(t, true, bt, mViewedCityId, ppid);
        if(p.empty()) {
            const auto& tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for(int i = tx - 2; i < tx + 3; i++) {
                for(int j = ty - 3; j < ty + 3; j++) {
                    double rx;
                    double ry;
                    drawXY(i, j, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S) ? 0 : 1;
            const auto& road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for(int j = ty - 3; j < ty + 3; j++) {
                double rx;
                double ry;
                drawXY(tx, j, rx, ry, 1, 1, a);
                tp.drawTexture(rx, ry, road, eAlignment::top);
            }
            road->clearColorMod();
        } else {
            if(bt == eAgoraOrientation::bottomRight) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    } else if((i > 12 && i < 18) || (i > 24 && i < 30)) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            } else if(bt == eAgoraOrientation::bottomLeft) {
                const int iMax = p.size();
                for(int i = 0; i < iMax; i++) {
                    const auto t = p[i];
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    stdsptr<eTexture> tex;
                    int dim;
                    if(i < 6) {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    } else if((i < 12) || (i > 29 && i < 35)) {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    } else {
                        continue;
                    }
                    double rx;
                    double ry;
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if(dim == 2) {
                        if(dir == eWorldDirection::E) {
                            rx -= 1;
                        } else if(dir == eWorldDirection::S) {
                            rx -= 1;
                            ry += 1;
                        } else if(dir == eWorldDirection::W) {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
        }
    } break;
    default: break;
    }

    const auto& wrld = mBoard->world();
    if(t && mGm->visible()) {
        bool fertile = false;
        std::function<bool(const int tx, const int ty,
                           const int sw, const int sh)> canBuildFunc;
        switch(mode) {
        case eBuildingMode::urchinQuay:
        case eBuildingMode::fishery: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                eDiagonalOrientation o;
                return canBuildFishery(tx, ty, o);
            };
        } break;
        case eBuildingMode::triremeWharf: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                eDiagonalOrientation o;
                return canBuildTriremeWharf(tx, ty, o);
            };
        } break;
        case eBuildingMode::pier: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                if(sw > 2 || sh > 2) return true;
                eDiagonalOrientation o;
                return canBuildPier(tx, ty, o, mViewedCityId, ppid, mEditorMode);
            };
        } break;
        case eBuildingMode::palace: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                if(mBoard->hasPalace(mViewedCityId)) return false;
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        } break;
        case eBuildingMode::stadium: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                if(mBoard->hasStadium(mViewedCityId)) return false;
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        } break;
        case eBuildingMode::foodVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::food);
            };
        } break;
        case eBuildingMode::fleeceVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::fleece);
            };
        } break;
        case eBuildingMode::oilVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::oliveOil);
            };
        } break;
        case eBuildingMode::wineVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::wine);
            };
        } break;
        case eBuildingMode::armsVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::armor);
            };
        } break;
        case eBuildingMode::horseTrainer: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::horse);
            };
        } break;
        case eBuildingMode::chariotVendor: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::chariot);
            };
        } break;
        case eBuildingMode::avenue: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                const auto t = mBoard->tile(tx, ty);
                return canBuildAvenue(t, mViewedCityId, ppid, mEditorMode);
            };
        } break;
        default: {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        } break;
        }

        struct eB {
            eB(const int tx, const int ty,
               const stdsptr<eBuilding>& b) :
                fTx(tx), fTy(ty), fB(b) {}

            int fTx;
            int fTy;
            stdsptr<eBuilding> fB;
            stdsptr<eBuildingRenderer> fBR;
        };

        std::vector<eB> ebs;
        switch(mode) {
        case eBuildingMode::modestPyramid:
        case eBuildingMode::pyramid:
        case eBuildingMode::greatPyramid:
        case eBuildingMode::majesticPyramid:

        case eBuildingMode::smallMonumentToTheSky:
        case eBuildingMode::monumentToTheSky:
        case eBuildingMode::grandMonumentToTheSky:

        case eBuildingMode::minorShrineAphrodite:
        case eBuildingMode::minorShrineApollo:
        case eBuildingMode::minorShrineAres:
        case eBuildingMode::minorShrineArtemis:
        case eBuildingMode::minorShrineAthena:
        case eBuildingMode::minorShrineAtlas:
        case eBuildingMode::minorShrineDemeter:
        case eBuildingMode::minorShrineDionysus:
        case eBuildingMode::minorShrineHades:
        case eBuildingMode::minorShrineHephaestus:
        case eBuildingMode::minorShrineHera:
        case eBuildingMode::minorShrineHermes:
        case eBuildingMode::minorShrinePoseidon:
        case eBuildingMode::minorShrineZeus:

        case eBuildingMode::shrineAphrodite:
        case eBuildingMode::shrineApollo:
        case eBuildingMode::shrineAres:
        case eBuildingMode::shrineArtemis:
        case eBuildingMode::shrineAthena:
        case eBuildingMode::shrineAtlas:
        case eBuildingMode::shrineDemeter:
        case eBuildingMode::shrineDionysus:
        case eBuildingMode::shrineHades:
        case eBuildingMode::shrineHephaestus:
        case eBuildingMode::shrineHera:
        case eBuildingMode::shrineHermes:
        case eBuildingMode::shrinePoseidon:
        case eBuildingMode::shrineZeus:

        case eBuildingMode::majorShrineAphrodite:
        case eBuildingMode::majorShrineApollo:
        case eBuildingMode::majorShrineAres:
        case eBuildingMode::majorShrineArtemis:
        case eBuildingMode::majorShrineAthena:
        case eBuildingMode::majorShrineAtlas:
        case eBuildingMode::majorShrineDemeter:
        case eBuildingMode::majorShrineDionysus:
        case eBuildingMode::majorShrineHades:
        case eBuildingMode::majorShrineHephaestus:
        case eBuildingMode::majorShrineHera:
        case eBuildingMode::majorShrineHermes:
        case eBuildingMode::majorShrinePoseidon:
        case eBuildingMode::majorShrineZeus:

        case eBuildingMode::pyramidToThePantheon:
        case eBuildingMode::altarOfOlympus:
        case eBuildingMode::templeOfOlympus:
        case eBuildingMode::observatoryKosmika:
        case eBuildingMode::museumAtlantika: {
            const auto& tex = trrTexs.fBuildingBase;
            const auto type = eBuildingModeHelpers::toBuildingType(mode);
            int sw;
            int sh;
            ePyramid::sDimensions(type, sw, sh);
            const int xMin = mHoverTX - sw/2;
            const int yMin = mHoverTY - sh/2;
            const int xMax = xMin + sw;
            const int yMax = yMin + sh;
            const bool cb = mBoard->canBuildBase(xMin, xMax, yMin, yMax,
                                                 mEditorMode,
                                                 mViewedCityId, ppid);
            if(!cb) tex->setColorMod(255, 0, 0);
            for(int x = xMin; x < xMax; x++) {
                for(int y = yMin; y < yMax; y++) {
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    if(t->underBuilding()) continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
            if(!cb) tex->clearColorMod();
        } break;
        case eBuildingMode::templeAphrodite:
        case eBuildingMode::templeApollo:
        case eBuildingMode::templeAres:
        case eBuildingMode::templeArtemis:
        case eBuildingMode::templeAthena:
        case eBuildingMode::templeAtlas:
        case eBuildingMode::templeDemeter:
        case eBuildingMode::templeDionysus:
        case eBuildingMode::templeHades:
        case eBuildingMode::templeHephaestus:
        case eBuildingMode::templeHera:
        case eBuildingMode::templeHermes:
        case eBuildingMode::templePoseidon:
        case eBuildingMode::templeZeus: {
            const auto& tex = trrTexs.fBuildingBase;
            const auto type = eBuildingModeHelpers::toBuildingType(mode);
            const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, mRotate);
            const int sw = h->fW;
            const int sh = h->fH;
            const int xMin = mHoverTX - sw/2;
            const int yMin = mHoverTY - sh/2;
            const int xMax = xMin + sw;
            const int yMax = yMin + sh;
            const bool cb = mBoard->canBuildBase(xMin, xMax, yMin, yMax,
                                                 mEditorMode,
                                                 mViewedCityId, ppid);
            if(!cb) tex->setColorMod(255, 0, 0);
            for(int x = xMin; x < xMax; x++) {
                for(int y = yMin; y < yMax; y++) {
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if(!t) continue;
                    if(t->underBuilding()) continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
           if(!cb) tex->clearColorMod();
//            const auto bt = eBuildingModeHelpers::toBuildingType(mode);
//            const auto h = eSanctBlueprints::sSanctuaryBlueprint(bt, mRotate);
//            const int sw = h->fW;
//            const int sh = h->fH;
//            const int minX = mHoverTX - sw/2;
//            const int maxX = minX + sw;
//            const int minY = mHoverTY - sh/2;
//            const int maxY = minY + sh;
//            const bool cb = canBuildBase(minX, maxX, minY, maxY);
//            const int rmod = cb ? 0 : 255;
//            const int gmod = cb ? 255 : 0;

//            eGodType god;
//            stdsptr<eSanctuary> b;
//            switch(mode) {
//            case eBuildingMode::templeZeus: {
//                god = eGodType::zeus;
//                b = e::make_shared<eZeusSanctuary>(
//                        sw, sh, *mBoard);
//            } break;
//            case eBuildingMode::templeArtemis: {
//                god = eGodType::artemis;
//                b = e::make_shared<eArtemisSanctuary>(
//                        sw, sh, *mBoard);
//            } break;
//            case eBuildingMode::templeHephaestus: {
//                god = eGodType::hephaestus;
//                b = e::make_shared<eHephaestusSanctuary>(
//                        sw, sh, *mBoard);
//            } break;
//            default:
//                break;
//            }

//            const auto mint = mBoard->tile(mHoverTX, mHoverTY);
//            if(!mint) return;
//            const int a = mint->altitude();
//            b->setAltitude(a);

//            b->setTileRect({minX, minY, sw, sh});
//            const auto ct = mBoard->tile((minX + maxX)/2, (minY + maxY)/2);
//            b->setCenterTile(ct);

//            const auto tb = e::make_shared<eTempleBuilding>(*mBoard);
//            tb->setSanctuary(b.get());
//            b->registerElement(tb);

//            for(const auto& tv : h->fTiles) {
//                for(const auto& t : tv) {
//                    const double tx = minX + t.fX + 0.5;
//                    const double ty = minY + t.fY + 1.5;
//                    const int alt = a + t.fA;
//                    switch(t.fType) {
//                    case eSanctEleType::tile: {
//                        const auto tt = e::make_shared<eTempleTileBuilding>(
//                                            t.fId, *mBoard);
//                        tt->setSanctuary(b.get());
//                        b->registerElement(tt);
//                        const auto tex = tt->getTileTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt, ty - alt, tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    case eSanctEleType::stairs: {
//                        const auto s = e::make_shared<eStairsRenderer>(t.fId, b);
//                        const auto tex = s->getTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt, ty - alt, tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    default:
//                        break;
//                    };
//                }
//            }
//            for(const auto& tv : h->fTiles) {
//                for(const auto& t : tv) {
//                    const double tx = minX + t.fX + 0.5;
//                    const double ty = minY + t.fY + 1.5;
//                    const auto tile = mBoard->tile(tx, ty);
//                    if(!tile) continue;
//                    const int alt = a + t.fA;
//                    switch(t.fType) {
//                    case eSanctEleType::copper: {
//                        const int tid = static_cast<int>(mTileSize);
//                        const auto& textures = eGameTextures::terrain().at(tid);
//                        const auto tex = textures.fBronzeTerrainTexs.getTexture(0);
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt, ty - alt, tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    case eSanctEleType::statue: {
//                        const auto tt = e::make_shared<eTempleStatueBuilding>(
//                                           god, t.fId, *mBoard);
//                        tt->setSanctuary(b.get());
//                        b->registerElement(tt);
//                        const auto tex = tt->getTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt, ty - alt, tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    case eSanctEleType::monument: {
//                        const auto tt = e::make_shared<eTempleMonumentBuilding>(
//                                            god, t.fId, *mBoard);
//                        tt->setSanctuary(b.get());
//                        const int d = mRotate ? 1 : 0;
//                        b->registerElement(tt);
//                        const auto tex = tt->getTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt - d + 0.5, ty - alt + d + 0.5,
//                                       tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    case eSanctEleType::altar: {
//                        const auto tt = e::make_shared<eTempleAltarBuilding>(
//                                            *mBoard);
//                        tt->setSanctuary(b.get());
//                        const int d = mRotate ? 1 : 0;
//                        b->registerElement(tt);
//                        const auto tex = tt->getTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt - d + 0.5, ty - alt + d + 0.5,
//                                       tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    case eSanctEleType::sanctuary: {
//                        int dx;
//                        int dy;
//                        if(mRotate) {
//                            dx = -2;
//                            dy = 2;
//                        } else {
//                            dx = 1;
//                            dy = -1;
//                        }
//                        const auto tt = e::make_shared<eTempleRenderer>(t.fId, tb);
//                        const auto tex = tt->getTexture(tp.size());
//                        tex->setColorMod(rmod, gmod, 0);
//                        tp.drawTexture(tx - alt + dx + 0.5, ty - alt + dy + 0.5 + 2, tex, eAlignment::top);
//                        tex->clearColorMod();
//                    } break;
//                    default:
//                        break;
//                    }
//                }
//            }
//            tb->setCost({ts*5, ts*5, 0});
//            for(const auto& tv : h->fTiles) {
//                for(const auto& t : tv) {
//                    const int tx = minX + t.fX;
//                    const int ty = minY + t.fY;
//                    const auto tile = mBoard->tile(tx, ty);
//                    tile->setAltitude(tile->altitude() + t.fA);
//                    const auto trr = tile->terrain();
//                    const bool bldbl = static_cast<bool>(
//                                           trr & eTerrain::buildable);
//                    if(!tile->underBuilding() && bldbl) {
//                        tile->setUnderBuilding(b);
//                        b->addUnderBuilding(tile);
//                    }
//                }
//            }
        } break;
        case eBuildingMode::road: {
            const auto b1 = e::make_shared<eRoad>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::bridge: {

        } break;
        case eBuildingMode::roadblock: {
            const auto b1 = e::make_shared<eRoad>(*mBoard, mViewedCityId);
            b1->setRoadblock(true);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh) {
                (void)sw;
                (void)sh;
                const auto t = mBoard->tile(tx, ty);
                if(!t) return false;
                const bool hr = t->hasRoad();
                if(!hr) return false;
                const bool b = t->hasBridge();
                if(b) return false;
                const auto ub = t->underBuilding();
                const auto r = static_cast<eRoad*>(ub);
                return !r->isRoadblock();
            };
        } break;
        case eBuildingMode::achillesHall:
        case eBuildingMode::atalantaHall:
        case eBuildingMode::bellerophonHall:
        case eBuildingMode::herculesHall:
        case eBuildingMode::jasonHall:
        case eBuildingMode::odysseusHall:
        case eBuildingMode::perseusHall:
        case eBuildingMode::theseusHall: {
            const auto hallType = eBuildingModeHelpers::toBuildingType(mode);
            const auto heroType = eHerosHall::sHallTypeToHeroType(hallType);
            const auto b1 = e::make_shared<eHerosHall>(heroType, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::commonHousing: {
            const auto b1 = e::make_shared<eSmallHouse>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::gymnasium: {
            const auto b1 = e::make_shared<eGymnasium>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::podium: {
            const auto b1 = e::make_shared<ePodium>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::fountain: {
            const auto b1 = e::make_shared<eFountain>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::watchpost: {
            const auto b1 = e::make_shared<eWatchpost>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::maintenanceOffice: {
            const auto b1 = e::make_shared<eMaintenanceOffice>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::college: {
            const auto b1 = e::make_shared<eCollege>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::dramaSchool: {
            const auto b1 = e::make_shared<eDramaSchool>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::theater: {
            const auto b1 = e::make_shared<eTheater>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::hospital: {
            const auto b1 = e::make_shared<eHospital>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::stadium: {
            const auto b1 = e::make_shared<eStadium>(*mBoard, mRotate, mViewedCityId);
            auto& ebs1 = ebs.emplace_back(mHoverTX, mHoverTY, b1);
            ebs1.fBR = e::make_shared<eStadium1Renderer>(b1);
            int dx;
            int dy;
            if(mRotate) {
                dx = 0;
                dy = 5;
            } else {
                dx = 5;
                dy = 0;
            }
            auto& ebs2 = ebs.emplace_back(mHoverTX + dx, mHoverTY + dy, b1);
            ebs2.fBR = e::make_shared<eStadium2Renderer>(b1);
        } break;
        case eBuildingMode::bibliotheke: {
            const auto b1 = e::make_shared<eBibliotheke>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::observatory: {
            const auto b1 = e::make_shared<eObservatory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::university: {
            const auto b1 = e::make_shared<eUniversity>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::laboratory: {
            const auto b1 = e::make_shared<eLaboratory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::inventorsWorkshop: {
            const auto b1 = e::make_shared<eInventorsWorkshop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::museum: {
            const auto b1 = e::make_shared<eMuseum>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::palace: {
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const auto b1 = e::make_shared<ePalace>(*mBoard, mRotate, mViewedCityId);

            int dx;
            int dy;
            int sw;
            int sh;
            const int tminX = tx - 2;
            const int tminY = ty - 3;
            int tmaxX;
            int tmaxY;
            if(mRotate) {
                dx = 0;
                dy = 4;
                sw = 4;
                sh = 8;
                tmaxX = tminX + 6;
                tmaxY = tminY + 9;
            } else {
                dx = 4;
                dy = 0;
                sw = 8;
                sh = 4;
                tmaxX = tminX + 9;
                tmaxY = tminY + 6;
            }
            const SDL_Rect rect{tminX + 1, tminY + 1, sw, sh};
            for(int x = tminX; x < tmaxX; x++) {
                for(int y = tminY; y < tmaxY; y++) {
                    const SDL_Point pt{x, y};
                    const bool r = SDL_PointInRect(&pt, &rect);
                    if(r) continue;
                    bool other = x == tminX && y == tminY;
                    if(!other) {
                        if(mRotate) {
                            other = x == tmaxX - 1 && y == tminY;
                        } else {
                            other = x == tminX && y == tmaxY - 1;
                        }
                    }
                    const auto b0 = e::make_shared<ePalaceTile>(
                                        *mBoard, other, mViewedCityId);
                    b0->setPalace(b1.get());
                    ebs.emplace_back(x, y, b0);
                }
            }

            auto& ebs1 = ebs.emplace_back(tx, ty, b1);
            ebs1.fBR = e::make_shared<ePalace1Renderer>(b1);
            auto& ebs2 = ebs.emplace_back(tx + dx, ty + dy, b1);
            ebs2.fBR = e::make_shared<ePalace2Renderer>(b1);
            if(dir == eWorldDirection::S) {
                std::swap(ebs1, ebs2);
            }
        } break;
        case eBuildingMode::eliteHousing: {
            int dx1;
            int dy1;
            int dx2;
            int dy2;
            int dx3;
            int dy3;
            int dx4;
            int dy4;
            if(dir == eWorldDirection::N) {
                dx1 = 0;
                dy1 = 0;
                dx2 = 2;
                dy2 = 0;
                dx3 = 2;
                dy3 = 2;
                dx4 = 0;
                dy4 = 2;
            } else if(dir == eWorldDirection::E) {
                dx1 = 2;
                dy1 = 0;
                dx2 = 2;
                dy2 = 2;
                dx3 = 0;
                dy3 = 2;
                dx4 = 0;
                dy4 = 0;
            } else if(dir == eWorldDirection::S) {
                dx1 = 2;
                dy1 = 2;
                dx2 = 0;
                dy2 = 2;
                dx3 = 0;
                dy3 = 0;
                dx4 = 2;
                dy4 = 0;
            } else { // if(dir == eWorldDirection::W) {
                dx1 = 0;
                dy1 = 2;
                dx2 = 0;
                dy2 = 0;
                dx3 = 2;
                dy3 = 0;
                dx4 = 2;
                dy4 = 2;
            }
            const auto b1 = e::make_shared<eEliteHousing>(*mBoard, mViewedCityId);
            auto& ebs1 = ebs.emplace_back(mHoverTX + dx1, mHoverTY + dy1, b1);
            ebs1.fBR = e::make_shared<eEliteHousingRenderer>(
                           eEliteRendererType::top, b1);
            auto& ebs2 = ebs.emplace_back(mHoverTX + dx2, mHoverTY + dy2, b1);
            ebs2.fBR = e::make_shared<eEliteHousingRenderer>(
                           eEliteRendererType::right, b1);
            auto& ebs3 = ebs.emplace_back(mHoverTX + dx3, mHoverTY + dy3, b1);
            ebs3.fBR = e::make_shared<eEliteHousingRenderer>(
                           eEliteRendererType::bottom, b1);
            auto& ebs4 = ebs.emplace_back(mHoverTX + dx4, mHoverTY + dy4, b1);
            ebs4.fBR = e::make_shared<eEliteHousingRenderer>(
                           eEliteRendererType::left, b1);
        } break;
        case eBuildingMode::taxOffice: {
            const auto b1 = e::make_shared<eTaxOffice>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::mint: {
            const auto b1 = e::make_shared<eMint>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::foundry: {
            const auto b1 = e::make_shared<eFoundry>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::timberMill: {
            const auto b1 = e::make_shared<eTimberMill>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::masonryShop: {
            const auto b1 = e::make_shared<eMasonryShop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::refinery: {
            const auto b1 = e::make_shared<eRefinery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::blackMarbleWorkshop: {
            const auto b1 = e::make_shared<eBlackMarbleWorkshop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::oliveTree: {
            const auto b1 = e::make_shared<eResourceBuilding>(
                                *mBoard, eResourceBuildingType::oliveTree, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;
        case eBuildingMode::vine: {
            const auto b1 = e::make_shared<eResourceBuilding>(
                                *mBoard, eResourceBuildingType::vine, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;
        case eBuildingMode::orangeTree: {
            const auto b1 = e::make_shared<eResourceBuilding>(
                                *mBoard, eResourceBuildingType::orangeTree, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;

        case eBuildingMode::huntingLodge: {
            const auto b1 = e::make_shared<eHuntingLodge>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::corral: {
            const auto b1 = e::make_shared<eCorral>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::urchinQuay: {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eUrchinQuay>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(tx, ty, b1);
        } break;

        case eBuildingMode::fishery: {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eFishery>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::triremeWharf: {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildTriremeWharf(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eTriremeWharf>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::pier: {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<ePier>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            const int ctid = mGm->tradeCityId();
            const auto& cts = wrld.cities();
            const auto ct = cts[ctid];
            const auto b2 = e::make_shared<eTradePost>(
                                *mBoard, *ct, mViewedCityId, eTradePostType::pier);
            int tx = mHoverTX;
            int ty = mHoverTY;
            switch(o) {
            case eDiagonalOrientation::topRight: {
                ty += 3;
            } break;
            case eDiagonalOrientation::bottomRight: {
                tx -= 3;
            } break;
            case eDiagonalOrientation::bottomLeft: {
                ty -= 3;
            } break;
            default:
            case eDiagonalOrientation::topLeft: {
                tx += 3;
            } break;
            }
            bool insert = false;
            if(dir == eWorldDirection::N) {
                insert = o == eDiagonalOrientation::bottomRight ||
                         o == eDiagonalOrientation::bottomLeft;
            } else if(dir == eWorldDirection::E) {
                insert = o == eDiagonalOrientation::topLeft ||
                         o == eDiagonalOrientation::bottomLeft;
            } else if(dir == eWorldDirection::S) {
                insert = o == eDiagonalOrientation::topRight ||
                         o == eDiagonalOrientation::topLeft;
            } else { // if(dir == eWorldDirection::W) {
                insert = o == eDiagonalOrientation::topRight ||
                         o == eDiagonalOrientation::bottomRight;
            }
            if(insert) {
                ebs.insert(ebs.begin(), {tx, ty, b2});
            } else {
                ebs.emplace_back(tx, ty, b2);
            }
        } break;

        case eBuildingMode::wheatFarm: {
            const auto b1 = e::make_shared<eWheatFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;
        case eBuildingMode::onionFarm: {
            const auto b1 = e::make_shared<eOnionFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;
        case eBuildingMode::carrotFarm: {
            const auto b1 = e::make_shared<eCarrotFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        } break;

        case eBuildingMode::growersLodge: {
            const auto b1 = e::make_shared<eGrowersLodge>(
                                *mBoard, eGrowerType::grapesAndOlives, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::orangeTendersLodge: {
            const auto b1 = e::make_shared<eGrowersLodge>(
                                *mBoard, eGrowerType::oranges, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::granary: {
            const auto b1 = e::make_shared<eGranary>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::warehouse: {
            const auto b1 = e::make_shared<eWarehouse>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::tradePost: {
            const int ctid = mGm->tradeCityId();
            const auto& cts = wrld.cities();
            const auto ct = cts[ctid];
            const auto b1 = e::make_shared<eTradePost>(*mBoard, *ct, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;


        case eBuildingMode::wall: {
            const auto b1 = e::make_shared<eWall>(*mBoard, mViewedCityId);
            b1->setDeleteArchers(false);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::tower: {
            const auto b1 = e::make_shared<eTower>(*mBoard, mViewedCityId);
            b1->setDeleteArchers(false);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::gatehouse: {
            int ddx = 0;
            int ddy = 0;
            int dx;
            int dy;
            if(dir == eWorldDirection::N) {
                if(mRotate) {
                    dx = 0;
                    dy = 3;
                } else {
                    dx = 3;
                    dy = 0;
                }
            } else if(dir == eWorldDirection::E) {
                if(mRotate) {
                    dx = 0;
                    dy = 3;
                } else {
                    dx = 3;
                    dy = 0;
                }
            } else if(dir == eWorldDirection::S) {
                if(mRotate) {
                    dx = 0;
                    dy = -3;
                    ddy = 3;
                } else {
                    dx = -3;
                    dy = 0;
                    ddx = 3;
                }
            } else { // if(dir == eWorldDirection::W) {
                if(mRotate) {
                    dx = 0;
                    dy = 3;
                } else {
                    dx = 3;
                    dy = 0;
                }
            }
            const bool switched = (mRotate && dir == eWorldDirection::W) ||
                                  (!mRotate && dir == eWorldDirection::E);
            int x1 = mHoverTX + ddx;
            int y1 = mHoverTY + ddy;
            int x2 = mHoverTX + ddx + dx;
            int y2 = mHoverTY + ddy + dy;
            if(switched) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            const auto b1 = e::make_shared<eGatehouse>(*mBoard, mRotate, mViewedCityId);
            auto& ebs1 = ebs.emplace_back(x1, y1, b1);
            ebs1.fBR = e::make_shared<eGatehouseRenderer>(
                           mRotate, eGatehouseRendererType::grt1, b1);
            auto& ebs2 = ebs.emplace_back(x2, y2, b1);
            ebs2.fBR = e::make_shared<eGatehouseRenderer>(
                           mRotate, eGatehouseRendererType::grt2, b1);
        } break;

        case eBuildingMode::armory: {
            const auto b1 = e::make_shared<eArmory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::horseRanch: {
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const auto b1 = e::make_shared<eHorseRanch>(*mBoard, mViewedCityId);
            ebs.emplace_back(tx, ty, b1);

            int dx = 0;
            int dy = 0;
            bool under = false;
            if(mRotateId == 0) { // bottomRight
                dx = 3;
            } else if(mRotateId == 1) { // topRight
                dy = -3;
                dx = -1;
            } else if(mRotateId == 2) { // topLeft
                dx = -4;
                dy = 1;
                under = true;
            } else if(mRotateId == 3) { // bottomLeft
                dy = 4;
            }
            if(dir == eWorldDirection::N) {
                under = mRotateId == 1 || mRotateId == 2; // topRight, topLeft
            } else if(dir == eWorldDirection::E) {
                under = mRotateId == 1 || mRotateId == 0; // topLeft, bottomLeft
            } else if(dir == eWorldDirection::S) {
                under = mRotateId == 0 || mRotateId == 3; // bottomRight, bottomLeft
            } else if(dir == eWorldDirection::W) {
                under = mRotateId == 2 || mRotateId == 3; // topRight, bottomRight
            }
            const auto b2 = e::make_shared<eHorseRanchEnclosure>(*mBoard, mViewedCityId);
            b2->setRanch(b1.get());
            b1->setEnclosure(b2.get());
            if(under) {
                ebs.insert(ebs.begin(), eB{tx + dx, ty + dy, b2});
            } else {
                ebs.emplace_back(tx + dx, ty + dy, b2);
            }
        } break;
        case eBuildingMode::chariotFactory: {
            const auto b1 = e::make_shared<eChariotFactory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::olivePress: {
            const auto b1 = e::make_shared<eOlivePress>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::winery: {
            const auto b1 = e::make_shared<eWinery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::sculptureStudio: {
            const auto b1 = e::make_shared<eSculptureStudio>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::artisansGuild: {
            const auto b1 = e::make_shared<eArtisansGuild>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::dairy: {
            const auto b1 = e::make_shared<eDairy>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::cardingShed: {
            const auto b1 = e::make_shared<eCardingShed>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::foodVendor: {
            const auto b1 = e::make_shared<eFoodVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::fleeceVendor: {
            const auto b1 = e::make_shared<eFleeceVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::oilVendor: {
            const auto b1 = e::make_shared<eOilVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::wineVendor: {
            const auto b1 = e::make_shared<eWineVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::armsVendor: {
            const auto b1 = e::make_shared<eArmsVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::horseTrainer: {
            const auto b1 = e::make_shared<eHorseVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::chariotVendor: {
            const auto b1 = e::make_shared<eChariotVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::park: {
            const auto b1 = e::make_shared<ePark>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::doricColumn: {
            const auto b1 = e::make_shared<eDoricColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::ionicColumn: {
            const auto b1 = e::make_shared<eIonicColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::corinthianColumn: {
            const auto b1 = e::make_shared<eCorinthianColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::avenue: {
            const auto b1 = e::make_shared<eAvenue>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::populationMonument: {
            const auto b1 = e::make_shared<eCommemorative>(0, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::victoryMonument: {
            const auto b1 = e::make_shared<eCommemorative>(1, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::colonyMonument: {
            const auto b1 = e::make_shared<eCommemorative>(2, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::athleteMonument: {
            const auto b1 = e::make_shared<eCommemorative>(3, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::conquestMonument: {
            const auto b1 = e::make_shared<eCommemorative>(4, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::happinessMonument: {
            const auto b1 = e::make_shared<eCommemorative>(5, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::heroicFigureMonument: {
            const auto b1 = e::make_shared<eCommemorative>(6, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::diplomacyMonument: {
            const auto b1 = e::make_shared<eCommemorative>(7, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::scholarMonument: {
            const auto b1 = e::make_shared<eCommemorative>(8, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

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
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const int tminX = tx - 1;
            const int tminY = ty - 2;
            const int tmaxX = tminX + 4;
            const int tmaxY = tminY + 4;
            const auto am = eBuildingMode::aphroditeMonument;
            const int id = static_cast<int>(mode) -
                           static_cast<int>(am);
            const auto gt = static_cast<eGodType>(id);
            const auto b1 = e::make_shared<eGodMonument>(
                                gt, eGodQuestId::godQuest1, *mBoard, mViewedCityId);

            for(int x = tminX; x < tmaxX; x++) {
                for(int y = tminY; y < tmaxY; y++) {
                    const auto b0 = e::make_shared<eGodMonumentTile>(
                                        *mBoard, mViewedCityId);
                    b0->setMonument(b1.get());
                    ebs.emplace_back(x, y, b0);
                }
            }

            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::bench: {
            const auto b1 = e::make_shared<eBench>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::flowerGarden: {
            const auto b1 = e::make_shared<eFlowerGarden>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::gazebo: {
            const auto b1 = e::make_shared<eGazebo>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::hedgeMaze: {
            const auto b1 = e::make_shared<eHedgeMaze>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::fishPond: {
            const auto b1 = e::make_shared<eFishPond>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::waterPark: {
            const auto b1 = e::make_shared<eWaterPark>(*mBoard, mViewedCityId);
            b1->setId(rotationId());
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;

        case eBuildingMode::hippodromePiece: {
            updateHippodromeIds();
            const int hid = hippodromeId();
            const auto b1 = e::make_shared<eHippodromePiece>(*mBoard, mViewedCityId);
            b1->setId(hid == -1 ? 0 : hid);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            if(hid == -1) {
                canBuildFunc = [](int, int, int, int) { return false; };
            }
        } break;

        case eBuildingMode::birdBath: {
            const auto b1 = e::make_shared<eBirdBath>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::shortObelisk: {
            const auto b1 = e::make_shared<eShortObelisk>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::tallObelisk: {
            const auto b1 = e::make_shared<eTallObelisk>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::shellGarden: {
            const auto b1 = e::make_shared<eShellGarden>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::sundial: {
            const auto b1 = e::make_shared<eSundial>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::orrery: {
            const auto b1 = e::make_shared<eOrrery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::dolphinSculpture: {
            const auto b1 = e::make_shared<eDolphinSculpture>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::spring: {
            const auto b1 = e::make_shared<eSpring>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::topiary: {
            const auto b1 = e::make_shared<eTopiary>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::baths: {
            const auto b1 = e::make_shared<eBaths>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        case eBuildingMode::stoneCircle: {
            const auto b1 = e::make_shared<eStoneCircle>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        } break;
        default: break;
        }
        bool cbg = true;
        const int a = t->altitude();
        for(auto& eb : ebs) {
            if(!eb.fBR) eb.fBR = e::make_shared<eBuildingRenderer>(eb.fB);
            const auto b = eb.fBR;
            const int sw = b->spanW();
            const int sh = b->spanH();
            const bool cb = canBuildFunc(eb.fTx, eb.fTy, sw, sh);
            if(!cb) cbg = false;
        }
        for(auto& eb : ebs) {
            if(!eb.fB) continue;
            const auto b = eb.fB;
            b->setFrameShift(0);
            b->setSeed(0);
            b->addUnderBuilding(t);
            b->setCenterTile(t);
            double rx;
            double ry;
            const int sw = eb.fBR->spanW();
            const int sh = eb.fBR->spanH();
            drawXY(eb.fTx, eb.fTy, rx, ry, sw, sh, a);
            if(dir == eWorldDirection::E) {
                if((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) {
                    rx -= 1;
                } else if(sw == 6 && sh == 6) {
                    ry -= 1;
                }
            } else if(dir == eWorldDirection::S) {
                if((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) {
                    rx -= 1;
                    ry += 1;
                } else if(sw == 6 && sh == 6) {
                    rx -= 1;
                    ry -= 1;
                }
            } else if(dir == eWorldDirection::W) {
                if((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) {
                    ry += 1;
                } else if(sw == 6 && sh == 6) {
                    rx -= 1;
                }
            }
            const auto tex = eb.fBR->getTexture(tp.size());

            if(tex) {
                if(cbg) tex->setColorMod(0, 255, 0);
                else tex->setColorMod(255, 0, 0);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
                tex->clearColorMod();
            }

            const auto overlays = eb.fBR->getOverlays(tp.size());
            for(const auto& o : overlays) {
                const auto& ttex = o.fTex;
                if(cbg) ttex->setColorMod(0, 255, 0);
                else ttex->setColorMod(255, 0, 0);
                if(o.fAlignTop) tp.drawTexture(rx + o.fX, ry + o.fY, ttex,
                                               eAlignment::top);
                else tp.drawTexture(rx + o.fX, ry + o.fY, ttex);
                ttex->clearColorMod();
            }
        }
    }
}
