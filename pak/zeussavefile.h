#ifndef ZEUSSAVEFILE_H
#define ZEUSSAVEFILE_H

#include "gamefile.h"
#include "engine/eresourcetype.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

// One entry of the building table of a saved game. A building made of
// several parts, like the palace or an agora with its vendors, has one
// entry per part, chained through fPrev and fNext.
struct eZeusSaveBuilding {
    int fId = 0;
    int fType = 0; // index of the building's name in Zeus_Text group 28
    int fSubtype = 0; // the level of a house, the kind of a monument
    int fX = 0; // position relative to the map, like the points of a .pak
    int fY = 0;
    int fSize = 0;
    int fRotation = 0;
    int fPrev = 0;
    int fNext = 0;
    int fPeople = 0;
    // what a house holds
    int fFood = 0;
    int fFleece = 0;
    int fOil = 0;
    int fWine = 0;
    // what a warehouse space or a granary holds, in eZeus units
    std::vector<std::pair<eResourceType, int>> fGoods;

    // What a storage is set to do with each good, kept with the part the
    // settings belong to: the warehouse or granary, the trade post, the
    // part of a pier on land.
    bool fHasOrders = false;
    eResourceType fAccept = eResourceType::none;
    eResourceType fGet = eResourceType::none;
    eResourceType fEmpty = eResourceType::none;
    eResourceType fImports = eResourceType::none;
    eResourceType fExports = eResourceType::none;
    // how much of a good to keep at most, in eZeus units
    std::vector<std::pair<eResourceType, int>> fMaxCount;
};

// A domesticated animal of a saved game. The original keeps them with the
// walkers, named in Zeus_Text group 64.
struct eZeusSaveAnimal {
    int fType = 0; // 49 sheep, 50 goats
    int fX = 0; // like the buildings
    int fY = 0;
};

// Reads a saved game (.sav) of Zeus and Poseidon. The order of the blocks
// follows citybuilding-mappers by Bianca van Schaik, which documents most
// of them by their size only.
class ZeusSaveFile : public GameFile {
public:
    ZeusSaveFile(const std::string& filename);

    bool load();

    // file name of the adventure without .pak, including the character
    // in front of the original adventures, like "#Zeus und Europa"
    const std::string& adventure() const { return mAdventure; }
    int mapsize() const { return mMapsize; }
    bool poseidon() const { return mPoseidon; }
    int drachmas() const { return mDrachmas; }

    const Grid<uint32_t>& terrain() const { return *mTerrain; }
    const Grid<uint8_t>& scrub() const { return *mScrub; }
    const Grid<uint8_t>& elevation() const { return *mElevation; }

    const std::vector<eZeusSaveBuilding>& buildings() const
    { return mBuildings; }
    const std::vector<eZeusSaveAnimal>& animals() const
    { return mAnimals; }
private:
    void readAdventure();
    void readWalkers();
    void readBuildings();
    void readStorages();

    std::string mAdventure;
    int mMapsize = 0;
    bool mPoseidon = false;
    int mDrachmas = 0;

    std::unique_ptr<Grid<uint32_t>> mTerrain;
    std::unique_ptr<Grid<uint8_t>> mScrub;
    std::unique_ptr<Grid<uint8_t>> mElevation;

    std::vector<eZeusSaveBuilding> mBuildings;
    std::vector<eZeusSaveAnimal> mAnimals;
};

#endif // ZEUSSAVEFILE_H
