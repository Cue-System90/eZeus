#include "zeussavefile.h"

#include "pkwareinputstream.h"

ZeusSaveFile::ZeusSaveFile(const std::string& filename) :
    GameFile(filename) {
    MAX_MAPSIZE = 228;
}

bool ZeusSaveFile::load() {
    if(!in.isReadable()) return false;
    const int gridSize = MAX_MAPSIZE*MAX_MAPSIZE;

    seek(0x1779);
    skipCompressed(); // 19184 bytes
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
    skipCompressed(); // walkers
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
    skipBytes(53783);
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
    }
    pk.empty();
    if(pk.hasError()) ok = false;
}
