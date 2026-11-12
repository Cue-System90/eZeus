#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include <vector>
#include <functional>

enum class eTerrain;
class eGameBoard;
class eTile;
class eTileBase;
class ePerlinNoise;

enum class eCityId;

// The defaults are the medians of the maps of the original adventures
// (eZeus --map-stats): 22% water, 8% forest, 3% fertile, 2% stones, 4%
// elevated land; a river on most maps, a lake on one in four; about
// three animal points and two fishing points on the largest maps. The
// counts are per 26000 tiles, the size of the largest original map, and
// scale with the area, so a map four times as large gets four times as
// many.
struct eMapGeneratorSettings {
    eCityId fCid = static_cast<eCityId>(-1);
    int fSeed = 0; // 0 picks a random one

    int fWater = 22; // percent of the tiles
    int fForest = 8; // percent of the tiles
    int fFertile = 3; // percent of the tiles
    int fStones = 2; // percent of the tiles
    int fElevation = 4; // percent of the tiles
    int fScrub = 25; // percent of the dry tiles

    // counts per sAreaUnit tiles
    int fLakes = 1;
    int fRivers = 1; // at least one if not 0
    int fCopper = 1; // patches
    int fSilver = 1;
    int fMarble = 1;
    int fOrichalc = 0;
    int fAnimals = 3; // boar, deer and wolf points together
    int fFish = 2; // fishing points

    static constexpr int sAreaUnit = 26000;
    static eMapGeneratorSettings sLast;
};

class eMapGenerator {
public:
    eMapGenerator(eGameBoard& board);

    using eMGS = eMapGeneratorSettings;
    void generate(const eMGS& settings);
private:
    using eTileVerifier = std::function<bool(eTileBase* const)>;

    double value(const std::vector<double>& field, eTile* const t) const;
    double& value(std::vector<double>& field, eTile* const t) const;
    std::vector<double> noiseField(const ePerlinNoise& n,
                                   const double featureSize,
                                   const int octaves) const;
    // the value above which the given fraction of the tiles (that pass
    // the verifier) lies
    double quantile(const std::vector<double>& field,
                    const double fraction,
                    const eTileVerifier& v) const;
    std::vector<int> distanceTo(const eTileVerifier& v,
                                const int max) const;
    std::vector<std::vector<eTile*>> patches(const eTileVerifier& v) const;
    bool onEdge(eTileBase* const t) const;

    void generateSea(const eMGS& s, const std::vector<double>& height);
    void generateLakes(const eMGS& s, const std::vector<double>& height);
    void generateRivers(const eMGS& s, const std::vector<double>& height);
    void worldPos(eTileBase* const t, double& x, double& y) const;
    eTile* tileAt(const double x, const double y) const;
    std::vector<std::pair<double, double>> walkRiver(
            eTile* const source, const std::vector<int>& dist,
            const eTileVerifier& mouth, const int seed) const;
    void carveRiver(const std::vector<std::pair<double, double>>& path,
                    const double halfWidth, const int seed);
    void generateBeach(const eMGS& s);
    void generateElevation(const eMGS& s, const std::vector<double>& height);
    void generateForest(const eMGS& s);
    void generateFertile(const eMGS& s);
    void generateStones(const eMGS& s, const std::vector<double>& height);
    void generateOre(const eMGS& s);
    void generateScrub(const eMGS& s);
    void generateAnimals(const eMGS& s);
    void generateFish(const eMGS& s);
    void generateEntryExit(const eMGS& s);

    int rand(const int n) const;

    eGameBoard& mBoard;
    int mW = 0;
    int mH = 0;
    int mTiles = 0;
    unsigned mSeed = 0;
    std::vector<eTile*> mAll;
    // the sides of the map the sea lies on: 0 top, 1 right, 2 bottom, 3 left
    std::vector<int> mSeaSides;
    std::vector<bool> mSea; // the water tiles that belong to the sea
    std::vector<bool> mRiver; // the water tiles of the rivers
    // how many of something a map of this size gets
    int count(const int perUnit) const;
};

#endif // EMAPGENERATOR_H
