#include "esounds.h"

#include "characters/egrower.h"
#include "efspath.h"
#include "characters/ehunter.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "characters/echaracter.h"
#include "characters/ecarttransporter.h"
#include "characters/esettler.h"

#include "egamedir.h"
#include <filesystem>

eSounds* eSounds::sInstance = nullptr;

eSounds::eSounds() {
    sInstance = this;
}

void eSounds::loadButtonSound() {
    const std::string wavsDir{eGameDir::path("Audio/Wavs/")};
    sInstance->mButton.addPath(wavsDir + "button.wav");
}

void eSounds::load() {
    sInstance->loadImpl();
}

bool eSounds::loaded() {
    return sInstance->mLoaded;
}

void eSounds::playButtonSound() {
    sInstance->mButton.playRandomSound();
}

void eSounds::playPlaceBuildingSound() {
    sInstance->mPlaceBuilding.playRandomSound();
}

void eSounds::playSoundForTile(eTile* const tile) {
    if(tile->onFire()) return eSounds::playFireSound();
    if(const auto b = tile->underBuilding()) {
        const bool r = playSoundForBuilding(b);
        if(r) return;
    }

    const auto& chars = tile->characters();
    for(const auto& c : chars) {
        const bool r = playSoundForCharacter(c.get());
        if(r) return;
    }

    const bool r = playSoundForTerrain(tile->terrain());
    if(r) return;

    eSounds::playEnvironmentSound();
}

bool eSounds::playSoundForBuilding(eBuilding* const b) {
    if(!b) return false;
    const auto type = b->type();
    return playSoundForBuilding(type);
}

bool eSounds::playSoundForBuilding(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) {
        eSounds::playBeautificationSound();
        return true;
    }
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) {
        eSounds::playSanctuarySound();
        return true;
    }
    switch(type) {
    case eBuildingType::sheep:
        eSounds::playSheepSound();
        return true;
    case eBuildingType::goat:
        eSounds::playGoatSound();
        return true;
    case eBuildingType::cattle:
        eSounds::playCattleSound();
        return true;
    case eBuildingType::wheatFarm:
    case eBuildingType::onionsFarm:
    case eBuildingType::carrotsFarm:
        eSounds::playFarmingSound();
        return true;
    case eBuildingType::cardingShed:
        eSounds::playShearingSound();
        return true;
    case eBuildingType::dairy:
        eSounds::playGoatherdSound();
        return true;
    case eBuildingType::commonHouse:
        eSounds::playCommonHousingSound();
        return true;
    case eBuildingType::eliteHousing:
        eSounds::playEliteHousingSound();
        return true;
    case eBuildingType::theater:
        eSounds::playTheatreSound();
        return true;
    case eBuildingType::dramaSchool:
        eSounds::playDramaSound();
        return true;
    case eBuildingType::bibliotheke:
        eSounds::playBibliothekeSound();
        return true;
    case eBuildingType::university:
        eSounds::playUniversitySound();
        return true;
    case eBuildingType::observatory:
        eSounds::playObservatorySound();
        return true;
    case eBuildingType::inventorsWorkshop:
        eSounds::playInventorsWorkshopSound();
        return true;
    case eBuildingType::laboratory:
        eSounds::playLaboratorySound();
        return true;
    case eBuildingType::museum:
        eSounds::playMuseumSound();
        return true;
    case eBuildingType::timberMill:
        eSounds::playTimberMillSound();
        return true;
    case eBuildingType::warehouse:
    case eBuildingType::granary:
        eSounds::playStorageSound();
        return true;
    case eBuildingType::foundry:
    case eBuildingType::refinery:
        eSounds::playFoundrySound();
        return true;
    case eBuildingType::masonryShop:
    case eBuildingType::blackMarbleWorkshop:
        eSounds::playMarbleSound();
        return true;
    case eBuildingType::growersLodge:
    case eBuildingType::orangeTendersLodge:
        eSounds::playFruitLodgeSound();
        return true;
    case eBuildingType::mint:
        eSounds::playMintSound();
        return true;
    case eBuildingType::maintenanceOffice:
        eSounds::playMaintananceSound();
        return true;
    case eBuildingType::taxOffice:
        eSounds::playTaxesSound();
        return true;
    case eBuildingType::hospital:
        eSounds::playInfirmarySound();
        return true;
    case eBuildingType::palace:
        eSounds::playPalaceSound();
        return true;
    case eBuildingType::huntingLodge:
        eSounds::playHuntingSound();
        return true;
    case eBuildingType::corral:
        eSounds::playCorralSound();
        return true;
    case eBuildingType::podium:
    case eBuildingType::college:
        eSounds::playPhilosophySound();
        return true;
    case eBuildingType::gymnasium:
        eSounds::playGymnasiumSound();
        return true;
    case eBuildingType::stadium:
        eSounds::playStadiumSound();
        return true;
    case eBuildingType::fountain:
        eSounds::playFountainSound();
        return true;
    case eBuildingType::armory:
        eSounds::playArmorySound();
        return true;
    case eBuildingType::foodVendor:
        eSounds::playFoodVendorSound();
        return true;
    case eBuildingType::fleeceVendor:
        eSounds::playFleeceVendorSound();
        return true;
    case eBuildingType::oilVendor:
        eSounds::playOilVendorSound();
        return true;
    case eBuildingType::wineVendor:
        eSounds::playWineVendorSound();
        return true;
    case eBuildingType::armsVendor:
        eSounds::playArmsVendorSound();
        return true;
    case eBuildingType::horseTrainer:
        eSounds::playHorseTrainerSound();
        return true;
    case eBuildingType::chariotVendor:
        eSounds::playChariotVendorSound();
        return true;
    case eBuildingType::horseRanch:
    case eBuildingType::horseRanchEnclosure:
        eSounds::playHorseRanchSound();
        return true;
    case eBuildingType::chariotFactory:
        eSounds::playChariotFactorySound();
        return true;
    case eBuildingType::artisansGuild:
        eSounds::playArtisansGuildSound();
        return true;
    case eBuildingType::fishery:
    case eBuildingType::urchinQuay:
        eSounds::playSeaSound();
        return true;

    case eBuildingType::tradePost:
    case eBuildingType::pier:
        eSounds::playTradeSound();
        return true;
    default:
        break;
    }
    return false;
}

bool eSounds::playSoundForCharacter(eCharacter* const c) {
    const auto type = c->type();
    switch(type) {
    case eCharacterType::hunter:
        eSounds::playHuntingSound();
        return true;
    case eCharacterType::silverMiner:
        eSounds::playMintSound();
        return true;
    case eCharacterType::lumberjack:
        eSounds::playTimberMillSound();
        return true;
    case eCharacterType::boar:
        eSounds::playBoarSound();
        return true;
    case eCharacterType::deer:
        eSounds::playDeerSound();
        return true;
    case eCharacterType::wolf:
        eSounds::playWolfSound();
        return true;
    case eCharacterType::sheep:
        eSounds::playSheepSound();
        return true;
    case eCharacterType::goat:
        eSounds::playGoatSound();
        return true;
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
    case eCharacterType::bull:
        eSounds::playCattleSound();
        return true;
    case eCharacterType::cartTransporter:
        eSounds::playStorageSound();
        return true;
    default:
        break;
    }
    return false;
}

bool eSounds::playSoundForTerrain(const eTerrain terrain) {
    switch(terrain) {
    case eTerrain::beach:
        eSounds::playBeachSound();
        return true;
    case eTerrain::water:
        eSounds::playWaterSound();
        return true;
    case eTerrain::silver:
    case eTerrain::copper:
    case eTerrain::orichalc:
    case eTerrain::flatStones:
    case eTerrain::tallStones:
        eSounds::playRockySound();
        return true;
    default:
        break;
    }
    return false;
}

void eSounds::playEnvironmentSound() {
    sInstance->mEnvironment.playRandomSound();
}

void eSounds::playBeachSound() {
    sInstance->mBeach.playRandomSound();
}

void eSounds::playWaterSound() {
    sInstance->mWater.playRandomSound();
}

void eSounds::playRockySound() {
    sInstance->mRocky.playRandomSound();
}

void eSounds::playBeautificationSound() {
    sInstance->mBeautification.playRandomSound();
}

void eSounds::playSanctuarySound() {
    sInstance->mSanctuary.playRandomSound();
}

void eSounds::playFarmingSound() {
    sInstance->mFarming.playRandomSound();
}

void eSounds::playCommonHousingSound() {
    sInstance->mCommonHousing.playRandomSound();
}

void eSounds::playEliteHousingSound() {
    sInstance->mEliteHousing.playRandomSound();
}

void eSounds::playTheatreSound() {
    sInstance->mTheatre.playRandomSound();
}

void eSounds::playDramaSound() {
    sInstance->mDrama.playRandomSound();
}

void eSounds::playPhilosophySound() {
    sInstance->mPhilosophy.playRandomSound();
}

void eSounds::playGymnasiumSound() {
    sInstance->mGymnasium.playRandomSound();
}

void eSounds::playStadiumSound() {
    sInstance->mStadium.playRandomSound();
}

void eSounds::playBibliothekeSound() {
    sInstance->mBibliotheke.playRandomSound();
}

void eSounds::playUniversitySound() {
    sInstance->mUniversity.playRandomSound();
}

void eSounds::playObservatorySound() {
    sInstance->mObservatory.playRandomSound();
}

void eSounds::playInventorsWorkshopSound() {
    sInstance->mInventorsWorkshop.playRandomSound();
}

void eSounds::playLaboratorySound() {
    sInstance->mLaboratory.playRandomSound();
}

void eSounds::playMuseumSound() {
    sInstance->mMuseum.playRandomSound();
}

void eSounds::playArtisansGuildSound() {
    sInstance->mArtisansGuild.playRandomSound();
}

void eSounds::playFountainSound() {
    sInstance->mFountain.playRandomSound();
}

void eSounds::playInfirmarySound() {
    sInstance->mInfirmary.playRandomSound();
}

void eSounds::playTaxesSound() {
    sInstance->mTaxes.playRandomSound();
}

void eSounds::playPalaceSound() {
    sInstance->mPalace.playRandomSound();
}

void eSounds::playMaintananceSound() {
    sInstance->mMaintenance.playRandomSound();
}

void eSounds::playFoundrySound() {
    sInstance->mFoundry.playRandomSound();
}

void eSounds::playMarbleSound() {
    sInstance->mMarble.playRandomSound();
}

void eSounds::playMintSound() {
    sInstance->mMint.playRandomSound();
}

void eSounds::playTimberMillSound() {
    sInstance->mTimberMill.playRandomSound();
}

void eSounds::playArmorySound() {
    sInstance->mArmory.playRandomSound();
}

void eSounds::playHorseRanchSound() {
    sInstance->mHorseRanch.playRandomSound();
}

void eSounds::playChariotFactorySound() {
    sInstance->mChariotFactory.playRandomSound();
}

void eSounds::playHuntingSound() {
    sInstance->mHunting.playRandomSound();
}

void eSounds::playBoarSound() {
    sInstance->mBoar.playRandomSound();
}

void eSounds::playDeerSound(){
    sInstance->mDeer.playRandomSound();
}

void eSounds::playWolfSound() {
    sInstance->mWolf.playRandomSound();
}

void eSounds::playSheepSound() {
    sInstance->mSheep.playRandomSound();
}

void eSounds::playGoatSound() {
    sInstance->mGoat.playRandomSound();
}

void eSounds::playCattleSound() {
    sInstance->mCattle.playRandomSound();
}

void eSounds::playCorralSound() {
    sInstance->mCorral.playRandomSound();
}

void eSounds::playFruitLodgeSound() {
    sInstance->mFruitLodge.playRandomSound();
}

void eSounds::playGoatherdSound() {
    sInstance->mGoatFarm.playRandomSound();
}

void eSounds::playShearingSound() {
    sInstance->mSheepFarm.playRandomSound();
}

void eSounds::playStorageSound() {
    sInstance->mStorage.playRandomSound();
}

void eSounds::playFoodVendorSound() {
    sInstance->mAgoraFood.playRandomSound();
}

void eSounds::playFleeceVendorSound() {
    sInstance->mAgoraFleece.playRandomSound();
}

void eSounds::playOilVendorSound() {
    sInstance->mAgoraOil.playRandomSound();
}

void eSounds::playWineVendorSound() {
    sInstance->mAgoraWine.playRandomSound();
}

void eSounds::playArmsVendorSound() {
    sInstance->mAgoraArms.playRandomSound();
}

void eSounds::playHorseTrainerSound() {
    sInstance->mAgoraHorse.playRandomSound();
}

void eSounds::playChariotVendorSound() {
    sInstance->mAgoraChariot.playRandomSound();
}

void eSounds::playTradeSound() {
    sInstance->mTrade.playRandomSound();
}

void eSounds::playFireSound() {
    sInstance->mFire.playRandomSound();
}

void eSounds::playCollapseSound() {
    sInstance->mCollapse.playRandomSound();
}

void eSounds::playFireballHitSound() {
    sInstance->mFireballHit.playRandomSound();
}

void eSounds::playEarthquakeSound() {
    sInstance->mEarthquake.playRandomSound();
}

void eSounds::playLavaSound() {
    sInstance->mLava.playRandomSound();
}

void eSounds::playCopperMinerSound() {
    sInstance->mCopperMiner.playRandomSound();
}

void eSounds::playSilverMinerSound() {
    sInstance->mSilverMiner.playRandomSound();
}

void eSounds::playStoneCutterSound() {
    sInstance->mStoneCutter.playRandomSound();
}

void eSounds::playTreeCutterSound() {
    sInstance->mTreeCutter.playRandomSound();
}

void eSounds::playArtisanSound() {
    sInstance->mArtisan.playRandomSound();
}

void eSounds::playSeaSound() {
    sInstance->mSea.playRandomSound();
}

void eSounds::playGodSound(const eGodType g, const eGodSound s) {
    eGodSounds* god = nullptr;
    switch(g) {
    case eGodType::aphrodite:
        god = &sInstance->mAphrodite; break;
    case eGodType::apollo:
        god = &sInstance->mApollo; break;
    case eGodType::ares:
        god = &sInstance->mAres; break;
    case eGodType::artemis:
        god = &sInstance->mArtemis; break;
    case eGodType::athena:
        god = &sInstance->mAthena; break;
    case eGodType::atlas:
        god = &sInstance->mAtlas; break;
    case eGodType::demeter:
        god = &sInstance->mDemeter; break;
    case eGodType::dionysus:
        god = &sInstance->mDionysus; break;
    case eGodType::hades:
        god = &sInstance->mHades; break;
    case eGodType::hephaestus:
        god = &sInstance->mHephaestus; break;
    case eGodType::hera:
        god = &sInstance->mHera; break;
    case eGodType::hermes:
        god = &sInstance->mHermes; break;
    case eGodType::poseidon:
        god = &sInstance->mPoseidon; break;
    case eGodType::zeus:
        god = &sInstance->mZeus; break;
    }
    god->play(s);
}

void eSounds::playHeroSound(const eHeroType h, const eHeroSound s) {
    eHeroSounds* hero = nullptr;
    switch(h) {
    case eHeroType::achilles:
        hero = &sInstance->mAchilles; break;
    case eHeroType::atalanta:
        hero = &sInstance->mAtalanta; break;
    case eHeroType::bellerophon:
        hero = &sInstance->mBellerophon; break;
    case eHeroType::hercules:
        hero = &sInstance->mHercules; break;
    case eHeroType::jason:
        hero = &sInstance->mJason; break;
    case eHeroType::odysseus:
        hero = &sInstance->mOdysseus; break;
    case eHeroType::perseus:
        hero = &sInstance->mPerseus; break;
    case eHeroType::theseus:
        hero = &sInstance->mTheseus; break;
    }
    hero->play(s);
}

void eSounds::playMonsterSound(const eMonsterType m,
                               const eMonsterSound s) {
    eMonsterSounds* monster = nullptr;
    switch(m) {
    case eMonsterType::calydonianBoar:
        monster = &sInstance->mCalydonianBoar; break;
    case eMonsterType::cerberus:
        monster = &sInstance->mCerberus; break;
    case eMonsterType::chimera:
        monster = &sInstance->mChimera; break;
    case eMonsterType::cyclops:
        monster = &sInstance->mCyclops; break;
    case eMonsterType::dragon:
        monster = &sInstance->mDragon; break;
    case eMonsterType::echidna:
        monster = &sInstance->mEchidna; break;
    case eMonsterType::harpies:
        monster = &sInstance->mHarpies; break;
    case eMonsterType::hector:
        monster = &sInstance->mHector; break;
    case eMonsterType::hydra:
        monster = &sInstance->mHydra; break;
    case eMonsterType::kraken:
        monster = &sInstance->mKraken; break;
    case eMonsterType::maenads:
        monster = &sInstance->mMaenads; break;
    case eMonsterType::medusa:
        monster = &sInstance->mMedusa; break;
    case eMonsterType::minotaur:
        monster = &sInstance->mMinotaur; break;
    case eMonsterType::scylla:
        monster = &sInstance->mScylla; break;
    case eMonsterType::sphinx:
        monster = &sInstance->mSphinx; break;
    case eMonsterType::talos:
        monster = &sInstance->mTalos; break;
    case eMonsterType::satyr: {
        sInstance->mSatyr.play(s);
        return;
    } break;
    }
    monster->play(s);
}

void eSounds::playAttackSound(eCharacter* const c) {
    const auto type = c->type();
    playAttackSound(type);
}

void eSounds::playAttackSound(const eCharacterType type) {
    switch(type) {
    case eCharacterType::greekRockThrower:
    case eCharacterType::rockThrower:
        sInstance->mRockthrowerAttack.playRandomSound();
        break;
    case eCharacterType::boar:
        sInstance->mBoarAttack.playRandomSound();
        break;
    case eCharacterType::hunter:
        sInstance->mBoarHunterAttack.playRandomSound();
        break;
    case eCharacterType::deer:
        sInstance->mDeerAttack.playRandomSound();
        break;
    case eCharacterType::wolf:
        sInstance->mWolfAttack.playRandomSound();
        break;
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        sInstance->mCattleAttack.playRandomSound();
        break;
    case eCharacterType::goat:
        sInstance->mGoatAttack.playRandomSound();
        break;
    case eCharacterType::greekHoplite:
    case eCharacterType::hoplite:
        sInstance->mHopliteAttack.playRandomSound();
        break;

    case eCharacterType::archer:
        sInstance->mArcherAttack.playRandomSound();
        break;

    case eCharacterType::disgruntled:
        sInstance->mOutlawAttack.playRandomSound();
        break;

    case eCharacterType::aresWarrior:
        sInstance->mAresWarriorAttack.playRandomSound();
        break;

    case eCharacterType::trojanSpearthrower:
        sInstance->mTrojanSpearthrowerAttack.playRandomSound();
        break;
    case eCharacterType::trojanHoplite:
        sInstance->mTrojanSwordAttack.playRandomSound();
        break;

    case eCharacterType::aphrodite:
        sInstance->mAphrodite.playAttack();
        break;
    case eCharacterType::apollo:
        sInstance->mApollo.playAttack();
        break;
    case eCharacterType::ares:
        sInstance->mAres.playAttack();
        break;
    case eCharacterType::artemis:
        sInstance->mArtemis.playAttack();
        break;
    case eCharacterType::athena:
        sInstance->mAthena.playAttack();
        break;
    case eCharacterType::atlas:
        sInstance->mAtlas.playAttack();
        break;
    case eCharacterType::demeter:
        sInstance->mDemeter.playAttack();
        break;
    case eCharacterType::dionysus:
        sInstance->mDionysus.playAttack();
        break;
    case eCharacterType::hades:
        sInstance->mHades.playAttack();
        break;
    case eCharacterType::hephaestus:
        sInstance->mHephaestus.playAttack();
        break;
    case eCharacterType::hera:
        sInstance->mHera.playAttack();
        break;
    case eCharacterType::hermes:
        sInstance->mHermes.playAttack();
        break;
    case eCharacterType::poseidon:
        sInstance->mPoseidon.playAttack();
        break;
    case eCharacterType::zeus:
        sInstance->mZeus.playAttack();
        break;

    case eCharacterType::calydonianBoar:
        sInstance->mCalydonianBoar.playAttack();
        break;
    case eCharacterType::cerberus:
        sInstance->mCerberus.playAttack();
        break;
    case eCharacterType::chimera:
        sInstance->mChimera.playAttack();
        break;
    case eCharacterType::cyclops:
        sInstance->mCyclops.playAttack();
        break;
    case eCharacterType::dragon:
        sInstance->mDragon.playAttack();
        break;
    case eCharacterType::echidna:
        sInstance->mEchidna.playAttack();
        break;
    case eCharacterType::harpies:
        sInstance->mHarpies.playAttack();
        break;
    case eCharacterType::hector:
        sInstance->mHector.playAttack();
        break;
    case eCharacterType::hydra:
        sInstance->mHydra.playAttack();
        break;
    case eCharacterType::kraken:
        sInstance->mKraken.playAttack();
        break;
    case eCharacterType::maenads:
        sInstance->mMaenads.playAttack();
        break;
    case eCharacterType::medusa:
        sInstance->mMedusa.playAttack();
        break;
    case eCharacterType::minotaur:
        sInstance->mMinotaur.playAttack();
        break;
    case eCharacterType::scylla:
        sInstance->mScylla.playAttack();
        break;
    case eCharacterType::sphinx:
        sInstance->mSphinx.playAttack();
        break;
    case eCharacterType::talos:
        sInstance->mTalos.playAttack();
        break;

    case eCharacterType::satyr:
        sInstance->mSatyr.playAttack();
        break;

    case eCharacterType::achilles:
        sInstance->mAchilles.playAttack();
        break;
    case eCharacterType::atalanta:
        sInstance->mAtalanta.playAttack();
        break;
    case eCharacterType::bellerophon:
        sInstance->mBellerophon.playAttack();
        break;
    case eCharacterType::hercules:
        sInstance->mHercules.playAttack();
        break;
    case eCharacterType::jason:
        sInstance->mJason.playAttack();
        break;
    case eCharacterType::odysseus:
        sInstance->mOdysseus.playAttack();
        break;
    case eCharacterType::perseus:
        sInstance->mPerseus.playAttack();
        break;
    case eCharacterType::theseus:
        sInstance->mTheseus.playAttack();
        break;
    default:
        break;
    }
}

void eSounds::playDieSound(eCharacter* const c) {
    const auto ct = c->type();
    switch(ct) {
    case eCharacterType::actor:
        sInstance->mActorDie.playRandomSound();
        break;
    case eCharacterType::greekRockThrower:
    case eCharacterType::rockThrower:
        sInstance->mRockthrowerDie.playRandomSound();
        break;
    case eCharacterType::boar:
        sInstance->mBoarDie.playRandomSound();
        break;
    case eCharacterType::hunter:
        sInstance->mBoarHunterDie.playRandomSound();
        break;
    case eCharacterType::deer:
        sInstance->mDeerDie.playRandomSound();
        break;
    case eCharacterType::wolf:
        sInstance->mWolfDie.playRandomSound();
        break;
    case eCharacterType::sheep:
        sInstance->mSheepDie.playRandomSound();
        break;
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        sInstance->mCattleDie.playRandomSound();
        break;
    case eCharacterType::goat:
        sInstance->mGoatDie.playRandomSound();
        break;
    case eCharacterType::greekHoplite:
    case eCharacterType::hoplite:
        sInstance->mHopliteDie.playRandomSound();
        break;

    case eCharacterType::archer:
        sInstance->mArcherDie.playRandomSound();
        break;

    case eCharacterType::disgruntled:
        sInstance->mOutlawDie.playRandomSound();
        break;

    case eCharacterType::aresWarrior:
        sInstance->mAresWarriorDie.playRandomSound();
        break;

    case eCharacterType::trojanHorseman:
        sInstance->mTrojanHorseDie.playRandomSound();
        break;
    case eCharacterType::trojanSpearthrower:
        sInstance->mTrojanSpearthrowerDie.playRandomSound();
        break;
    case eCharacterType::trojanHoplite:
        sInstance->mTrojanSwordDie.playRandomSound();
        break;

    case eCharacterType::calydonianBoar:
        sInstance->mCalydonianBoar.playDie();
        break;
    case eCharacterType::cerberus:
        sInstance->mCerberus.playDie();
        break;
    case eCharacterType::chimera:
        sInstance->mChimera.playDie();
        break;
    case eCharacterType::cyclops:
        sInstance->mCyclops.playDie();
        break;
    case eCharacterType::dragon:
        sInstance->mDragon.playDie();
        break;
    case eCharacterType::echidna:
        sInstance->mEchidna.playDie();
        break;
    case eCharacterType::harpies:
        sInstance->mHarpies.playDie();
        break;
    case eCharacterType::hector:
        sInstance->mHector.playDie();
        break;
    case eCharacterType::hydra:
        sInstance->mHydra.playDie();
        break;
    case eCharacterType::kraken:
        sInstance->mKraken.playDie();
        break;
    case eCharacterType::maenads:
        sInstance->mMaenads.playDie();
        break;
    case eCharacterType::medusa:
        sInstance->mMedusa.playDie();
        break;
    case eCharacterType::minotaur:
        sInstance->mMinotaur.playDie();
        break;
    case eCharacterType::scylla:
        sInstance->mScylla.playDie();
        break;
    case eCharacterType::sphinx:
        sInstance->mSphinx.playDie();
        break;
    case eCharacterType::talos:
        sInstance->mTalos.playDie();
        break;

    case eCharacterType::satyr:
        sInstance->mSatyr.playDie();
        break;
    default:
        sInstance->mGenDie.playRandomSound();
        break;
    }
}

void eSounds::playHitSound(eCharacter* const c) {
    const auto ct = c->type();
    switch(ct) {
    case eCharacterType::actor:
        sInstance->mActorHit.playRandomSound();
        break;
    case eCharacterType::greekRockThrower:
    case eCharacterType::rockThrower:
        sInstance->mRockthrowerHit.playRandomSound();
        break;
    case eCharacterType::boar:
        sInstance->mBoarHit.playRandomSound();
        break;
    case eCharacterType::hunter:
        sInstance->mBoarHunterHit.playRandomSound();
        break;
    case eCharacterType::deer:
        sInstance->mDeerHit.playRandomSound();
        break;
    case eCharacterType::wolf:
        sInstance->mWolfHit.playRandomSound();
        break;
    case eCharacterType::sheep:
        sInstance->mSheepHit.playRandomSound();
        break;
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        sInstance->mCattleHit.playRandomSound();
        break;
    case eCharacterType::goat:
        sInstance->mGoatHit.playRandomSound();
        break;
    case eCharacterType::greekHoplite:
    case eCharacterType::hoplite:
        sInstance->mHopliteHit.playRandomSound();
        break;

    case eCharacterType::archer:
        sInstance->mArcherHit.playRandomSound();
        break;

    case eCharacterType::disgruntled:
        sInstance->mOutlawHit.playRandomSound();
        break;

    case eCharacterType::aresWarrior:
        sInstance->mAresWarriorHit.playRandomSound();
        break;

    case eCharacterType::trojanHorseman:
        sInstance->mTrojanHorseHit.playRandomSound();
        break;
    case eCharacterType::trojanSpearthrower:
        sInstance->mTrojanSpearthrowerHit.playRandomSound();
        break;
    case eCharacterType::trojanHoplite:
        sInstance->mTrojanSwordHit.playRandomSound();
        break;

    case eCharacterType::aphrodite:
        sInstance->mAphrodite.playHit();
        break;
    case eCharacterType::apollo:
        sInstance->mApollo.playHit();
        break;
    case eCharacterType::ares:
        sInstance->mAres.playHit();
        break;
    case eCharacterType::artemis:
        sInstance->mArtemis.playHit();
        break;
    case eCharacterType::athena:
        sInstance->mAthena.playHit();
        break;
    case eCharacterType::atlas:
        sInstance->mAtlas.playHit();
        break;
    case eCharacterType::demeter:
        sInstance->mDemeter.playHit();
        break;
    case eCharacterType::dionysus:
        sInstance->mDionysus.playHit();
        break;
    case eCharacterType::hades:
        sInstance->mHades.playHit();
        break;
    case eCharacterType::hephaestus:
        sInstance->mHephaestus.playHit();
        break;
    case eCharacterType::hera:
        sInstance->mHera.playHit();
        break;
    case eCharacterType::hermes:
        sInstance->mHermes.playHit();
        break;
    case eCharacterType::poseidon:
        sInstance->mPoseidon.playHit();
        break;
    case eCharacterType::zeus:
        sInstance->mZeus.playHit();
        break;

    case eCharacterType::calydonianBoar:
        sInstance->mCalydonianBoar.playHit();
        break;
    case eCharacterType::cerberus:
        sInstance->mCerberus.playHit();
        break;
    case eCharacterType::chimera:
        sInstance->mChimera.playHit();
        break;
    case eCharacterType::cyclops:
        sInstance->mCyclops.playHit();
        break;
    case eCharacterType::dragon:
        sInstance->mDragon.playHit();
        break;
    case eCharacterType::echidna:
        sInstance->mEchidna.playHit();
        break;
    case eCharacterType::harpies:
        sInstance->mHarpies.playHit();
        break;
    case eCharacterType::hector:
        sInstance->mHector.playHit();
        break;
    case eCharacterType::hydra:
        sInstance->mHydra.playHit();
        break;
    case eCharacterType::kraken:
        sInstance->mKraken.playHit();
        break;
    case eCharacterType::maenads:
        sInstance->mMaenads.playHit();
        break;
    case eCharacterType::medusa:
        sInstance->mMedusa.playHit();
        break;
    case eCharacterType::minotaur:
        sInstance->mMinotaur.playHit();
        break;
    case eCharacterType::scylla:
        sInstance->mScylla.playHit();
        break;
    case eCharacterType::sphinx:
        sInstance->mSphinx.playHit();
        break;
    case eCharacterType::talos:
        sInstance->mTalos.playHit();
        break;

    case eCharacterType::satyr:
        sInstance->mSatyr.playHit();
        break;
    default:
        sInstance->mGenHit.playRandomSound();
        break;
    }
}

eSoundVector* eSounds::getCharacterVoices(eCharacter* const c) {
    const eCharacterType type =  c->type();
    switch(type) {
    case eCharacterType::settler: {
        const auto s = static_cast<eSettler*>(c);
        const bool emi = s->emigrant();
        if(emi) {
            return &sInstance->mHomelessVoices;
        } else {
            return &sInstance->mSettlerVoices;
        }
    } break;
    case eCharacterType::homeless:
        return &sInstance->mHomelessVoices;
    case eCharacterType::disgruntled:
        return &sInstance->mDisgruntledVoices;
    case eCharacterType::sick:
        return &sInstance->mSickVoices;

    case eCharacterType::aphrodite:
        return &sInstance->mAphroditeVoices;
    case eCharacterType::apollo:
        return &sInstance->mApolloVoices;
    case eCharacterType::ares:
        return &sInstance->mAresVoices;
    case eCharacterType::artemis:
        return &sInstance->mArtemisVoices;
    case eCharacterType::athena:
        return &sInstance->mAthenaVoices;
    case eCharacterType::atlas:
        return &sInstance->mAtlasVoices;
    case eCharacterType::demeter:
        return &sInstance->mDemeterVoices;
    case eCharacterType::dionysus:
        return &sInstance->mDionysusVoices;
    case eCharacterType::hades:
        return &sInstance->mHadesVoices;
    case eCharacterType::hephaestus:
        return &sInstance->mHephaestusVoices;
    case eCharacterType::hera:
        return &sInstance->mHeraVoices;
    case eCharacterType::hermes:
        return &sInstance->mHermesVoices;
    case eCharacterType::poseidon:
        return &sInstance->mPoseidonVoices;
    case eCharacterType::zeus:
        return &sInstance->mZeusVoices;

    case eCharacterType::achilles:
        return &sInstance->mAchillesVoices;
    case eCharacterType::atalanta:
        return &sInstance->mAtalantaVoices;
    case eCharacterType::bellerophon:
        return &sInstance->mBellerophonVoices;
    case eCharacterType::hercules:
        return &sInstance->mHerculesVoices;
    case eCharacterType::jason:
        return &sInstance->mJasonVoices;
    case eCharacterType::odysseus:
        return &sInstance->mOdysseusVoices;
    case eCharacterType::perseus:
        return &sInstance->mPerseusVoices;
    case eCharacterType::theseus:
        return &sInstance->mTheseusVoices;

    case eCharacterType::calydonianBoar:
        return &sInstance->mCalydonianBoarVoices;
    case eCharacterType::cerberus:
        return &sInstance->mCerberusVoices;
    case eCharacterType::chimera:
        return &sInstance->mChimeraVoices;
    case eCharacterType::cyclops:
        return &sInstance->mCyclopsVoices;
    case eCharacterType::dragon:
        return &sInstance->mDragonVoices;
    case eCharacterType::echidna:
        return &sInstance->mEchidnaVoices;
    case eCharacterType::harpies:
        return &sInstance->mHarpiesVoices;
    case eCharacterType::hector:
        return &sInstance->mHectorVoices;
    case eCharacterType::hydra:
        return &sInstance->mHydraVoices;
    case eCharacterType::kraken:
        return &sInstance->mKrakenVoices;
    case eCharacterType::maenads:
        return &sInstance->mMaenadsVoices;
    case eCharacterType::medusa:
        return &sInstance->mMedusaVoices;
    case eCharacterType::minotaur:
        return &sInstance->mMinotaurVoices;
    case eCharacterType::scylla:
        return &sInstance->mScyllaVoices;
    case eCharacterType::sphinx:
        return &sInstance->mSphinxVoices;
    case eCharacterType::talos:
        return &sInstance->mTalosVoices;
    case eCharacterType::satyr:
        return &sInstance->mSatyrVoices;

    case eCharacterType::marbleMiner:
        return &sInstance->mStoneMinerVoices;
    case eCharacterType::bronzeMiner:
        return &sInstance->mBronzeMinerVoices;
    case eCharacterType::silverMiner:
        return &sInstance->mSilverMinerVoices;
    case eCharacterType::fireFighter:
        return &sInstance->mFireFighterVoices;
    case eCharacterType::actor:
        return &sInstance->mActorVoices;
    case eCharacterType::philosopher:
        return &sInstance->mPhilosopherVoices;
    case eCharacterType::artisan:
        return &sInstance->mArtisanVoices;
    case eCharacterType::astronomer:
        return &sInstance->mAstronomerVoices;
    case eCharacterType::inventor:
        return &sInstance->mInventorVoices;
    case eCharacterType::curator:
        return &sInstance->mCuratorVoices;
    case eCharacterType::competitor:
        return &sInstance->mCompetitorVoices;
    case eCharacterType::gymnast:
        return &sInstance->mAthleteVoices;
    case eCharacterType::taxCollector:
        return &sInstance->mClerkVoices;
    case eCharacterType::waterDistributor:
        return &sInstance->mWaterDistributorVoices;
    case eCharacterType::hunter: {
        const auto h = static_cast<eHunter*>(c);
        const bool deer = h->deerHunter();
        if(deer) {
            return &sInstance->mDeerHunterVoices;
        } else {
            return &sInstance->mBoarHunterVoices;
        }
    } break;
    case eCharacterType::fishingBoat:
        return &sInstance->mFishingBoatVoices;
    case eCharacterType::grower: {
        const auto g = static_cast<eGrower*>(c);
        const auto type = g->growerType();
        switch(type) {
        case eGrowerType::grapesAndOlives:
            return &sInstance->mGrowerVoices;
        case eGrowerType::oranges:
            return &sInstance->mOrangeTenderVoices;
        }
    } break;
    case eCharacterType::goatherd:
        return &sInstance->mGoatherdVoices;
    case eCharacterType::shepherd:
        return &sInstance->mShepherdVoices;
    case eCharacterType::urchinGatherer:
        return &sInstance->mUrchinGathererVoices;
    case eCharacterType::healer:
        return &sInstance->mHealerVoices;
    case eCharacterType::lumberjack:
        return &sInstance->mLumberjackVoices;
    case eCharacterType::eliteCitizen:
        return &sInstance->mEliteVoices;
    case eCharacterType::peddler:
        return &sInstance->mPeddlerVoices;
    case eCharacterType::butcher:
        return &sInstance->mButcherVoices;
    case eCharacterType::scholar:
        return &sInstance->mScholarVoices;
    case eCharacterType::trader:
        return &sInstance->mTraderVoices;
    case eCharacterType::tradeBoat:
        return &sInstance->mTradeBoatVoices;
    case eCharacterType::trireme:
        return &sInstance->mTriremeVoices;
    case eCharacterType::watchman:
        return &sInstance->mWatchmanVoices;

    case eCharacterType::cartTransporter: {
        const auto ct = static_cast<eCartTransporter*>(c);
        const auto type = ct->cartType();
        switch(type) {
        case eCartTransporterType::food:
            return &sInstance->mFoodVendorVoices;
        case eCartTransporterType::fleece:
            return &sInstance->mFleeceVendorVoices;
        case eCartTransporterType::oil:
            return &sInstance->mOilVendorVoices;
        case eCartTransporterType::wine:
            return &sInstance->mWineVendorVoices;
        case eCartTransporterType::arms:
            return &sInstance->mArmorVendorVoices;
        case eCartTransporterType::horse:
            return &sInstance->mHorseVendorVoices;
        case eCartTransporterType::chariot:
            return &sInstance->mChariotVendorVoices;
        case eCartTransporterType::basic:
            return &sInstance->mCartTransporterVoices;
        default:
            return nullptr;
        }
    } break;
    case eCharacterType::chariotPoseidon:
        return &sInstance->mChariotVoices;
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon:
        return &sInstance->mHopliteVoices;
    case eCharacterType::horseman:
        return &sInstance->mHorsemanVoices;
    case eCharacterType::rockThrower:
        return &sInstance->mRockthrowerVoices;
    case eCharacterType::amazon:
        return &sInstance->mAmazonVoices;

    case eCharacterType::sheep:
        return &sInstance->mSheep;
    case eCharacterType::goat:
        return &sInstance->mGoat;
    case eCharacterType::wolf:
        return &sInstance->mWolf;
    case eCharacterType::deer:
        return &sInstance->mDeer;
    case eCharacterType::boar:
        return &sInstance->mBoar;
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        return &sInstance->mCattle;
    default:
        return nullptr;
    }
    return nullptr;
}

void eSounds::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    mAphrodite.load();
    mApollo.load();
    mAres.load();
    mArtemis.load();
    mAthena.load();
    mAtlas.load();
    mDemeter.load();
    mDionysus.load();
    mHades.load();
    mHephaestus.load();
    mHera.load();
    mHermes.load();
    mPoseidon.load();
    mZeus.load();

    mAchilles.load();
    mAtalanta.load();
    mBellerophon.load();
    mHercules.load();
    mJason.load();
    mOdysseus.load();
    mPerseus.load();
    mTheseus.load();

    mCalydonianBoar.load();
    mCerberus.load();
    mChimera.load();
    mCyclops.load();
    mDragon.load();
    mEchidna.load();
    mHarpies.load();
    mHector.load();
    mHydra.load();
    mKraken.load();
    mMaenads.load();
    mMedusa.load();
    mMinotaur.load();
    mScylla.load();
    mSphinx.load();
    mTalos.load();

    mSatyr.load();

    {
        const std::string layer1Dir{eGameDir::path("Audio/Ambient/Layer1/")};
        for(const auto& s : {"wind1.wav",
                             "wind2.wav",
                             "wind3.wav",
                             "wind4.wav",
                             "wind5.wav",
                             "wind6.wav",
                             "wind7.wav",
                             "shimmer3.wav",
                             "wind_sparkle2.wav",
                             "dark2.wav",
                             "bells.wav",
                             "space_bells7.wav",
                             "space_bells10.wav",
                             "space_bells12.wav",
                             "space_bells13.wav",
                             "space_bells14.wav",
                             "space_bells4a.wav",
                             "chime2.wav",
                             "voice.wav",
                             "shimmer6.wav",
                             "shimmer7.wav"}) {
            mEnvironment.addPath(layer1Dir + s);
        }
    }
    const std::string layer2Dir{eGameDir::path("Audio/Ambient/Layer2/")};

    mEarthquake.addPath(layer2Dir + "rumble.wav");

    for(const auto& s : {"lava1.wav",
                         "lava2.wav",
                         "lava3.wav"}) {
        mLava.addPath(layer2Dir + s);
    }

    for(const auto& s : {"maintenance1.wav",
                         "maintenance2.wav",
                         "maintenance3.wav",
                         "maintenance4.wav"}) {
        mMaintenance.addPath(layer2Dir + s);
    }

    for(const auto& s : {"wagon1.wav",
                         "wagon2.wav",
                         "wagon3.wav",
                         "city1.wav",
                         "city3.wav",
                         "city4.wav",
                         "city5.wav",
                         "city8.wav",
                         "city9.wav"}) {
        mCommonHousing.addPath(layer2Dir + s);
    }

    for(const auto& s : {"elite1.wav",
                         "elite2.wav",
                         "city2.wav",
                         "city7.wav"}) {
        mEliteHousing.addPath(layer2Dir + s);
    }

    for(const auto& s : {"dig3.wav",
                         "dig4.wav",
                         "dig5.wav",
                         "dig6.wav",
                         "dig7.wav",
                         "dig8.wav",
                         "bird1.wav",
                         "bird2.wav"}) {
        mFarming.addPath(layer2Dir + s);
    }

    for(const auto& s : {"rustling1.wav",
                         "rustling2.wav",
                         "orchard1.wav",
                         "orchard2.wav"}) {
        mOrchard.addPath(layer2Dir + s);
    }

    for(const auto& s : {"shearing1.wav",
                         "shearing2.wav",
                         "shearing3.wav"}) {
        mSheepFarm.addPath(layer2Dir + s);
    }

    for(const auto& s : {"goatherd1.wav",
                         "goatherd2.wav",
                         "goatherd3.wav"}) {
        mGoatFarm.addPath(layer2Dir + s);
    }

    for(const auto& s : {"dock1.wav",
                         "dock2.wav",
                         "dock3.wav",
                         "urchin1.wav",
                         "urchin2.wav",
                         "urchin3.wav"}) {
        mSea.addPath(layer2Dir + s);
    }

    for(const auto& s : {"trireme1.wav",
                         "trireme2.wav",
                         "trireme3.wav",
                         "trireme4.wav"}) {
        mTriremeWharf.addPath(layer2Dir + s);
    }

    for(const auto& s : {"hunting1.wav",
                         "hunting2.wav"}) {
        mHunting.addPath(layer2Dir + s);
    }
    for(const auto& s : {"corral1.wav",
                         "corral2.wav",
                         "corral3.wav"}) {
        mCorral.addPath(layer2Dir + s);
    }

    for(const auto& s : {"timber_mill1.wav",
                         "timber_mill2.wav",
                         "timber_mill3.wav"}) {
        mTimberMill.addPath(layer2Dir + s);
    }

    for(const auto& s : {"marble1.wav",
                         "marble2.wav"}) {
        mMarble.addPath(layer2Dir + s);
    }

    for(const auto& s : {"mint1.wav",
                         "mint2.wav",
                         "mint3.wav",
                         "mint4.wav"}) {
        mMint.addPath(layer2Dir + s);
    }

    for(const auto& s : {"foundry1.wav",
                         "foundry2.wav",
                         "foundry3.wav",
                         "foundry4.wav",
                         "foundry5.wav"}) {
        mFoundry.addPath(layer2Dir + s);
    }

    for(const auto& s : {"wagon4.wav",
                         "wagon5.wav",
                         "wagon6.wav",
                         "wagon7.wav"}) {
        mWorkshops.addPath(layer2Dir + s);
    }

    for(const auto& s : {"chest1.wav",
                         "chest2.wav",
                         "chest3.wav",
                         "shovel.wav",
                         "drop_wood.wav",
                         "storage_cart1.wav",
                         "warehouse.wav"}) {
        mStorage.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_food1.wav",
                         "agora_food2.wav",
                         "agora_food3.wav",
                         "agora_food4.wav"}) {
        mAgoraFood.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_fleece1.wav",
                         "agora_fleece2.wav"}) {
        mAgoraFleece.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_fleece1.wav",
                         "agora_fleece2.wav"}) {
        mAgoraFleece.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_oil.wav",
                         "agora_gen1.wav"}) {
        mAgoraOil.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_wine1.wav",
                         "agora_wine2.wav"}) {
        mAgoraWine.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_arms1.wav",
                         "agora_arms2.wav",
                         "agora_gen2.wav"}) {
        mAgoraArms.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_horse1.wav",
                         "agora_horse2.wav"}) {
        mAgoraHorse.addPath(layer2Dir + s);
    }

    for(const auto& s : {"agora_chariot1.wav",
                         "agora_chariot2.wav"}) {
        mAgoraChariot.addPath(layer2Dir + s);
    }

    for(const auto& s : {"trade1.wav",
                         "trade2.wav",
                         "trade3.wav",
                         "trade4.wav"}) {
        mTrade.addPath(layer2Dir + s);
    }

    for(const auto& s : {"tax1.wav",
                         "tax2.wav",
                         "tax3.wav"}) {
        mTaxes.addPath(layer2Dir + s);
    }

    for(const auto& s : {"palace1.wav",
                         "palace2.wav",
                         "palace3.wav"}) {
        mPalace.addPath(layer2Dir + s);
    }

    for(const auto& s : {"defensive1.wav",
                         "defensive2.wav",
                         "defensive3.wav"}) {
        mDefensive.addPath(layer2Dir + s);
    }

    for(const auto& s : {"infirmary1.wav",
                         "infirmary2.wav",
                         "infirmary3.wav",
                         "infirmary4.wav"}) {
        mInfirmary.addPath(layer2Dir + s);
    }

    for(const auto& s : {"drama1.wav",
                         "flying.wav",
                         "thtr_spear1.wav",
                         "thtr_spear2.wav",
                         "thtr_mumbl1.wav",
                         "thtr_mumbl2.wav",
                         "thtr_mumbl3.wav",
                         "thtr_women.wav",
        }) {
        mTheatre.addPath(layer2Dir + s);
    }

    for(const auto& s : {"thtr_spin1.wav",
                         "thtr_spin2.wav"}) {
        mDrama.addPath(layer2Dir + s);
    }

    for(const auto& s : {"Plato.wav",
                         "Aristotle.wav",
                         "Socrates.wav",
                         "college.wav",
                         "college2.wav",
                         "podium1.wav"}) {
        mPhilosophy.addPath(layer2Dir + s);
    }

    for(const auto& s : {"gym1.wav",
                         "gym2.wav",
                         "gym3.wav"}) {
        mGymnasium.addPath(layer2Dir + s);
    }

    for(const auto& s : {"cheer.wav",
                         "javelin.wav",
                         "wrestling.wav",
                         "cheer2.wav"}) {
        mStadium.addPath(layer2Dir + s);
    }

    for(const auto& s : {"sanctuary1.wav",
                         "sanctuary2.wav"}) {
        mSanctuary.addPath(layer2Dir + s);
    }

    for(const auto& s : {"armory1.wav",
                         "armory2.wav",
                         "armory3.wav"}) {
        mArmory.addPath(layer2Dir + s);
    }

    for(const auto& s : {"horse1.wav",
                         "horse2.wav",
                         "horse3.wav",
                         "horse4.wav",
                         "horse5.wav"}) {
        mHorseRanch.addPath(layer2Dir + s);
    }

    for(const auto& s : {"win_chime1.wav",
                         "win_chime2.wav",
                         "recreation1.wav"}) {
        mBeautification.addPath(layer2Dir + s);
    }

    for(const auto& s : {"fountain1.wav",
                         "fountain2.wav"}) {
        mFountain.addPath(layer2Dir + s);
    }

    for(const auto& s : {"artisan_guild1.wav",
                         "artisan_guild2.wav"}) {
        mArtisansGuild.addPath(layer2Dir + s);
    }




    for(const auto& s : {"bird7.wav",
                         "finch2.wav",
                         "sparrow1.wav",
                         "bird3.wav"}) {
        mMeadow.addPath(layer2Dir + s);
    }

    for(const auto& s : {"bird1.wav",
                         "bird2.wav",
                         "bird4.wav",
                         "rustling1.wav",
                         "rustling2.wav"}) {
        mFarmland.addPath(layer2Dir + s);
    }

    for(const auto& s : {"wave1.wav",
                         "wave2.wav",
                         "wave3.wav",
                         "wave4.wav"}) {
        mBeach.addPath(layer2Dir + s);
    }

    for(const auto& s : {"rumble.wav",
                         "rockslide1.wav",
                         "rockslide2.wav"}) {
        mRocky.addPath(layer2Dir + s);
    }

    for(const auto& s : {"bird3.wav",
                         "bird4.wav",
                         "bird5.wav"}) {
        mVegetation.addPath(layer2Dir + s);
    }

    for(const auto& s : {"water_space1.wav",
                         "water_space2.wav"}) {
        mWater.addPath(layer2Dir + s);
    }



    for(const auto& s : {"boar1.wav",
                         "boar2.wav",
                         "boar3.wav",
                         "boar4.wav"}) {
        mBoar.addPath(layer2Dir + s);
    }

    for(const auto& s : {"goat1.wav",
                         "goat2.wav",
                         "goat3.wav",
                         "goat4.wav"}) {
        mGoat.addPath(layer2Dir + s);
    }

    for(const auto& s : {"wolf1.wav",
                         "wolf2.wav",
                         "wolf3.wav",
                         "wolf4.wav"}) {
        mWolf.addPath(layer2Dir + s);
    }

    for(const auto& s : {"sheep1.wav",
                         "sheep2.wav",
                         "sheep3.wav",
                         "sheep4.wav"}) {
        mSheep.addPath(layer2Dir + s);
    }

    for(const auto& s : {"cattle1.wav",
                         "cattle2.wav",
                         "cattle3.wav",
                         "cattle4.wav"}) {
        mCattle.addPath(layer2Dir + s);
    }

    for(const auto& s : {"deer1.wav",
                         "deer2.wav",
                         "deer3.wav",
                         "deer4.wav"}) {
        mDeer.addPath(layer2Dir + s);
    }

    for(const auto& s : {"reading1.wav",
                         "reading2.wav",
                         "get_book.wav",
                         "close_book.wav"}) {
        mBibliotheke.addPath(layer2Dir + s);
    }

    for(const auto& s : {"university1.wav",
                         "university2.wav"}) {
        mUniversity.addPath(layer2Dir + s);
    }

    for(const auto& s : {"observatory1.wav",
                         "observatory2.wav"}) {
        mObservatory.addPath(layer2Dir + s);
    }

    for(const auto& s : {"invention1.wav",
                         "invention2.wav"}) {
        mInventorsWorkshop.addPath(layer2Dir + s);
    }

    for(const auto& s : {"lab1.wav",
                         "lab2.wav"}) {
        mLaboratory.addPath(layer2Dir + s);
    }

    for(const auto& s : {"museum1.wav",
                         "museum2.wav"}) {
        mMuseum.addPath(layer2Dir + s);
    }

    for(const auto& s : {"chariot_build1.wav",
                         "chariot_build2.wav"}) {
        mChariotFactory.addPath(layer2Dir + s);
    }

    for(const auto& s : {"fruit_lodge1.wav",
                         "fruit_lodge2.wav"}) {
        mFruitLodge.addPath(layer2Dir + s);
    }

    const std::string wavsDir{eGameDir::path("Audio/Wavs/")};

    mPlaceBuilding.addPath(wavsDir + "place_building.wav");

    mFire.addPath(wavsDir + "Fire.wav");
    mCollapse.addPath(wavsDir + "collapse.wav");

    mFireballHit.addPath(wavsDir + "Fireball_hit.wav");


    for(const auto& s : {"copperminer1.wav",
                         "copperminer2.wav"}) {
        mCopperMiner.addPath(wavsDir + s);
    }

    for(const auto& s : {"silverminer1.wav",
                         "silverminer2.wav"}) {
        mSilverMiner.addPath(wavsDir + s);
    }

    for(const auto& s : {"treecutter1.wav",
                         "treecutter2.wav"}) {
        mTreeCutter.addPath(wavsDir + s);
    }

    for(const auto& s : {"stonecutter1.wav",
                         "stonecutter2.wav"}) {
        mStoneCutter.addPath(wavsDir + s);
    }

    for(const auto& s : {"artisan1.wav",
                         "artisan2.wav"}) {
        mArtisan.addPath(wavsDir + s);
    }

    mActorDie.addPath(wavsDir + "actor_die.wav");
    mActorHit.addPath(wavsDir + "actor_hit.wav");

    for(const auto& s : {"am_warr_atk1.wav",
                         "am_warr_atk2.wav"}) {
        mRockthrowerAttack.addPath(wavsDir + s);
    }
    for(const auto& s : {"am_arch_die.wav",
                         "am_warr_die.wav"}) {
        mRockthrowerDie.addPath(wavsDir + s);
    }
    mRockthrowerHit.addPath(wavsDir + "am_warr_hit.wav");

    mBoarAttack.addPath(wavsDir + "boar_attack.wav");
    mBoarDie.addPath(wavsDir + "boar_die.wav");
    mBoarHit.addPath(wavsDir + "boar_hit.wav");

    mDeerAttack.addPath(wavsDir + "deer_attack.wav");
    mDeerDie.addPath(wavsDir + "deer_die.wav");
    mDeerHit.addPath(wavsDir + "deer_hit.wav");

    mWolfAttack.addPath(wavsDir + "wolf_attack.wav");
    mWolfDie.addPath(wavsDir + "wolf_die.wav");
    mWolfHit.addPath(wavsDir + "wolf_hit.wav");

    mSheepDie.addPath(wavsDir + "sheep_die.wav");
    mSheepHit.addPath(wavsDir + "sheep_hit.wav");

    mCattleAttack.addPath(wavsDir + "cattle_attack.wav");
    mCattleDie.addPath(wavsDir + "cattle_die.wav");
    mCattleHit.addPath(wavsDir + "cattle_hit.wav");

    mGoatAttack.addPath(wavsDir + "goat_attack.wav");
    mGoatDie.addPath(wavsDir + "goat_die.wav");
    mGoatHit.addPath(wavsDir + "goat_hit.wav");

    mOutlawAttack.addPath(wavsDir + "outlaw_attack.wav");
    mOutlawDie.addPath(wavsDir + "outlaw_die.wav");
    mOutlawHit.addPath(wavsDir + "outlaw_hit.wav");

    mArcherDie.addPath(wavsDir + "archer_die.wav");
    mArcherHit.addPath(wavsDir + "archer_hit.wav");
    for(const auto& s : {"archer_volley1.wav",
                         "archer_volley2.wav"}) {
        mArcherAttack.addPath(wavsDir + s);
    }

    for(const auto& s : {"Ares_warr_atk1.wav",
                         "Ares_warr_atk2.wav",
                         "Ares_warr_atk3.wav",
                         "Ares_warr_atk4.wav"}) {
        mAresWarriorAttack.addPath(wavsDir + s);
    }
    mAresWarriorHit.addPath(wavsDir + "Ares_warr_hit.wav");
    mAresWarriorDie.addPath(wavsDir + "Ares_warr_die.wav");

    mTrojanHorseDie.addPath(wavsDir + "troj_horse_die.wav");
    mTrojanHorseHit.addPath(wavsDir + "troj_horse_hit.wav");

    mTrojanSpearthrowerDie.addPath(wavsDir + "troj_jav_die.wav");
    mTrojanSpearthrowerHit.addPath(wavsDir + "troj_jav_hit.wav");
    for(const auto& s : {"troj_jav_throw1.wav",
                         "troj_jav_throw2.wav"}) {
        mTrojanSpearthrowerAttack.addPath(wavsDir + s);
    }

    mTrojanSwordDie.addPath(wavsDir + "troj_sword_die.wav");
    mTrojanSwordHit.addPath(wavsDir + "troj_sword_hit.wav");
    for(const auto& s : {"troj_sword1.wav",
                         "troj_sword2.wav"}) {
        mTrojanSwordAttack.addPath(wavsDir + s);
    }

    for(const auto& s : {"gen_die1.wav",
                         "gen_die2.wav",
                         "gen_die3.wav",
                         "gen_die4.wav",
                         "gen_die5.wav",
                         "gen_die6.wav",
                         "gen_die7.wav",
                         "gen_die8.wav"}) {
        mGenDie.addPath(wavsDir + s);
    }

    for(const auto& s : {"gen_hit1.wav",
                         "gen_hit2.wav",
                         "gen_hit3.wav",
                         "gen_hit4.wav",
                         "gen_hit5.wav",
                         "gen_hit6.wav",
                         "gen_hit7.wav",
                         "gen_hit8.wav"}) {
        mGenHit.addPath(wavsDir + s);
    }


    const std::string walkerDir{eGameDir::path("Audio/Voice/Walker/")};

    const auto loadGodVoices = [&](const std::string& pref,
                                   eSoundVector& vec) {
        for(const auto& s : {pref + "_e_1.mp3",
                             pref + "_e_2.mp3",
                             pref + "_e_3.mp3"}) {
            vec.addPath(walkerDir + s);
        }
        const auto e4Path = pref + "_e_4.mp3";
        const bool e4 = std::filesystem::exists(eFsPath::sPath(e4Path));
        if(e4) vec.addPath(e4Path);
    };

    loadGodVoices("Aph", mAphroditeVoices);
    loadGodVoices("Apo", mApolloVoices);
    loadGodVoices("Are", mAresVoices);
    loadGodVoices("Atm", mArtemisVoices);
    loadGodVoices("Atn", mAthenaVoices);
    loadGodVoices("Ats", mAtlasVoices);
    loadGodVoices("Dem", mDemeterVoices);
    loadGodVoices("Dio", mDionysusVoices);
    loadGodVoices("Had", mHadesVoices);
    loadGodVoices("Hep", mHephaestusVoices);
    loadGodVoices("Hra", mHeraVoices);
    loadGodVoices("Her", mHermesVoices);
    loadGodVoices("Pos", mPoseidonVoices);
    loadGodVoices("Zeu", mZeusVoices);

    const auto loadVoices = [&](const std::string& pref,
                                eSoundVector& vec,
                                const int iMin,
                                const int iMax) {
        for(int i = iMin; i <= iMax; i++) {
            const auto iStr = std::to_string(i);
            vec.addPath(walkerDir + pref + iStr + ".mp3");
        }
    };

    const auto loadEVoices = [&](const std::string& pref,
                                 eSoundVector& vec,
                                 const int iMin,
                                 const int iMax) {
        loadVoices(pref + "_e_", vec, iMin, iMax);
    };

    const auto loadGVoices = [&](const std::string& pref,
                                eSoundVector& vec) {
        loadVoices(pref + "_g_", vec, 1, 7);
    };

    const auto loadIVoice = [&](const std::string& pref,
                                eSoundVector& vec) {
        loadVoices(pref + "_i_", vec, 1, 1);
    };

    const auto loadHeroVoices = [&](const std::string& pref,
                                    eSoundVector& vec) {
        loadGVoices(pref, vec);
        vec.addPath(walkerDir + pref + "_e_1.mp3");
        vec.addPath(walkerDir + pref + "_i_1.mp3");
    };

    loadHeroVoices("Ach", mAchillesVoices);
    loadHeroVoices("Ata", mAtalantaVoices);
    loadHeroVoices("Bel", mBellerophonVoices);
    loadHeroVoices("Hec", mHerculesVoices);
    loadHeroVoices("Jas", mJasonVoices);
    loadHeroVoices("Ody", mOdysseusVoices);
    loadHeroVoices("Per", mPerseusVoices);
    loadHeroVoices("The", mTheseusVoices);

    const auto loadMonsterVoices = [&](const std::string& pref,
                                       eSoundVector& vec) {
        vec.addPath(walkerDir + pref + ".mp3");
    };

    loadMonsterVoices("M_cal", mCalydonianBoarVoices);
    loadMonsterVoices("M_cer", mCerberusVoices);
    loadMonsterVoices("M_chi", mChimeraVoices);
    loadMonsterVoices("M_cyc", mCyclopsVoices);
    loadMonsterVoices("M_dra", mDragonVoices);
    loadMonsterVoices("M_ech", mEchidnaVoices);
    loadMonsterVoices("M_har", mHarpiesVoices);
    loadMonsterVoices("M_hec", mHectorVoices);
    loadMonsterVoices("M_hyd", mHydraVoices);
    loadMonsterVoices("M_kra", mKrakenVoices);
    loadMonsterVoices("M_mae", mMaenadsVoices);
    loadMonsterVoices("M_med", mMedusaVoices);
    loadMonsterVoices("M_min", mMinotaurVoices);
    loadMonsterVoices("M_scy", mScyllaVoices);
    loadMonsterVoices("M_sph", mSphinxVoices);
    loadMonsterVoices("M_tal", mTalosVoices);
    loadMonsterVoices("sat_e_1", mSatyrVoices);

    loadIVoice("imm", mSettlerVoices);
    loadEVoices("imm", mSettlerVoices, 1, 3);

    loadIVoice("emi", mHomelessVoices);
    loadEVoices("emi", mHomelessVoices, 1, 7);

    // loadIVoice("ovn", mDisgruntledVoices); // 206
    // loadEVoices("ovn", mDisgruntledVoices, 1, 7);

    // loadIVoice("oth", mDisgruntledVoices); // 207
    // loadEVoices("oth", mDisgruntledVoices, 1, 7);

    loadIVoice("omg", mDisgruntledVoices); // 208
    loadEVoices("omg", mDisgruntledVoices, 1, 7);

    // loadIVoice("ods", mDisgruntledVoices); // 209
    // loadEVoices("ods", mDisgruntledVoices, 1, 7);

    loadIVoice("pla", mSickVoices);
    loadEVoices("pla", mSickVoices, 1, 1);

    loadGVoices("stn", mStoneMinerVoices);
    loadIVoice("stn", mStoneMinerVoices);
    loadEVoices("stn", mStoneMinerVoices, 1, 3);

    loadGVoices("act", mActorVoices);
    loadIVoice("act", mActorVoices);
    loadEVoices("act", mActorVoices, 1, 4);

    loadGVoices("phl", mPhilosopherVoices);
    loadIVoice("phl", mPhilosopherVoices);
    loadEVoices("phl", mPhilosopherVoices, 1, 4);

    loadGVoices("ast", mAstronomerVoices);
    loadIVoice("ast", mAstronomerVoices);

    loadGVoices("inv", mInventorVoices);
    loadIVoice("inv", mInventorVoices);

    loadGVoices("ath", mAthleteVoices);
    loadIVoice("ath", mAthleteVoices);
    loadEVoices("ath", mAthleteVoices, 1, 4);

    loadGVoices("cmp", mCompetitorVoices);
    loadIVoice("cmp", mCompetitorVoices);
    loadEVoices("cmp", mCompetitorVoices, 1, 4);

    loadGVoices("cop", mBronzeMinerVoices);
    loadIVoice("cop", mBronzeMinerVoices);
    loadEVoices("cop", mBronzeMinerVoices, 1, 2);

    loadGVoices("slv", mSilverMinerVoices);
    loadIVoice("slv", mSilverMinerVoices);
    loadEVoices("slv", mSilverMinerVoices, 1, 2);

    loadGVoices("sup", mFireFighterVoices);
    loadIVoice("sup", mFireFighterVoices);
    loadEVoices("sup", mFireFighterVoices, 1, 5);

    loadGVoices("art", mArtisanVoices);
    loadIVoice("art", mArtisanVoices);
    loadEVoices("art", mArtisanVoices, 1, 4);

    loadGVoices("bht", mBoarHunterVoices);
    loadIVoice("bht", mBoarHunterVoices);
    loadEVoices("bht", mBoarHunterVoices, 1, 2);

    loadGVoices("dht", mDeerHunterVoices);
    loadIVoice("dht", mDeerHunterVoices);
    loadEVoices("dht", mDeerHunterVoices, 1, 2);

    loadGVoices("fsh", mFishingBoatVoices);
    loadIVoice("fsh", mFishingBoatVoices);
    loadEVoices("fsh", mFishingBoatVoices, 1, 4);

    loadGVoices("gro", mGrowerVoices);
    loadIVoice("gro", mGrowerVoices);
    loadEVoices("gro", mGrowerVoices, 1, 4);

    loadGVoices("tdr", mOrangeTenderVoices);
    loadIVoice("tdr", mOrangeTenderVoices);
    loadEVoices("tdr", mOrangeTenderVoices, 1, 2);

    loadGVoices("lmb", mLumberjackVoices);
    loadIVoice("lmb", mLumberjackVoices);
    loadEVoices("lmb", mLumberjackVoices, 1, 2);

    loadGVoices("gth", mGoatherdVoices);
    loadIVoice("gth", mGoatherdVoices);
    loadEVoices("gth", mGoatherdVoices, 1, 4);

    loadGVoices("urc", mUrchinGathererVoices);
    loadIVoice("urc", mUrchinGathererVoices);
    loadEVoices("urc", mUrchinGathererVoices, 1, 4);

    loadGVoices("shp", mShepherdVoices);
    loadIVoice("shp", mShepherdVoices);
    loadEVoices("shp", mShepherdVoices, 1, 4);

    loadGVoices("rch", mButcherVoices);
    loadIVoice("rch", mButcherVoices);
    loadEVoices("rch", mButcherVoices, 1, 3);

    loadGVoices("hlr", mHealerVoices);
    loadIVoice("hlr", mHealerVoices);
    loadEVoices("hlr", mHealerVoices, 1, 1);

    loadIVoice("pdl", mPeddlerVoices);
    loadEVoices("pdl", mPeddlerVoices, 1, 6);

    loadGVoices("nob", mEliteVoices);
    loadIVoice("nob", mEliteVoices);
    loadEVoices("nob", mEliteVoices, 1, 1);

    loadIVoice("arv", mArmorVendorVoices);
    loadEVoices("arv", mArmorVendorVoices, 1, 2);

    loadIVoice("fdv", mFoodVendorVoices);
    loadEVoices("fdv", mFoodVendorVoices, 1, 2);

    loadIVoice("flv", mFleeceVendorVoices);
    loadEVoices("flv", mFleeceVendorVoices, 1, 2);

    loadIVoice("oiv", mOilVendorVoices);
    loadEVoices("oiv", mOilVendorVoices, 1, 2);

    loadIVoice("wiv", mWineVendorVoices);
    loadEVoices("wiv", mWineVendorVoices, 1, 2);

    loadIVoice("chv", mChariotVendorVoices);
    loadEVoices("chv", mChariotVendorVoices, 1, 2);

    loadIVoice("htr", mHorseVendorVoices);
    loadEVoices("htr", mHorseVendorVoices, 1, 2);

    loadIVoice("crt", mCartTransporterVoices);
    loadEVoices("crt", mCartTransporterVoices, 1, 4);

    loadIVoice("trd", mTraderVoices);
    loadVoices("trd_w_", mTraderVoices, 1, 5);
    loadEVoices("trd", mTraderVoices, 1, 2);

    loadIVoice("trs", mTradeBoatVoices);
    loadVoices("trs_w_", mTradeBoatVoices, 1, 5);
    loadEVoices("trs", mTradeBoatVoices, 1, 2);

    loadIVoice("tri", mTriremeVoices);
    loadEVoices("tri", mTriremeVoices, 1, 4);

    loadIVoice("cht", mChariotVoices);
    loadEVoices("cht", mChariotVoices, 1, 4);

    loadIVoice("hpl", mHopliteVoices);
    loadEVoices("hpl", mHopliteVoices, 1, 4);

    loadIVoice("hsm", mHorsemanVoices);
    loadEVoices("hsm", mHorsemanVoices, 1, 4);

    loadIVoice("rbl", mRockthrowerVoices);
    loadEVoices("rbl", mRockthrowerVoices, 1, 4);

    loadIVoice("acf", mAmazonVoices);
    loadEVoices("acf", mAmazonVoices, 1, 4);

    loadGVoices("clk", mClerkVoices);
    loadIVoice("clk", mClerkVoices);

    loadGVoices("cur", mCuratorVoices);
    loadIVoice("cur", mCuratorVoices);

    loadGVoices("wtb", mWaterDistributorVoices);
    loadIVoice("wtb", mWaterDistributorVoices);

    loadGVoices("sch", mScholarVoices);
    loadIVoice("sch", mScholarVoices);

    loadIVoice("pre", mPriestVoices);
    loadEVoices("pre", mPriestVoices, 1, 6);

    loadIVoice("wtc", mWatchmanVoices);
    loadEVoices("wtc", mWatchmanVoices, 1, 5);
}
