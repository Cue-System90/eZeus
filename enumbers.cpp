#include "enumbers.h"
#include "efspath.h"

#include "egamedir.h"
#include "eloadtexthelper.h"

#include <fstream>
#include <iostream>

int eNumbers::sDayLength;

double eNumbers::sEliteHousingTaxMultiplier;
double eNumbers::sCommonHousingTaxMulitplier;

double eNumbers::sWageMultiplier;

int eNumbers::sReinforcementsTravelTime;
int eNumbers::sArmyTravelTime;
int eNumbers::sAIInvasionMonthsBreak;
int eNumbers::sInvasionAppearAtPlaces;
double eNumbers::sArmyStrengthHorsemanMult;

int eNumbers::sRabbleDivisor;
int eNumbers::sSoldiersPerBanner;
int eNumbers::sSoldiersPerAresArtemisBanner;

int eNumbers::sEarthquakeProgressPeriod;

int eNumbers::sBoarMaxCount;
int eNumbers::sBoarSpawnPeriod;
int eNumbers::sDeerMaxCount;
int eNumbers::sDeerSpawnPeriod;
int eNumbers::sWolfMaxCount;
int eNumbers::sWolfSpawnPeriod;
int eNumbers::sWolfHuntWait;
int eNumbers::sWolfHuntDistance;

int eNumbers::sOlivePressProcessingPeriod;
int eNumbers::sWineryProcessingPeriod;
int eNumbers::sArmoryProcessingPeriod;
int eNumbers::sSculptureStudioProcessingPeriod;

int eNumbers::sFarmRipePeriod;

int eNumbers::sShepherdGoatherdMaxDistance;
int eNumbers::sShepherdGoatherdMaxGroom;
int eNumbers::sShepherdGoatherdGroomTime;
int eNumbers::sShepherdGoatherdCollectTime;
int eNumbers::sShepherdGoatherdWaitTime;

int eNumbers::sSheepMaxGroom;
int eNumbers::sGoatMaxGroom;
int eNumbers::sAnimalMoveRange;

int eNumbers::sCorralProcessingPeriod;
int eNumbers::sCorralTakePeriod;
int eNumbers::sCorralKillPeriod;
int eNumbers::sCorralReplacePeriod;
int eNumbers::sCattleMaturePeriod;

int eNumbers::sUrchinQuayUnpackTime;
int eNumbers::sFisheryBoatBuildTime;
int eNumbers::sFisheryUnpackTime;

int eNumbers::sHuntingLodgeWaitPeriod;

int eNumbers::sMintDrachmasPerSilver;

int eNumbers::sSpreadFirePeriod;
int eNumbers::sFireCollapsePeriod;
int eNumbers::sRuinsFireEndPeriod;
int eNumbers::sMaintenanceDecrementPeriod;
double eNumbers::sFireRiskPeriodMultiplier;
double eNumbers::sFireRiskPeriodBaseIncrement;
double eNumbers::sFireRiskPeriodExponent;
double eNumbers::sCollapseRiskPeriodMultiplier;
double eNumbers::sCollapseRiskPeriodBaseIncrement;
double eNumbers::sCollapseRiskPeriodExponent;

int eNumbers::sHouseCultureDecrementPeriod;
int eNumbers::sHouseWaterDecrementPeriod;
int eNumbers::sHouseHygieneDecrementPeriod;
double eNumbers::sHouseHealPlaguePeriodMultiplier;
double eNumbers::sHouseHealPlaguePeriodBaseMultiplier;
double eNumbers::sHouseHealPlaguePeriodExponent;
double eNumbers::sHousePlagueRiskPeriodMultiplier;
double eNumbers::sHousePlagueRiskPeriodBaseIncrement;
double eNumbers::sHousePlagueRiskPeriodExponent;
int eNumbers::sHouseDisgruntledRemovePeriod;
double eNumbers::sHouseDisgruntledRiskPeriodMultiplier;
double eNumbers::sHouseDisgruntledRiskPeriodBaseIncrement;
double eNumbers::sHouseDisgruntledRiskPeriodExponent;
double eNumbers::sHouseLeaveRiskPeriodMultiplier;
double eNumbers::sHouseLeaveRiskPeriodBaseIncrement;
double eNumbers::sHouseLeaveRiskPeriodExponent;
int eNumbers::sHouseSatisfactionUpdatePeriod;
int eNumbers::sHouseDisgruntledSpawnPeriod;
int eNumbers::sHouseSickSpawnPeriod;

int eNumbers::sHerosHallArrivalPeriod;
int eNumbers::sHerosHallSpawnPeriod;

int eNumbers::sHorseRanchWheatUsePeriod;
int eNumbers::sHorseRanchHorseSpawnPeriod;

int eNumbers::sPatrolerMaxDistance;
int eNumbers::sPatrolerWaitTime;
int eNumbers::sPatrolerSourceSpawnPeriod;

int eNumbers::sArtisanWaitTime;
int eNumbers::sArtisanBuildTime;

int eNumbers::sTowerSpawnPeriod;

int eNumbers::sTraderSpawnPeriod;
int eNumbers::sTwoWayTradeMax;

int eNumbers::sChariotBuildingTime;

int eNumbers::sOliveTreeRipePeriod;
int eNumbers::sVineRipePeriod;
int eNumbers::sOrangeTreeRipePeriod;

double eNumbers::sTreeVineFullyRipePeriodMultiplier;

double eNumbers::sTreeVineBlessedFullyRipePeriodMultiplier;
double eNumbers::sTreeVineBlessedRipePeriodMultiplier;
double eNumbers::sTreeVineCursedFullyRipePeriodMultiplier;
double eNumbers::sTreeVineCursedRipePeriodMultiplier;

int eNumbers::sMarbleCollectTime;
int eNumbers::sBronzeCollectTime;
int eNumbers::sSilverCollectTime;
int eNumbers::sWoodCollectTime;
int eNumbers::sFishCollectTime;
int eNumbers::sUrchinCollectTime;

int eNumbers::sGrowerMaxDistance;
int eNumbers::sGrowerMaxGroom;
int eNumbers::sGrowerWorkTime;
int eNumbers::sGrowerSpawnWaitTime;

int eNumbers::sMonsterAttackRange;

int eNumbers::sPassiveMonsterAttackPeriod;
int eNumbers::sActiveMonsterAttackPeriod;
int eNumbers::sVeryActiveMonsterAttackPeriod;
int eNumbers::sAggressiveMonsterAttackPeriod;

int eNumbers::sPassiveLandMonsterInvadePeriod;
int eNumbers::sActiveLandMonsterInvadePeriod;
int eNumbers::sVeryActiveLandMonsterInvadePeriod;
int eNumbers::sAggressiveLandMonsterInvadePeriod;

int eNumbers::sPassiveWaterMonsterInvadePeriod;
int eNumbers::sActiveWaterMonsterInvadePeriod;
int eNumbers::sVeryActiveWaterMonsterInvadePeriod;
int eNumbers::sAggressiveWaterMonsterInvadePeriod;

int eNumbers::sPassiveLandMonsterMoveAroundPeriod;
int eNumbers::sActiveLandMonsterMoveAroundPeriod;
int eNumbers::sVeryActiveLandMonsterMoveAroundPeriod;
int eNumbers::sAggressiveLandMonsterMoveAroundPeriod;

int eNumbers::sPassiveWaterMonsterMoveAroundPeriod;
int eNumbers::sActiveWaterMonsterMoveAroundPeriod;
int eNumbers::sVeryActiveWaterMonsterMoveAroundPeriod;
int eNumbers::sAggressiveWaterMonsterMoveAroundPeriod;

int eNumbers::sFriendlyGodVisitPeriod;
int eNumbers::sGodHelpPeriod;
int eNumbers::sGodHelpAttackPeriod;
int eNumbers::sGodHelpAttackPlayerPeriod;

int eNumbers::sGodAttackCursePeriod;
int eNumbers::sGodAttackCurseRange;
int eNumbers::sGodAttackAttackPeriod;
int eNumbers::sGodAttackAttackRange;
int eNumbers::sGodAttackAggressiveAttackPeriod;
int eNumbers::sGodAttackTargetedCursePeriod;
int eNumbers::sGodAttackTargetedCurseRange;
int eNumbers::sGodAttackTargetedAttackPeriod;
int eNumbers::sGodAttackTargetedAttackRange;
int eNumbers::sGodAttackGodFightRange;
int eNumbers::sGodAttackApolloPlaguePeriod;
int eNumbers::sGodAttackApolloPlagueRange;
int eNumbers::sGodAttackAphroditeEvictPeriod;
int eNumbers::sGodAttackAphroditeEvictRange;

int eNumbers::sGodVisitSoldierAttackPeriod;
int eNumbers::sGodVisitSoldierAttackRange;
int eNumbers::sGodVisitBlessPeriod;
int eNumbers::sGodVisitBlessRange;
int eNumbers::sGodVisitPatrolDistance;
int eNumbers::sGodVisitMoveAroundTime;

int eNumbers::sGodWorshippedSoldierAttackPeriod;
int eNumbers::sGodWorshippedSoldierAttackRange;
int eNumbers::sGodWorshippedBlessPeriod;
int eNumbers::sGodWorshippedBlessRange;

int eNumbers::sSoldierBeingAttackedCallRange;

int eNumbers::sTowerHP;
int eNumbers::sTowerRange;
double eNumbers::sTowerAttack;

int eNumbers::sWallHP;

int eNumbers::sWallArcherRange;
double eNumbers::sWallArcherAttack;

int eNumbers::sRabbleHP;
int eNumbers::sRabbleRange;
double eNumbers::sRabbleAttack;

int eNumbers::sSpearthrowerHP;
int eNumbers::sSpearthrowerRange;
double eNumbers::sSpearthrowerAttack;

int eNumbers::sArcherHP;
int eNumbers::sArcherRange;
double eNumbers::sArcherAttack;

int eNumbers::sHopliteHP;
double eNumbers::sHopliteAttack;

int eNumbers::sChariotHP;
double eNumbers::sChariotSpeed;
double eNumbers::sChariotAttack;

int eNumbers::sHorsemanHP;
double eNumbers::sHorsemanSpeed;
double eNumbers::sHorsemanAttack;

int eNumbers::sWatchmanHP;
double eNumbers::sWatchmanAttack;
int eNumbers::sWatchmanSatisfactionProvide;

int eNumbers::sDefendCityMaxKilled;
int eNumbers::sDefendCityTalosMaxKilled;

int eNumbers::sAttackCityMaxKilled;
int eNumbers::sAttackCityAresMaxKilled;

int eNumbers::sBasicVendorMaxResourceTakeDistance;
int eNumbers::sHorseVendorMaxResourceTakeDistance;
int eNumbers::sResourceBuildingMaxResourceGiveDistance;
int eNumbers::sProcessingBuildingMaxResourceTakeDistance;
int eNumbers::sTriremeWharfMaxResourceTakeDistance;

int eNumbers::sTriremeWharfBuildTime;
int eNumbers::sTriremeWharfBuildStages;

void eNumbers::sLoad() {
    const auto path = eGameDir::numbersPath();
    sLoad(path);
}

void eNumbers::sLoad(const std::string& path) {
    std::map<std::string, std::string> map;
    eLoadTextHelper::load(path, map);

    const bool write = map.empty();

    std::ofstream file;
    if(write) file.open(eFsPath::sPath(path));

    const auto loadI = [&](const std::string& name,
                           int& val, const int def,
                           const int min, const int max) {
        if(write) {
            file << name << " \"" << def << "\"\n";
            val = def;
            return;
        }
        const auto it = map.find(name);
        if(it == map.end()) {
            printf("Could not find '%s' in numbers.txt. Using default value %d\n", name.c_str(), def);
            val = def;
            return;
        }
        const auto iStr = it->second;
        try {
            val = std::stoi(iStr);
            if(val > max) {
                printf("Value of '%s' in numbers.txt is too big. Using max value %d\n", name.c_str(), max);
                val = max;
            } else if(val < min) {
                printf("Value of '%s' in numbers.txt is too small. Using min value %d\n", name.c_str(), min);
                val = min;
            }
        } catch(...) {
            printf("Invalid '%s' value '%s' in numbers.txt. Using default value %d\n", name.c_str(), iStr.c_str(), def);
            val = def;
            return;
        }
    };

    const auto loadD = [&](const std::string& name,
                           double& val, const double def,
                           const double min, const double max) {
        if(write) {
            file << name << " \"" << def << "\"\n";
            val = def;
            return;
        }
        const auto it = map.find(name);
        if(it == map.end()) {
            printf("Could not find '%s' in numbers.txt. Using default value %f\n", name.c_str(), def);
            val = def;
            return;
        }
        const auto iStr = it->second;
        try {
            val = std::stod(iStr);
            if(val > max) {
                printf("Value of '%s' in numbers.txt is too big. Using max value %f\n", name.c_str(), max);
                val = max;
            } else if(val < min) {
                printf("Value of '%s' in numbers.txt is too small. Using min value %f\n", name.c_str(), min);
                val = min;
            }
        } catch(...) {
            printf("Invalid '%s' value '%s' in numbers.txt. Using default value %f\n", name.c_str(), iStr.c_str(), def);
            val = def;
            return;
        }
    };
    loadI("day_length_i", sDayLength, 350, 1, 10000);

    loadD("elite_housing_tax_mult_d", sEliteHousingTaxMultiplier, 1, 0, 10);
    loadD("common_housing_tax_mult_d", sCommonHousingTaxMulitplier, 1, 0, 10);
    loadD("wage_mult_d", sWageMultiplier, 1, 0, 10);

    loadI("reinforcements_travel_time_i", sReinforcementsTravelTime, 50, 1, 1000);
    loadI("army_travel_time_i", sArmyTravelTime, 200, 1, 1000);
    loadI("ai_invasion_months_break_i", sAIInvasionMonthsBreak, 30, 0, 1000);
    loadI("invasion_appear_at_places_b", sInvasionAppearAtPlaces, 0, 0, 1);
    loadD("army_strength_horseman_mult_d", sArmyStrengthHorsemanMult, 1.5, 0.1, 100.);

    loadI("rabble_divisor_i", sRabbleDivisor, 6, 1, 100);
    loadI("soldiers_per_banner_i", sSoldiersPerBanner, 8, 2, 100);
    loadI("soldiers_per_ares_artemis_banner_i", sSoldiersPerAresArtemisBanner, 8, 2, 100);

    loadI("earthquake_progress_period_i", sEarthquakeProgressPeriod, 500, 1, 10000);

    loadI("boar_max_count_i", sBoarMaxCount, 8, 2, 100);
    loadI("boar_spawn_period_i", sBoarSpawnPeriod, 4000, 1, 100000);
    loadI("deer_max_count_i", sDeerMaxCount, 8, 2, 100);
    loadI("deer_spawn_period_i", sDeerSpawnPeriod, 4000, 1, 100000);
    loadI("wolf_max_count_i", sWolfMaxCount, 8, 2, 100);
    loadI("wolf_spawn_period_i", sWolfSpawnPeriod, 4000, 1, 100000);
    loadI("wolf_hunt_wait_i", sWolfHuntWait, 20, 1, 1000);
    loadI("wolf_hunt_distance_i", sWolfHuntDistance, 60, 1, 1000);

    loadI("olive_press_processing_period_i", sOlivePressProcessingPeriod, 20000, 1, 1000000);
    loadI("winery_processing_period_i", sWineryProcessingPeriod, 20000, 1, 1000000);
    loadI("armory_processing_period_i", sArmoryProcessingPeriod, 40000, 1, 1000000);
    loadI("sculpture_studio_processing_period_i", sSculptureStudioProcessingPeriod, 100000, 1, 1000000);

    loadI("farm_ripe_period_i", sFarmRipePeriod, 15000, 1, 1000000);

    loadI("shepherd_goatherd_max_distance_i", sShepherdGoatherdMaxDistance, 40, 1, 1000);
    loadI("shepherd_goatherd_max_groom_i", sShepherdGoatherdMaxGroom, 20, 1, 1000);
    loadI("shepherd_goatherd_groom_time_i", sShepherdGoatherdGroomTime, 1000, 1, 10000);
    loadI("shepherd_goatherd_collect_time_i", sShepherdGoatherdCollectTime, 2000, 1, 10000);
    loadI("shepherd_goatherd_wait_time_i", sShepherdGoatherdWaitTime, 2000, 1, 100000);

    loadI("sheep_max_groom_i", sSheepMaxGroom, 15, 1, 1000);
    loadI("goat_max_groom_i", sGoatMaxGroom, 10, 1, 1000);
    loadI("animal_move_range_i", sAnimalMoveRange, 5, 2, 100);

    loadI("corral_processing_period_i", sCorralProcessingPeriod, 10000, 1, 1000000);
    loadI("corral_take_period_i", sCorralTakePeriod, 10000, 1, 1000000);
    loadI("corral_kill_period_i", sCorralKillPeriod, 10000, 1, 1000000);
    loadI("corral_replace_period_i", sCorralReplacePeriod, 10000, 1, 1000000);
    loadI("cattle_mature_period_i", sCattleMaturePeriod, 25000, 1, 1000000);

    loadI("urchin_quay_unpack_time_i", sUrchinQuayUnpackTime, 10000, 1, 1000000);
    loadI("fishery_boat_build_time_i", sFisheryBoatBuildTime, 40000, 1, 1000000);
    loadI("fishery_unpack_time_i", sFisheryUnpackTime, 10000, 1, 1000000);

    loadI("hunting_lodge_wait_period_i", sHuntingLodgeWaitPeriod, 5000, 1, 1000000);

    loadI("mint_drachmas_per_silver_i", sMintDrachmasPerSilver, 50, 0, 1000);

    loadI("spread_fire_period_i", sSpreadFirePeriod, 50000, 1, 1000000);
    loadI("fire_collapse_period_i", sFireCollapsePeriod, 50000, 1, 1000000);
    loadI("ruins_fire_end_period_i", sRuinsFireEndPeriod, 10000, 1, 1000000);
    loadI("maintenance_decrement_period_i", sMaintenanceDecrementPeriod, 2000, 1, 1000000);
    loadD("fire_risk_period_multiplier_d", sFireRiskPeriodMultiplier, 10, 1, 100);
    loadD("fire_risk_period_base_increment_d", sFireRiskPeriodBaseIncrement, 10, 1, 1000);
    loadD("fire_risk_period_exponent_d", sFireRiskPeriodExponent, 4, 1, 9);
    loadD("collapse_risk_period_multiplier_d", sCollapseRiskPeriodMultiplier, 10, 1, 100);
    loadD("collapse_risk_period_base_increment_d", sCollapseRiskPeriodBaseIncrement, 10, 1, 1000);
    loadD("collapse_risk_period_exponent_d", sCollapseRiskPeriodExponent, 4, 1, 9);

    loadI("house_culture_decrement_period_i", sHouseCultureDecrementPeriod, 20000, 1, 1000000);
    loadI("house_water_decrement_period_i", sHouseWaterDecrementPeriod, 7500, 1, 1000000);
    loadI("house_hygiene_decrement_period_i", sHouseHygieneDecrementPeriod, 25000, 1, 1000000);
    loadD("house_heal_plague_period_multiplier_d", sHouseHealPlaguePeriodMultiplier, 10, 1, 100);
    loadD("house_heal_plague_period_base_multiplier_d", sHouseHealPlaguePeriodBaseMultiplier, 1000, 1, 100000);
    loadD("house_heal_plague_period_exponent_d", sHouseHealPlaguePeriodExponent, 4, 1, 9);
    loadD("house_plague_risk_period_multiplier_d", sHousePlagueRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_plague_risk_period_base_increment_d", sHousePlagueRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_plague_risk_period_exponent_d", sHousePlagueRiskPeriodExponent, 4, 1, 9);
    loadI("house_disgruntled_remove_period_i", sHouseDisgruntledRemovePeriod, 500000, 1, 1000000);
    loadD("house_disgruntled_risk_period_multiplier_d", sHouseDisgruntledRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_disgruntled_risk_period_base_increment_d", sHouseDisgruntledRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_disgruntled_risk_period_exponent_d", sHouseDisgruntledRiskPeriodExponent, 3, 1, 9);
    loadD("house_leave_risk_period_multiplier_d", sHouseLeaveRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_leave_risk_period_base_increment_d", sHouseLeaveRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_leave_risk_period_exponent_d", sHouseLeaveRiskPeriodExponent, 3, 1, 9);
    loadI("house_satisfaction_update_period_i", sHouseSatisfactionUpdatePeriod, 7500, 1, 1000000);
    loadI("house_disgruntled_spawn_period_i", sHouseDisgruntledSpawnPeriod, 10000, 1, 1000000);
    loadI("house_sick_spawn_period_i", sHouseSickSpawnPeriod, 10000, 1, 1000000);

    loadI("heros_hall_arrival_period_i", sHerosHallArrivalPeriod, 15000, 1, 1000000);
    loadI("heros_hall_spawn_period_i", sHerosHallSpawnPeriod, 5000, 1, 1000000);

    loadI("horse_ranch_wheat_use_period_i", sHorseRanchWheatUsePeriod, 1000, 1, 1000000);
    loadI("horse_ranch_horse_spawn_period_i", sHorseRanchHorseSpawnPeriod, 40000, 1, 1000000);

    loadI("patroler_max_distance_i", sPatrolerMaxDistance, 40, 5, 1000);
    loadI("patroler_wait_time_i", sPatrolerWaitTime, 5000, 1, 1000000);
    loadI("patroler_source_spawn_period_i", sPatrolerSourceSpawnPeriod, 10000, 1, 1000000);

    loadI("artisan_wait_time_i", sArtisanWaitTime, 2000, 1, 1000000);
    loadI("artisan_build_time_i", sArtisanBuildTime, 6000, 1, 1000000);

    loadI("tower_spawn_period_i", sTowerSpawnPeriod, 5000, 1, 1000000);

    loadI("trader_spawn_period_i", sTraderSpawnPeriod, 30000, 1, 1000000);
    loadI("two_way_trade_max_i", sTwoWayTradeMax, 4, 0, 100);

    loadI("chariot_building_time_i", sChariotBuildingTime, 20000, 1, 1000000);

    loadI("olive_tree_ripe_period_i", sOliveTreeRipePeriod, 40000, 1, 1000000);
    loadI("vine_ripe_period_i", sVineRipePeriod, 40000, 1, 1000000);
    loadI("orange_tree_ripe_period_i", sOrangeTreeRipePeriod, 40000, 1, 1000000);

    loadD("tree_vine_fully_ripe_period_multiplier_d", sTreeVineFullyRipePeriodMultiplier, 2., 1, 9);

    loadD("tree_vine_blessed_fully_ripe_period_multiplier_d", sTreeVineBlessedFullyRipePeriodMultiplier, 3., 1, 9);
    loadD("tree_vine_blessed_ripe_period_multiplier_d", sTreeVineBlessedRipePeriodMultiplier, 0.333, 0.1, 1.);
    loadD("tree_vine_cursed_fully_ripe_period_multiplier_d", sTreeVineCursedFullyRipePeriodMultiplier, 0.333, 0.1, 1.);
    loadD("tree_vine_cursed_ripe_period_multiplier_d", sTreeVineCursedRipePeriodMultiplier, 3., 1, 9);

    loadI("marble_collect_period_i", sMarbleCollectTime, 10000, 1, 1000000);
    loadI("bronze_collect_period_i", sBronzeCollectTime, 10000, 1, 1000000);
    loadI("silver_collect_period_i", sSilverCollectTime, 10000, 1, 1000000);
    loadI("wood_collect_period_i", sWoodCollectTime, 10000, 1, 1000000);
    loadI("fish_collect_period_i", sFishCollectTime, 10000, 1, 1000000);
    loadI("urchin_collect_period_i", sUrchinCollectTime, 10000, 1, 1000000);

    loadI("grower_max_distance_i", sGrowerMaxDistance, 40, 5, 1000);
    loadI("grower_max_groom_i", sGrowerMaxGroom, 5, 1, 1000);
    loadI("grower_work_time_i", sGrowerWorkTime, 2000, 1, 1000000);
    loadI("grower_spawn_wait_time_i", sGrowerSpawnWaitTime, 5000, 1, 1000000);

    loadI("monster_attack_range_i", sMonsterAttackRange, 10, 1, 20);
    loadI("passive_monster_attack_period_i", sPassiveMonsterAttackPeriod, 14000, 1, 1000000);
    loadI("active_monster_attack_period_i", sActiveMonsterAttackPeriod, 12000, 1, 1000000);
    loadI("very_active_monster_attack_period_i", sVeryActiveMonsterAttackPeriod, 10000, 1, 1000000);
    loadI("aggressive_monster_attack_period_i", sAggressiveMonsterAttackPeriod, 8000, 1, 1000000);

    loadI("passive_land_monster_invade_period_i", sPassiveLandMonsterInvadePeriod, 1000000, 1, 1000000);
    loadI("active_land_monster_invade_period_i", sActiveLandMonsterInvadePeriod, 600000, 1, 1000000);
    loadI("very_active_land_monster_invade_period_i", sVeryActiveLandMonsterInvadePeriod, 400000, 1, 1000000);
    loadI("aggressive_land_monster_invade_period_i", sAggressiveLandMonsterInvadePeriod, 200000, 1, 1000000);

    loadI("passive_water_monster_invade_period_i", sPassiveWaterMonsterInvadePeriod, 800000, 1, 1000000);
    loadI("active_water_monster_invade_period_i", sActiveWaterMonsterInvadePeriod, 400000, 1, 1000000);
    loadI("very_active_water_monster_invade_period_i", sVeryActiveWaterMonsterInvadePeriod, 200000, 1, 1000000);
    loadI("aggressive_water_monster_invade_period_i", sAggressiveWaterMonsterInvadePeriod, 100000, 1, 1000000);

    loadI("passive_land_monster_move_around_period_i", sPassiveLandMonsterMoveAroundPeriod, 20000, 1, 1000000);
    loadI("active_land_monster_move_around_period_i", sActiveLandMonsterMoveAroundPeriod, 30000, 1, 1000000);
    loadI("very_active_land_monster_move_around_period_i", sVeryActiveLandMonsterMoveAroundPeriod, 40000, 1, 1000000);
    loadI("aggressive_land_monster_move_around_period_i", sAggressiveLandMonsterMoveAroundPeriod, 50000, 1, 1000000);

    loadI("passive_water_monster_move_around_period_i", sPassiveWaterMonsterMoveAroundPeriod, 20000, 1, 1000000);
    loadI("active_water_monster_move_around_period_i", sActiveWaterMonsterMoveAroundPeriod, 30000, 1, 1000000);
    loadI("very_active_water_monster_move_around_period_i", sVeryActiveWaterMonsterMoveAroundPeriod, 40000, 1, 1000000);
    loadI("aggressive_water_monster_move_around_period_i", sAggressiveWaterMonsterMoveAroundPeriod, 50000, 1, 1000000);

    loadI("friendly_god_visit_period_i", sFriendlyGodVisitPeriod, 450, 1, 10000);
    loadI("god_help_period_i", sGodHelpPeriod, 250000, 1, 1000000);
    loadI("god_help_attack_period_i", sGodHelpAttackPeriod, 500000, 1, 1000000);
    loadI("god_help_attack_player_period_i", sGodHelpAttackPlayerPeriod, 250000, 1, 1000000);

    loadI("god_attack_curse_period_i", sGodAttackCursePeriod, 6000, 1, 1000000);
    loadI("god_attack_curse_range_i", sGodAttackCurseRange, 10, 1, 20);
    loadI("god_attack_attack_period_i", sGodAttackAttackPeriod, 12000, 1, 1000000);
    loadI("god_attack_attack_range_i", sGodAttackAttackRange, 10, 1, 20);
    loadI("god_attack_aggressive_attack_period_i", sGodAttackAggressiveAttackPeriod, 8000, 1, 1000000);
    loadI("god_attack_targeted_curse_period_i", sGodAttackTargetedCursePeriod, 3000, 1, 1000000);
    loadI("god_attack_targeted_curse_range_i", sGodAttackTargetedCurseRange, 10, 1, 20);
    loadI("god_attack_targeted_attack_period_i", sGodAttackTargetedAttackPeriod, 4000, 1, 1000000);
    loadI("god_attack_targeted_attack_range_i", sGodAttackTargetedAttackRange, 10, 1, 20);
    loadI("god_attack_god_fight_range_i", sGodAttackGodFightRange, 10, 1, 20);
    loadI("god_attack_apollo_plague_period_i", sGodAttackApolloPlaguePeriod, 7500, 1, 1000000);
    loadI("god_attack_apollo_plague_range_i", sGodAttackApolloPlagueRange, 10, 1, 20);
    loadI("god_attack_aphrodite_evict_period_i", sGodAttackAphroditeEvictPeriod, 3500, 1, 1000000);
    loadI("god_attack_aphrodite_evict_range_i", sGodAttackAphroditeEvictRange, 10, 1, 20);

    loadI("god_visit_soldier_attack_period_i", sGodVisitSoldierAttackPeriod, 2500, 1, 1000000);
    loadI("god_visit_soldier_attack_range_i", sGodVisitSoldierAttackRange, 10, 1, 20);
    loadI("god_visit_bless_period_i", sGodVisitBlessPeriod, 18000, 1, 1000000);
    loadI("god_visit_bless_range_i", sGodVisitBlessRange, 10, 1, 20);
    loadI("god_visit_patrol_distance_i", sGodVisitPatrolDistance, 100, 10, 1000);
    loadI("god_visit_move_around_time_i", sGodVisitMoveAroundTime, 25000, 1, 1000000);

    loadI("god_worshipped_soldier_attack_period_i", sGodWorshippedSoldierAttackPeriod, 2500, 1, 1000000);
    loadI("god_worshipped_soldier_attack_range_i", sGodWorshippedSoldierAttackRange, 10, 1, 20);
    loadI("god_worshipped_bless_period_i", sGodWorshippedBlessPeriod, 6000, 1, 1000000);
    loadI("god_worshipped_bless_range_i", sGodWorshippedBlessRange, 10, 1, 20);

    loadI("soldier_being_attacked_call_range_i", sSoldierBeingAttackedCallRange, 2, 0, 100);

    loadI("tower_hp_i", sTowerHP, 5000, 1, 1000000);
    loadI("tower_range_i", sTowerRange, 8, 2, 100);
    loadD("tower_attack_d", sTowerAttack, 0.5, 0.1, 10.);

    loadI("wall_hp_i", sWallHP, 2000, 1, 1000000);

    loadI("wall_archer_range_i", sWallArcherRange, 8, 2, 200);
    loadD("wall_archer_attack_d", sWallArcherAttack, 0.5, 0.1, 10.);

    loadI("rabble_hp_i", sRabbleHP, 300, 1, 1000000);
    loadI("rabble_range_i", sRabbleRange, 4, 1, 20);
    loadD("rabble_attack_d", sRabbleAttack, 0.05, 0.01, 10);

    loadI("spearthrower_hp_i", sSpearthrowerHP, 300, 1, 1000000);
    loadI("spearthrower_range_i", sSpearthrowerRange, 4, 1, 20);
    loadD("spearthrower_attack_d", sSpearthrowerAttack, 0.05, 0.01, 10);

    loadI("archer_hp_i", sArcherHP, 300, 1, 1000000);
    loadI("archer_range_i", sArcherRange, 4, 1, 20);
    loadD("archer_attack_d", sArcherAttack, 0.05, 0.01, 10);

    loadI("hoplite_hp_i", sHopliteHP, 500, 1, 1000000);
    loadD("hoplite_attack_d", sHopliteAttack, 0.5, 0.01, 10);

    loadI("chariot_hp_i", sChariotHP, 500, 1, 1000000);
    loadD("chariot_speed_d", sChariotSpeed, 1.5, 1, 3);
    loadD("chariot_attack_d", sChariotAttack, 0.6, 0.01, 10);

    loadI("horseman_hp_i", sHorsemanHP, 500, 1, 1000000);
    loadD("horseman_speed_d", sHorsemanSpeed, 1.5, 1, 3);
    loadD("horseman_attack_d", sHorsemanAttack, 0.6, 0.01, 10);

    loadI("watchman_hp_i", sWatchmanHP, 200, 1, 1000000);
    loadD("watchman_attack_d", sWatchmanAttack, 0.5, 0.01, 10);
    loadI("watchman_satisfaction_provide_i", sWatchmanSatisfactionProvide, 25, 0, 100);

    loadI("defend_city_max_killed_i", sDefendCityMaxKilled, 6, 1, 100);
    loadI("defend_city_talos_max_killed_i", sDefendCityTalosMaxKilled, 16, 1, 100);

    loadI("attack_city_max_killed_i", sAttackCityMaxKilled, 4, 1, 100);
    loadI("attack_city_ares_max_killed_i", sAttackCityAresMaxKilled, 8, 1, 100);

    loadI("basic_vendor_max_resource_take_distance_i", sBasicVendorMaxResourceTakeDistance, 60, 5, 1000);
    loadI("horse_vendor_max_resource_take_distance_i", sHorseVendorMaxResourceTakeDistance, 200, 5, 1000);

    loadI("resource_building_max_resource_give_distance_i", sResourceBuildingMaxResourceGiveDistance, 60, 5, 1000);
    loadI("processing_building_max_resource_take_distance_i", sProcessingBuildingMaxResourceTakeDistance, 60, 5, 1000);
    loadI("trireme_wharf_max_resource_take_distance_i", sTriremeWharfMaxResourceTakeDistance, 60, 5, 1000);
    loadI("trireme_wharf_build_time_i", sTriremeWharfBuildTime, 100000, 1, 1000000);
    loadI("trireme_wharf_build_stages_i", sTriremeWharfBuildStages, 3, 1, 10);
}
