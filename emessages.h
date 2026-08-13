#ifndef EMESSAGES_H
#define EMESSAGES_H

#include "emessage.h"

#include <map>

enum class eGodType;
enum class eHeroType;
enum class eMonsterType;

struct eQuestMessages {
    eEventMessageType fQuest;
    eEventMessageType fFulfilled;
};

struct eGodMessages {
    mutable int fLastMessage = -1;

    eMessageType fWooing0;
    eMessageType fJealousy1;
    eMessageType fJealousy2;
    eMessageType fInvades;
    eEventMessageType fHelps;

    eEventMessageType fMonster;

    eQuestMessages fQuest1;
    eQuestMessages fQuest2;

    eMessageType fSanctuaryComplete;

    eMessageType fDisaster;
    eMessageType fDisasterEnds;
};

struct eMonsterMessages {
    eMessageType fInCity;
    std::string fInCityReason;
    eMessageType fPrediction;
    eMessageType fInvasion36;
    eMessageType fInvasion24;
    eMessageType fInvasion12;
    eMessageType fInvasion6;
    eMessageType fInvasion1;
    eMessageType fInvasion;
    eMessageType fSlain;
    std::string fToSlainReason;
    std::string fSlainReason;
    std::string fMonsterAttackReason;
};

struct eHeroMessages {
    eEventMessageType fArrival;
    eConsequenceMessageType fHallAvailable;
};

struct eReceiveRequestMessages {
    eMessageType fInitial;
    eMessageType fReminder;
    eMessageType fOverdue;
    eMessageType fWarning;
    eMessageType fComply;
    eMessageType fTooLate;
    eMessageType fRefuse;
    eReason fEarlyReason;
    eReason fComplyReason;
    eReason fTooLateReason;
    eReason fRefuseReason;
};

struct eResourceGrantedMessages {
    eMessageType fGranted;
    eMessageType fCashGranted;
    eMessageType fPartialSpace;
    eMessageType fInsufficientSpace;
    eMessageType fLastChance;
    eMessageType fForfeited;
    eMessageType fAccepted;
    eMessageType fCashAccepted;
    eMessageType fPostponed;
    eMessageType fRefused;
};

struct eTroopsRequestedMessages {
    eMessageType fInitialAnnouncement;
    eMessageType fFirstReminder;
    eMessageType fLastReminder;
    eReason fEarlyReason;
    eReason fComplyReason;
    eReason fTooLateReason;
    eReason fRefuseReason;
    eReason fLostBattleReason;

    eMessageType fConquered;
};

struct eTroopsRequestedMonsterMessages : public eTroopsRequestedMessages {
};

struct eTroopsRequestedAttackMessages : public eTroopsRequestedMessages {
};

class eMessages {
public:
    eMessages() {}

    static eMessages instance;

    static bool load();
    static bool loaded();
    // drops everything and loads again, used after a language change
    static void reload();

    static std::string message(const std::string& key);
    static eMessageType favorMessage(const eMessageType& reason);
    static eMessageType dfavorMessage(const eMessageType& reason);
    static eMessageType invasionMessage(const eMessageType& baseMsg,
                                        const std::string& reason,
                                        const int months);

    std::map<std::string, std::string> fMessages;

    eMessageType fPop100;
    eMessageType fPop500;
    eMessageType fPop1000; // comm
    eMessageType fPop2000; // comm
    eMessageType fPop3000;
    eMessageType fPop5000; // comm
    eMessageType fPop10000; // comm
    eMessageType fPop15000; // comm
    eMessageType fPop20000; // comm
    eMessageType fPop25000; // comm

    eMessageType fHappinessMonument;

    eMessageType fFire;
    eMessageType fCollapse;
    eMessageType fPlagueStrikes;

    const eGodMessages* godMessages(const eGodType godType) const;

    eGodMessages fAphrodite;
    eGodMessages fApollo;
    eGodMessages fAres;
    eGodMessages fArtemis;
    eGodMessages fAthena;
    eGodMessages fAtlas;
    eGodMessages fDemeter;
    eGodMessages fDionysus;
    eGodMessages fHades;
    eGodMessages fHephaestus;
    eGodMessages fHera;
    eGodMessages fHermes;
    eGodMessages fPoseidon;
    eGodMessages fZeus;

    eMessageType fZeusTradeResumes;
    eMessageType fPoseidonTradeResumes;
    eMessageType fHermesTradeResumes;

    const eHeroMessages* heroMessages(const eHeroType heroType) const;

    eHeroMessages fAchilles;
    eHeroMessages fAtalanta;
    eHeroMessages fBellerophon;
    eHeroMessages fHercules;
    eHeroMessages fJason;
    eHeroMessages fOdysseus;
    eHeroMessages fPerseus;
    eHeroMessages fTheseus;

    const eMonsterMessages* monsterMessages(const eMonsterType monsterType) const;

    eMonsterMessages fCalydonianBoar;
    eMonsterMessages fCerberus;
    eMonsterMessages fChimera;
    eMonsterMessages fCyclops;
    eMonsterMessages fDragon;
    eMonsterMessages fEchidna;
    eMonsterMessages fHarpies;
    eMonsterMessages fHector;
    eMonsterMessages fHydra;
    eMonsterMessages fKraken;
    eMonsterMessages fMaenads;
    eMonsterMessages fMedusa;
    eMonsterMessages fMinotaur;
    eMonsterMessages fScylla;
    eMonsterMessages fSphinx;
    eMonsterMessages fTalos;

    eGameMessages fHomeGames;
    eGameMessages fIsthmianGames;
    eGameMessages fNemeanGames;
    eGameMessages fPythianGames;
    eGameMessages fOlympianGames;

    eMessageType fInvasionInitial;
    eMessageType fInvasion24;
    eMessageType fInvasion12;
    eMessageType fInvasion6;
    eMessageType fInvasion1;
    eMessageType fInvasion;
    std::string fInvasionNoReason;
    eMessageType fInvasionBribed;
    eMessageType fInvasionDefeat;
    eMessageType fInvasionVictory;
    eMessageType fInvasionVictoryMonument;

    eMessageType fTributePaidGranted;
    eMessageType fTributePaidAccepted;
    eMessageType fTributePaidRefused;
    eMessageType fTributePostponed;

    eMessageType fRequestGranted;
    eMessageType fRequestPartialSpace;
    eMessageType fRequestInsufficientSpace;
    eMessageType fRequestLastChance;
    eMessageType fRequestAccepted;
    eMessageType fRequestPostponed;
    eMessageType fRequestRefused;
    eMessageType fRequestForfeited;

    eMessageType fDemandRefused;

    eMessageType fGiftReceivedDrachmas;
    eMessageType fGiftReceivedNeeded;
    eMessageType fGiftReceivedSells;
    eMessageType fGiftReceivedNotNeeded;
    eMessageType fGiftReceivedRefuse;

    eReceiveRequestMessages fGeneralRequestAllyS;
    eReceiveRequestMessages fGeneralRequestRivalD;
    eReceiveRequestMessages fGeneralRequestSubjectP;
    eReceiveRequestMessages fGeneralRequestParentR;

    eReceiveRequestMessages fFamineAllyS;
    eReceiveRequestMessages fFamineRivalD;
    eReceiveRequestMessages fFamineSubjectP;
    eReceiveRequestMessages fFamineParentR;

    eReceiveRequestMessages fProjectAllyS;
    eReceiveRequestMessages fProjectRivalD;
    eReceiveRequestMessages fProjectSubjectP;
    eReceiveRequestMessages fProjectParentR;

    eReceiveRequestMessages fFestivalAllyS;
    eReceiveRequestMessages fFestivalRivalD;
    eReceiveRequestMessages fFestivalSubjectP;
    eReceiveRequestMessages fFestivalParentR;

    eReceiveRequestMessages fFinancialWoesAllyS;
    eReceiveRequestMessages fFinancialWoesRivalD;
    eReceiveRequestMessages fFinancialWoesSubjectP;
    eReceiveRequestMessages fFinancialWoesParentR;

    eReceiveRequestMessages fTributeRequest;

    eResourceGrantedMessages fGiftGranted;

    eMessageType fSupportTroopsArrive;
    eMessageType fSupportTroopsDepart;
    eMessageType fSupportTroopsDefeated;

    eMessageType fStrikeDepart;
    eMessageType fStrikeSuccessful;
    eMessageType fStrikeUnsuccessul;

    eMessageType fRivalArmyAway;

    eMessageType fIncreasedFavor;
    eMessageType fDecreasedFavor;

    eMessageType fUnemployment;
    eMessageType fEmployees;

    eMessageType fPlague;

    eMessageType fCityConqueredByPlayer;
    eMessageType fCityConquerFailedByPlayer;

    eResourceGrantedMessages fRaidGranted;
    eMessageType fCityRaidFailedByPlayer;

    eMessageType fAllyAttackedByPlayer;

    eMessageType fArmyReturns;

    eMessageType fMilitaryBuildup;
    eMessageType fMilitaryDecline;

    eMessageType fEconomicProsperity;
    eMessageType fEconomicDecline;

    eTroopsRequestedMessages fAllyTroopsRequest;
    eTroopsRequestedMessages fVassalTroopsRequest;
    eTroopsRequestedMessages fColonyTroopsRequest;
    eTroopsRequestedMessages fParentCityTroopsRequest;

    eMessageType fTroopsRequestAttackAverted;

    eTroopsRequestedMonsterMessages fAllyTroopsMonsterRequest;
    eTroopsRequestedMonsterMessages fVassalTroopsMonsterRequest;
    eTroopsRequestedMonsterMessages fColonyTroopsMonsterRequest;
    eTroopsRequestedMonsterMessages fParentCityTroopsMonsterRequest;

    eMessageType fTroopsMonsterRequestAttackAverted;

    eTroopsRequestedAttackMessages fAllyTroopsAttackRequest;
    eTroopsRequestedAttackMessages fVassalTroopsAttackRequest;

    eMessageType fEarthqaukeNoReason;
    eMessageType fEarthqaukeGod;
    eMessageType fTidalWaveNoReason;
    eMessageType fTidalWaveGod;
    eMessageType fLavaFlowNoReason;
    eMessageType fLavaFlowGod;
    eMessageType fSinkLandNoReason;
    eMessageType fSinkLandGod;
    eMessageType fLandSlide;

    eMessageType fModestPyramidComplete1;
    eMessageType fPyramidComplete2;
    eMessageType fGreatPyramidComplete3;
    eMessageType fMajesticPyramidComplete4;

    eMessageType fSmallMonumentToTheSkyComplete5;
    eMessageType fMonumentToTheSkyComplete6;
    eMessageType fGrandMonumentToTheSkyComplete7;

    eMessageType fMinorShrineComplete8;
    eMessageType fShrineComplete9;
    eMessageType fMajorShrineComplete10;

    eMessageType fPyramidOfThePantheonComplete11;
    eMessageType fAltarOfOlympusComplete12;
    eMessageType fTempleOfOlympusComplete13;
    eMessageType fObservatoryKosmikaComplete14;
    eMessageType fMuseumAtlantikaComplete15;

    eMessageType fCityBecomesAlly;
    eMessageType fCityBecomesRival;
    eMessageType fCityBecomesVassal;

    eMessageType fCityBecomesActive;
    eMessageType fCityBecomesInactive;

    eMessageType fCityBecomesVisible;
    eMessageType fCityBecomesInvisible;

    eMessageType fTributeSuspended;
    eMessageType fTributeResumed;

    eMessageType fCityRebellion;
    eMessageType fCityRebellionQuelled;
    eMessageType fCityRebellionOver;

    eMessageType fRivalConqueredByAlly;
    eMessageType fRivalConqueredByVassal;

    eMessageType fAllyConqueredByRival;
    eMessageType fVassalConqueredByRival;
    eMessageType fColonyConqueredByRival;
    eMessageType fParentConqueredByRival;

    eMessageType fColonyRestored;

    eMessageType fDebtAnniversary;

    eMessageType fColonyMonument;

    eMessageType fPriceIncrease;
    eMessageType fPriceDecrease;

    eMessageType fDemandIncrease;
    eMessageType fDemandDecrease;

    eMessageType fSupplyIncrease;
    eMessageType fSupplyDecrease;

    eMessageType fTradeShutsDown;
    eMessageType fTradeOpensUp;

    eMessageType fWageIncrease;
    eMessageType fWageDecrease;

    eMessageType fAreaCutOff;
private:
    bool loadImpl();

    std::string loadMessage(const std::string& key) const;
    void loadMessage(eMessageType& m,
                     const std::string& titleKey,
                     const std::string& textKey);
    void loadReason(eReason& r, const std::string& key);

    void load(eGodMessages& god, const std::string& godName);
    void load(eHeroMessages& hero, const std::string& heroName);
    void load(eMonsterMessages& monster,
              const std::string& monsterName1,
              const std::string& monsterName2);

    void loadGeneralRequest(eReceiveRequestMessages& request,
                            const std::string& letter,
                            const std::string& mid = "general_request");
    void loadResourceGranted(eResourceGrantedMessages& granted,
                             const std::string& name);
    void load(eTroopsRequestedMessages& troops,
              const std::string& name,
              const std::string& letter);
    void load(eTroopsRequestedMonsterMessages& troops,
              const std::string& name);
    void load(eTroopsRequestedAttackMessages& troops,
              const std::string& name,
              std::string letter);

    bool mLoaded = false;
};

#endif // EMESSAGES_H
