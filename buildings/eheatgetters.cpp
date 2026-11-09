#include "eheatgetters.h"

#include "characters/gods/egodtype.h"

// Desirability after the Model\Zeus_Model_*.txt files of the original
// game, the same for every difficulty: value, range, rings per step and
// change per step. The original numbers its buildings like the names in
// Zeus_Text group 28, given in the comments.
eHeat eHeatGetters::appeal(const eBuildingType type) {
    switch(type) {
    case eBuildingType::wheatFarm: return {-3, 3, 1, 1}; // 31
    // carrots and onions had swapped values before
    case eBuildingType::onionsFarm: return {-4, 3, 2, 1}; // 32
    case eBuildingType::carrotsFarm: return {-2, 2, 1, 1}; // 33
    case eBuildingType::growersLodge: return {-3, 2, 1, 1}; // 37
    case eBuildingType::oliveTree: // 36
    case eBuildingType::vine: return {1, 1, 1, -1}; // 35
    case eBuildingType::orangeTree: return {1, 2, 2, -1}; // 217
    case eBuildingType::cardingShed: return {-5, 3, 1, 2}; // 41
    case eBuildingType::dairy: return {-5, 3, 1, 2}; // 42
    case eBuildingType::urchinQuay: // 45
    case eBuildingType::fishery: return {-15, 4, 1, 2}; // 44
    case eBuildingType::huntingLodge: return {-6, 4, 2, -2}; // 46
    case eBuildingType::corral: return {-12, 5, 2, 1}; // 216
    case eBuildingType::orangeTendersLodge: return {2, 2, 1, -1}; // 214
    case eBuildingType::timberMill: return {-5, 4, 1, 1}; // 51
    case eBuildingType::masonryShop: return {-6, 4, 1, 1}; // 49
    case eBuildingType::blackMarbleWorkshop: return {-6, 4, 1, 1}; // 213
    case eBuildingType::mint: return {-10, 6, 2, 2}; // 48
    case eBuildingType::foundry: return {-10, 6, 1, 1}; // 50
    case eBuildingType::refinery: return {-10, 5, 1, 1}; // 211
    case eBuildingType::sculptureStudio: return {-2, 2, 1, 1}; // 55
    case eBuildingType::olivePress: return {-4, 4, 2, 2}; // 54
    case eBuildingType::winery: return {4, 4, 1, -1}; // 53
    case eBuildingType::armory: return {-8, 4, 1, 1}; // 135
    case eBuildingType::tower: return {-4, 4, 4, 1}; // 132
    case eBuildingType::granary: return {-12, 4, 1, 2}; // 57
    case eBuildingType::warehouse: return {-2, 4, 4, 1}; // 58
    case eBuildingType::pier: // 60
    case eBuildingType::tradePost: return {-2, 4, 4, 1}; // 62
    // the vendors have no desirability of their own, the agora has
    case eBuildingType::commonAgora: // 63
    case eBuildingType::grandAgora: return {12, 6, 2, -2}; // 64
    case eBuildingType::horseRanch: return {-10, 4, 1, 1}; // 133
    case eBuildingType::horseRanchEnclosure: return {-6, 4, 1, 1}; // 134
    case eBuildingType::hippodromePiece: return {-6, 4, 1, 1}; // 199
    case eBuildingType::fountain: return {4, 4, 2, -2}; // 74
    case eBuildingType::hospital: return {-2, 4, 4, 0}; // 76
    case eBuildingType::watchPost: return {-2, 2, 1, 1}; // 124
    case eBuildingType::palace: return {18, 6, 3, -3}; // 117
    case eBuildingType::taxOffice: return {-4, 2, 1, 1}; // 122
    case eBuildingType::college: return {-5, 2, 1, -3}; // 77
    case eBuildingType::podium: return {3, 3, 1, -1}; // 81
    case eBuildingType::theater: return {6, 3, 1, -1}; // 82
    case eBuildingType::bibliotheke: return {3, 3, 1, -1}; // 202
    case eBuildingType::university: return {-5, 3, 1, -3}; // 204
    case eBuildingType::observatory: return {7, 4, 1, -1}; // 203
    case eBuildingType::museum: return {12, 4, 2, -3}; // 207
    case eBuildingType::doricColumn: // 129
    case eBuildingType::ionicColumn: // 145
    case eBuildingType::corinthianColumn: return {4, 3, 1, -2}; // 146
    case eBuildingType::avenue: return {3, 3, 1, -1}; // 118
    case eBuildingType::commemorative: return {30, 6, 2, -5}; // 119
    // not in the model, spread like a commemorative monument
    case eBuildingType::godMonument: return {35, 6, 2, -5};
    case eBuildingType::park: return {3, 3, 1, -1}; // 128
    case eBuildingType::bench: return {2, 4, 2, -1}; // 127
    case eBuildingType::flowerGarden: return {8, 3, 1, -1}; // 15
    case eBuildingType::gazebo: return {6, 6, 1, -1}; // 16
    case eBuildingType::hedgeMaze: return {12, 4, 1, -1}; // 17
    case eBuildingType::fishPond: return {18, 6, 1, -3}; // 18
    case eBuildingType::waterPark: return {5, 4, 1, -1}; // 25
    case eBuildingType::birdBath: return {3, 3, 1, -1}; // 152
    case eBuildingType::shortObelisk: return {6, 2, 1, -1}; // 24
    case eBuildingType::tallObelisk: return {8, 2, 1, 0}; // 19
    case eBuildingType::shellGarden: return {5, 5, 1, 0}; // 150
    case eBuildingType::sundial: return {7, 4, 1, -2}; // 20
    case eBuildingType::dolphinSculpture: return {12, 4, 2, -3}; // 148
    case eBuildingType::orrery: return {9, 6, 1, -1}; // 149
    case eBuildingType::spring: return {10, 5, 2, -1}; // 22
    case eBuildingType::topiary: return {15, 3, 1, 0}; // 21
    case eBuildingType::baths: return {15, 6, 1, -1}; // 151
    case eBuildingType::stoneCircle: return {24, 4, 2, -2}; // 23
    case eBuildingType::templeAphrodite: // 84 to 98
    case eBuildingType::templeApollo:
    case eBuildingType::templeAres:
    case eBuildingType::templeArtemis:
    case eBuildingType::templeAthena:
    case eBuildingType::templeAtlas:
    case eBuildingType::templeDemeter:
    case eBuildingType::templeDionysus:
    case eBuildingType::templeHades:
    case eBuildingType::templeHephaestus:
    case eBuildingType::templeHera:
    case eBuildingType::templeHermes:
    case eBuildingType::templePoseidon:
    case eBuildingType::templeZeus: return {20, 6, 1, -2};
    case eBuildingType::modestPyramid: return {15, 4, 1, -2}; // 100
    case eBuildingType::pyramid: return {20, 4, 1, -2}; // 101
    case eBuildingType::greatPyramid: return {25, 5, 1, -2}; // 102
    case eBuildingType::majesticPyramid: return {30, 6, 1, -2}; // 103
    case eBuildingType::smallMonumentToTheSky: return {16, 4, 1, -2}; // 104
    case eBuildingType::monumentToTheSky: return {20, 5, 1, -2}; // 105
    case eBuildingType::grandMonumentToTheSky: return {25, 6, 1, -2}; // 106
    case eBuildingType::minorShrineAphrodite: // 107
    case eBuildingType::minorShrineApollo:
    case eBuildingType::minorShrineAres:
    case eBuildingType::minorShrineArtemis:
    case eBuildingType::minorShrineAthena:
    case eBuildingType::minorShrineAtlas:
    case eBuildingType::minorShrineDemeter:
    case eBuildingType::minorShrineDionysus:
    case eBuildingType::minorShrineHades:
    case eBuildingType::minorShrineHephaestus:
    case eBuildingType::minorShrineHera:
    case eBuildingType::minorShrineHermes:
    case eBuildingType::minorShrinePoseidon:
    case eBuildingType::minorShrineZeus: return {20, 4, 2, -2};
    case eBuildingType::shrineAphrodite: // 108
    case eBuildingType::shrineApollo:
    case eBuildingType::shrineAres:
    case eBuildingType::shrineArtemis:
    case eBuildingType::shrineAthena:
    case eBuildingType::shrineAtlas:
    case eBuildingType::shrineDemeter:
    case eBuildingType::shrineDionysus:
    case eBuildingType::shrineHades:
    case eBuildingType::shrineHephaestus:
    case eBuildingType::shrineHera:
    case eBuildingType::shrineHermes:
    case eBuildingType::shrinePoseidon:
    case eBuildingType::shrineZeus: return {25, 5, 2, -2};
    case eBuildingType::majorShrineAphrodite: // 109
    case eBuildingType::majorShrineApollo:
    case eBuildingType::majorShrineAres:
    case eBuildingType::majorShrineArtemis:
    case eBuildingType::majorShrineAthena:
    case eBuildingType::majorShrineAtlas:
    case eBuildingType::majorShrineDemeter:
    case eBuildingType::majorShrineDionysus:
    case eBuildingType::majorShrineHades:
    case eBuildingType::majorShrineHephaestus:
    case eBuildingType::majorShrineHera:
    case eBuildingType::majorShrineHermes:
    case eBuildingType::majorShrinePoseidon:
    case eBuildingType::majorShrineZeus: return {30, 6, 2, -2};
    case eBuildingType::pyramidOfThePantheon: return {30, 6, 1, -2}; // 110
    case eBuildingType::altarOfOlympus: return {25, 5, 1, -2}; // 111
    case eBuildingType::templeOfOlympus: return {30, 6, 1, -3}; // 112
    case eBuildingType::observatoryKosmika: return {25, 6, 1, -1}; // 113
    case eBuildingType::museumAtlantika: return {25, 6, 1, -1}; // 114
    case eBuildingType::artisansGuild: return {-6, 3, 2, 1}; // 56
    case eBuildingType::achillesHall: // 125
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall: return {15, 6, 2, -2};
    case eBuildingType::triremeWharf: return {-12, 4, 2, 2}; // 136
    case eBuildingType::chariotFactory: return {-10, 4, 1, 1}; // 212
    default: return {0, 0};
    }
}

eHeat eHeatGetters::housing(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::commonHouse: return {10, 5};
    case eBuildingType::eliteHousing: return {10, 5};
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::industry(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::timberMill: return {10, 5};
    case eBuildingType::masonryShop: return {10, 5};
    case eBuildingType::mint: return {10, 5};
    case eBuildingType::foundry: return {10, 5};
    case eBuildingType::sculptureStudio: return {10, 5};
    case eBuildingType::armory: return {10, 5};
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::farming(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::wheatFarm: return {10, 5};
    case eBuildingType::carrotsFarm: return {10, 5};
    case eBuildingType::onionsFarm: return {10, 5};
    case eBuildingType::growersLodge: return {10, 5};
    case eBuildingType::cardingShed: return {10, 5};
    case eBuildingType::dairy: return {10, 5};
    case eBuildingType::urchinQuay: return {10, 5};
    case eBuildingType::fishery: return {10, 5};
    case eBuildingType::corral: return {10, 5};
    case eBuildingType::huntingLodge: return {10, 5};
    case eBuildingType::orangeTendersLodge: return {10, 5};
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine: return {2, 3};
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::storage(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::granary: return {10, 5};
    case eBuildingType::warehouse: return {10, 5};
    case eBuildingType::tradePost: return {10, 5};
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::culture(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::college: return {10, 5};
    case eBuildingType::podium: return {10, 5};
    case eBuildingType::gymnasium: return {10, 5};
    case eBuildingType::dramaSchool: return {10, 5};
    case eBuildingType::theater: return {10, 5};
    case eBuildingType::stadium: return {10, 5};
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeatGetters::eHeatGetter eHeatGetters::godLeaning(const eGodType gt) {
    eHeatGetter hg;
    switch(gt) {
    case eGodType::aphrodite:
        hg = eHeatGetters::housing;
        break;
    case eGodType::apollo:
        hg = eHeatGetters::culture;
        break;
    case eGodType::ares:
        hg = eHeatGetters::industry;
        break;
    case eGodType::artemis:
        hg = eHeatGetters::farming;
        break;
    case eGodType::athena:
        hg = eHeatGetters::industry;
        break;
    case eGodType::atlas:
        hg = eHeatGetters::industry;
        break;
    case eGodType::demeter:
        hg = eHeatGetters::farming;
        break;
    case eGodType::dionysus:
        hg = eHeatGetters::farming;
        break;
    case eGodType::hades:
        hg = eHeatGetters::storage;
        break;
    case eGodType::hephaestus:
        hg = eHeatGetters::industry;
        break;
    case eGodType::hera:
        hg = eHeatGetters::farming;
        break;
    case eGodType::hermes:
        hg = eHeatGetters::culture;
        break;
    case eGodType::poseidon:
        hg = eHeatGetters::farming;
        break;
    case eGodType::zeus:
        hg = eHeatGetters::storage;
        break;
    }
    return hg;
}

eHeat eHeatGetters::empty(const eBuildingType type) {
    (void)type;
    switch(type) {
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {-2, 5};
    }
}

eHeat eHeatGetters::any(const eBuildingType type) {
    (void)type;
    switch(type) {
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::fertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return {2, 8};
    } else {
        return {0, 0};
    }
}

eHeat eHeatGetters::notFertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return {0, 0};
    } else {
        return {2, 8};
    }
}
