#include "zeussavefile.h"

#include "pkwareinputstream.h"
#include "epakhelpers.h"

#include <map>

ZeusSaveFile::ZeusSaveFile(const std::string& filename) :
    GameFile(filename) {
    MAX_MAPSIZE = 228;
}

bool ZeusSaveFile::load() {
    if(!in.isReadable()) return false;
    const int gridSize = MAX_MAPSIZE*MAX_MAPSIZE;

    seek(0x1779);
    { // 19184 bytes of city data
        const auto city = readCompressed();
        const auto readInt = [&city](const size_t i) {
            if(i + 4 > city.size()) return 0;
            return static_cast<int>(city[i] | (city[i + 1] << 8) |
                                    (city[i + 2] << 16) | (city[i + 3] << 24));
        };
        mDrachmas = readInt(12); // the population follows at 36
    }
    skipBytes(8);
    skipCompressed(); // 12584 bytes
    skipBytes(188);
    mMapsize = readUInt();
    if(mMapsize <= 0 || mMapsize > MAX_MAPSIZE) return false;
    skipBytes(600);
    mPoseidon = readUByte() == 1;
    readAdventure();
    skipCompressed(); // 14400 bytes
    skipBytes(18609);
    skipCompressed(); // edges
    skipCompressed(); // short grid, zeroes
    mTerrain.reset(readCompressedIntGrid());
    skipCompressed(); // byte grid, zeroes
    skipCompressed(); // short grid with ids
    skipCompressed(); // byte grid, 00 or 20
    skipCompressed(); // byte grid, zeroes
    skipBytes(gridSize); // random
    for(int i = 0; i < 5; i++) {
        skipCompressed(); // appeal, zeroes and perhaps damage
    }
    readWalkers();
    skipCompressed(); // 2000 bytes
    skipCompressed(); // 500000 bytes
    skipCompressed(); // 15600 bytes
    skipBytes(69);
    readBuildings();
    skipBytes(352);
    skipCompressed(); // 60000 bytes
    skipBytes(17974);
    skipCompressed(); // 1000 bytes
    skipCompressed(); // 1000 bytes
    skipCompressed(); // 8000 bytes
    readStorages(); // 53783 bytes
    skipBytes(gridSize); // meadow
    skipBytes(16);
    skipCompressed(); // 75168 bytes
    skipBytes(gridSize); // marble, sheep, goats and timber
    skipBytes(32);
    skipCompressed(); // 36 bytes
    skipCompressed(); // int grid
    skipBytes(39);
    skipCompressed(); // byte grid
    mScrub.reset(readCompressedByteGrid());
    skipBytes(4);
    mElevation.reset(readCompressedByteGrid()); // including edges

    return ok && mTerrain && mScrub && mElevation;
}

void ZeusSaveFile::readAdventure() {
    // 1383 bytes, the name of the adventure starts at 1048
    skipBytes(1048);
    int read = 0;
    while(read < 1383 - 1048) {
        const char c = readUByte();
        read++;
        if(c == 0) break;
        mAdventure += c;
    }
    skipBytes(1383 - 1048 - read);
}

void ZeusSaveFile::readWalkers() {
    if(!ok) return;
    const int length = readUInt();
    PKWareInputStream pk(&in, length, false);

    // 2000 entries of 388 bytes, the kind at 10, the position at 20. Only
    // the animals come along, the people of a city find their way anew.
    const int maxWalkers = 2000;
    const int entrySize = 388;
    uint8_t e[entrySize];
    const auto readShort = [&e](const int i) {
        return static_cast<int16_t>(e[i] | (e[i + 1] << 8));
    };
    for(int i = 0; i < maxWalkers; i++) {
        if(pk.hasError()) break;
        for(int j = 0; j < entrySize; j++) {
            e[j] = pk.readByte();
        }
        const int type = readShort(10);
        if(type != 49 && type != 50) continue;
        auto& a = mAnimals.emplace_back();
        a.fType = type;
        a.fX = readShort(20);
        a.fY = readShort(22);
    }
    pk.empty();
    if(pk.hasError()) ok = false;
}

void ZeusSaveFile::readBuildings() {
    if(!ok) return;
    const int length = readUInt();
    PKWareInputStream pk(&in, length, false);

    const int maxBuildings = 4000;
    const int entrySize = 280;
    uint8_t e[entrySize];
    const auto readShort = [&e](const int i) {
        return static_cast<int16_t>(e[i] | (e[i + 1] << 8));
    };
    for(int i = 0; i < maxBuildings; i++) {
        if(pk.hasError()) break;
        for(int j = 0; j < entrySize; j++) {
            e[j] = pk.readByte();
        }
        const int type = readShort(16);
        if(type == 0) continue;
        auto& b = mBuildings.emplace_back();
        b.fId = i;
        b.fType = type;
        b.fSubtype = readShort(18);
        b.fSize = e[3];
        b.fX = readShort(6);
        b.fY = readShort(8);
        b.fPeople = readShort(28);
        b.fPrev = readShort(56);
        b.fNext = readShort(58);
        b.fRotation = e[172];
        // the original counts goods in hundreds, a full warehouse space
        // holds 400, in eZeus 4
        if(type == 59) { // a warehouse space
            const auto res = ePakHelpers::pakResourceByteToType(
                                 readShort(18), mPoseidon);
            const int n = readShort(60)/100;
            if(n > 0) b.fGoods.push_back({res, n});
        } else if(type == 57) { // granary
            // one amount per kind of food, followed by the free space.
            // The first is wheat, the fifth cheese, the others are not
            // identified yet and come in as wheat.
            for(int i = 0; i < 7; i++) {
                const int n = readShort(86 + 2*i)/100;
                if(n <= 0) continue;
                const auto res = i == 4 ? eResourceType::cheese :
                                          eResourceType::wheat;
                b.fGoods.push_back({res, n});
            }
        } else if(type == 63 || type == 64) { // agora
            // what its stalls hold: the kinds of food like in a granary,
            // then one amount per good
            int food = 0;
            for(int i = 0; i < 8; i++) food += readShort(86 + 2*i);
            const auto add = [&](const eResourceType res, const int n) {
                const int loads = (n + 50)/100;
                if(loads > 0) b.fGoods.push_back({res, loads});
            };
            add(eResourceType::food, food);
            add(eResourceType::armor, readShort(102));
            add(eResourceType::fleece, readShort(104));
            add(eResourceType::oliveOil, readShort(106));
            add(eResourceType::wine, readShort(108));
            add(eResourceType::horse, readShort(116));
        }
    }
    pk.empty();
    if(pk.hasError()) ok = false;
}

namespace {
// The settings count the goods like the rest of the file, the numbers it
// leaves out, money and food stand for no single good.
eResourceType sGood(const int r, const bool poseidon) {
    if(r <= 0 || r == 22) return eResourceType::none;
    if(!poseidon && (r == 20 || r == 21)) return eResourceType::none;
    const auto t = ePakHelpers::pakResourceByteToType(r, poseidon);
    if(t == eResourceType::drachmas || t == eResourceType::food) {
        return eResourceType::none;
    }
    return t;
}
}

void ZeusSaveFile::readStorages() {
    // 200 entries of 268 bytes from byte 28, one per storage, trade post,
    // pier and agora: +4 the index of the building, +8 in use, then per
    // good +10 what to do with it, +52 how much to keep in hundreds and
    // +196 how to trade it
    const int size = 53783;
    std::vector<uint8_t> d(size);
    for(int i = 0; i < size; i++) d[i] = readUByte();
    if(!ok) return;
    std::map<int, eZeusSaveBuilding*> byId;
    for(auto& b : mBuildings) byId[b.fId] = &b;
    const int start = 28;
    const int entrySize = 268;
    const int goods = 23;
    for(int e = start; e + entrySize <= size; e += entrySize) {
        const uint8_t* const s = &d[e];
        if(!s[8]) continue;
        const int id = s[4] | (s[5] << 8) | (s[6] << 16) | (s[7] << 24);
        const auto it = byId.find(id);
        if(it == byId.end()) continue;
        auto& b = *it->second;
        if(b.fType != 57 && b.fType != 58 && b.fType != 61 &&
           b.fType != 62) continue;
        b.fHasOrders = true;
        for(int r = 0; r < goods; r++) {
            const auto t = sGood(r, mPoseidon);
            if(t == eResourceType::none) continue;
            switch(s[10 + r]) {
            case 0: b.fAccept = b.fAccept | t; break;
            case 2: b.fGet = b.fGet | t; break;
            case 3: b.fEmpty = b.fEmpty | t; break;
            default: break; // 1, not accepted
            }
            switch(s[196 + r]) {
            case 5: b.fImports = b.fImports | t; break;
            case 7: b.fExports = b.fExports | t; break;
            default: break; // 6 and 8 not now, 9 not traded
            }
            // a sculpture takes a whole space, which holds 400
            const int keep = s[52 + 2*r] | (s[53 + 2*r] << 8);
            const int unit = t == eResourceType::sculpture ? 400 : 100;
            b.fMaxCount.push_back({t, keep/unit});
        }
    }
}
