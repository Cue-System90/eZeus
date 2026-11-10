#ifndef EPERLINNOISE_H
#define EPERLINNOISE_H

#include <array>
#include <cmath>
#include <random>

// Ken Perlin's improved noise in two dimensions, with a seeded permutation
// table and a sum of octaves. Values of noise() lie roughly in [-1, 1],
// those of fractal() too. Replaces libnoise, which was only available on
// Linux.
class ePerlinNoise {
public:
    explicit ePerlinNoise(const unsigned seed) {
        for(int i = 0; i < 256; i++) mPerm[i] = i;
        std::mt19937 rng(seed);
        for(int i = 255; i > 0; i--) {
            std::uniform_int_distribution<int> d(0, i);
            std::swap(mPerm[i], mPerm[d(rng)]);
        }
        for(int i = 0; i < 256; i++) mPerm[256 + i] = mPerm[i];
    }

    double noise(const double x, const double y) const {
        const int xi = static_cast<int>(std::floor(x));
        const int yi = static_cast<int>(std::floor(y));
        const double xf = x - xi;
        const double yf = y - yi;
        const int X = xi & 255;
        const int Y = yi & 255;
        const double u = fade(xf);
        const double v = fade(yf);
        const int aa = mPerm[mPerm[X] + Y];
        const int ab = mPerm[mPerm[X] + Y + 1];
        const int ba = mPerm[mPerm[X + 1] + Y];
        const int bb = mPerm[mPerm[X + 1] + Y + 1];
        const double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
        const double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
        return lerp(x1, x2, v);
    }

    // octaves of noise, each twice as fine and half as strong as the last;
    // featureSize is the size in tiles of the largest shapes
    double fractal(const double x, const double y,
                   const double featureSize,
                   const int octaves = 4,
                   const double persistence = 0.5) const {
        double sum = 0;
        double amp = 1;
        double ampSum = 0;
        double freq = 1./featureSize;
        for(int i = 0; i < octaves; i++) {
            sum += amp*noise(x*freq, y*freq);
            ampSum += amp;
            amp *= persistence;
            freq *= 2;
        }
        return sum/ampSum;
    }
private:
    static double fade(const double t) {
        return t*t*t*(t*(t*6 - 15) + 10);
    }
    static double lerp(const double a, const double b, const double t) {
        return a + t*(b - a);
    }
    static double grad(const int hash, const double x, const double y) {
        switch(hash & 7) {
        case 0: return x + y;
        case 1: return -x + y;
        case 2: return x - y;
        case 3: return -x - y;
        case 4: return x;
        case 5: return -x;
        case 6: return y;
        default: return -y;
        }
    }

    std::array<int, 512> mPerm;
};

#endif // EPERLINNOISE_H
