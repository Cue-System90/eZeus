#include "ecampaign.h"

#include "elanguage.h"
#include "pak/zeusfile.h"
#include "pak/epakhelpers.h"
#include "estringhelpers.h"

#include "buildings/pyramids/epyramid.h"

#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/egiftfromevent.h"
#include "gameEvents/emonsterincityevent.h"
#include "gameEvents/emonsterinvasionevent.h"
#include "gameEvents/emonsterunleashedevent.h"
#include "gameEvents/egodattackevent.h"
#include "gameEvents/ewagechangeevent.h"
#include "gameEvents/etradeshutdownevent.h"
#include "gameEvents/etradeopenupevent.h"
#include "gameEvents/esupplychangeevent.h"
#include "gameEvents/edemandchangeevent.h"
#include "gameEvents/epricechangeevent.h"
#include "gameEvents/eearthquakeevent.h"
#include "gameEvents/ecitybecomesevent.h"
#include "gameEvents/eeconomicchangeevent.h"
#include "gameEvents/emilitarychangeevent.h"
#include "gameEvents/egoddisasterevent.h"
#include "gameEvents/elavaevent.h"
#include "gameEvents/etidalwaveevent.h"
#include "gameEvents/esinklandevent.h"
#include "gameEvents/elandslideevent.h"
#include "gameEvents/einvasionevent.h"
#include "gameEvents/erivalarmyawayevent.h"
#include "gameEvents/etroopsrequestevent.h"
#include <cmath>

eResourceType pakCityResourceByteToType(
        const uint8_t byte, const bool poseidon) {
    if(byte == 0) {
        return eResourceType::none;
    } else if(byte == 1) {
        return eResourceType::urchin;
    } else if(byte == 2) {
        return eResourceType::fish;
    } else if(byte == 3) {
        return eResourceType::meat;
    } else if(byte == 4) {
        return eResourceType::cheese;
    } else if(byte == 5) {
        return eResourceType::carrots;
    } else if(byte == 6) {
        return eResourceType::onions;
    } else if(byte == 7) {
        return eResourceType::wheat;
    } else if(poseidon && byte == 8) {
        return eResourceType::oranges;
    } else if(byte == 9 + (poseidon ? 0 : -1)) {
        return eResourceType::wood;
    } else if(byte == 10 + (poseidon ? 0 : -1)) {
        return eResourceType::bronze;
    } else if(byte == 11 + (poseidon ? 0 : -1)) {
        return eResourceType::marble;
    } else if(byte == 12 + (poseidon ? 0 : -1)) {
        return eResourceType::grapes;
    } else if(byte == 13 + (poseidon ? 0 : -1)) {
        return eResourceType::olives;
    } else if(byte == 14 + (poseidon ? 0 : -1)) {
        return eResourceType::fleece;
    } else if(poseidon && byte == 16) {
        return eResourceType::blackMarble;
    } else if(poseidon && byte == 17) {
        return eResourceType::orichalc;
    }

    printf("Invalid city resource byte %i\n", byte);
    return eResourceType::none;
}

void readEpisodeAllowedBuildings(eEpisode& ep, ZeusFile& file,
                                 const bool atlantean, const eCityId cid) {
    auto& av = ep.fAvailableBuildings[cid];
    for(const auto type : {eBuildingType::mint,
                           eBuildingType::huntingLodge,
                           eBuildingType::olivePress,
                           eBuildingType::grandAgora,
                           atlantean ? eBuildingType::corral : eBuildingType::none,
                           eBuildingType::granary,
                           eBuildingType::commonAgora,
                           eBuildingType::warehouse,
                           eBuildingType::tradePost, // trade buildings
                           atlantean ? eBuildingType::museum : eBuildingType::stadium,
                           atlantean ? eBuildingType::laboratory : eBuildingType::theater,
                           atlantean ? eBuildingType::bibliotheke : eBuildingType::gymnasium,
                           eBuildingType::winery,
                           eBuildingType::sculptureStudio,
                           atlantean ? eBuildingType::observatory : eBuildingType::podium,
                           atlantean ? eBuildingType::university : eBuildingType::college,
                           eBuildingType::fountain,
                           eBuildingType::horseRanch,
                           atlantean ? eBuildingType::inventorsWorkshop : eBuildingType::dramaSchool,
                           eBuildingType::maintenanceOffice,
                           eBuildingType::hospital,
                           eBuildingType::taxOffice,
                           eBuildingType::watchPost,
                           eBuildingType::palace,
                           eBuildingType::none, // hero's hall
                           eBuildingType::none, // road block
                           eBuildingType::bridge,
                           eBuildingType::none, // columns
                           eBuildingType::park,
                           eBuildingType::avenue,
                           eBuildingType::none, // boulevards
                           eBuildingType::wall,
                           eBuildingType::tower,
                           eBuildingType::gatehouse,
                           eBuildingType::bench,
                           eBuildingType::gazebo,
                           eBuildingType::flowerGarden,
                           eBuildingType::hedgeMaze,
                           eBuildingType::fishPond,
                           eBuildingType::armory,
                           eBuildingType::triremeWharf,
                           eBuildingType::eliteHousing,
                           atlantean ? eBuildingType::hippodromePiece : eBuildingType::none, // hippodrome
                           atlantean ? eBuildingType::chariotFactory : eBuildingType::none,
                           eBuildingType::tallObelisk,
                           eBuildingType::sundial,
                           eBuildingType::topiary,
                           eBuildingType::spring,
                           eBuildingType::stoneCircle,
                           eBuildingType::shortObelisk,
                           eBuildingType::waterPark,
                           eBuildingType::dolphinSculpture,
                           eBuildingType::orrery,
                           eBuildingType::shellGarden,
                           eBuildingType::baths,
                           eBuildingType::birdBath}) {
        const auto allowedByte = file.readUByte();
        file.skipBytes(1);
        if(type == eBuildingType::grandAgora) file.skipBytes(2);
        if(allowedByte != 1) {
            printf("%s %i\n", eBuilding::sNameForBuilding(type).c_str(), allowedByte);
        }
        const bool allowed = allowedByte == 1;
        if(allowed) av.allow(type);
        else av.disallow(type);
    }
    printf("\n");
}

void readEpisodeResources(eEpisode& ep, ZeusFile& file,
                          const eCityId cid) {
    const bool atlantean = file.isAtlantean();
    const auto version = file.version();
    const bool poseidon = version == eZeusFileVersion::poseidon_2_0;
    auto& av = ep.fAvailableBuildings[cid];
    const int jMax = poseidon ? 12 : 10;
    for(int j = 0; j < jMax; j++) {
        const auto resourceByte = file.readUByte();
        const auto type = pakCityResourceByteToType(
                              resourceByte, poseidon);
        if(type != eResourceType::none) {
            printf("%s\n", eResourceTypeHelpers::typeName(type).c_str());
        } else {
            printf("none\n");
        }
        switch(type) {
        case eResourceType::urchin:
            av.allow(eBuildingType::urchinQuay);
            break;
        case eResourceType::fish:
            av.allow(eBuildingType::fishery);
            break;
        case eResourceType::meat:
            av.allow(eBuildingType::none);
            break;
        case eResourceType::cheese:
            av.allow(eBuildingType::dairy);
            av.allow(eBuildingType::goat);
            break;
        case eResourceType::carrots:
            av.allow(eBuildingType::carrotsFarm);
            break;
        case eResourceType::onions:
            av.allow(eBuildingType::onionsFarm);
            break;
        case eResourceType::wheat:
            av.allow(eBuildingType::wheatFarm);
            break;
        case eResourceType::oranges:
            if(atlantean) {
                av.allow(eBuildingType::orangeTendersLodge);
                av.allow(eBuildingType::orangeTree);
            }
            break;
        case eResourceType::wood:
            av.allow(eBuildingType::timberMill);
            break;
        case eResourceType::bronze:
            av.allow(eBuildingType::foundry);
            break;
        case eResourceType::marble:
            av.allow(eBuildingType::masonryShop);
            break;
        case eResourceType::grapes:
            av.allow(eBuildingType::growersLodge);
            av.allow(eBuildingType::vine);
            break;
        case eResourceType::olives:
            av.allow(eBuildingType::growersLodge);
            av.allow(eBuildingType::oliveTree);
            break;
        case eResourceType::fleece:
            av.allow(eBuildingType::cardingShed);
            break;
        case eResourceType::blackMarble:
            if(atlantean) {
                av.allow(eBuildingType::blackMarbleWorkshop);
            }
            break;
        case eResourceType::orichalc:
            if(atlantean) {
                av.allow(eBuildingType::refinery);
            }
            break;
        default:
            break;
        }
    }
    printf("\n");
}

struct ePakMonster {
    bool fValid = false;
    eMonsterType fType = eMonsterType::hydra;
};

struct ePakGod {
    bool fValid = false;
    eGodType fType = eGodType::zeus;
};

struct ePakPyramid {
    bool fAllowed = false;
    eBuildingType fType = eBuildingType::modestPyramid;
    std::vector<bool> fLevels;
};

void readEpisodeAllowedSanctuaries(eEpisode& ep, ZeusFile& file,
                                   const std::vector<ePakGod>& friendlyGods,
                                   const eCityId cid) {
    for(int i = 0; i < 6; i++) {
        const uint8_t sanctuaryByte = file.readUByte();
        const auto& v = friendlyGods[i];
        const bool sanctuary = sanctuaryByte;
        if(v.fValid && sanctuary) {
            ep.fFriendlyGods[cid].push_back(v.fType);
        }
        if(v.fValid) {
            printf("%s %i\n", eGod::sGodName(v.fType).c_str(),
                   sanctuaryByte);
        }
    }
    printf("\n");
    file.skipBytes(6);

    const uint8_t maxSanct = file.readUByte();
    ep.fMaxSanctuaries[cid] = maxSanct;
    printf("max sanctuaries %i\n", maxSanct);
    printf("\n");
}

enum class ePakEventType {
    receiveRequest,
    invasion,
    earthquake,
    godQuest,
    landSlide,
    wageIncrease,
    wageDecrease,
    demandIncrease,
    demandDecrease,
    supplyIncrease,
    supplyDecrease,
    priceIncrease,
    priceDecrease,
    cityStatusChange,
    rivalArmyAway,
    giftFrom,
    lavaFlow,
    tidalWave,
    monsterInvasion,
    godAttack,
    sinkLand
};

ePakEventType pakIdToEventType(const uint8_t id, bool& valid) {
    valid = true;
    if(id == 1) return ePakEventType::receiveRequest;
    else if(id == 2) return ePakEventType::invasion;
    else if(id == 3) return ePakEventType::earthquake;
    else if(id == 4) return ePakEventType::godQuest;
    else if(id == 5) return ePakEventType::landSlide;
    else if(id == 8) return ePakEventType::wageIncrease;
    else if(id == 9) return ePakEventType::wageDecrease;
    else if(id == 13) return ePakEventType::demandIncrease;
    else if(id == 14) return ePakEventType::demandDecrease;
    else if(id == 15) return ePakEventType::priceIncrease;
    else if(id == 16) return ePakEventType::priceDecrease;
    else if(id == 19) return ePakEventType::cityStatusChange;
    else if(id == 20) return ePakEventType::rivalArmyAway;
    else if(id == 21) return ePakEventType::supplyIncrease;
    else if(id == 22) return ePakEventType::supplyDecrease;
    else if(id == 23) return ePakEventType::giftFrom;
    else if(id == 24) return ePakEventType::lavaFlow;
    else if(id == 25) return ePakEventType::tidalWave;
    else if(id == 26) return ePakEventType::monsterInvasion;
    else if(id == 27) return ePakEventType::godAttack;
    else if(id == 28) return ePakEventType::sinkLand;

    valid = false;
    printf("Invalid event type id %i\n", id);
    return ePakEventType::receiveRequest;
}

eReceiveRequestType pakIdToReceiveRequestType(const uint16_t id) {
    if(id == 0) return eReceiveRequestType::general;
    else if(id == 3) return eReceiveRequestType::festival;
    else if(id == 4) return eReceiveRequestType::project;
    else if(id == 5) return eReceiveRequestType::famine;
    else if(id == 6) return eReceiveRequestType::financialWoes;
    printf("Invalid receive request type %i\n", id);
    return eReceiveRequestType::general;
}

eGodType pakIdToGodType(const uint8_t id, bool& valid) {
    valid = true;
    if(id == 0) return eGodType::zeus;
    else if(id == 1) return eGodType::poseidon;
    else if(id == 2) return eGodType::demeter;
    else if(id == 3) return eGodType::apollo;
    else if(id == 4) return eGodType::artemis;
    else if(id == 5) return eGodType::ares;
    else if(id == 6) return eGodType::aphrodite;
    else if(id == 7) return eGodType::hermes;
    else if(id == 8) return eGodType::athena;
    else if(id == 9) return eGodType::hephaestus;
    else if(id == 10) return eGodType::dionysus;
    else if(id == 11) return eGodType::hades;
    else if(id == 12) return eGodType::hera;
    else if(id == 13) return eGodType::atlas;
    printf("Invalid god id %i\n", id);
    valid = false;
    return eGodType::zeus;
}

eMonsterType pakIdToMonsterType(const uint8_t id, bool& valid) {
    valid = true;
    if(id == 0) return eMonsterType::hydra;
    else if(id == 1) return eMonsterType::kraken;
    else if(id == 2) return eMonsterType::cerberus;
    else if(id == 3) return eMonsterType::cyclops;
    else if(id == 4) return eMonsterType::minotaur;
    else if(id == 5) return eMonsterType::medusa;
    else if(id == 6) return eMonsterType::calydonianBoar;
    else if(id == 8) return eMonsterType::talos;
    else if(id == 9) return eMonsterType::hector;
    else if(id == 10) return eMonsterType::scylla;
    else if(id == 11) return eMonsterType::maenads;
    else if(id == 12) return eMonsterType::sphinx;
    else if(id == 13) return eMonsterType::chimera;
    else if(id == 14) return eMonsterType::harpies;
    else if(id == 15) return eMonsterType::echidna;
    printf("Invalid monster id %i\n", id);
    valid = false;
    return eMonsterType::calydonianBoar;
}

enum class eMonsterTarget {
    food,
    sea,
    industry,
    military,
    money,
    troops,
    common,
    aesthetic,
    mythological,
    best,
    random
};

eMonsterTarget pakIdToMonsterTarget(const uint16_t id) {
    if(id > 10) {
        printf("Invalid monster target id %i\n", id);
        return eMonsterTarget::food;
    }
    return static_cast<eMonsterTarget>(id);
}

void readEpisodeEvents(eEpisode& ep, ZeusFile& file,
                       const uint8_t nEvents, const eCityId cid,
                       const std::vector<ePakGod>& opponentGods,
                       const ePakMonster& independentMonster) {
    const auto version = file.version();
    const bool poseidon = version == eZeusFileVersion::poseidon_2_0;
    auto& events = ep.fEvents[cid];
    printf("%i events\n", nEvents);
    std::map<int, uint16_t> triggerMap;
    for(int i = 0; i < nEvents; i++) {
        const uint8_t eventTypeId = file.readUByte();
        if(eventTypeId == 0) continue;
        bool valid = false;
        const auto type = pakIdToEventType(eventTypeId, valid);
        if(!valid) {
            file.skipBytes(123);
            continue;
        }

        const uint8_t pMonths = file.readUByte();
        file.skipBytes(2);
        const uint16_t value1 = file.readUShort();
        const uint16_t value2 = file.readUShort();
        const uint16_t value3 = file.readUShort();
        file.skipBytes(2);
        const uint16_t value5 = file.readUShort();
        const uint16_t value6 = file.readUShort();
        const uint16_t value7 = file.readUShort();
        const uint16_t questId = file.readUShort();
        const uint16_t years0 = file.readUShort();
        const uint16_t years1 = file.readUShort();
        const uint16_t years2 = file.readUShort();
        file.skipBytes(2);
        const uint16_t cityId0 = file.readUShort();
        const uint16_t cityId1 = file.readUShort();
        const uint16_t cityId2 = file.readUShort();
        const uint16_t triggeredEventId = file.readUShort();
        file.skipBytes(2);
        const uint16_t occuranceType = file.readUShort();
        const uint16_t episodeComplete = file.readUShort(); // 2 when episode complete event, 0 otherwise
        const uint16_t duration = file.readUShort(); // also warning
        file.skipBytes(8);
        const uint16_t godMonsterHeroId = file.readUShort(); // also warships

        const uint16_t target1 = file.readUShort();
        const uint16_t target2 = file.readUShort();
        const uint16_t target3 = file.readUShort();

        const uint16_t aggressivnessId = file.readUShort();
        file.skipBytes(22);
        const uint16_t effectOnCityId = file.readShort();
        file.skipBytes(2);
        const uint16_t invCityId0 = file.readUShort();
        const uint16_t invCityId1 = file.readUShort();
        const uint16_t invCityId2 = file.readUShort();
        const uint16_t subType = file.readUShort();
        file.skipBytes(13);
        const uint16_t attackingCityId = file.readUShort();
        file.skipBytes(8);
        const uint16_t permanent = file.readUShort();
        file.skipBytes(3);

        auto& world = *ep.fWorldBoard;
        const auto attackingCityCid = static_cast<eCityId>(attackingCityId);
        const auto attackingCity = world.cityWithId(attackingCityCid);

        uint16_t amountMin;
        uint16_t amountMax;
        if(value5 == 0xFFFF) {
            amountMin = value6;
            amountMax = value7;
        } else {
            amountMin = value5;
            amountMax = value5;
        }
        uint16_t cityMin;
        uint16_t cityMax;
        if(cityId0 == 0xFFFF) {
            cityMin = cityId1;
            cityMax = cityId2;
        } else {
            cityMin = cityId0;
            cityMax = cityId0;
        }

        uint16_t invCityMin;
        uint16_t invCityMax;
        if(invCityId0 == 0xFFFF) {
            invCityMin = invCityId1;
            invCityMax = invCityId2;
        } else {
            invCityMin = invCityId0;
            invCityMax = invCityId0;
        }

        const auto setResources = [&](eResourceEventValue& ee) {
            if(value1 == 0xFFFF) {
                ee.setResourceType(0, eResourceType::none);
            } else {
                ee.setResourceType(0, ePakHelpers::pakResourceByteToType(value1, poseidon));
            }
            if(value2 == 0xFFFF) {
                ee.setResourceType(1, eResourceType::none);
            } else {
                ee.setResourceType(1, ePakHelpers::pakResourceByteToType(value2, poseidon));
            }
            if(value3 == 0xFFFF) {
                ee.setResourceType(2, eResourceType::none);
            } else {
                ee.setResourceType(2, ePakHelpers::pakResourceByteToType(value3, poseidon));
            }
        };

        const auto monsterIdToType = [&](const uint16_t id,
                                         bool& valid) {
            valid = true;
            if(id == 0) {
                const auto& o = opponentGods[0];
                if(o.fValid) {
                    const auto type = eMonster::sGodsMinion(o.fType);
                    return type;
                }
            } else if(id == 1) {
                const auto& o = opponentGods[1];
                if(o.fValid) {
                    const auto type = eMonster::sGodsMinion(o.fType);
                    return type;
                }
            } else if(id == 2) {
                if(independentMonster.fValid) {
                    return independentMonster.fType;
                }
            }
            valid = false;
            return eMonsterType::calydonianBoar;
        };

        stdsptr<eGameEvent> e;
        switch(type) {
        case ePakEventType::receiveRequest: {
            if(subType == 1 || subType == 2 || subType == 7) { // troops request
                const auto ee = e::make_shared<eTroopsRequestEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
                eTroopsRequestEventType type;
                if(subType == 1) {
                    type = eTroopsRequestEventType::cityUnderAttack;
                } else if(subType == 2) {
                    type = eTroopsRequestEventType::cityAttacksRival;
                } else if(subType == 7) {
                    type = eTroopsRequestEventType::greekCityTerrorized;
                } else {
                    printf("Invalid troops request type id %i\n", subType);
                    events.push_back(nullptr);
                    continue;
                }
                ee->setType(type);
                ee->setAttackingCity(attackingCity);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                ee->setWarningMonths(duration);
                eTroopsRequestEventEffect effect;
                if(effectOnCityId == 0) {
                    effect = eTroopsRequestEventEffect::unaffected;
                } else if(effectOnCityId == 1) {
                    effect = eTroopsRequestEventEffect::destroyed;
                } else if(effectOnCityId == 2) {
                    effect = eTroopsRequestEventEffect::conquered;
                } else {
                    printf("Invalid effect on city id %i\n", effectOnCityId);
                    continue;
                }
                ee->setEffect(effect);
                bool valid;
                const auto monster = monsterIdToType(godMonsterHeroId, valid);
                if(valid) ee->setMonster(monster);

                e = ee;
            } else {
                const auto type = pakIdToReceiveRequestType(subType);
                const auto ee = e::make_shared<eReceiveRequestEvent>(
                        cid, eGameEventBranch::root, *ep.fBoard);
                setResources(*ee);
                bool valid = false;
                const auto god = pakIdToGodType(godMonsterHeroId, valid);
                ee->setRequestType(type);
                ee->setGod(god);
                ee->setMinCount(amountMin);
                ee->setMaxCount(amountMax);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                ee->setWarningMonths(duration);
                e = ee;
            }
        } break;
        case ePakEventType::giftFrom: {
            const auto ee = e::make_shared<eGiftFromEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
            setResources(*ee);
            ee->setMinCount(amountMin);
            ee->setMaxCount(amountMax);
            ee->setMinCityId(cityMin);
            ee->setMaxCityId(cityMax);
            e = ee;
        } break;
        case ePakEventType::monsterInvasion: {
            std::vector<eMonsterType> types;
            const auto addType = [&](const uint16_t id) {
                bool valid;
                const auto type = monsterIdToType(id, valid);
                if(valid) types.push_back(type);
            };
            addType(value1);
            addType(value2);
            addType(value3);
            if(types.empty()) {
                printf("No monster types to choose from\n");
                events.push_back(nullptr);
                continue;
            }
            eMonsterAggressivness aggressivness{eMonsterAggressivness::passive};
            if(aggressivnessId > 3) {
                printf("Invalid monster aggressivness id %i\n", aggressivnessId);
            } else {
                aggressivness = static_cast<eMonsterAggressivness>(
                                    aggressivnessId);
            }

            eMonsterTarget targets[3];
            targets[0] = pakIdToMonsterTarget(target1);
            targets[1] = pakIdToMonsterTarget(target2);
            targets[2] = pakIdToMonsterTarget(target3);

            stdsptr<eMonsterInvasionEventBase> ee;
            if(subType == 0) { // monster in city
                ee = e::make_shared<eMonsterInCityEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
            } else if(subType == 1) { // monster unleashed
                ee = e::make_shared<eMonsterUnleashedEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
            } else if(subType == 2) { // monster invades
                ee = e::make_shared<eMonsterInvasionEvent>(
                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setWarningMonths(duration);
            } else {
                printf("Invalid monster invasion subtype %i\n", subType);
                events.push_back(nullptr);
                continue;
            }
            ee->setMonsterTypes(types);
            ee->setAggressivness(aggressivness);
            ee->setMinPointId(1);
            ee->setMaxPointId(3);
            e = ee;
        } break;
        case ePakEventType::godQuest: {
            bool valid = false;
            const auto god = pakIdToGodType(subType, valid);
            if(!valid) continue;
           eHeroType hero;
            if(godMonsterHeroId == 0) {
                const auto& o = opponentGods[0];
                if(o.fValid) {
                    const auto monster = eMonster::sGodsMinion(o.fType);
                    hero = eMonster::sSlayer(monster);
                } else {
                    printf("No hero with id %i\n", godMonsterHeroId);
                    events.push_back(nullptr);
                    continue;
                }
            } else if(godMonsterHeroId == 1) {
                const auto& o = opponentGods[1];
                if(o.fValid) {
                    const auto monster = eMonster::sGodsMinion(o.fType);
                    hero = eMonster::sSlayer(monster);
                } else {
                    printf("No hero with id %i\n", godMonsterHeroId);
                    events.push_back(nullptr);
                    continue;
                }
            } else if(godMonsterHeroId == 2) {
                if(independentMonster.fValid) {
                    const auto monster = independentMonster.fType;
                    hero = eMonster::sSlayer(monster);
                } else {
                    printf("No hero with id %i\n", godMonsterHeroId);
                    events.push_back(nullptr);
                    continue;
                }
            } else {
                printf("Hero id %i out of scope\n", godMonsterHeroId);
                events.push_back(nullptr);
                continue;
            }
            const auto ee = e::make_shared<eGodQuestEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setGod(god);
            ee->setHero(hero);
            ee->setId(questId == 1 ? eGodQuestId::godQuest2 :
                                     eGodQuestId::godQuest1);
            e = ee;
        } break;
        case ePakEventType::godAttack: {
            std::vector<eGodType> godTypes;
            bool valid1 = false;
            const auto god1 = pakIdToGodType(value1, valid1);
            if(valid1) godTypes.push_back(god1);
            bool valid2 = false;
            const auto god2 = pakIdToGodType(value2, valid2);
            if(valid2) godTypes.push_back(god2);
            bool valid3 = false;
            const auto god3 = pakIdToGodType(value3, valid3);
            if(valid3) godTypes.push_back(god3);
            if(godTypes.empty()) {
                printf("No gods to choose from\n");
                events.push_back(nullptr);
                continue;
            }

            const auto ee = e::make_shared<eGodAttackEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setTypes(godTypes);
            ee->setRandom(true);
            e = ee;
        } break;
        case ePakEventType::wageIncrease:
        case ePakEventType::wageDecrease: {
            const auto ee = e::make_shared<eWageChangeEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            if(type == ePakEventType::wageDecrease) {
                ee->setMinCount(-amountMax);
                ee->setMaxCount(-amountMin);
            } else {
                ee->setMinCount(amountMin);
                ee->setMaxCount(amountMax);
            }

            e = ee;
        } break;
        case ePakEventType::demandIncrease:
        case ePakEventType::demandDecrease: {
            const auto ee = e::make_shared<eDemandChangeEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            setResources(*ee);
            ee->setMinCityId(cityMin);
            ee->setMaxCityId(cityMax);
            const int by = type == ePakEventType::demandIncrease ? 12 : -12;
            ee->setMinCount(by);
            ee->setMaxCount(by);

            e = ee;
        } break;
        case ePakEventType::supplyIncrease:
        case ePakEventType::supplyDecrease: {
            const auto ee = e::make_shared<eSupplyChangeEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            setResources(*ee);
            ee->setMinCityId(cityMin);
            ee->setMaxCityId(cityMax);
            const int by = type == ePakEventType::supplyIncrease ? 12 : -12;
            ee->setMinCount(by);
            ee->setMaxCount(by);

            e = ee;
        } break;
        case ePakEventType::priceIncrease:
        case ePakEventType::priceDecrease: {
            const auto ee = e::make_shared<ePriceChangeEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            setResources(*ee);
            ee->setMinCount(type == ePakEventType::priceIncrease ? amountMin : -amountMax);
            ee->setMaxCount(type == ePakEventType::priceIncrease ? amountMax : -amountMin);

            e = ee;
        } break;
        case ePakEventType::cityStatusChange: {
            if(subType == 2) {
                const auto ee = e::make_shared<eTradeShutDownEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);

                e = ee;
            } else if(subType == 3) {
                const auto ee = e::make_shared<eTradeOpenUpEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);

                e = ee;
            } else if(subType == 9 || subType == 10 || subType == 11 ||
                      subType == 18 || subType == 19 ||
                      subType == 20 || subType == 21 ||
                      subType == 23 || subType == 24) {
                const auto ee = e::make_shared<eCityBecomesEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                eCityBecomesType type;
                if(subType == 9) {
                    type = eCityBecomesType::ally;
                } else if(subType == 10) {
                    type = eCityBecomesType::rival;
                } else if(subType == 11) {
                    type = eCityBecomesType::vassal;
                } else if(subType == 18) {
                    type = eCityBecomesType::active;
                } else if(subType == 19) {
                    type = eCityBecomesType::inactive;
                } else if(subType == 20) {
                    type = eCityBecomesType::visible;
                } else if(subType == 21) {
                    type = eCityBecomesType::invisible;
                } else if(subType == 23) {
                    type = eCityBecomesType::rebellionOver;
                } else { // if(subType == 24) {
                    type = eCityBecomesType::conquered;
                    ee->setAttackingCity(attackingCity);
                }
                ee->setType(type);

                e = ee;
            } else if(subType == 13) {
                bool valid = false;
                const auto godType = pakIdToGodType(godMonsterHeroId, valid);
                const auto ee = e::make_shared<eGodDisasterEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                ee->setGod(godType);
                ee->setDuration(30*duration);

                e = ee;
            } else if(subType == 14 || subType == 15) {
                const auto ee = e::make_shared<eMilitaryChangeEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                ee->setMinCount(amountMin);
                ee->setMaxCount(amountMax);

                e = ee;
            } else if(subType == 16 || subType == 17) {
                const auto ee = e::make_shared<eEconomicChangeEvent>(
                                    cid, eGameEventBranch::root, *ep.fBoard);
                ee->setMinCityId(cityMin);
                ee->setMaxCityId(cityMax);
                ee->setMinCount(amountMin);
                ee->setMaxCount(amountMax);

                e = ee;
            } else {
                printf("Unhandled trade change event type %i\n", subType);
                events.push_back(nullptr);
                continue;
            }
        } break;
        case ePakEventType::earthquake: {
            const auto ee = e::make_shared<eEarthquakeEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinPointId(cityMin);
            ee->setMaxPointId(cityMax);

            e = ee;
        } break;
        case ePakEventType::tidalWave: {
            const auto ee = e::make_shared<eTidalWaveEvent>(
                                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinPointId(cityMin);
            ee->setMaxPointId(cityMax);
            ee->setPermanent(permanent);

            e = ee;
        } break;
        case ePakEventType::lavaFlow: {
            const auto ee = e::make_shared<eLavaEvent>(
                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinPointId(cityMin);
            ee->setMaxPointId(cityMax);

            e = ee;
        } break;
        case ePakEventType::sinkLand: {
            const auto ee = e::make_shared<eSinkLandEvent>(
                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinCount(amountMin);
            ee->setMaxCount(amountMax);

            e = ee;
        } break;
        case ePakEventType::landSlide: {
            const auto ee = e::make_shared<eLandSlideEvent>(
                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinPointId(cityMin);
            ee->setMaxPointId(cityMax);

            e = ee;
        } break;
        case ePakEventType::invasion: {
            const auto ee = e::make_shared<eInvasionEvent>(
                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setWarningMonths(duration);

            ee->setMinCityId(invCityMin);
            ee->setMaxCityId(invCityMax);

            ee->setMinCount(amountMin);
            ee->setMaxCount(amountMax);

            ee->setMinPointId(cityMin);
            ee->setMaxPointId(cityMax);

            e = ee;
        } break;
        case ePakEventType::rivalArmyAway: {
            const auto ee = e::make_shared<eRivalArmyAwayEvent>(
                cid, eGameEventBranch::root, *ep.fBoard);
            ee->setMinCityId(cityMin);
            ee->setMaxCityId(cityMax);

            e = ee;
        } break;
        default:
            printf("Unhandled event type %i\n", eventTypeId);
            events.push_back(nullptr);
            continue;
        }

        if(occuranceType != 1) {
            e->setDatePlusDays(0);
            e->setDatePlusMonths(pMonths);
            if(years0 == 0xFFFF) {
                e->setDatePlusYearsMin(years1);
                e->setDatePlusYearsMax(years2);
            } else {
                e->setDatePlusYearsMin(years0);
                e->setDatePlusYearsMax(years0);
            }
        }

        if(occuranceType == 0 || occuranceType == 8192) { // one time event
            e->setRepeat(1);
            if(episodeComplete == 2) {
                e->setEpisodeCompleteEvent(true);
            }
        } else if(occuranceType == 1) { // triggered event
            if(years0 == 0xFFFF) {
                e->setPeriodMin(31*years1);
                e->setPeriodMax(31*years2);
            } else {
                const int period = 31*years0;
                e->setPeriodMin(period);
                e->setPeriodMax(period);
            }
            e->setRepeat(0);
        } else if(occuranceType == 2) { // recurring event
            if(years0 == 0xFFFF) {
                const int periodMin = years1 == 0 ? 365 : 365*years1;
                const int periodMax = years2 == 0 ? 365 : 365*years2;
                e->setPeriodMin(periodMin);
                e->setPeriodMax(periodMax);
            } else {
                const int period = years0 == 0 ? 365 : 365*years0;
                e->setPeriodMin(period);
                e->setPeriodMax(period);
            }
            e->setRepeat(99999);
        } else {
            printf("Invalid occurance type id %i\n", occuranceType);
            events.push_back(nullptr);
            continue;
        }
        triggerMap[i] = triggeredEventId;
        events.push_back(e);

        printf("%s\n", e->longDatedName().c_str());
    }
    printf("\n");

    for(const auto& m : triggerMap) {
        const uint16_t toId = m.second;
        if(toId >= (int)events.size()) continue;
        const auto to = events[toId];
        if(!to) continue;

        const int fromId = m.first;
        const auto from = events[fromId];
        if(!from) continue;

        if(from == to) continue;

        eEventTrigger* trigger = nullptr;
        if(const auto godQuest = dynamic_cast<eGodQuestEvent*>(from.get())) {
            trigger = &godQuest->fulfilledTrigger();
        } else if(const auto monsterEvent = dynamic_cast<eMonsterInvasionEventBase*>(from.get())) {
            trigger = &monsterEvent->killedTrigger();
        } else {
            trigger = &from->baseTrigger();
        }
        if(!trigger) continue;
        trigger->addEvent(to);
    }
}

eBuildingType pakIdToPyramidType(const uint8_t id) {
    if(id == 100) return eBuildingType::modestPyramid;
    else if(id == 101) return eBuildingType::pyramid;
    else if(id == 102) return eBuildingType::greatPyramid;
    else if(id == 103) return eBuildingType::majesticPyramid;

    else if(id == 104) return eBuildingType::smallMonumentToTheSky;
    else if(id == 105) return eBuildingType::monumentToTheSky;
    else if(id == 106) return eBuildingType::grandMonumentToTheSky;

    else if(id == 107) return eBuildingType::minorShrineAphrodite;
    else if(id == 108) return eBuildingType::shrineAphrodite;
    else if(id == 109) return eBuildingType::majorShrineAphrodite;

    else if(id == 110) return eBuildingType::pyramidOfThePantheon;
    else if(id == 111) return eBuildingType::altarOfOlympus;
    else if(id == 112) return eBuildingType::templeOfOlympus;
    else if(id == 113) return eBuildingType::observatoryKosmika;
    else if(id == 114) return eBuildingType::museumAtlantika;
    printf("Invalid pyramid type id %i\n", id);
    return eBuildingType::modestPyramid;
}

void readEpisodeAllowedPyramids(
        ZeusFile& file, std::vector<ePakPyramid>& pyramids) {
    const uint8_t n = file.readUByte();
    file.skipBytes(3);
    for(int i = 0; i < n; i++) {
        const uint8_t typeId = file.readUByte();
        std::vector<bool> levels;
        file.skipBytes(3);
        auto type = pakIdToPyramidType(typeId);
        if(type == eBuildingType::minorShrineAphrodite ||
           type == eBuildingType::shrineAphrodite ||
           type == eBuildingType::majorShrineAphrodite) {
            const uint8_t godId = file.readUByte();
            bool valid;
            const auto godType = pakIdToGodType(godId, valid);
            type = ePyramid::sSwitchGod(type, godType);
        } else {
            file.skipBytes(1);
        }
        file.skipBytes(7);
        pyramids.push_back({false, type, levels});
    }
}

void readEpisodeEnabledPyramids(
        ZeusFile& file, std::vector<ePakPyramid>& pyramids) {
    for(auto& p : pyramids) {
        const uint8_t id = file.readUByte();
        uint8_t levelsId;
        bool enabled = false;
        if(id >= 0xC0) {
            enabled = true;
            levelsId = id - 0xC0;
        } else if(id >= 0x40) {
            enabled = true;
            levelsId = id - 0x40;
        } else {
            levelsId = id;
        }
        const int levels = ePyramid::sLevels(p.fType);
        p.fLevels.clear();
        for(int i = 0; i < levels; i++) {
            const uint8_t pow = std::pow(2, i);
            p.fLevels.push_back(levelsId & pow);
        }
        p.fAllowed = p.fAllowed || enabled;
        file.skipBytes(11);
        if(p.fAllowed) {
            const auto name = eBuilding::sNameForBuilding(p.fType);
            printf("%s", name.c_str());
            for(const bool l : p.fLevels) {
                printf(l ? " b" : " w");
            }
            printf("\n");
        }
    }
    printf("\n");
}

void applyPyramidsToEpisode(const std::vector<ePakPyramid>& pyramids,
                            eEpisode& ep, const eCityId cid) {
    auto& a = ep.fAvailableBuildings[cid];
    for(const auto& p : pyramids) {
        if(!p.fAllowed) continue;
        a.allowPyramid(p.fType, p.fLevels);
    }
}

eEpisodeGoalType pakIdToEpisodeGoalType(const uint8_t id) {
    if(id == 0) return eEpisodeGoalType::population;
    else if(id == 1) return eEpisodeGoalType::treasury;
    else if(id == 2) return eEpisodeGoalType::sanctuary;
    else if(id == 3) return eEpisodeGoalType::support;
    else if(id == 4) return eEpisodeGoalType::quest;
    else if(id == 5) return eEpisodeGoalType::slay;
    else if(id == 6) return eEpisodeGoalType::yearlyProduction;
    else if(id == 7) return eEpisodeGoalType::rule;
    else if(id == 8) return eEpisodeGoalType::yearlyProfit;
    else if(id == 9) return eEpisodeGoalType::housing;
    else if(id == 10) return eEpisodeGoalType::tradingPartners;
    else if(id == 14) return eEpisodeGoalType::setAsideGoods;
    else if(id == 15) return eEpisodeGoalType::pyramid;
    else if(id == 16) return eEpisodeGoalType::hippodrome;
    printf("Invalid episode goal id %i\n", id);
    return eEpisodeGoalType::population;
}

eBannerType pakIdToBannerType(const uint16_t id) {
    if(id == 11) return eBannerType::rockThrower;
    else if(id == 12) return eBannerType::hoplite;
    else if(id == 13) return eBannerType::horseman;
    else if(id == 57) return eBannerType::trireme;
    printf("Invalid banner type id %i\n", id);
    return eBannerType::rockThrower;
}

void readEpisodeGoal(eEpisode& ep, ZeusFile& file, const eCityId cid) {
    const auto version = file.version();
    const bool poseidon = version == eZeusFileVersion::poseidon_2_0;
    const uint16_t typeId = file.readUShort();
    file.skipBytes(2);
    const uint16_t value1 = file.readUShort();
    file.skipBytes(2);
    const uint16_t value2 = file.readUShort();
    file.skipBytes(2);
    const uint16_t value3 = file.readUShort();
    file.skipBytes(2);
    const uint16_t value4 = file.readUShort();
    file.skipBytes(58);

    const auto goalType = pakIdToEpisodeGoalType(typeId);
    const auto goal = std::make_shared<eEpisodeGoal>();
    goal->fType = goalType;
    switch(goalType) {
    case eEpisodeGoalType::population:
    case eEpisodeGoalType::treasury:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::pyramid: {
        if(value2 == 0) {
            goal->fEnumInt1 = -1;
            goal->fRequiredCount = value4;
        } else {
            auto type = pakIdToPyramidType(value2);
            const bool isToGod = ePyramid::sIsToGod(type);
            if(isToGod) {
                bool valid;
                const auto god = pakIdToGodType(value1, valid);
                type = ePyramid::sSwitchGod(type, god);
            }
            goal->fEnumInt1 = static_cast<int>(type);
            goal->fRequiredCount = 100;
        }
    } break;
    case eEpisodeGoalType::hippodrome: {
        goal->fRequiredCount = value1;
    } break;
    case eEpisodeGoalType::sanctuary: {
        bool valid;
        const auto godType = pakIdToGodType(value1, valid);
        if(valid) {
            goal->fEnumInt1 = static_cast<int>(godType);
            goal->fRequiredCount = 100;
        } else {
            goal->fEnumInt1 = -1;
            goal->fRequiredCount = value3;
        }
    } break;
    case eEpisodeGoalType::support: {
        const auto btype = pakIdToBannerType(value1);
        goal->fEnumInt1 = static_cast<int>(btype);
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::quest: {
        const int eventId = value1;
        const auto& events = ep.fEvents[cid];
        eGodQuestEvent* event = nullptr;
        for(int i = eventId; i >= 0; i--) {
            if(i >= int(events.size())) continue;
            const auto& e = events[i];
            if(const auto ee = dynamic_cast<eGodQuestEvent*>(e.get())) {
                event = ee;
                break;
            }
        }
        if(!event) return;
        const auto god = event->god();
        const auto questId = event->id();
        goal->fEnumInt1 = static_cast<int>(god);
        goal->fEnumInt2 = static_cast<int>(questId);
        goal->fRequiredCount = 1;
    } break;
    case eEpisodeGoalType::slay: {
        const int eventId = value1;
        const auto& events = ep.fEvents[cid];
        eMonsterInvasionEventBase* event = nullptr;
        for(int i = eventId; i >= 0; i--) {
            if(i >= int(events.size())) continue;
            const auto& e = events[i];
            if(const auto ee = dynamic_cast<eMonsterInvasionEventBase*>(e.get())) {
                event = ee;
                break;
            }
        }
        if(!event) return;
        event->chooseMonster();
        const auto type = event->monsterType();
        goal->fEnumInt1 = static_cast<int>(type);
        goal->fRequiredCount = 1;
    } break;
    case eEpisodeGoalType::yearlyProduction: {
        goal->fRequiredCount = value2;
        const auto type = ePakHelpers::pakResourceByteToType(value1, poseidon);
        goal->fEnumInt1 = static_cast<int>(type);
    } break;
    case eEpisodeGoalType::rule:
        goal->fEnumInt1 = value1;
        goal->fRequiredCount = 1;
        break;
    case eEpisodeGoalType::yearlyProfit:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::housing: {
        if(value1 < 9) {
            goal->fEnumInt1 = 0;
            goal->fEnumInt2 = value1 - 2;
        } else {
            goal->fEnumInt1 = 1;
            goal->fEnumInt2 = value1 - 10;
        }
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::tradingPartners:
        goal->fRequiredCount = value1;
        break;
    case eEpisodeGoalType::setAsideGoods: {
        const auto type = ePakHelpers::pakResourceByteToType(value1, poseidon);
        goal->fEnumInt1 = static_cast<int>(type);
        goal->fRequiredCount = value2;
    } break;
    case eEpisodeGoalType::surviveUntil:
    case eEpisodeGoalType::completeBefore:
        break;
    }

    printf("%s\n", goal->text(false, false, *ep.fBoard).c_str());
    ep.fGoals.push_back(goal);
}

void readEpisodeText(eEpisode& ep, ZeusFile& file) {
    const uint16_t introId = file.readUShort();
    file.skipBytes(10);
    const uint16_t completeId = file.readUShort();
    if(introId != 0 && introId != 65535) {
        ep.fIntroId = introId;
        const auto intro = eLanguage::zeusMM(introId);
        ep.fTitle = intro.fTitle;
        ep.fIntroduction = intro.fContent;
        if(const auto cep = dynamic_cast<eColonyEpisode*>(&ep)) {
            cep->fSelection = intro.fSubtitle;
        }
    }
    if(completeId != 0 && completeId != 65535) {
        ep.fCompleteId = completeId;
        const auto complete = eLanguage::zeusMM(completeId);
        ep.fComplete = complete.fContent;
    }
}

uint16_t toUShort(const uint8_t b1, const uint8_t b2) {
    return (uint16_t)(b1 | ((b2) << 8));
}

void removeNullEvents(const eCityId cid, eEpisode& ep) {
    auto& events = ep.fEvents[cid];
    for(int i = 0; i < (int)events.size(); i++) {
        const auto& e = events[i];
        if(e) continue;
        events.erase(events.begin() + i);
        i--;
    }
}

void eCampaign::readPak(const std::string& title,
                        const std::string& path) {
    mIsPak = true;
    mPakFilename = eStringHelpers::pathToName(path);
    mName = mPakFilename.substr(0, mPakFilename.size() - 4);
    ZeusFile file(path);
    file.readVersion();
    const auto version = file.version();
    const bool poseidon = version == eZeusFileVersion::poseidon_2_0;
    const bool atlantean = file.isAtlantean();

    std::string versionStr;
    switch(version) {
    case eZeusFileVersion::zeus_1_0:
        versionStr = "1.0";
        break;
    case eZeusFileVersion::poseidon_2_0:
        versionStr = "2.0";
        break;
    }

    const auto atlanteanStr = file.isAtlantean() ? "atlantean" : "greek";

    printf("%s %s %s\n", title.c_str(), versionStr.c_str(), atlanteanStr);
    mParentBoard = e::make_shared<eGameBoard>(mWorldBoard);

    uint8_t bitmapId;
    if(poseidon) {
        file.seek(836249);
    } else {
        file.seek(835185);
    }
    bitmapId = file.readUByte();
    mBitmap = ePakHelpers::pakBitmapIdConvert(bitmapId);

    file.seek(0);
    file.getNumMaps();
    eCityId parentCid;
    file.loadBoard(*mParentBoard, *this, parentCid);

    file.seek(8);
    const uint8_t nParentEps = file.readUByte();
    // file.seek(12); // and 16?
    // const uint8_t nColonyEps = file.readUByte();

    file.seek(35648);
    const uint16_t briefId = file.readUShort();
    if(briefId != 0 && briefId != 65535) {
        mBriefId = briefId;
        const auto brief = eLanguage::zeusMM(briefId);
        mTitle = brief.fTitle;
        mIntroduction = brief.fContent;
    }

    file.seek(35652);
    const uint16_t completeId = file.readUShort();
    if(completeId != 0 && completeId != 65535) {
        mCompleteId = completeId;
        mComplete = eLanguage::zeusMM(completeId).fContent;
    }

    std::vector<ePakGod> friendlyGods;
    friendlyGods.resize(6);
    std::vector<ePakGod> opponentGods;
    opponentGods.resize(4);
    std::vector<ePakPyramid> pyramids;
    ePakMonster independentMonster;

    for(int i = 0; i < nParentEps; i++) {
        printf("parent episode %i:\n\n", i);

        const auto ep = addParentCityEpisode();
        file.seek(104 + i*356);
        const auto nextByte = file.readUByte();
        const bool nextColony = nextByte == 1;
        ep->fNextEpisode = nextColony ? eEpisodeType::colony :
                                        eEpisodeType::parentCity;
        file.seek(8174 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, parentCid);
        file.seek(8752 + i*2032);
        readEpisodeText(*ep, file);
        file.seek(9100 + i*2032);
        readEpisodeResources(*ep, file, parentCid);

        const int epInc = poseidon ? 300 : 224;

        if(i == 0) {
            file.seek(35736 + i*epInc);
            for(int i = 0; i < 4; i++) {
                auto& v = opponentGods[i];
                const uint8_t godId = file.readUByte();
                v.fType = pakIdToGodType(godId, v.fValid);
                file.skipBytes(3);
                if(v.fValid) {
                    printf("Opponent % i %s\n", i, eGod::sGodName(v.fType).c_str());
                } else {
                    printf("No opponent god %i\n", i);
                }
            }
            printf("\n");
            file.seek(35784 + i*epInc);
            for(int i = 0; i < 6; i++) {
                auto& v = friendlyGods[i];
                const uint8_t godId = file.readUByte();
                v.fType = pakIdToGodType(godId, v.fValid);
                file.skipBytes(3);
                if(v.fValid) {
                    printf("Friendly % i %s\n", i, eGod::sGodName(v.fType).c_str());
                } else {
                    printf("No friendly god %i\n", i);
                }
            }
            printf("\n");
            file.seek(35832 + i*epInc);
            const uint8_t monsterId = file.readUByte();
            independentMonster.fType = pakIdToMonsterType(monsterId, independentMonster.fValid);
            if(independentMonster.fValid) {
                printf("%s\n\n", eMonster::sMonsterName(independentMonster.fType).c_str());
            } else {
                printf("No independent monster\n\n");
            }

            if(atlantean) {
                file.seek(35960 + i*epInc);
                readEpisodeAllowedPyramids(file, pyramids);
            }
        }

        file.seek(35944 + i*epInc);
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, parentCid);
        if(atlantean) {
            file.seek(35972 + i*epInc);
            readEpisodeEnabledPyramids(file, pyramids);
            applyPyramidsToEpisode(pyramids, *ep, parentCid);
        }

        if(poseidon) {
            file.seek(800361 + i*4);
        } else {
            file.seek(799297 + i*4);
        }
        const uint8_t nEvents = file.readUByte();
        if(poseidon) {
            file.seek(39938 + i*18600);
        } else {
            file.seek(38874 + i*18600);
        }
        readEpisodeEvents(*ep, file, nEvents, parentCid,
                          opponentGods, independentMonster);

        if(poseidon) {
            file.seek(838331 + i*4);
        } else {
            file.seek(837263 + i*4);
        }
        const uint8_t nGoals = file.readUByte();
        if(poseidon) {
            file.seek(838371 + i*456);
        } else {
            file.seek(837303 + i*456);
        }
        for(int j = 0; j < nGoals; j++) {
            readEpisodeGoal(*ep, file, parentCid);
        }
        printf("\n");
        removeNullEvents(parentCid, *ep);
    }

    file.seek(7140);
    const int16_t startDate = file.readShort();
    file.seek(7172);
    const uint16_t initialFunds = file.readUShort();

    setInitialFunds(ePlayerId::player0, initialFunds);

    setDate(eDate{1, eMonth::january, startDate});

    for(int i = 0; i < 4; i++) {
        printf("colony episode %i:\n\n", i);
        auto& board = mColonyBoards.emplace_back();
        board = e::make_shared<eGameBoard>(mWorldBoard);

        const auto ep = std::make_shared<eColonyEpisode>();
        ep->fBoard = board.get();
        ep->fWorldBoard = &mWorldBoard;
        mColonyEpisodes.push_back(ep);

        eCityId colonyCid;
        const bool r = file.loadBoard(*board, *this, colonyCid);
        if(!r) {
            board->initialize(1, 1);
            continue;
        }

        ep->fCity = mWorldBoard.cityWithId(colonyCid);

        file.seek(28494 + i*2032); // mint
        readEpisodeAllowedBuildings(*ep, file, atlantean, colonyCid);
        file.seek(29072 + i*2032);
        readEpisodeText(*ep, file);
        file.seek(29420 + i*2032);
        readEpisodeResources(*ep, file, colonyCid);

        const int epInc = poseidon ? 300 : 224;

        if(poseidon) {
            file.seek(38944 + i*epInc);
        } else {
            file.seek(38184 + i*epInc);
        }
        readEpisodeAllowedSanctuaries(*ep, file, friendlyGods, colonyCid);

        if(atlantean) {
            std::vector<ePakPyramid> pyramids;
            file.seek(38960 + i*epInc);
            readEpisodeAllowedPyramids(file, pyramids);
            file.seek(38972 + i*epInc);
            readEpisodeEnabledPyramids(file, pyramids);
            applyPyramidsToEpisode(pyramids, *ep, colonyCid);
        }

        if(poseidon) {
            file.seek(800401 + i*4);
        } else {
            file.seek(799337 + i*4);
        }
        const uint8_t nEvents = file.readUByte();
        if(poseidon) {
            file.seek(225938 + i*18600);
        } else {
            file.seek(224874 + i*18600);
        }
        readEpisodeEvents(*ep, file, nEvents, colonyCid,
                          opponentGods, independentMonster);

        if(poseidon) {
            file.seek(836491 + i*4);
        } else {
            file.seek(835423 + i*4);
        }
        const uint8_t nGoals = file.readUByte();
        if(poseidon) {
            file.seek(836507 + i*456);
        } else {
            file.seek(835439 + i*456);
        }
        for(int j = 0; j < nGoals; j++) {
            readEpisodeGoal(*ep, file, colonyCid);
        }
        printf("\n");
        removeNullEvents(colonyCid, *ep);
    }

    loadStrings();
    loadNumbers();
}
