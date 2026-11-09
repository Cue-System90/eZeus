#ifndef EHEATMAP_H
#define EHEATMAP_H

#include <vector>
#include <SDL2/SDL_rect.h>

struct eHeat {
    int fValue;
    int fRange;
    // Desirability as Zeus spreads it: in square rings around the
    // building, the value changes by fStepSize every fStep rings. Without
    // steps the value falls off linearly towards the range.
    int fStep = 0;
    int fStepSize = 0;
};

class eHeatMap {
public:
    struct eHeatTile {
        bool fOutsideRange = false;
        bool fEnabled = false;
        double fAppeal = 0;
    };

    void add(const eHeatMap& other);
    void multiply(const eHeatMap& other);

    void reset();
    void initialize(const int dx, const int dy,
                    const int w, const int h);

    void addHeat(const eHeat& a,
                 const SDL_Rect& tileRect);

    void addHeat(const eHeat& a,
                 const int ax, const int ay,
                 const int sw, const int sh);

    void addHeat(const int x, const int y,
                 const double a);

    void setOutsideRange(const int x, const int y);
    bool enabled(const int x, const int y) const;
    double heat(const int x, const int y) const;

    int dx() const { return mDX; }
    int dy() const { return mDY; }

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    void set(const int x, const int y,
             const bool e, const double h);
private:
    void addRings(const eHeat& a,
                  const int ax, const int ay,
                  const int sw, const int sh);

    int mDX = 0;
    int mDY = 0;
    int mWidth = 0;
    int mHeight = 0;
    std::vector<std::vector<eHeatTile>> mMap;
};

#endif // EHEATMAP_H
