#include "emapstats.h"

#include "egameboard.h"
#include "etile.h"
#include "ecampaign.h"
#include "spawners/ebanner.h"
#include "efspath.h"
#include "egamedir.h"

#include <filesystem>
#include <map>
#include <vector>
#include <algorithm>

namespace {

const std::vector<std::pair<eTerrain, const char*>> gTerrains = {
    {eTerrain::dry, "dry"},
    {eTerrain::beach, "beach"},
    {eTerrain::water, "water"},
    {eTerrain::fertile, "fertile"},
    {eTerrain::forest, "forest"},
    {eTerrain::choppedForest, "choppedForest"},
    {eTerrain::flatStones, "flatStones"},
    {eTerrain::copper, "copper"},
    {eTerrain::silver, "silver"},
    {eTerrain::tallStones, "tallStones"},
    {eTerrain::marble, "marble"},
    {eTerrain::quake, "quake"},
    {eTerrain::orichalc, "orichalc"},
    {eTerrain::blackMarble, "blackMarble"},
    {eTerrain::marsh, "marsh"},
    {eTerrain::lava, "lava"}};

const std::vector<std::pair<eBannerTypeS, const char*>> gBanners = {
    {eBannerTypeS::boar, "boar"},
    {eBannerTypeS::deer, "deer"},
    {eBannerTypeS::wolf, "wolf"},
    {eBannerTypeS::entryPoint, "entry"},
    {eBannerTypeS::exitPoint, "exit"},
    {eBannerTypeS::riverEntryPoint, "riverEntry"},
    {eBannerTypeS::riverExitPoint, "riverExit"},
    {eBannerTypeS::landInvasion, "landInvasion"},
    {eBannerTypeS::seaInvasion, "seaInvasion"},
    {eBannerTypeS::disembarkPoint, "disembark"},
    {eBannerTypeS::monsterPoint, "monster"},
    {eBannerTypeS::disasterPoint, "disaster"},
    {eBannerTypeS::landSlidePoint, "landSlide"}};

} // namespace

void eMapStats::sPrintHeader() {
    printf("MAPSTATS\tadventure\tboard\tw\th\ttiles\tterritory");
    for(const auto& t : gTerrains) printf("\t%s", t.second);
    printf("\televated\tmaxAltitude\tfish\turchin\tcoast");
    for(const auto& b : gBanners) printf("\t%s", b.second);
    printf("\twaterBodies\tseaBodies\tseaTiles\tlakes\tlakeTiles\tlargestLake"
           "\tforestPatches\tlargestForest\tstonePatches\tlargestStones\n");
}

void eMapStats::sPrint(const std::string& adventure,
                       const std::string& board,
                       eGameBoard& b) {
    const int w = b.width();
    const int h = b.height();
    std::map<eTerrain, int> terr;
    int tiles = 0;
    int territory = 0;
    int elevated = 0;
    int maxAlt = 0;
    int fish = 0;
    int urchin = 0;
    int coast = 0;
    std::map<eBannerTypeS, int> banners;
    const auto isWater = [](eTileBase* const t) {
        return t && t->terrain() == eTerrain::water;
    };
    for(int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            const auto t = b.dtile(x, y);
            if(!t) continue;
            tiles++;
            if(t->cityId() != eCityId::neutralFriendly &&
               t->cityId() != eCityId::neutralAggresive) territory++;
            const auto tt = t->terrain();
            terr[tt]++;
            if(t->isElevationTile()) elevated++;
            maxAlt = std::max(maxAlt, t->altitude());
            if(t->hasFish()) fish++;
            if(t->hasUrchin()) urchin++;
            if(tt != eTerrain::water && !t->neighbours(isWater).empty()) {
                coast++;
            }
            for(const auto& bn : t->banners()) {
                banners[bn->type()]++;
            }
        }
    }

    // connected patches of a kind of terrain, with the tiles of the
    // largest; a water body that reaches the edge of the map is the sea,
    // the others are lakes (or rivers)
    struct ePatch {
        int fTiles = 0;
        bool fEdge = false;
    };
    const auto patches = [&](const std::function<bool(eTileBase*)>& kind) {
        std::vector<ePatch> result;
        std::vector<std::vector<bool>> seen(w, std::vector<bool>(h, false));
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                if(seen[x][y]) continue;
                const auto t = b.dtile(x, y);
                if(!t || !kind(t)) continue;
                ePatch p;
                std::vector<eTile*> stack{t};
                seen[x][y] = true;
                while(!stack.empty()) {
                    const auto c = stack.back();
                    stack.pop_back();
                    p.fTiles++;
                    const int cx = c->dx();
                    const int cy = c->dy();
                    if(cx == 0 || cy == 0 || cx == w - 1 || cy == h - 1) {
                        p.fEdge = true;
                    }
                    const auto ns = c->neighbours(kind);
                    for(const auto& n : ns) {
                        const auto nt = static_cast<eTile*>(n.second);
                        const int nx = nt->dx();
                        const int ny = nt->dy();
                        if(seen[nx][ny]) continue;
                        seen[nx][ny] = true;
                        stack.push_back(nt);
                    }
                }
                result.push_back(p);
            }
        }
        return result;
    };
    const auto water = patches(isWater);
    int seaBodies = 0;
    int seaTiles = 0;
    int lakes = 0;
    int lakeTiles = 0;
    int largestLake = 0;
    for(const auto& p : water) {
        if(p.fEdge) {
            seaBodies++;
            seaTiles += p.fTiles;
        } else {
            lakes++;
            lakeTiles += p.fTiles;
            largestLake = std::max(largestLake, p.fTiles);
        }
    }
    const auto largest = [](const std::vector<ePatch>& ps) {
        int r = 0;
        for(const auto& p : ps) r = std::max(r, p.fTiles);
        return r;
    };
    const auto forests = patches([](eTileBase* const t) {
        return t && t->terrain() == eTerrain::forest;
    });
    const auto stones = patches([](eTileBase* const t) {
        if(!t) return false;
        const auto tt = t->terrain();
        return static_cast<bool>(tt & eTerrain::stones);
    });

    printf("MAPSTATS\t%s\t%s\t%d\t%d\t%d\t%d", adventure.c_str(),
           board.c_str(), w, h, tiles, territory);
    for(const auto& t : gTerrains) printf("\t%d", terr[t.first]);
    printf("\t%d\t%d\t%d\t%d\t%d", elevated, maxAlt, fish, urchin, coast);
    for(const auto& bn : gBanners) printf("\t%d", banners[bn.first]);
    printf("\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
           static_cast<int>(water.size()), seaBodies, seaTiles,
           lakes, lakeTiles, largestLake,
           static_cast<int>(forests.size()), largest(forests),
           static_cast<int>(stones.size()), largest(stones));
    fflush(stdout);
}

bool eMapStats::sWriteImage(eGameBoard& b, const std::string& path) {
    const int w = b.width();
    const int h = b.height();
    const int pw = 2*w + 1;
    std::vector<unsigned char> px(3*pw*h, 0);
    const auto set = [&](const int x, const int y,
                         const int r, const int g, const int bb) {
        if(x < 0 || y < 0 || x >= pw || y >= h) return;
        unsigned char* p = &px[3*(y*pw + x)];
        p[0] = r;
        p[1] = g;
        p[2] = bb;
    };
    for(int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            const auto t = b.dtile(x, y);
            if(!t) continue;
            int r = 66, g = 89, bb = 148;
            switch(t->terrain()) {
            case eTerrain::dry: {
                const double s = t->scrub();
                if(s < 0.2) { r = 214; g = 170; bb = 99; }
                else if(s < 0.5) { r = 190; g = 160; bb = 90; }
                else { r = 160; g = 145; bb = 80; }
            } break;
            case eTerrain::beach: r = 238; g = 202; bb = 164; break;
            case eTerrain::fertile: r = 155; g = 110; bb = 110; break;
            case eTerrain::forest:
            case eTerrain::choppedForest: r = 90; g = 129; bb = 41; break;
            case eTerrain::water: r = 25; g = 105; bb = 115; break;
            case eTerrain::flatStones:
            case eTerrain::tallStones: r = 130; g = 130; bb = 130; break;
            case eTerrain::copper: r = 206; g = 105; bb = 8; break;
            case eTerrain::silver: r = 230; g = 230; bb = 250; break;
            case eTerrain::marble: r = 125; g = 235; bb = 255; break;
            case eTerrain::orichalc: r = 255; g = 0; bb = 0; break;
            default: break;
            }
            const int alt = t->altitude();
            if(alt > 0) {
                r = std::max(0, r - 35*alt);
                g = std::max(0, g - 35*alt);
                bb = std::max(0, bb - 35*alt);
            }
            if(t->isElevationTile()) {
                r = r/2; g = g/2; bb = bb/2;
            }
            if(t->hasFish()) { r = 255; g = 255; bb = 255; }
            for(const auto& bn : t->banners()) {
                switch(bn->type()) {
                case eBannerTypeS::boar:
                case eBannerTypeS::deer:
                case eBannerTypeS::wolf: r = 255; g = 40; bb = 40; break;
                case eBannerTypeS::entryPoint:
                case eBannerTypeS::exitPoint: r = 255; g = 0; bb = 255; break;
                case eBannerTypeS::riverEntryPoint:
                case eBannerTypeS::riverExitPoint: r = 255; g = 255; bb = 0; break;
                default: break;
                }
            }
            const int xx = 2*x + (y % 2);
            set(xx, y, r, g, bb);
            set(xx + 1, y, r, g, bb);
        }
    }
    FILE* f = fopen(eFsPath::sPath(path).string().c_str(), "wb");
    if(!f) return false;
    fprintf(f, "P6\n%d %d\n255\n", pw, h);
    fwrite(px.data(), 1, px.size(), f);
    fclose(f);
    return true;
}

void eMapStats::sRun(const std::string& path) {
    std::vector<std::string> paks;
    const auto p = eFsPath::sPath(path);
    if(std::filesystem::is_directory(p)) {
        for(const auto& e : std::filesystem::recursive_directory_iterator(p)) {
            if(e.is_directory()) continue;
            if(e.path().extension() != ".pak") continue;
            paks.push_back(eFsPath::sUtf8(e.path()));
        }
    } else {
        paks.push_back(path);
    }
    std::sort(paks.begin(), paks.end());
    sPrintHeader();
    for(const auto& pak : paks) {
        const auto name = eFsPath::sUtf8(std::filesystem::path(
                                             eFsPath::sPath(pak)).stem());
        const auto c = std::make_shared<eCampaign>();
        c->readPak(name, pak);
        // with a single adventure also a picture of each board
        const bool single = paks.size() == 1;
        sPrint(name, "parent", c->parentCityBoard());
        if(single) sWriteImage(c->parentCityBoard(), "mapstats-parent.ppm");
        for(int i = 0; i < 4; i++) {
            auto& cb = c->colonyBoard(i);
            if(cb.width() <= 1) continue;
            const auto bn = "colony" + std::to_string(i);
            sPrint(name, bn, cb);
            if(single) sWriteImage(cb, "mapstats-" + bn + ".ppm");
        }
    }
}
