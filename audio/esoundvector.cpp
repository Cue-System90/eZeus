#include "esoundvector.h"
#include "efspath.h"

#include "erand.h"
#include <filesystem>

Mix_Chunk* loadSound(const std::string& path) {
    const auto wav = Mix_LoadWAV(path.c_str());
    if(!wav) {
        printf("Failed to load sound '%s'!\n SDL_mixer Error: %s\n",
               path.c_str(), Mix_GetError());
        return nullptr;
    }
    return wav;
}

eSoundVector::~eSoundVector() {
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        Mix_FreeChunk(s.first);
    }
}

const bool sLoadOnAdd = false;

void eSoundVector::addPath(const std::string& path) {
    const bool e = std::filesystem::exists(eFsPath::sPath(path));
    if(!e) printf("Missing audio file %s\n", path.c_str());
    const auto sound = sLoadOnAdd ? loadSound(path) : nullptr;
    mPaths.push_back({sound, path});
}

void eSoundVector::play(const int id, const int chn) {
    const int idMax = mPaths.size();
    if(id < 0 || id >= idMax) return;
    auto& p = mPaths[id];
    if(!p.first) p.first = loadSound(p.second);
    if(p.first) Mix_PlayChannel(chn, p.first, 0);
}

void eSoundVector::playRandomSound() {
    const int sc = soundCount();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(id);
}
