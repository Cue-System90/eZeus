/*
 *   Citybuilding Mappers - create minimaps from citybuilding game files
 *   Copyright (C) 2007, 2008  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef ZEUSFILE_H
#define ZEUSFILE_H

#include "gamefile.h"

#include "engine/egameboard.h"

class eCampaign;

enum class eZeusFileVersion {
    zeus_1_0,
    poseidon_2_0
};

class ZeusFile : public GameFile {
	public:
		static const int MAX_MAPS = 5; // parent city + 4 colonies

        ZeusFile(const std::string& filename);

        struct eNullTile {
            eTile* fTile = nullptr;
        };
        using eOriginalTileMap = std::map<int, std::map<int, eNullTile>>;

        // Sets the terrain of the tiles of board from the grids of an
        // original map or saved game and returns the tile at each
        // position relative to the map, as [y][x]
        static eOriginalTileMap sApplyTerrain(
                eGameBoard& board, const int mapsize, const bool atlantean,
                const Grid<uint32_t>& terrain,
                const Grid<uint8_t>& scrub,
                const Grid<uint8_t>& elevation);

        void readVersion();
        eZeusFileVersion version() const { return mVersion; }
        bool isAtlantean() const { return mAtlantean; }


		/**
		* Returns the number of maps in this file. Call this function
		* before attempting to get any images from this map using
		* getImage();
		*/
		int getNumMaps();
		
		/**
		* Returns the next image available. Throws an exception if the
		* image can't be loaded for whatever reason.
		* NOTE: call getNumMaps() before calling this function
		*/
        bool loadBoard(eGameBoard& board, eCampaign& campaign,
                       eCityId& cid);
	private:
		int getMapsize();

        eZeusFileVersion mVersion;
        bool mAtlantean;

		int filetype;
		int numMaps;
		int retrievedMaps;
		int positions[MAX_MAPS];
};

#endif /* ZEUSFILE_H */
