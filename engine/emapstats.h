#ifndef EMAPSTATS_H
#define EMAPSTATS_H

#include <string>

class eGameBoard;

// Prints what the maps of the original adventures are made of: terrain,
// water bodies, elevation, fish, animals and the points of the map. The
// figures are the measure for generating larger maps with the same
// density of everything.
class eMapStats {
public:
    // one line per board, tab separated, starting with "MAPSTATS"
    static void sPrintHeader();
    static void sPrint(const std::string& adventure,
                       const std::string& board,
                       eGameBoard& b);
    // all adventures (.pak) of the directory, or a single file
    static void sRun(const std::string& path);
    // a picture of the board like the minimap, two pixels per tile, as
    // a binary PPM file
    static bool sWriteImage(eGameBoard& b, const std::string& path);
};

#endif // EMAPSTATS_H
