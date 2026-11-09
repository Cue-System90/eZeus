#ifndef EGAMEBOARD_H
#define EGAMEBOARD_H

#include <vector>
#include "elimits.h"
#include "etile.h"

#include "boardData/epopulationdata.h"
#include "boardData/ehusbandrydata.h"
#include "boardData/eemploymentdata.h"
#include "boardData/eheatmap.h"

#include "edifficulty.h"
#include "edate.h"

#include "eresourcetype.h"

#include "pointers/eobject.h"

#include "ethreadpool.h"
#include "eplannedaction.h"

#include "eworldboard.h"
#include "widgets/ebuildingmode.h"

#include "characters/gods/egod.h"
#include "characters/monsters/emonster.h"
#include "buildings/sanctuaries/esanctuary.h"

#include "buildings/eavailablebuildings.h"

#include "characters/eenlistedforces.h"

#include "engine/egodquest.h"
#include "ecityrequest.h"
#include "gameEvents/egodquestevent.h"
#include "eepisodegoal.h"
#include "eemploymentdistributor.h"

#include "eboardcity.h"
#include "eboardplayer.h"

class eGameEvent;

class eSpawner;
enum class eBannerTypeS;
class eCharacter;
class eBuilding;
class eTradePost;
class eStorageBuilding;
class eSoldierBanner;
class ePalace;
class eLandInvasionPoint;
class eReceiveRequestEvent;
class eInvasionEvent;
class eAgoraBase;
class eHerosHall;
class ePyramid;
class eTrireme;

class eThreadPool;

class eSoldier;
class eInvasionHandler;
class eMonsterPoint;
class ePlayerConquestEventBase;
class eArmyEventBase;
class eTroopsRequestEvent;
enum class eMessageEventType;
class eMonsterInvasionEventBase;

struct eMessageType;
struct eEventData;
enum class eEvent;
class ePlague;
class eSmallHouse;
class eMuseum;
class eStadium;
struct eEpisode;

using eAction = std::function<void()>;

class eDistrictIdTmp {
public:
    eDistrictIdTmp(eGameBoard& board);
    ~eDistrictIdTmp();
private:
    eGameBoard& mBoard;
    const int mTmpId;
};

class eGameBoard : public eStdSelfRef {
public:
    eGameBoard(eWorldBoard& world);
    ~eGameBoard();

    void initialize(const int w, const int h);
    void resize(const int w, const int h);
    void clear();

    void setWorldDirection(const eWorldDirection dir);

    eTile* rotateddtile(const int x, const int y) const;

    eWorldDirection direction() const { return mDirection; }

    int rotatedWidth() const;
    int rotatedHeight() const;

    eTile* tile(const int x, const int y) const;
    eTile* dtile(const int x, const int y) const;

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    double taxRateF(const eCityId cid) const;
    eTaxRate taxRate(const eCityId cid) const;
    eWageRate wageRate(const eCityId cid) const;

    void setTaxRate(const eCityId cid, const eTaxRate tr);
    void setWageRate(const eCityId cid, const eWageRate wr);

    int taxesPaidThisYear(const eCityId cid) const;
    int taxesPaidLastYear(const eCityId cid) const;
    int peoplePaidTaxesThisYear(const eCityId cid) const;
    int peoplePaidTaxesLastYear(const eCityId cid) const;

    int philosophyResearchCoverage(const eCityId cid) const;
    int athleticsLearningCoverage(const eCityId cid) const;
    int dramaAstronomyCoverage(const eCityId cid) const;
    int allCultureScienceCoverage(const eCityId cid) const;
    int taxesCoverage(const eCityId cid) const;
    int unrest(const eCityId cid) const;
    int popularity(const eCityId cid) const;
    int health(const eCityId cid) const;
    eCityFinances finances(const eCityId cid) const;
    int maintanance(const eCityId cid) const;

    int drachmas(const ePlayerId pid) const;
    void incDrachmas(const ePlayerId pid, const int by,
                     const eFinanceTarget t);
    void setDrachmas(const ePlayerId pid, const int to);

    void registerCharacter(eCharacter* const c);
    bool unregisterCharacter(eCharacter* const c);
    const std::vector<eCharacter*>& characters() const
    { return mCharacters; }

    void registerCharacterAction(eCharacterAction* const ca);
    bool unregisterCharacterAction(eCharacterAction* const ca);

    void registerSoldier(eSoldier* const c);
    bool unregisterSoldier(eSoldier* const c);

    void registerBuilding(eBuilding* const b);
    bool unregisterBuilding(eBuilding* const b);

    bool unregisterCommonHouse(eSmallHouse* const ch);

    void registerEmplBuilding(eEmployingBuilding* const b);
    bool unregisterEmplBuilding(eEmployingBuilding* const b);

    void registerTradePost(eTradePost* const b);
    bool unregisterTradePost(eTradePost* const b);
    bool hasTradePost(const eCityId cid, const eWorldCity& city);

    void registerSpawner(eSpawner* const s);
    bool unregisterSpawner(eSpawner* const s);

    void registerStadium(eStadium* const s);
    void unregisterStadium(const eCityId cid);

    void registerMuseum(eMuseum* const s);
    void unregisterMuseum(const eCityId cid);

    void registerStorBuilding(eStorageBuilding* const b);
    bool unregisterStorBuilding(eStorageBuilding* const b);

    void registerSanctuary(eSanctuary* const b);
    bool unregisterSanctuary(eSanctuary* const b);

    void registerMonument(eMonument* const b);
    bool unregisterMonument(eMonument* const b);

    void registerHeroHall(eHerosHall* const b);
    bool unregisterHeroHall(eHerosHall* const b);

    void registerMissile(eMissile* const m);
    bool unregisterMissile(eMissile* const m);

    bool hasStadium(const eCityId cid) const;
    bool hasMuseum(const eCityId cid) const;
    eStadium* stadium(const eCityId cid) const;
    eMuseum* museum(const eCityId cid) const;

    void registerPalace(ePalace* const p);
    void unregisterPalace(const eCityId cid);

    void registerMonster(const eCityId cid, eMonster* const m);
    void unregisterMonster(const eCityId cid, eMonster* const m);

    eBanner* banner(const eCityId cid,
                    const eBannerTypeS type,
                    const int id = 1) const;
    void registerBanner(eBanner* const b);
    void unregisterBanner(eBanner* const b);

    void registerAllSoldierBanner(eSoldierBanner* const b);
    void unregisterAllSoldierBanner(eSoldierBanner* const b);    

    bool manTowers(const eCityId cid) const;
    void setManTowers(const eCityId cid, const bool m);

    std::vector<eAgoraBase*> agoras(const eCityId cid) const;

    void updateResources(const eCityId cid);
    using eResources = std::vector<std::pair<eResourceType, int>>;
    const eResources* resources(const eCityId cid) const;

    ePalace* palace(const eCityId cid) const;
    bool hasPalace(const eCityId cid) const;

    int eliteHouses(const eCityId cid) const;

    int maxSanctuaries(const eCityId cid) const;
    std::vector<eSanctuary*> sanctuaries(const eCityId cid) const;
    eSanctuary* sanctuary(const eCityId cid, const eGodType god) const;

    std::vector<ePyramid*> pyramids(const eCityId cid) const;
    ePyramid* pyramid(const eCityId cid, const eBuildingType type) const;

    std::vector<eHerosHall*> heroHalls(const eCityId cid) const;
    eHerosHall* heroHall(const eCityId cid, const eHeroType hero) const;

    std::vector<stdsptr<eSoldierBanner>> banners(const eCityId cid) const;
    int countBanners(const eBannerType bt, const eCityId cid) const;
    int countSoldiers(const eBannerType bt, const eCityId cid) const;
    int countWorkingTriremes(const eCityId cid) const;

    int state() const { return mState; }
    void incState() { mState++; }
    void incTime(const int by);
    void incFrame();
    int frame() const { return mFrame; }
    void handleFinishedTasks();
    void scheduleDataUpdate();
    int totalTime() const { return mTotalTime; }

    eThreadPool& threadPool() { return mThreadPool; }

    int population(const ePlayerId pid) const;
    int population(const eCityId cid) const;

    ePopulationData* populationData(const eCityId cid);
    eHusbandryData* husbandryData(const eCityId cid);
    eEmploymentData* employmentData(const eCityId cid);
    eHeatMap& appealMap() { return mAppealMap; }

    eEmploymentDistributor* employmentDistributor(const eCityId cid) const;

    void addShutDown(const eCityId cid, const eResourceType type);
    void removeShutDown(const eCityId cid, const eResourceType type);
    int industryJobVacancies(const eCityId cid, const eResourceType type) const;

    void distributeEmployees(const eCityId cid, const eSector s);
    void distributeEmployees(const eCityId cid);

    bool isShutDown(const eCityId cid,
                    const eResourceType type) const;
    bool isShutDown(const eCityId cid,
                    const eBuildingType type) const;

    void payTaxes(const eCityId cid, const int d, const int people);
    void setDifficulty(const eDifficulty d);
    void setDifficulty(const ePlayerId pid, const eDifficulty d);
    eDifficulty personPlayerDifficulty() const;
    eDifficulty difficulty(const ePlayerId pid) const;
    const eDate& date() const { return mDate; }
    void setDate(const eDate& d);

    double appeal(const int tx, const int ty) const;

    void addRubbish(const stdsptr<eObject>& o);
    void emptyRubbish();

    using eEventHandler = std::function<void(eEvent, eEventData&)>;
    void setEventHandler(const eEventHandler& eh);
    void event(const eEvent e, eEventData& ed);
    void setEpisodeFinishedHandler(const eAction& a);
    void setAutosaver(const eAction& a);

    void setRequestUpdateHandler(const eAction& ru);

    using eVisibilityChecker = std::function<bool(eTile*)>;
    void setVisibilityChecker(const eVisibilityChecker& vc);

    using eTipShower = std::function<void(const ePlayerCityTarget&,
                                          const std::string&)>;
    void setTipShower(const eTipShower& ts);
    void showTip(const ePlayerCityTarget& target,
                 const std::string& tip) const;

    using eEnlistAction = std::function<void(const eEnlistedForces&, eResourceType)>;
    using eEnlistRequest = std::function<void(const eEnlistedForces& enlistable,
                                              const std::vector<eCityId>& cids,
                                              const std::vector<std::string>& cnames,
                                              const std::vector<eHeroType>& heroesAbroad,
                                              const eEnlistAction& action,
                                              const std::vector<eResourceType>& plunderResources)>;
    void setEnlistForcesRequest(const eEnlistRequest& req);
    void requestForces(const eEnlistAction& action,
                       const std::vector<eResourceType>& plunderResources = {},
                       const std::vector<stdsptr<eWorldCity>>& exclude = {},
                       const bool onlySoldiers = false);

    using eAction = std::function<void()>;
    bool ifVisible(eTile* const tile, const eAction& func) const;

    using eMessageShower = std::function<void(
                eEventData&, const eMessageType&)>;
    void setMessageShower(const eMessageShower& msg);

    void showMessage(eEventData& ed, const eMessageType& msg);

    using eTileAction = std::function<void(eTile* const)>;
    void iterateOverAllTiles(const eTileAction& a);

    void scheduleAppealMapUpdate(const eCityId cid);
    void updateAppealMapIfNeeded();

    eWorldBoard& world() const { return mWorld; }

    void enlistForces(const eEnlistedForces& forces);

    void clearBannerSelection();
    void deselectBanner(eSoldierBanner* const c);
    void selectBanner(eSoldierBanner* const c);
    const std::vector<eSoldierBanner*>& selectedSoldiers() const
    { return mSelectedBanners; }

    void clearTriremeSelection();
    void deselectTrireme(eTrireme* const c);
    void selectTrireme(eTrireme* const c);
    const std::vector<eTrireme*>& selectedTriremes() const
    { return mSelectedTriremes; }

    void bannersGoHome();
    void bannersBackFromHome();

    void setRegisterBuildingsEnabled(const bool e);

    void setButtonsVisUpdater(const eAction& u);

    eBuilding* buildingAt(const int x, const int y) const;

    using eBuildingValidator = std::function<bool(eBuilding*)>;
    std::vector<eBuilding*> buildings(const eCityId cid,
                                      const eBuildingValidator& v) const;
    std::vector<eBuilding*> buildings(const eCityId cid,
                                      const eBuildingType type) const;
    int countBuildings(const eCityId cid,
                       const eBuildingValidator& v) const;
    int countBuildings(const eCityId cid,
                       const eBuildingType t) const;
    bool hasBuilding(const eCityId cid,
                     const eBuildingType t) const;
    int countAllowed(const eCityId cid,
                     const eBuildingType t) const;
    eBuilding* randomBuilding(const eCityId cid,
                              const eBuildingValidator& v) const;
    std::vector<eBuilding*> commemorativeBuildings(
            const eCityId cid) const;

    bool supportsBuilding(const eCityId cid,
                          const eBuildingMode mode) const;
    bool availableBuilding(const eCityId cid,
                           const eBuildingType type,
                           const int id = -1) const;
    void built(const eCityId cid,
               const eBuildingType type,
               const int id = -1);
    void destroyed(const eCityId cid,
                   const eBuildingType type,
                   const int id = -1);
    void allow(const eCityId cid,
               const eBuildingType type,
               const int id = -1);
    void disallow(const eCityId cid,
                  const eBuildingType type,
                  const int id = -1);
    void updateButtonsVisibility();
    bool registerBuildingsEnabled() const
    { return mRegisterBuildingsEnabled; }

    bool supportsResource(const eCityId cid, const eResourceType rt) const;
    eResourceType supportedResources(const eCityId cid) const;

    int wonGames(const eCityId cid) const;

    int horses() const;

    void planAction(ePlannedAction* const a);

    void restockMarbleTiles();
    void updateMarbleTiles();

    void restockBlackMarbleTiles();
    void updateBlackMarbleTiles();

    void allowHero(const eCityId cid, const eHeroType heroType,
                   const std::string& reason = "");

    void addRootGameEvent(const stdsptr<eGameEvent>& e);
    void removeRootGameEvent(const stdsptr<eGameEvent>& e);

    void addGameEvent(eGameEvent* const e);
    void removeGameEvent(eGameEvent* const e);

    using eGoals = std::vector<stdsptr<eEpisodeGoal>>;
    const eGoals& goals() const { return mGoals; }
    // without goals an episode never ends
    void clearGoals() { mGoals.clear(); }

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    eBuilding* buildingWithIOID(const int id) const;
    eCharacter* characterWithIOID(const int id) const;
    eCharacterAction* characterActionWithIOID(const int id) const;
    eBanner* bannerWithIOID(const int id) const;
    eSoldierBanner* soldierBannerWithIOID(const int id) const;
    eGameEvent* eventWithIOID(const int id) const;
    eInvasionHandler* invasionHandlerWithIOID(const int id) const;

    eTile* monsterTile(const eCityId cid,
                       const int id) const;
    eTile* landInvasionTile(const eCityId cid,
                            const int id) const;
    eTile* seaInvasionTile(const eCityId cid,
                           const int id) const;
    eTile* invasionTile(const eCityId cid,
                        const int id) const;
    eTile* disasterTile(const eCityId cid,
                        const int id) const;
    eTile* landSlideTile(const eCityId cid,
                         const int id) const;

    std::vector<eInvasionHandler*> invasionHandlers(
            const eCityId cid) const;
    void addInvasionHandler(const eCityId cid,
                            eInvasionHandler* const i);
    void removeInvasionHandler(const eCityId cid,
                               eInvasionHandler* const i);
    bool hasActiveInvasions(const eCityId cid) const;

    int addResource(const eCityId cid,
                    const eResourceType type,
                    const int count);
    int spaceForResource(const eCityId cid,
                         const eResourceType type) const;
    int maxMonumentSpaceForResource(const eCityId cid,
                                    eMonument **b) const;
    int maxSingleSpaceForResource(const eCityId cid,
                                  const eResourceType type,
                                  eStorageBuilding** b) const;
    int resourceCount(const eCityId cid,
                      const eResourceType type) const;
    int takeResource(const eCityId cid,
                     const eResourceType type,
                     const int count);
    void request(const stdsptr<eWorldCity>& c,
                 const eResourceType type,
                 const eCityId cid);
    void requestAid(const stdsptr<eWorldCity>& c,
                    const eCityId cid);
    void planGiftFrom(const stdsptr<eWorldCity>& c,
                      const eResourceType type,
                      const int count,
                      const int delay);
    void tributeFrom(const ePlayerId pid,
                     const stdsptr<eWorldCity>& c,
                     const bool postpone);
    bool giftTo(const stdsptr<eWorldCity>& c,
                const eResourceType type,
                const int count, const eCityId cid);
    void giftToReceived(const stdsptr<eWorldCity>& c,
                        const eResourceType type,
                        const int count,
                        const ePlayerId pid);

    void waitUntilFinished();

    void registerSoldierBanner(const stdsptr<eSoldierBanner>& b);
    bool unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b);

    void killCommonFolks(const eCityId cid, int toKill);
    void walkerKilled(const eCityId cid);
    void rockThrowerKilled(const eCityId cid);
    void hopliteKilled(const eCityId cid);
    void horsemanKilled(const eCityId cid);

    eEnlistedForces getEnlistableForces(const ePlayerId pid) const;

    void addMonsterEvent(const eMonsterType type, eMonsterInvasionEventBase* const e);
    void removeMonsterEvent(eMonsterInvasionEventBase* const e);

    using eQuests = std::vector<eGodQuestEvent*>;
    eQuests godQuests(const ePlayerId pid) const;
    void addGodQuest(eGodQuestEvent* const q);
    void removeGodQuest(eGodQuestEvent* const q);

    using eRequests = std::vector<eReceiveRequestEvent*>;
    eRequests cityRequests(const ePlayerId pid) const;
    void addCityRequest(eReceiveRequestEvent* const q);
    void removeCityRequest(eReceiveRequestEvent* const q);

    using eTroopsRequests = std::vector<eTroopsRequestEvent*>;
    eTroopsRequests cityTroopsRequests(const ePlayerId pid) const;
    void addCityTroopsRequest(eTroopsRequestEvent* const q);
    void removeCityTroopsRequest(eTroopsRequestEvent* const q);

    using eConquests = std::vector<ePlayerConquestEventBase*>;
    eConquests conquests(const ePlayerId pid) const;
    void addConquest(ePlayerConquestEventBase* const q);
    void removeConquest(ePlayerConquestEventBase* const q);

    using eInvasions = std::vector<eInvasionEvent*>;
    const eInvasions& invasions() const { return mInvasions; }
    eInvasionEvent* invasionToDefend(const eCityId cid) const;
    void addInvasion(eInvasionEvent* const i);
    void removeInvasion(eInvasionEvent* const i);

    using eArmyEvents = std::vector<eArmyEventBase*>;
    eArmyEvents armyEvents() const;
    void addArmyEvent(eArmyEventBase* const q);
    void removeArmyEvent(eArmyEventBase* const q);

    std::vector<eMonster*> monsters(const eCityId cid) const;
    using eChars = std::vector<eCharacter*>;
    eChars attackingGods(const eCityId cid) const;
    void registerAttackingGod(const eCityId cid, eCharacter* const c);

    void startPlague(eSmallHouse* const h);
    stdsptr<ePlague> plagueForHouse(eSmallHouse* const h);
    void healPlague(const stdsptr<ePlague>& p);
    void healHouse(eSmallHouse* const h);
    using ePlagues = std::vector<stdsptr<ePlague>>;
    ePlagues plagues(const eCityId cid) const;
    stdsptr<ePlague> nearestPlague(const eCityId cid,
                                   const int tx, const int ty,
                                   int& dist) const;

    void updateMusic();

    eTile* entryPoint(const eCityId cid) const;
    eTile* exitPoint(const eCityId cid) const;
    eTile* riverEntryPoint(const eCityId cid) const;
    eTile* riverExitPoint(const eCityId cid) const;

    bool editorMode() const { return mEditorMode; }
    void setEditorMode(const bool m) { mEditorMode = m; }
    void editorClearBuildings();
    void editorDisplayBuildings();
    void saveEditorCityPlan();
    int currentDistrictId() const { return mCurrentDistrictId; }
    void setCurrentDistrictId(const int id);

    bool atlantean(const eCityId cid) const;
    bool setAtlantean(const eCityId cid, const bool a);

    bool fogOfWar() const { return mFogOfWar; }
    void setFogOfWar(const bool fog) { mFogOfWar = fog; }

    void scheduleTerrainUpdate() { mUpdateTerrain = true; }
    bool terrainUpdateScheduled() const { return mUpdateTerrain; }
    void afterTerrainUpdated() { mUpdateTerrain = false; }

    using eWC = stdsptr<eWorldCity>;
    void startEpisode(eEpisode* const e,
                      const eWC& lastPlayedColony);
    void loadResources();

    bool checkGoalsFulfilled() const;

    void musterAllSoldiers(const eCityId cid);
    void sendAllSoldiersHome(const eCityId cid);

    void scheduleDistributeEmployees();

    void incPopulation(const eCityId cid, const int by);

    void topElevationExtremas(int& min, int& max) const;
    void rightElevationExtremas(int& min, int& max) const;
    void bottomElevationExtremas(int& min, int& max) const;
    void leftElevationExtremas(int& min, int& max) const;

    void minMaxAltitude(int& min, int& max) const;

    bool landTradeShutdown(const eCityId cid) const;
    void setLandTradeShutdown(const eCityId cid, const bool s);

    bool seaTradeShutdown(const eCityId cid) const;
    void setSeaTradeShutdown(const eCityId cid, const bool s);

    eMilitaryAid* militaryAid(const eCityId cid,
                              const stdsptr<eWorldCity>& c) const;
    void removeMilitaryAid(const eCityId cid,
                           const stdsptr<eWorldCity>& c);
    void addMilitaryAid(const eCityId cid,
                        const stdsptr<eMilitaryAid>& a);

    void setEpisodeLost() const;
    bool episodeLost() const { return mEpisodeLost; }
    int tradingPartners() const;

    void earthquake(eTile* const startTile, const int size);
    bool duringEarthquake() const;

    void addTidalWave(eTile* const startTile,
                      const bool permanent);
    bool duringTidalWave() const;

    void addLandSlide(eTile* const startTile);
    bool duringLandSlide() const;

    void sinkLand(const eCityId cid, const int amount);

    void addLavaFlow(eTile* const startTile);
    bool duringLavaFlow() const;

    void defeatedBy(const eCityId defeated,
                    const stdsptr<eWorldCity>& by);
    using eCities = std::vector<stdsptr<eWorldCity>>;
    eCities defeatedBy(const eCityId cid);

    eImmigrationLimitedBy immigrationLimit(const eCityId cid) const;

    void addFulfilledQuest(const ePlayerId pid, const eGodQuest q);
    void addSlayedMonster(const eCityId cid, const eMonsterType m);

    std::vector<eGodQuest> fulfilledQuests(const ePlayerId pid) const;
    std::vector<eMonsterType> slayedMonsters(const ePlayerId pid) const;

    bool wasHeroSummoned(const eCityId cid, const eHeroType hero) const;
    void heroSummoned(const eCityId cid, const eHeroType hero);

    const std::map<eResourceType, int>& prices() const
    { return mPrices; }
    int price(const eResourceType type) const;
    void incPrice(const eResourceType type, const int by);

    double wageMultiplier() const { return mWageMultiplier; }
    void changeWage(const int per);

    void updateTerritoryBorders();
    void assignAllTerritory(const eCityId cid);

    ePlayerId cityIdToPlayerId(const eCityId cid) const;
    eTeamId cityIdToTeamId(const eCityId cid) const;
    eTeamId playerIdToTeamId(const ePlayerId pid) const;
    void moveCityToPlayer(const eCityId cid, const ePlayerId pid);
    void setPlayerTeam(const ePlayerId pid, const eTeamId tid);
    std::vector<eCityId> playerCities(const ePlayerId pid) const;
    eCityId currentCityId() const;
    stdsptr<eWorldCity> currentCity() const;
    eCityId playerCapital(const ePlayerId pid) const;
    std::vector<eCityId> playerCitiesOnBoard(const ePlayerId pid) const;
    std::vector<eCityId> personPlayerCitiesOnBoard() const;
    ePlayerId personPlayer() const;
    eCityId personPlayerCapital() const;
    eBoardCity* boardCityWithId(const eCityId cid) const;
    SDL_Rect boardCityTileBRect(const eCityId cid) const;
    eBoardPlayer* boardPlayerWithId(const ePlayerId pid) const;
    std::vector<eCityId> citiesOnBoard() const;
    std::vector<ePlayerId> playersOnBoard() const;
    std::string cityName(const eCityId cid) const;
    std::vector<eCityId> allyCidsNotOnBoard(const ePlayerId pid) const;
    std::vector<eCityId> enemyCidsOnBoard(const eTeamId ptid) const;

    void updatePlayersOnBoard();
    eBoardPlayer* addPlayerToBoard(const ePlayerId pid);
    void removePlayerFromBoard(const ePlayerId pid);

    eBoardCity* addCityToBoard(const eCityId cid);
    void removeCityFromBoard(const eCityId cid);

    bool canBuildAvenue(eTile* const t, const eCityId cid,
                        const ePlayerId pid,
                        const bool forestAllowed) const;
    bool canBuildBase(const int minX, const int maxX,
                      const int minY, const int maxY,
                      const bool forestAllowed,
                      const eCityId cid,
                      const ePlayerId pid,
                      const bool fertile = false,
                      const bool flat = false,
                      const int allowedWater = 0) const;
    bool canBuild(const int tx, const int ty,
                  const int sw, const int sh,
                  const bool forestAllowed,
                  const eCityId cid,
                  const ePlayerId pid,
                  const bool fertile = false,
                  const bool flat = false) const;

    using eBuildingCreator = std::function<stdsptr<eBuilding>()>;
    bool buildBase(const int minX, const int minY,
                   const int maxX, const int maxY,
                   const eBuildingCreator& bc,
                   const ePlayerId pid,
                   const eCityId cid,
                   const bool editorDisplay,
                   const bool fertile = false,
                   const bool flat = false,
                   const int allowWater = 0);
    bool build(const int tx, const int ty,
               const int sw, const int sh,
               const eCityId cid,
               const ePlayerId pid,
               const bool editorDisplay,
               const eBuildingCreator& bc,
               const bool fertile = false,
               const bool flat = false);

    using eDA = eCharacter;
    using eAnimalCreator = std::function<stdsptr<eDA>(eGameBoard&)>;
    bool buildAnimal(eTile* const tile,
                     const eBuildingType type,
                     const eAnimalCreator& creator,
                     const eCityId cid,
                     const ePlayerId pid,
                     const bool editorDisplay);

    void removeAllBuildings();

    bool buildPyramid(const int minX, const int maxX,
                      const int minY, const int maxY,
                      const eBuildingType type,
                      const bool rotate,
                      const eCityId cid,
                      const ePlayerId pid,
                      const bool editorDisplay);

    bool buildSanctuary(const int minX, const int maxX,
                        const int minY, const int maxY,
                        const eBuildingType bt,
                        const bool rotate,
                        const eCityId cid,
                        const ePlayerId pid,
                        const bool editorDisplay);

    int bestYearlyProduction(const eResourceType type) const;
    void incProduced(const eResourceType type,
                     const int by);

    static void sBuildTiles(int& minX, int& minY,
                            int& maxX, int& maxY,
                            const int tx, const int ty,
                            const int sw, const int sh);
private:
    void updateNeighbours();

    void addSoldier(const eCharacterType st, const eCityId cid);
    void removeSoldier(const eCharacterType st,
                       const eCityId cid,
                       const bool skipNotHome = true);

    void handleGamesBegin(const eGames game);
    void handleGamesEnd(const eGames game);

    bool handleEpisodeCompleteEvents();

    void progressEarthquakes();
    void progressTidalWaves();
    void progressLavaFlow();
    void progressLandSlide();
    void earthquakeWaveCollapse(eTile * const t);

    bool mEditorMode = false;
    bool mFogOfWar = true;
    int mCurrentDistrictId = -1;
    mutable bool mEpisodeLost = false;
    eWorldBoard& mWorld;
    eWorldDirection mDirection{eWorldDirection::N};

    bool mUpdateTerrain = true;

    eEventHandler mEventHandler;
    eAction mRequestUpdateHandler;
    eVisibilityChecker mVisibilityChecker;
    eTipShower mTipShower;
    eEnlistRequest mEnlistRequester;
    eAction mEpisodeFinishedHandler;
    eAction mAutosaver;

    std::vector<eBoardCity*> mActiveCitiesOnBoard;
    std::vector<stdsptr<eBoardCity>> mCitiesOnBoard;
    std::vector<stdsptr<eBoardPlayer>> mPlayersOnBoard;

    std::map<eResourceType, int> mPrices;

    int mState = 0;
    int mFrame = 0;
    int mTotalTime = 0;
    int mTime = 0;
    int mEmploymentCheckTime = 0;
    struct int0 { int fV = 0; };
    std::map<eCityId, int0> mLastEmploymentState;
    eDate mDate = eDate(1, eMonth::january, -1500);

    eThreadPool mThreadPool;

    int mWidth = 0;
    int mHeight = 0;
    std::vector<std::vector<eTile*>> mTiles;

    std::vector<stdsptr<eObject>> mRubbish;

    bool mRegisterBuildingsEnabled = true;

    std::vector<eArmyEventBase*> mArmyEvents;
    double mWageMultiplier = 1.;

    // begin moved to eBoardCity
    std::vector<eCharacterAction*> mCharacterActions;
    std::vector<eBuilding*> mTimedBuildings;
    std::vector<eBuilding*> mAllBuildings;
    std::vector<eSpawner*> mSpawners;
    std::vector<eBanner*> mBanners;

    std::vector<eInvasionEvent*> mInvasions;
    // end moved to eBoardCity

    std::vector<eSoldierBanner*> mAllSoldierBanners;
    std::vector<eCharacter*> mCharacters;
    std::vector<eSoldier*> mSoldiers;
    std::vector<eMissile*> mMissiles;

    std::vector<eSoldierBanner*> mSelectedBanners;
    std::vector<eTrireme*> mSelectedTriremes;

    std::vector<ePlannedAction*> mPlannedActions;

    struct eMarbleTiles {
        bool contains(eTile* const tile) const;
        void add(eTile* const tile);
        void addWithNeighbours(eTile* const tile);
        void restock() const;

        std::vector<eTile*> fTiles;
    };

    std::vector<eMarbleTiles> mMarbleTiles;
    std::vector<eMarbleTiles> mBlackMarbleTiles;

    int mSoldiersUpdate = 10000;

    std::map<eCityId, eTrueBool> mUpdateAppeal;
    eHeatMap mAppealMap;

    eAction mButtonVisUpdater;
    eMessageShower mMsgShower;

    std::vector<eGameEvent*> mAllGameEvents;
    int mGoalsCheckTime = 0;
    std::vector<stdsptr<eEpisodeGoal>> mGoals;
    bool mGoalsFulfilled = false;

    struct eEarthquake {
        eTile* fStartTile = nullptr;
        std::vector<eTile*> fTiles;
        int fLastDim = 0;

        void read(eReadStream& src, eGameBoard& board) {
            fStartTile = src.readTile(board);
            int nt;
            src >> nt;
            for(int i = 0; i < nt; i++) {
                const auto t = src.readTile(board);
                fTiles.push_back(t);
            }
            src >> fLastDim;
        }

        void write(eWriteStream& dst) {
            dst.writeTile(fStartTile);
            dst << fTiles.size();
            for(const auto t : fTiles) {
                dst.writeTile(t);
            }
            dst << fLastDim;
        }
    };

    int mProgressEarthquakes = 0;
    std::vector<stdsptr<eEarthquake>> mEarthquakes;

    struct eWaveDirection {
        eTile* fTile;
        eTerrain fSaved;
        eOrientation fO;
    };

    struct eTidalWave {
        std::vector<std::vector<eWaveDirection>> fTiles;
        int fLastId = 0;
        bool fPermanent = false;
        bool fRegres = false;

        void read(eReadStream& src, eGameBoard& board) {
            int nv;
            src >> nv;
            for(int i = 0; i < nv; i++) {
                auto& v = fTiles.emplace_back();
                int nt;
                src >> nt;
                for(int j = 0; j < nt; j++) {
                    const auto t = src.readTile(board);
                    eTerrain terr;
                    src >> terr;
                    eOrientation o;
                    src >> o;
                    v.push_back({t, terr, o});
                }
            }
            src >> fLastId;
            src >> fPermanent;
            src >> fRegres;
        }

        void write(eWriteStream& dst) {
            dst << fTiles.size();
            for(const auto& v : fTiles) {
                dst << v.size();
                for(const auto t : v) {
                    dst.writeTile(t.fTile);
                    dst << t.fSaved;
                    dst << t.fO;
                }
            }
            dst << fLastId;
            dst << fPermanent;
            dst << fRegres;
        }
    };

    struct eLavaDirection {
        eTile* fTile;
        eOrientation fO;
    };

    struct eLavaFlow {
        std::vector<std::vector<eLavaDirection>> fTiles;
        int fLastId = 0;

        void read(eReadStream& src, eGameBoard& board) {
            int nv;
            src >> nv;
            for(int i = 0; i < nv; i++) {
                auto& v = fTiles.emplace_back();
                int nt;
                src >> nt;
                for(int j = 0; j < nt; j++) {
                    const auto t = src.readTile(board);
                    eOrientation o;
                    src >> o;
                    v.push_back({t, o});
                }
            }
            src >> fLastId;
        }

        void write(eWriteStream& dst) {
            dst << fTiles.size();
            for(const auto& v : fTiles) {
                dst << v.size();
                for(const auto t : v) {
                    dst.writeTile(t.fTile);
                    dst << t.fO;
                }
            }
            dst << fLastId;
        }
    };

    struct eLandSlideDirection {
        eTile* fTile;
        int fNewAltitude;
        eOrientation fO;
    };

    struct eLandSlide {
        std::vector<std::vector<eLandSlideDirection>> fTiles;
        int fLastId = 0;

        void read(eReadStream& src, eGameBoard& board) {
            int nv;
            src >> nv;
            for(int i = 0; i < nv; i++) {
                auto& v = fTiles.emplace_back();
                int nt;
                src >> nt;
                for(int j = 0; j < nt; j++) {
                    const auto t = src.readTile(board);
                    int newAltitude;
                    src >> newAltitude;
                    eOrientation o;
                    src >> o;
                    v.push_back({t, newAltitude, o});
                }
            }
            src >> fLastId;
        }

        void write(eWriteStream& dst) {
            dst << fTiles.size();
            for(const auto& v : fTiles) {
                dst << v.size();
                for(const auto t : v) {
                    dst.writeTile(t.fTile);
                    dst << t.fNewAltitude;
                    dst << t.fO;
                }
            }
            dst << fLastId;
        }
    };

    int mProgressWaves = 0;
    std::vector<stdsptr<eTidalWave>> mTidalWaves;

    int mProgressLavaFlows = 0;
    std::vector<stdsptr<eLavaFlow>> mLavaFlows;

    int mProgressLandSlides = 0;
    std::vector<stdsptr<eLandSlide>> mLandSlides;

    std::map<eCityId, std::vector<stdsptr<eWorldCity>>> mDefeatedBy;

    // for person player
    struct eYearlyProduction {
        int fBest = 0;
        int fThisYear = 0; // so far
        int fLastYear = 0;
    };

    std::map<eResourceType, eYearlyProduction> mYearlyProduction;

    int mSavedYear = -100000;
};

#endif // EGAMEBOARD_H
