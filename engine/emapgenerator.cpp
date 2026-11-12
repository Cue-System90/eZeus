#define _USE_MATH_DEFINES
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

// the world position of a tile: the rows are half a tile high and every
// other row is shifted by half a tile
void eMapGenerator::worldPos(eTileBase* const t, double& x, double& y) const {
    x = t->dx() + 0.5*(t->dy() % 2);
    y = 0.5*t->dy();
}

eTile* eMapGenerator::tileAt(const double x, const double y) const {
    const int dy = std::lround(2*y);
    if(dy < 0 || dy >= mH) return nullptr;
    const int dx = std::lround(x - 0.5*(dy % 2));
    if(dx < 0 || dx >= mW) return nullptr;
    return mBoard.dtile(dx, dy);
}

// walks from the source towards the mouth, always roughly downhill on
// the distance field, turning with the noise so the course meanders
// like the rivers of the original maps; the walked positions are
// returned, empty if the walk did not reach the mouth
std::vector<std::pair<double, double>> eMapGenerator::walkRiver(
        eTile* const source, const std::vector<int>& dist,
        const eTileVerifier& mouth, const int seed) const {
    using ePos = std::pair<double, double>;
    std::vector<ePos> path;
    double x;
    double y;
    worldPos(source, x, y);
    const auto downhill = [&](eTile* const t, double& ang) {
        eTile* best = nullptr;
        int bestD = dist[t->dx()*mH + t->dy()];
        for(const auto& nn : t->neighbours(nullptr)) {
            const auto nt = static_cast<eTile*>(nn.second);
            const int d = dist[nt->dx()*mH + nt->dy()];
            if(d < bestD) {
                bestD = d;
                best = nt;
            }
        }
        if(!best) return false;
        double bx;
        double by;
        worldPos(best, bx, by);
        double tx;
        double ty;
        worldPos(t, tx, ty);
        ang = std::atan2(by - ty, bx - tx);
        return true;
    };
    const auto wrap = [](double a) {
        while(a > M_PI) a -= 2*M_PI;
        while(a < -M_PI) a += 2*M_PI;
        return a;
    };
    double heading;
    if(!downhill(source, heading)) return path;
    const ePerlinNoise turn(seed);
    const int bw = std::max(4, mW/10);
    const int bh = std::max(8, mH/10);
    const double cx = 0.5*mW;
    const double cy = 0.25*mH;
    const int maxSteps = 6*(mW + mH);
    const double step = 0.7;
    for(int i = 0; i < maxSteps; i++) {
        const auto t = tileAt(x, y);
        if(!t) break;
        path.push_back({x, y});
        const int d = dist[t->dx()*mH + t->dy()];
        if(i > 8 && (d == 0 || mouth(t) ||
                     mSea[t->dx()*mH + t->dy()])) return path;
        double want;
        if(!downhill(t, want)) want = heading;
        const double diff = wrap(want - heading);
        // the pull towards the mouth: light on the way, firm when close
        // or when the course has turned too far away from it
        double pull = d < 10 ? 0.5 : 0.06;
        if(std::abs(diff) > 0.5*M_PI) pull = 0.25;
        heading += pull*diff;
        // the meander
        heading += 0.3*turn.noise(i/16., 0.5);
        // away from the edge of the map
        const int dx = t->dx();
        const int dy = t->dy();
        const bool band = dx < bw || dy < bh || dx >= mW - bw || dy >= mH - bh;
        if(band && i > 6 && d > 10) {
            const double toCenter = std::atan2(cy - y, cx - x);
            heading += 0.15*wrap(toCenter - heading);
        }
        x += step*std::cos(heading);
        y += step*std::sin(heading);
    }
    path.clear();
    return path;
}

// water in a band along the walked positions, the width wavering a little
void eMapGenerator::carveRiver(const std::vector<std::pair<double, double>>& path,
                               const double halfWidth, const int seed) {
    const ePerlinNoise wave(seed);
    for(int i = 0; i < static_cast<int>(path.size()); i++) {
        const double x = path[i].first;
        const double y = path[i].second;
        const double r = halfWidth*(1 + 0.35*wave.noise(i/12., 0.5));
        const int dyMin = std::max(0, static_cast<int>(std::floor(2*(y - r))));
        const int dyMax = std::min(mH - 1, static_cast<int>(std::ceil(2*(y + r))));
        for(int dy = dyMin; dy <= dyMax; dy++) {
            const double ty = 0.5*dy;
            const int dxMin = std::max(0, static_cast<int>(std::floor(x - r - 1)));
            const int dxMax = std::min(mW - 1, static_cast<int>(std::ceil(x + r + 1)));
            for(int dx = dxMin; dx <= dxMax; dx++) {
                const double tx = dx + 0.5*(dy % 2);
                const double ddx = tx - x;
                const double ddy = ty - y;
                if(ddx*ddx + ddy*ddy > r*r) continue;
                const auto t = mBoard.dtile(dx, dy);
                if(!t) continue;
                t->setTerrain(eTerrain::water);
                mRiver[dx*mH + dy] = true;
            }
        }
    }
}

// rivers like those of the original maps: wide, winding, from a side of
// the map to the sea or to the far side, some with a tributary
void eMapGenerator::generateRivers(const eMGS& s,
                                   const std::vector<double>& height) {
    mRiver.assign(mW*mH, false);
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
    const auto seaDist = distanceTo([this](eTileBase* const t) {
        return static_cast<bool>(mSea[t->dx()*mH + t->dy()]);
    }, mW + mH);
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
        const bool farIsLand = std::find(landSides.begin(), landSides.end(),
                                         farSide) != landSides.end();
        // to the sea, or now and then right across the map
        const bool acrossMap = !toSea || (farIsLand && rand(3) == 0);
        const auto mouth = [&, acrossMap, farSide](eTileBase* const t) {
            if(!acrossMap) return static_cast<bool>(mSea[t->dx()*mH + t->dy()]);
            return onSide(t, farSide);
        };
        const auto dist = distanceTo(mouth, mW + mH);
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
        const auto source = edge[rand(std::max<int>(1, edge.size()/3))];
        const auto path = walkRiver(source, dist, mouth, mSeed + 100 + 7*i);
        if(path.empty()) continue;
        // the main river is five to nine tiles wide
        const double halfWidth = 2.5 + 0.5*rand(5);
        carveRiver(path, halfWidth, mSeed + 200 + 7*i);
        const auto endTile = tileAt(path.back().first, path.back().second);
        source->addBanner(std::make_shared<eRiverEntryPoint>(0, source, mBoard));
        if(endTile) {
            endTile->addBanner(std::make_shared<eRiverExitPoint>(0, endTile, mBoard));
        }
        // a tributary from the higher land some way off joins it
        if(rand(5) < 3) {
            const auto riverDist = distanceTo([this](eTileBase* const t) {
                return static_cast<bool>(mRiver[t->dx()*mH + t->dy()]);
            }, 80);
            std::vector<eTile*> springs;
            for(const auto t : mAll) {
                if(!isLand(t) || onEdge(t)) continue;
                const int rd = riverDist[t->dx()*mH + t->dy()];
                if(rd < 30 || rd > 70) continue;
                if(seaDist[t->dx()*mH + t->dy()] < 15) continue;
                springs.push_back(t);
            }
            if(springs.empty()) continue;
            std::sort(springs.begin(), springs.end(),
                      [&](eTile* const a, eTile* const b) {
                return value(height, a) > value(height, b);
            });
            const auto spring = springs[rand(std::max<int>(1, springs.size()/3))];
            const auto toRiver = [this](eTileBase* const t) {
                return static_cast<bool>(mRiver[t->dx()*mH + t->dy()]);
            };
            const auto tpath = walkRiver(spring, riverDist, toRiver,
                                         mSeed + 300 + 7*i);
            if(tpath.empty()) continue;
            carveRiver(tpath, 1.2 + 0.5*rand(3), mSeed + 400 + 7*i);
        }
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
    auto field = noiseField(noise, 30., 3);
    // in large patches, the more by the rivers and lakes, not along the
    // sea shore
    const auto waterDist = distanceTo([this](eTileBase* const t) {
        return isWater(t) && !mSea[t->dx()*mH + t->dy()];
    }, 12);
    for(const auto t : mAll) {
        const int d = waterDist[t->dx()*mH + t->dy()];
        if(d <= 12) value(field, t) += 0.25 + 0.25/(1 + d);
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
    // the medium sized ones, 8 to 40 tiles, away from the edge of the map
    std::vector<std::vector<eTile*>> fit;
    for(auto& p : ps) {
        if(p.size() < 8 || p.size() > 40) continue;
        bool edge = false;
        for(const auto t : p) {
            if(onEdge(t)) edge = true;
        }
        if(!edge) fit.push_back(p);
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
