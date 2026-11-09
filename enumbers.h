#ifndef ENUMBERS_H
#define ENUMBERS_H

#include <string>

struct eNumbers {
    static void sLoad();
    static void sLoad(const std::string& path);

    static int sDayLength;

    static double sEliteHousingTaxMultiplier;
    static double sCommonHousingTaxMulitplier;
    static double sWageMultiplier;

    static int sReinforcementsTravelTime;
    static int sArmyTravelTime;
    static int sAIInvasionMonthsBreak;
    static int sInvasionAppearAtPlaces;
    static double sArmyStrengthHorsemanMult;

    static int sRabbleDivisor;
    static int sSoldiersPerBanner;
    static int sSoldiersPerAresArtemisBanner;

    static int sEarthquakeProgressPeriod;

    static int sBoarMaxCount;
    static int sBoarSpawnPeriod;
    static int sDeerMaxCount;
    static int sDeerSpawnPeriod;
    static int sWolfMaxCount;
    static int sWolfSpawnPeriod;
    static int sWolfHuntWait;
    static int sWolfHuntDistance;

    static int sOlivePressProcessingPeriod;
    static int sWineryProcessingPeriod;
    static int sArmoryProcessingPeriod;
    static int sSculptureStudioProcessingPeriod;

    static int sFarmRipePeriod;

    static int sShepherdGoatherdMaxDistance;
    static int sShepherdGoatherdMaxGroom;
    static int sShepherdGoatherdGroomTime;
    static int sShepherdGoatherdCollectTime;
    static int sShepherdGoatherdWaitTime;

    static int sSheepMaxGroom;
    static int sGoatMaxGroom;
    static int sAnimalMoveRange;

    static int sCorralProcessingPeriod;
    static int sCorralTakePeriod;
    static int sCorralKillPeriod;
    static int sCorralReplacePeriod;
    static int sCattleMaturePeriod;

    static int sUrchinQuayUnpackTime;
    static int sFisheryBoatBuildTime;
    static int sFisheryUnpackTime;

    static int sHuntingLodgeWaitPeriod;

    static int sMintDrachmasPerSilver;

    static int sSpreadFirePeriod;
    static int sFireCollapsePeriod;
    static int sRuinsFireEndPeriod;
    static int sMaintenanceDecrementPeriod;
    static double sFireRiskPeriodMultiplier;
    static double sFireRiskPeriodBaseIncrement;
    static double sFireRiskPeriodExponent;
    static double sCollapseRiskPeriodMultiplier;
    static double sCollapseRiskPeriodBaseIncrement;
    static double sCollapseRiskPeriodExponent;

    static int sHouseCultureDecrementPeriod;
    static int sHouseWaterDecrementPeriod;
    static int sHouseHygieneDecrementPeriod;
    static double sHouseHealPlaguePeriodMultiplier;
    static double sHouseHealPlaguePeriodBaseMultiplier;
    static double sHouseHealPlaguePeriodExponent;
    static double sHousePlagueRiskPeriodMultiplier;
    static double sHousePlagueRiskPeriodBaseIncrement;
    static double sHousePlagueRiskPeriodExponent;
    static int sHouseDisgruntledRemovePeriod;
    static double sHouseDisgruntledRiskPeriodMultiplier;
    static double sHouseDisgruntledRiskPeriodBaseIncrement;
    static double sHouseDisgruntledRiskPeriodExponent;
    static double sHouseLeaveRiskPeriodMultiplier;
    static double sHouseLeaveRiskPeriodBaseIncrement;
    static double sHouseLeaveRiskPeriodExponent;
    static int sHouseSatisfactionUpdatePeriod;
    static int sHouseDisgruntledSpawnPeriod;
    static int sHouseSickSpawnPeriod;

    static int sHerosHallArrivalPeriod;
    static int sHerosHallSpawnPeriod;

    static int sHorseRanchWheatUsePeriod;
    static int sHorseRanchHorseSpawnPeriod;

    static int sPatrolerMaxDistance;
    static int sPatrolerWaitTime;
    static int sPatrolerSourceSpawnPeriod;

    static int sArtisanWaitTime;
    static int sArtisanBuildTime;

    static int sTowerSpawnPeriod;

    static int sTraderSpawnPeriod;
    static int sTwoWayTradeMax;

    static int sChariotBuildingTime;

    static int sOliveTreeRipePeriod;
    static int sVineRipePeriod;
    static int sOrangeTreeRipePeriod;

    static double sTreeVineFullyRipePeriodMultiplier;

    static double sTreeVineBlessedFullyRipePeriodMultiplier;
    static double sTreeVineBlessedRipePeriodMultiplier;
    static double sTreeVineCursedFullyRipePeriodMultiplier;
    static double sTreeVineCursedRipePeriodMultiplier;

    static int sMarbleCollectTime;
    static int sBronzeCollectTime;
    static int sSilverCollectTime;
    static int sWoodCollectTime;
    static int sFishCollectTime;
    static int sUrchinCollectTime;

    static int sGrowerMaxDistance;
    static int sGrowerMaxGroom;
    static int sGrowerWorkTime;
    static int sGrowerSpawnWaitTime;

    static int sMonsterAttackRange;

    static int sPassiveMonsterAttackPeriod;
    static int sActiveMonsterAttackPeriod;
    static int sVeryActiveMonsterAttackPeriod;
    static int sAggressiveMonsterAttackPeriod;

    static int sPassiveLandMonsterInvadePeriod;
    static int sActiveLandMonsterInvadePeriod;
    static int sVeryActiveLandMonsterInvadePeriod;
    static int sAggressiveLandMonsterInvadePeriod;

    static int sPassiveWaterMonsterInvadePeriod;
    static int sActiveWaterMonsterInvadePeriod;
    static int sVeryActiveWaterMonsterInvadePeriod;
    static int sAggressiveWaterMonsterInvadePeriod;

    static int sPassiveLandMonsterMoveAroundPeriod;
    static int sActiveLandMonsterMoveAroundPeriod;
    static int sVeryActiveLandMonsterMoveAroundPeriod;
    static int sAggressiveLandMonsterMoveAroundPeriod;

    static int sPassiveWaterMonsterMoveAroundPeriod;
    static int sActiveWaterMonsterMoveAroundPeriod;
    static int sVeryActiveWaterMonsterMoveAroundPeriod;
    static int sAggressiveWaterMonsterMoveAroundPeriod;

    static int sFriendlyGodVisitPeriod;
    static int sGodHelpPeriod;
    static int sGodHelpAttackPeriod;
    static int sGodHelpAttackPlayerPeriod;

    static int sGodAttackCursePeriod;
    static int sGodAttackCurseRange;
    static int sGodAttackAttackPeriod;
    static int sGodAttackAttackRange;
    static int sGodAttackAggressiveAttackPeriod;
    static int sGodAttackTargetedCursePeriod;
    static int sGodAttackTargetedCurseRange;
    static int sGodAttackTargetedAttackPeriod;
    static int sGodAttackTargetedAttackRange;
    static int sGodAttackGodFightRange;
    static int sGodAttackApolloPlaguePeriod;
    static int sGodAttackApolloPlagueRange;
    static int sGodAttackAphroditeEvictPeriod;
    static int sGodAttackAphroditeEvictRange;

    static int sGodVisitSoldierAttackPeriod;
    static int sGodVisitSoldierAttackRange;
    static int sGodVisitBlessPeriod;
    static int sGodVisitBlessRange;
    static int sGodVisitPatrolDistance;
    static int sGodVisitMoveAroundTime;

    static int sGodWorshippedSoldierAttackPeriod;
    static int sGodWorshippedSoldierAttackRange;
    static int sGodWorshippedBlessPeriod;
    static int sGodWorshippedBlessRange;

    static int sSoldierBeingAttackedCallRange;

    static int sTowerHP;
    static int sTowerRange;
    static double sTowerAttack;

    static int sWallHP;

    static int sWallArcherRange;
    static double sWallArcherAttack;

    static int sRabbleHP;
    static int sRabbleRange;
    static double sRabbleAttack;

    static int sSpearthrowerHP;
    static int sSpearthrowerRange;
    static double sSpearthrowerAttack;

    static int sArcherHP;
    static int sArcherRange;
    static double sArcherAttack;

    static int sHopliteHP;
    static double sHopliteAttack;

    static int sChariotHP;
    static double sChariotSpeed;
    static double sChariotAttack;

    static int sHorsemanHP;
    static double sHorsemanSpeed;
    static double sHorsemanAttack;

    static int sWatchmanHP;
    static double sWatchmanAttack;
    static int sWatchmanSatisfactionProvide;

    static int sDefendCityMaxKilled;
    static int sDefendCityTalosMaxKilled;

    static int sAttackCityMaxKilled;
    static int sAttackCityAresMaxKilled;

    static int sBasicVendorMaxResourceTakeDistance;
    static int sBasicVendorMaxResource;
    static int sWalkerProvideDistance;
    static int sHorseVendorMaxResourceTakeDistance;
    static int sResourceBuildingMaxResourceGiveDistance;
    static int sProcessingBuildingMaxResourceTakeDistance;
    static int sTriremeWharfMaxResourceTakeDistance;
    static int sTriremeWharfBuildTime;
    static int sTriremeWharfBuildStages;
};

#endif // ENUMBERS_H
