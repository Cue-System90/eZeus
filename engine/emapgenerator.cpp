#include "emapgenerator.h"

#include "eperlinnoise.h"
#include "egameboard.h"
#include "etile.h"
#include "erand.h"

#include "spawners/eboarspawner.h"
#include "spawners/edeerspawner.h"
#include "spawners/ewolfspawner.h"
#include "spawners/eentrypoint.h"
#include "spawners/eexitpoint.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <random>

eMapGeneratorSettings eMapGeneratorSettings::sLast;

eMapGenerator::eMapGenerator(eGameBoard& board) :
    mBoard(board) {
}

int eMapGenerator::rand(const int n) const {
    if(n <= 0) return 0;
    return eRand::rand() % n;
}

int eMapGenerator::count(const int perUnit) const {
    return std::round(double(perUnit)*mTiles/eMGS::sAreaUnit);
}

double eMapGenerator::value(const std::vector<double>& field,
                            eTile* const t) const {
    return field[t->dx()*mH + t->dy()];
}

double& eMapGenerator::value(std::vector<double>& field,
                             eTile* const t) const {
    return field[t->dx()*mH + t->dy()];
}

// the dtiles sit in staggered rows, every row is half a tile high and
// every other row is shifted by half a tile, so the noise is sampled at
// the real position to come out the same in every direction
std::vector<double> eMapGenerator::noiseField(const ePerlinNoise& n,
                                              const double featureSize,
                                              const int octaves) const {
    std::vector<double> field(mW*mH, 0.);
    for(const auto t : mAll) {
        const int dx = t->dx();
        const int dy = t->dy();
        const double x = dx + 0.5*(dy % 2);
        const double y = 0.5*dy;
        value(field, t) = n.fractal(x, y, featureSize, octaves);
    }
    return field;
}

double eMapGenerator::quantile(const std::vector<double>& field,
                               const double fraction,
                               const eTileVerifier& v) const {
    std::vector<double> vals;
    vals.reserve(mAll.size());
    for(const auto t : mAll) {
        if(v && !v(t)) continue;
        vals.push_back(value(field, t));
    }
    if(vals.empty()) return 1e9;
    const int id = std::clamp<int>(std::round((1. - fraction)*vals.size()),
                                   0, vals.size() - 1);
    std::nth_element(vals.begin(), vals.begin() + id, vals.end());
    return vals[id];
}

// steps from the tiles that pass the verifier to all others, up to max
std::vector<int> eMapGenerator::distanceTo(const eTileVerifier& v,
                                           const int max) const {
    std::vector<int> dist(mW*mH, max + 1);
    std::queue<eTile*> q;
    for(const auto t : mAll) {
        if(!v(t)) continue;
        dist[t->dx()*mH + t->dy()] = 0;
        q.push(t);
    }
    while(!q.empty()) {
        const auto t = q.front();
        q.pop();
        const int d = dist[t->dx()*mH + t->dy()];
        if(d >= max) continue;
        for(const auto& n : t->neighbours(nullptr)) {
            const auto nt = static_cast<eTile*>(n.second);
            int& nd = dist[nt->dx()*mH + nt->dy()];
            if(nd <= d + 1) continue;
            nd = d + 1;
            q.push(nt);
        }
    }
    return dist;
}

std::vector<std::vector<eTile*>> eMapGenerator::patches(
        const eTileVerifier& v) const {
    std::vector<std::vector<eTile*>> result;
    std::vector<bool> seen(mW*mH, false);
    for(const auto t : mAll) {
        if(seen[t->dx()*mH + t->dy()] || !v(t)) continue;
        auto& patch = result.emplace_back();
        std::vector<eTile*> stack{t};
        seen[t->dx()*mH + t->dy()] = true;
        while(!stack.empty()) {
            const auto c = stack.back();
            stack.pop_back();
            patch.push_back(c);
            for(const auto& n : c->neighbours(v)) {
                const auto nt = static_cast<eTile*>(n.second);
                const int id = nt->dx()*mH + nt->dy();
                if(seen[id]) continue;
                seen[id] = true;
                stack.push_back(nt);
            }
        }
    }
    return result;
}

bool eMapGenerator::onEdge(eTileBase* const t) const {
    const int dx = t->dx();
    const int dy = t->dy();
    return dx == 0 || dy == 0 || dx == mW - 1 || dy == mH - 1;
}

namespace {

bool isWater(eTileBase* const t) {
    return t && t->terrain() == eTerrain::water;
}

bool isLand(eTileBase* const t) {
    return t && t->terrain() != eTerrain::water;
}

bool isDry(eTileBase* const t) {
    return t && t->terrain() == eTerrain::dry;
}

bool isFlatDry(eTileBase* const t) {
    return isDry(t) && !t->isElevationTile();
}

bool isStones(eTileBase* const t) {
    return t && t->terrain() == eTerrain::flatStones;
}

} // namespace

void eMapGenerator::generate(const eMGS& settings) {
    eMGS::sLast = settings;
    eMGS::sLast.fSeed = 0;

    mSeed = settings.fSeed > 0 ? settings.fSeed : eRand::rand();
    mBoard.initialize(mBoard.width(), mBoard.height());
    mW = mBoard.width();
    mH = mBoard.height();
    mAll.clear();
    mAll.reserve(mW*mH);
    for(int x = 0; x < mW; x++) {
        for(int y = 0; y < mH; y++) {
            const auto t = mBoard.dtile(x, y);
            if(!t) continue;
            mAll.push_back(t);
            t->setTerrain(eTerrain::dry);
            t->setScrub(0.);
            t->setAltitude(0, false);
            t->setHasFish(false);
            t->setHasUrchin(false);
        }
    }
    mTiles = mAll.size();
    // with the update, so the slope flags of the tiles are cleared
    for(const auto t : mAll) t->setAltitude(0);

    const ePerlinNoise heightNoise(mSeed);
    const auto height = noiseField(heightNoise, 60., 5);

    generateSea(settings, height);
    generateLakes(settings, height);
    generateRivers(settings, height);
    generateBeach(settings);
    generateElevation(settings, height);
    generateForest(settings);
    generateFertile(settings);
    generateStones(settings, height);
    generateOre(settings);
    generateScrub(settings);
    generateAnimals(settings);
    generateFish(settings);
    generateEntryExit(settings);

    const auto cid = settings.fCid;
    if(cid != eCityId::neutralFriendly &&
       cid != eCityId::neutralAggresive &&
       static_cast<int>(cid) >= 0) {
        mBoard.assignAllTerritory(cid);
    }
    mBoard.updateTerritoryBorders();
    mBoard.updateMarbleTiles();
    mBoard.scheduleTerrainUpdate();
}

// the sea lies along one or two sides of the map, the land falls off
// towards them with a noisy coast line
void eMapGenerator::generateSea(const eMGS& s,
                                const std::vector<double>& height) {
    mSeaSides.clear();
    mSea.assign(mW*mH, false);
    if(s.fWater <= 0) return;
    const int first = rand(4);
    mSeaSides.push_back(first);
    if(rand(2)) {
        // a neighbouring side, so the sea fills a corner
        mSeaSides.push_back((first + (rand(2) ? 1 : 3)) % 4);
    }
    std::vector<double> field(mW*mH, 0.);
    for(const auto t : mAll) {
        // closeness to the nearest sea side, 1 on it, 0 on the far side
        const double fx = double(t->dx())/(mW - 1);
        const double fy = double(t->dy())/(mH - 1);
        double close = 0;
        for(const int side : mSeaSides) {
            double c = 0;
            switch(side) {
            case 0: c = 1 - fy; break;
            case 1: c = fx; break;
            case 2: c = fy; break;
            default: c = 1 - fx; break;
            }
            close = std::max(close, c);
        }
        // the noise shapes the coast, the gradient keeps the sea at the edge
        value(field, t) = 1.2*close + 1.1*value(height, t);
    }
    const double thr = quantile(field, 0.01*s.fWater, nullptr);
    for(const auto t : mAll) {
        if(value(field, t) >= thr) t->setTerrain(eTerrain::water);
    }
    // only the water that reaches the sea sides is the sea, the rest
    // becomes land again; lakes come separately
    const auto ps = patches(isWater);
    for(const auto& p : ps) {
        bool sea = false;
        for(const auto t : p) {
            const int dx = t->dx();
            const int dy = t->dy();
            for(const int side : mSeaSides) {
                if(side == 0 && dy == 0) sea = true;
                if(side == 1 && dx == mW - 1) sea = true;
                if(side == 2 && dy == mH - 1) sea = true;
                if(side == 3 && dx == 0) sea = true;
            }
            if(sea) break;
        }
        if(sea) continue;
        for(const auto t : p) t->setTerrain(eTerrain::dry);
    }
    // islands of a few tiles sink
    const auto land = patches(isLand);
    for(const auto& p : land) {
        if(p.size() >= 20) continue;
        for(const auto t : p) t->setTerrain(eTerrain::water);
    }
    mSea.assign(mW*mH, false);
    for(const auto t : mAll) {
        if(isWater(t)) mSea[t->dx()*mH + t->dy()] = true;
    }
}

// a lake is a low spot inland, filled up to a noisy shore
void eMapGenerator::generateLakes(const eMGS& s,
                                  const std::vector<double>& height) {
    const int n = count(s.fLakes);
    if(n <= 0) return;
    const auto seaDist = distanceTo(isWater, 12);
    std::vector<eTile*> candidates;
    for(const auto t : mAll) {
        if(!isLand(t) || onEdge(t)) continue;
        if(seaDist[t->dx()*mH + t->dy()] <= 12) continue;
        candidates.push_back(t);
    }
    if(candidates.empty()) return;
    // the lowest spots first
    std::sort(candidates.begin(), candidates.end(),
              [&](eTile* const a, eTile* const b) {
        return value(height, a) < value(height, b);
    });
    const ePerlinNoise shore(mSeed + 1);
    int made = 0;
    std::vector<eTile*> centers;
    for(const auto c : candidates) {
        if(made >= n) break;
        bool near = false;
        for(const auto o : centers) {
            const int ddx = std::abs(o->dx() - c->dx());
            const int ddy = std::abs(o->dy() - c->dy());
            if(ddx < 20 && ddy < 40) near = true;
        }
        if(near) continue;
        centers.push_back(c);
        made++;
        const double radius = 4 + rand(5);
        for(const auto t : mAll) {
            const double ddx = t->dx() - c->dx() + 0.5*((t->dy() - c->dy()) % 2);
            const double ddy = 0.5*(t->dy() - c->dy());
            const double d = std::sqrt(ddx*ddx + ddy*ddy);
            if(d > 2*radius) continue;
            const double r = radius*(1 + 0.5*shore.noise(0.2*t->dx(), 0.1*t->dy()));
            if(d <= r && !onEdge(t)) t->setTerrain(eTerrain::water);
        }
    }
}

// a river starts at a side of the map without sea and winds down to the
// sea, or to the far side if there is no sea
void eMapGenerator::generateRivers(const eMGS& s,
                                   const std::vector<double>& height) {
    (void)height;
    int n = count(s.fRivers);
    if(s.fRivers > 0 && n < 1) n = 1;
    if(n <= 0) return;
    std::vector<int> landSides;
    for(int side = 0; side < 4; side++) {
        if(std::find(mSeaSides.begin(), mSeaSides.end(), side) !=
           mSeaSides.end()) continue;
        landSides.push_back(side);
    }
    if(landSides.empty()) return;
    const bool toSea = !mSeaSides.empty();
    const auto onSide = [&](eTileBase* const t, const int side) {
        const int dx = t->dx();
        const int dy = t->dy();
        if(side == 0) return dy == 0;
        if(side == 1) return dx == mW - 1;
        if(side == 2) return dy == mH - 1;
        return dx == 0;
    };
    const ePerlinNoise meander(mSeed + 2);
    const auto meanderField = noiseField(meander, 25., 3);
    const int bw = std::max(4, mW/10);
    const int bh = std::max(8, mH/10);
    const auto inBand = [&](eTileBase* const t) {
        const int dx = t->dx();
        const int dy = t->dy();
        return dx < bw || dy < bh || dx >= mW - bw || dy >= mH - bh;
    };
    for(int i = 0; i < n; i++) {
        // mostly from the side across from the sea, so the river crosses
        // the land instead of running along the shore
        int side = landSides[rand(landSides.size())];
        if(toSea && rand(4) != 0) {
            const int across = (mSeaSides[0] + 2) % 4;
            if(std::find(landSides.begin(), landSides.end(), across) !=
               landSides.end()) side = across;
        }
        const int farSide = (side + 2) % 4;
        // steps to the mouth: the sea, or the far side of the map
        const auto target = [&](eTileBase* const t) {
            if(toSea) return static_cast<bool>(mSea[t->dx()*mH + t->dy()]);
            return onSide(t, farSide);
        };
        const auto dist = distanceTo(target, mW + mH);
        // the noise bends the way, the band along the edge pushes it in
        std::vector<double> field(mW*mH, 0.);
        for(const auto t : mAll) {
            double v = dist[t->dx()*mH + t->dy()] +
                       30.*value(meanderField, t);
            if(inBand(t) && !target(t)) {
                // the deeper in the band, the dearer
                const int dx = t->dx();
                const int dy = t->dy();
                const int depth = std::max({bw - dx, bh - dy,
                                            dx - (mW - 1 - bw),
                                            dy - (mH - 1 - bh)});
                v += 3.*depth;
            }
            value(field, t) = v;
        }
        // the source on the side, away from the corners and as far from
        // the mouth as it gets
        std::vector<eTile*> edge;
        for(const auto t : mAll) {
            if(!onSide(t, side) || !isLand(t)) continue;
            const int dx = t->dx();
            const int dy = t->dy();
            const bool corner = side % 2 == 0 ? (dx < mW/5 || dx > 4*mW/5) :
                                                (dy < mH/5 || dy > 4*mH/5);
            if(!corner) edge.push_back(t);
        }
        if(edge.empty()) continue;
        std::sort(edge.begin(), edge.end(),
                  [&](eTile* const a, eTile* const b) {
            return dist[a->dx()*mH + a->dy()] > dist[b->dx()*mH + b->dy()];
        });
        const auto source = edge[rand(std::max<int>(1, edge.size()/4))];
        // downhill on the bent distance, never back, until the mouth
        std::vector<bool> visited(mW*mH, false);
        std::vector<eTile*> path{source};
        visited[source->dx()*mH + source->dy()] = true;
        eTile* t = source;
        const int maxSteps = 4*(mW + mH);
        while(!target(t) && static_cast<int>(path.size()) < maxSteps) {
            eTile* best = nullptr;
            double bestV = 1e18;
            for(const auto& nn : t->neighbours(nullptr)) {
                const auto nt = static_cast<eTile*>(nn.second);
                if(visited[nt->dx()*mH + nt->dy()]) continue;
                if(onEdge(nt) && !target(nt)) continue;
                const double v = value(field, nt);
                if(v < bestV) {
                    bestV = v;
                    best = nt;
                }
            }
            if(!best) break;
            t = best;
            visited[t->dx()*mH + t->dy()] = true;
            path.push_back(t);
        }
        if(!target(t)) continue;
        // the river is about three tiles wide
        for(const auto pt : path) {
            pt->setTerrain(eTerrain::water);
            for(const auto& nn : pt->neighbours(nullptr)) {
                const auto nt = static_cast<eTile*>(nn.second);
                if(onEdge(nt)) continue;
                const auto o = nn.first;
                const bool straight = o == eOrientation::right ||
                                      o == eOrientation::left ||
                                      o == eOrientation::top ||
                                      o == eOrientation::bottom;
                if(straight || rand(3) == 0) {
                    nt->setTerrain(eTerrain::water);
                }
            }
        }
        const auto entry = std::make_shared<eRiverEntryPoint>(0, source, mBoard);
        source->addBanner(entry);
        const auto exit = std::make_shared<eRiverExitPoint>(0, t, mBoard);
        t->addBanner(exit);
    }
}

// sand on about a fifth of the sea shore, in stretches
void eMapGenerator::generateBeach(const eMGS& s) {
    (void)s;
    if(mSeaSides.empty()) return;
    const ePerlinNoise noise(mSeed + 3);
    const auto field = noiseField(noise, 20., 2);
    const auto seaDist = distanceTo([this](eTileBase* const t) {
        return mSea[t->dx()*mH + t->dy()];
    }, 2);
    std::vector<eTile*> beach;
    for(const auto t : mAll) {
        if(!isDry(t)) continue;
        const int d = seaDist[t->dx()*mH + t->dy()];
        const double v = value(field, t);
        if(d == 1 && v > 0.15) beach.push_back(t);
        else if(d == 2 && v > 0.4) beach.push_back(t);
    }
    for(const auto t : beach) t->setTerrain(eTerrain::beach);
}

// the highest land rises one or two steps, away from the water
void eMapGenerator::generateElevation(const eMGS& s,
                                      const std::vector<double>& height) {
    if(s.fElevation <= 0) return;
    const auto waterDist = distanceTo(isWater, 3);
    const auto ok = [&](eTileBase* const t) {
        return isDry(t) && waterDist[t->dx()*mH + t->dy()] > 3 &&
               !onEdge(t);
    };
    const double frac = 0.015*s.fElevation*mTiles/
                        std::max(1., double(std::count_if(
                            mAll.begin(), mAll.end(), ok)));
    const double thr1 = quantile(height, std::min(1., frac), ok);
    const double thr2 = quantile(height, std::min(1., frac/4), ok);
    for(const auto t : mAll) {
        if(!ok(t)) continue;
        const double h = value(height, t);
        if(h >= thr2) t->setAltitude(2, false);
        else if(h >= thr1) t->setAltitude(1, false);
    }
    // smooth the steps: a tile that differs from most of its diagonal
    // neighbours takes their average, repeated until nothing changes
    for(int it = 0; it < 6; it++) {
        bool changed = false;
        for(const auto t : mAll) {
            const int alt = t->altitude();
            const auto tl = t->topLeft<eTile>();
            const auto tr = t->topRight<eTile>();
            const auto br = t->bottomRight<eTile>();
            const auto bl = t->bottomLeft<eTile>();
            const int tla = tl ? tl->altitude() : alt;
            const int tra = tr ? tr->altitude() : alt;
            const int bra = br ? br->altitude() : alt;
            const int bla = bl ? bl->altitude() : alt;
            int ndiff = 0;
            if(tla != alt) ndiff++;
            if(tra != alt) ndiff++;
            if(bra != alt) ndiff++;
            if(bla != alt) ndiff++;
            if(ndiff > 2) {
                const int avrg = std::round(0.25*(tla + tra + bra + bla));
                if(avrg != alt) {
                    t->setAltitude(avrg, false);
                    changed = true;
                }
            }
        }
        if(!changed) break;
    }
    // sets the slope flags of the tiles around
    for(const auto t : mAll) t->setAltitude(t->altitude());
    // slopes are plain dry land
    for(const auto t : mAll) {
        if(t->isElevationTile()) t->setTerrain(eTerrain::dry);
    }
}

void eMapGenerator::generateForest(const eMGS& s) {
    if(s.fForest <= 0) return;
    const ePerlinNoise noise(mSeed + 4);
    const auto field = noiseField(noise, 10., 3);
    const double frac = 0.01*s.fForest*mTiles/
                        std::max(1., double(std::count_if(
                            mAll.begin(), mAll.end(), isFlatDry)));
    const double thr = quantile(field, std::min(1., frac), isFlatDry);
    for(const auto t : mAll) {
        if(!isFlatDry(t)) continue;
        if(value(field, t) >= thr) t->setTerrain(eTerrain::forest);
    }
}

// meadows lie low, by the rivers and lakes and along the shore
void eMapGenerator::generateFertile(const eMGS& s) {
    if(s.fFertile <= 0) return;
    const ePerlinNoise noise(mSeed + 5);
    auto field = noiseField(noise, 15., 3);
    // by the rivers and lakes, not along the sea shore
    const auto waterDist = distanceTo([this](eTileBase* const t) {
        return isWater(t) && !mSea[t->dx()*mH + t->dy()];
    }, 8);
    for(const auto t : mAll) {
        const int d = waterDist[t->dx()*mH + t->dy()];
        if(d <= 8) value(field, t) += 0.6 + 0.6/(1 + d);
    }
    const double frac = 0.01*s.fFertile*mTiles/
                        std::max(1., double(std::count_if(
                            mAll.begin(), mAll.end(), isFlatDry)));
    const double thr = quantile(field, std::min(1., frac), isFlatDry);
    for(const auto t : mAll) {
        if(!isFlatDry(t)) continue;
        if(value(field, t) >= thr) t->setTerrain(eTerrain::fertile);
    }
}

// rocks lie on the higher, drier ground in patches
void eMapGenerator::generateStones(const eMGS& s,
                                   const std::vector<double>& height) {
    if(s.fStones <= 0) return;
    const ePerlinNoise noise(mSeed + 6);
    auto field = noiseField(noise, 7., 2);
    const auto waterDist = distanceTo(isWater, 3);
    const auto ok = [&](eTileBase* const t) {
        return isFlatDry(t) && waterDist[t->dx()*mH + t->dy()] > 2;
    };
    for(const auto t : mAll) value(field, t) += 0.5*value(height, t);
    const double frac = 0.01*s.fStones*mTiles/
                        std::max(1., double(std::count_if(
                            mAll.begin(), mAll.end(), ok)));
    const double thr = quantile(field, std::min(1., frac), ok);
    for(const auto t : mAll) {
        if(!ok(t)) continue;
        if(value(field, t) >= thr) t->setTerrain(eTerrain::flatStones);
    }
    // single stones are scrub instead
    const auto ps = patches(isStones);
    for(const auto& p : ps) {
        if(p.size() >= 4) continue;
        for(const auto t : p) t->setTerrain(eTerrain::dry);
    }
}

// whole patches of stones turn into copper, silver, marble or orichalc
void eMapGenerator::generateOre(const eMGS& s) {
    const std::vector<std::pair<int, eTerrain>> ores = {
        {s.fCopper, eTerrain::copper},
        {s.fSilver, eTerrain::silver},
        {s.fMarble, eTerrain::marble},
        {s.fOrichalc, eTerrain::orichalc}};
    auto ps = patches(isStones);
    // the medium sized ones, 8 to 40 tiles
    std::vector<std::vector<eTile*>> fit;
    for(auto& p : ps) {
        if(p.size() >= 8 && p.size() <= 40) fit.push_back(p);
    }
    eRand::randomShuffle(fit);
    int next = 0;
    for(const auto& ore : ores) {
        const int n = count(ore.first);
        for(int i = 0; i < n; i++) {
            if(next >= static_cast<int>(fit.size())) {
                // no patch left, make one on dry land
                std::vector<eTile*> spots;
                for(const auto t : mAll) {
                    if(isFlatDry(t) && !onEdge(t)) spots.push_back(t);
                }
                if(spots.empty()) return;
                const auto c = spots[rand(spots.size())];
                std::vector<eTile*> p;
                for(const auto t : mAll) {
                    const int ddx = std::abs(t->dx() - c->dx());
                    const int ddy = std::abs(t->dy() - c->dy());
                    if(ddx <= 1 + rand(2) && ddy <= 3 && isFlatDry(t)) {
                        p.push_back(t);
                    }
                }
                fit.push_back(p);
            }
            for(const auto t : fit[next]) t->setTerrain(ore.second);
            next++;
        }
    }
}

void eMapGenerator::generateScrub(const eMGS& s) {
    if(s.fScrub <= 0) return;
    const ePerlinNoise noise(mSeed + 7);
    const auto field = noiseField(noise, 10., 2);
    const double thr = quantile(field, std::min(1., 0.01*s.fScrub), isDry);
    for(const auto t : mAll) {
        if(!isDry(t)) continue;
        const double v = value(field, t);
        if(v < thr) continue;
        // denser the further above the threshold
        const double frac = std::clamp((v - thr)/(1. - thr), 0., 1.);
        t->setScrub(0.2 + 0.8*frac);
    }
}

// boar and deer live in the woods, wolves on the dry land
void eMapGenerator::generateAnimals(const eMGS& s) {
    const int n = count(s.fAnimals);
    if(n <= 0) return;
    const auto waterDist = distanceTo(isWater, 4);
    std::vector<eTile*> forest;
    std::vector<eTile*> dry;
    for(const auto t : mAll) {
        if(onEdge(t) || t->isElevationTile()) continue;
        if(waterDist[t->dx()*mH + t->dy()] <= 2) continue;
        const auto terr = t->terrain();
        if(terr == eTerrain::forest) forest.push_back(t);
        else if(terr == eTerrain::dry || terr == eTerrain::fertile) {
            dry.push_back(t);
        }
    }
    std::vector<eTile*> taken;
    const auto farFromTaken = [&](eTile* const t) {
        for(const auto o : taken) {
            if(std::abs(o->dx() - t->dx()) < 15 &&
               std::abs(o->dy() - t->dy()) < 30) return false;
        }
        return true;
    };
    for(int i = 0; i < n; i++) {
        const int kind = rand(3);
        auto& pool = kind == 2 || forest.empty() ? dry : forest;
        if(pool.empty()) return;
        eTile* tile = nullptr;
        for(int tries = 0; tries < 50 && !tile; tries++) {
            const auto t = pool[rand(pool.size())];
            if(farFromTaken(t)) tile = t;
        }
        if(!tile) tile = pool[rand(pool.size())];
        taken.push_back(tile);
        stdsptr<eBanner> b;
        if(kind == 0) b = std::make_shared<eBoarSpawner>(0, tile, mBoard);
        else if(kind == 1) b = std::make_shared<eDeerSpawner>(0, tile, mBoard);
        else b = std::make_shared<eWolfSpawner>(0, tile, mBoard);
        tile->addBanner(b);
    }
}

// fishing grounds lie in the sea a little off the shore
void eMapGenerator::generateFish(const eMGS& s) {
    const int n = count(s.fFish);
    if(n <= 0 || mSeaSides.empty()) return;
    const auto landDist = distanceTo(isLand, 6);
    std::vector<eTile*> spots;
    for(const auto t : mAll) {
        if(!isWater(t) || onEdge(t)) continue;
        const int d = landDist[t->dx()*mH + t->dy()];
        if(d >= 2 && d <= 4) spots.push_back(t);
    }
    std::vector<eTile*> taken;
    for(int i = 0; i < n && !spots.empty(); i++) {
        eTile* tile = nullptr;
        for(int tries = 0; tries < 50 && !tile; tries++) {
            const auto t = spots[rand(spots.size())];
            bool near = false;
            for(const auto o : taken) {
                if(std::abs(o->dx() - t->dx()) < 15 &&
                   std::abs(o->dy() - t->dy()) < 30) near = true;
            }
            if(!near) tile = t;
        }
        if(!tile) tile = spots[rand(spots.size())];
        taken.push_back(tile);
        tile->setHasFish(true);
    }
}

// the entry and the exit lie on the largest piece of land, on two
// different sides of the map that are not the sea
void eMapGenerator::generateEntryExit(const eMGS& s) {
    (void)s;
    const auto land = patches([](eTileBase* const t) {
        return t && static_cast<bool>(t->terrain() & eTerrain::walkable) &&
               !t->isElevationTile();
    });
    if(land.empty()) return;
    const auto largest = std::max_element(
        land.begin(), land.end(),
        [](const std::vector<eTile*>& a, const std::vector<eTile*>& b) {
            return a.size() < b.size();
        });
    std::vector<std::vector<eTile*>> sides(4);
    for(const auto t : *largest) {
        if(!isDry(t) && t->terrain() != eTerrain::fertile) continue;
        const int dx = t->dx();
        const int dy = t->dy();
        // one step in from the edge, so the point itself is reachable
        if(dy == 1 && dx > mW/6 && dx < 5*mW/6) sides[0].push_back(t);
        if(dx == mW - 2 && dy > mH/6 && dy < 5*mH/6) sides[1].push_back(t);
        if(dy == mH - 2 && dx > mW/6 && dx < 5*mW/6) sides[2].push_back(t);
        if(dx == 1 && dy > mH/6 && dy < 5*mH/6) sides[3].push_back(t);
    }
    std::vector<int> usable;
    for(int side = 0; side < 4; side++) {
        if(sides[side].empty()) continue;
        if(std::find(mSeaSides.begin(), mSeaSides.end(), side) !=
           mSeaSides.end()) continue;
        usable.push_back(side);
    }
    if(usable.empty()) {
        for(int side = 0; side < 4; side++) {
            if(!sides[side].empty()) usable.push_back(side);
        }
    }
    if(usable.empty()) return;
    const int entrySide = usable[rand(usable.size())];
    int exitSide = entrySide;
    if(usable.size() > 1) {
        while(exitSide == entrySide) exitSide = usable[rand(usable.size())];
    }
    const auto& es = sides[entrySide];
    const auto& xs = sides[exitSide];
    const auto entryTile = es[rand(es.size())];
    eTile* exitTile = xs[rand(xs.size())];
    if(exitTile == entryTile && xs.size() > 1) {
        exitTile = xs[(std::find(xs.begin(), xs.end(), exitTile) -
                       xs.begin() + 1) % xs.size()];
    }
    entryTile->addBanner(std::make_shared<eEntryPoint>(0, entryTile, mBoard));
    exitTile->addBanner(std::make_shared<eExitPoint>(0, exitTile, mBoard));
}
