#include "emessages.h"

#include "egamedir.h"

#include "eloadtexthelper.h"
#include "estringhelpers.h"

#include "characters/gods/egodtype.h"
#include "characters/heroes/ehero.h"
#include "characters/monsters/emonster.h"

eMessages eMessages::instance;

void eMessages::load(eGodMessages& god, const std::string& godName) {
    god.fWooing0.fFull.fTitle = loadMessage("PHRASE_" + godName + "_wooing0_title");
    god.fWooing0.fFull.fText = loadMessage("PHRASE_" + godName + "_wooing0_initial_announcement");
    god.fWooing0.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_wooing0_title");
    god.fWooing0.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_wooing0_initial_announcement");

    god.fJealousy1.fFull.fTitle = loadMessage("PHRASE_" + godName + "_jealousy1_title");
    god.fJealousy1.fFull.fText = loadMessage("PHRASE_" + godName + "_jealousy1_initial_announcement");
    god.fJealousy1.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_jealousy1_title");
    god.fJealousy1.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_jealousy1_initial_announcement");

    god.fJealousy2.fFull.fTitle = loadMessage("PHRASE_" + godName + "_jealousy2_title");
    god.fJealousy2.fFull.fText = loadMessage("PHRASE_" + godName + "_jealousy2_initial_announcement");
    god.fJealousy2.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_jealousy2_title");
    god.fJealousy2.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_jealousy2_initial_announcement");

    god.fInvades.fFull.fTitle = loadMessage("PHRASE_" + godName + "_invades_title");
    god.fInvades.fFull.fText = loadMessage("PHRASE_" + godName + "_invades_city_attacked_alert");
    god.fInvades.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_invades_title");
    god.fInvades.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_invades_city_attacked_alert");

    god.fHelps.fFull.fTitle = loadMessage("PHRASE_" + godName + "_help_title");
    god.fHelps.fFull.fText = loadMessage("PHRASE_" + godName + "_help_initial_announcement");
    god.fHelps.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_help_title");
    god.fHelps.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_help_initial_announcement");
    god.fHelps.fReason = loadMessage("PHRASE_" + godName + "_help_reason");

    god.fMonster.fFull.fTitle = loadMessage("PHRASE_" + godName + "_unleashes_monster_title");
    god.fMonster.fFull.fText = loadMessage("PHRASE_" + godName + "_unleashes_monster_city_attacked_alert");
    god.fMonster.fNoReason = loadMessage("PHRASE_" + godName + "_unleashes_monster_no_reason");
    eStringHelpers::replaceAll(god.fMonster.fFull.fText, "[reason_phrase)", god.fMonster.fNoReason);
    god.fMonster.fReason = loadMessage("PHRASE_" + godName + "_unleashes_monster_reason");
    god.fMonster.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_unleashes_monster_title");
    god.fMonster.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_unleashes_monster_city_attacked_alert");

    god.fQuest1.fQuest.fFull.fTitle = loadMessage("PHRASE_" + godName + "_quest_title");
    god.fQuest1.fQuest.fFull.fText = loadMessage("PHRASE_" + godName + "_quest_initial_announcement");
    god.fQuest1.fQuest.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_quest_title");
    god.fQuest1.fQuest.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_quest_initial_announcement");
    god.fQuest1.fQuest.fNoReason = loadMessage("PHRASE_" + godName + "_quest_no_reason");
    god.fQuest1.fQuest.fReason = loadMessage("PHRASE_" + godName + "_quest_reason");

    god.fQuest1.fFulfilled.fFull.fTitle = loadMessage("PHRASE_" + godName + "_quest_fulfilled_title");
    god.fQuest1.fFulfilled.fFull.fText = loadMessage("PHRASE_" + godName + "_quest_fulfilled_initial_announcement");
    god.fQuest1.fFulfilled.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_quest_fulfilled_title");
    god.fQuest1.fFulfilled.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_quest_fulfilled_initial_announcement");
    god.fQuest1.fFulfilled.fNoReason = loadMessage("PHRASE_" + godName + "_quest_fulfilled_no_reason");
    god.fQuest1.fFulfilled.fReason = loadMessage("PHRASE_" + godName + "_quest_fulfilled_reason");

    god.fQuest2.fQuest.fFull.fTitle = loadMessage("PHRASE_" + godName + "_quest2_title");
    god.fQuest2.fQuest.fFull.fText = loadMessage("PHRASE_" + godName + "_quest2_initial_announcement");
    god.fQuest2.fQuest.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_quest2_title");
    god.fQuest2.fQuest.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_quest2_initial_announcement");
    god.fQuest2.fQuest.fNoReason = loadMessage("PHRASE_" + godName + "_quest2_no_reason");
    god.fQuest2.fQuest.fReason = loadMessage("PHRASE_" + godName + "_quest2_reason");

    god.fQuest2.fFulfilled.fFull.fTitle = loadMessage("PHRASE_" + godName + "_quest2_fulfilled_title");
    god.fQuest2.fFulfilled.fFull.fText = loadMessage("PHRASE_" + godName + "_quest2_fulfilled_initial_announcement");
    god.fQuest2.fFulfilled.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + godName + "_quest2_fulfilled_title");
    god.fQuest2.fFulfilled.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_quest2_fulfilled_initial_announcement");
    god.fQuest2.fFulfilled.fNoReason = loadMessage("PHRASE_" + godName + "_quest2_fulfilled_no_reason");
    god.fQuest2.fFulfilled.fReason = loadMessage("PHRASE_" + godName + "_quest2_fulfilled_reason");

    god.fSanctuaryComplete.fFull.fTitle = loadMessage("PHRASE_" + godName + "_sanctuary_complete_title");
    god.fSanctuaryComplete.fFull.fText = loadMessage("PHRASE_" + godName + "_sanctuary_complete_initial_announcement");
    god.fSanctuaryComplete.fCondensed.fTitle = loadMessage("PHRASE_" + godName + "_sanctuary_complete_title");
    god.fSanctuaryComplete.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_sanctuary_complete_initial_announcement");

    god.fDisaster.fFull.fTitle = loadMessage("PHRASE_" + godName + "_disaster_title");
    god.fDisaster.fFull.fText = loadMessage("PHRASE_" + godName + "_disaster_initial_announcement");
    god.fDisaster.fCondensed.fTitle = loadMessage("PHRASE_" + godName + "_disaster_title");
    god.fDisaster.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_disaster_initial_announcement");

    god.fDisasterEnds.fFull.fTitle = loadMessage("PHRASE_" + godName + "_disaster_ends_title");
    god.fDisasterEnds.fFull.fText = loadMessage("PHRASE_" + godName + "_disaster_ends_initial_announcement");
    god.fDisasterEnds.fCondensed.fTitle = loadMessage("PHRASE_" + godName + "_disaster_ends_title");
    god.fDisasterEnds.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + godName + "_disaster_ends_initial_announcement");
}

void eMessages::load(eHeroMessages& hero, const std::string& heroName) {
    hero.fArrival.fFull.fTitle = loadMessage("PHRASE_" + heroName + "_arrives_title");
    hero.fArrival.fFull.fText = loadMessage("PHRASE_" + heroName + "_arrives_initial_announcement");
    hero.fArrival.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + heroName + "_arrives_title");
    hero.fArrival.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + heroName + "_arrives_initial_announcement");

    hero.fHallAvailable.fFull.fTitle = loadMessage("PHRASE_" + heroName + "_hall_available_title");
    hero.fHallAvailable.fNoReason = loadMessage("PHRASE_" + heroName + "_hall_available_no_reason");
    hero.fHallAvailable.fFull.fText = loadMessage("PHRASE_" + heroName + "_hall_available_initial_announcement");
    hero.fHallAvailable.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + heroName + "_hall_available_title");
    hero.fHallAvailable.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + heroName + "_hall_available_initial_announcement");
}

void eMessages::load(eMonsterMessages& monster,
                     const std::string& monsterName1,
                     const std::string& monsterName2) {
    monster.fInCity.fFull.fTitle = loadMessage("PHRASE_" + monsterName2 + "_in_city_title");
    monster.fInCity.fFull.fText = loadMessage("PHRASE_" + monsterName2 + "_in_city_initial_announcement");
    monster.fInCity.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + monsterName2 + "_in_city_title");
    monster.fInCity.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + monsterName2 + "_in_city_initial_announcement");

    monster.fInCityReason = loadMessage("PHRASE_" + monsterName2 + "_in_city_initial_reason");

    monster.fSlain.fFull.fTitle = loadMessage("PHRASE_" + monsterName2 + "_slain_title");
    monster.fSlain.fFull.fText = loadMessage("PHRASE_" + monsterName2 + "_slain_initial_announcement");
    monster.fSlain.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + monsterName2 + "_slain_title");
    monster.fSlain.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + monsterName2 + "_slain_initial_announcement");

    monster.fSlainReason = loadMessage("PHRASE_" + monsterName1 + "_slain_reason");

    monster.fPrediction.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fPrediction.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_oracle_prediction");
    monster.fPrediction.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fPrediction.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_oracle_prediction");

    monster.fInvasion36.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion36.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_initial_announcement");
    monster.fInvasion36.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion36.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_initial_announcement");

    monster.fInvasion24.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion24.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_2year_reminder");
    monster.fInvasion24.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion24.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_2year_reminder");

    monster.fInvasion12.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion12.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_1year_reminder");
    monster.fInvasion12.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion12.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_1year_reminder");

    monster.fInvasion6.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion6.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_6month_warning");
    monster.fInvasion6.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion6.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_6month_warning");

    monster.fInvasion1.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion1.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_1month_Warning");
    monster.fInvasion1.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion1.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_1month_Warning");

    monster.fInvasion.fFull.fTitle = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion.fFull.fText = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_city_attacked_alert");
    monster.fInvasion.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_title");
    monster.fInvasion.fCondensed.fText = loadMessage("PHRASE_CONDENSED_independent_" + monsterName2 + "_invades_city_attacked_alert");

    monster.fToSlainReason = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_reason");
    monster.fMonsterAttackReason = loadMessage("PHRASE_independent_" + monsterName2 + "_invades_no_reason");
}

void eMessages::loadGeneralRequest(
        eReceiveRequestMessages& request,
        const std::string& letter,
        const std::string& mid) {
    request.fInitial.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fInitial.fFull.fText = loadMessage("PHRASE_" + mid + "_initial_announcement_" + letter);
    request.fInitial.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fInitial.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_initial_announcement_" + letter);

    request.fReminder.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fReminder.fFull.fText = loadMessage("PHRASE_" + mid + "_reminder_" + letter);
    request.fReminder.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fReminder.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_reminder_" + letter);

    request.fOverdue.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fOverdue.fFull.fText = loadMessage("PHRASE_" + mid + "_overdue_" + letter);
    request.fOverdue.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fOverdue.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_overdue_" + letter);

    request.fWarning.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fWarning.fFull.fText = loadMessage("PHRASE_" + mid + "_warning_" + letter);
    request.fWarning.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fWarning.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_warning_" + letter);

    request.fComply.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fComply.fFull.fText = loadMessage("PHRASE_" + mid + "_comply_reason_" + letter);
    request.fComply.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fComply.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_comply_reason_" + letter);

    request.fTooLate.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fTooLate.fFull.fText = loadMessage("PHRASE_" + mid + "_too_late_reason_" + letter);
    request.fTooLate.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fTooLate.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_too_late_reason_" + letter);

    request.fRefuse.fFull.fTitle = loadMessage("PHRASE_" + mid + "_title_" + letter);
    request.fRefuse.fFull.fText = loadMessage("PHRASE_" + mid + "_refuse_reason_" + letter);
    request.fRefuse.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + mid + "_title_" + letter);
    request.fRefuse.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + mid + "_refuse_reason_" + letter);

    request.fEarlyReason.fFull = loadMessage("PHRASE_" + mid + "_early_reason_" + letter);
    request.fComplyReason.fFull = loadMessage("PHRASE_" + mid + "_comply_reason_" + letter);
    request.fTooLateReason.fFull = loadMessage("PHRASE_" + mid + "_too_late_reason_" + letter);
    request.fRefuseReason.fFull = loadMessage("PHRASE_" + mid + "_refuse_reason_" + letter);

    request.fEarlyReason.fCondensed = loadMessage("PHRASE_CONDENSED_" + mid + "_early_reason_" + letter);
    request.fComplyReason.fCondensed = loadMessage("PHRASE_CONDENSED_" + mid + "_comply_reason_" + letter);
    request.fTooLateReason.fCondensed = loadMessage("PHRASE_CONDENSED_" + mid + "_too_late_reason_" + letter);
    request.fRefuseReason.fCondensed = loadMessage("PHRASE_CONDENSED_" + mid + "_refuse_reason_" + letter);
}

void eMessages::loadResourceGranted(eResourceGrantedMessages& granted,
                                    const std::string& name) {
    granted.fGranted.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fGranted.fFull.fText = loadMessage("PHRASE_" + name + "_granted");
    granted.fGranted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fGranted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_granted");

    granted.fCashGranted.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fCashGranted.fFull.fText = loadMessage("PHRASE_" + name + "_cash_granted");
    granted.fCashGranted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fCashGranted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_cash_granted");

    granted.fPartialSpace.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fPartialSpace.fFull.fText = loadMessage("PHRASE_" + name + "_partial_space");
    granted.fPartialSpace.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fPartialSpace.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_partial_space");

    granted.fInsufficientSpace.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fInsufficientSpace.fFull.fText = loadMessage("PHRASE_" + name + "_insufficient_space");
    granted.fInsufficientSpace.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fInsufficientSpace.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_insufficient_space");

    granted.fLastChance.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fLastChance.fFull.fText = loadMessage("PHRASE_" + name + "_last_chance");
    granted.fLastChance.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fLastChance.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_last_chance");

    granted.fForfeited.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fForfeited.fFull.fText = loadMessage("PHRASE_" + name + "_forfeited");
    granted.fForfeited.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fForfeited.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_forfeited");

    granted.fAccepted.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fAccepted.fFull.fText = loadMessage("PHRASE_" + name + "_accepted");
    granted.fAccepted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fAccepted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_accepted");

    granted.fCashAccepted.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fCashAccepted.fFull.fText = loadMessage("PHRASE_" + name + "_cash_accepted");
    granted.fCashAccepted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fCashAccepted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_cash_accepted");

    granted.fPostponed.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fPostponed.fFull.fText = loadMessage("PHRASE_" + name + "_postponed");
    granted.fPostponed.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fPostponed.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + name + "_postponed");

    granted.fRefused.fFull.fTitle = loadMessage("PHRASE_" + name + "_title");
    granted.fRefused.fFull.fText = loadMessage("PHRASE_" + name + "_refused");
    granted.fRefused.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_" + name + "_title");
    granted.fRefused.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_refused");
}

std::string eMessages::loadMessage(const std::string& key) const {
    const auto it = fMessages.find(key);
    if(it == fMessages.end()) {
        printf("Could not find message '%s'\n", key.c_str());
        return "";
    }
    return it->second;
}

void eMessages::loadMessage(eMessageType& m,
                            const std::string& titleKey,
                            const std::string& textKey) {
    m.fFull.fTitle = loadMessage("PHRASE_" + titleKey);
    m.fFull.fText = loadMessage("PHRASE_" + textKey);
    m.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + titleKey);
    m.fCondensed.fText = loadMessage("PHRASE_CONDENSED_" + textKey);
}

void eMessages::loadReason(eReason& r,
                           const std::string& key) {
    r.fFull = loadMessage("PHRASE_" + key);
    r.fCondensed = loadMessage("PHRASE_CONDENSED_" + key);
}

void eMessages::load(eTroopsRequestedMessages& troops,
                     const std::string& name,
                     const std::string& letter) {
    loadMessage(troops.fInitialAnnouncement,
                name + "_under_attack_title_" + letter,
                name + "_under_attack_initial_announcement_" + letter);
    loadMessage(troops.fFirstReminder,
                name + "_under_attack_title_" + letter,
                name + "_under_attack_first_reminder_" + letter);
    loadMessage(troops.fLastReminder,
                name + "_under_attack_title_" + letter,
                name + "_under_attack_last_reminder_" + letter);

    loadReason(troops.fEarlyReason,
               name + "_under_attack_early_reason_" + letter);
    loadReason(troops.fComplyReason,
               name + "_under_attack_comply_reason_" + letter);
    loadReason(troops.fTooLateReason,
               name + "_under_attack_too_late_reason_" + letter);
    loadReason(troops.fRefuseReason,
               name + "_under_attack_refuse_reason_" + letter);
    loadReason(troops.fLostBattleReason,
               name + "_under_attack_lost_battle_reason_" + letter);

    if(name == "parentcity") {
        loadMessage(troops.fConquered,
                    "parent_city_falls_title",
                    "parent_city_falls_initial_announcement");
    } else {
        loadMessage(troops.fConquered,
                    name + "_conquered_by_rival_title",
                    name + "_conquered_by_rival_initial_announcement");
    }
}

void eMessages::load(eTroopsRequestedMonsterMessages &troops,
                     const std::string &name) {
    loadMessage(troops.fInitialAnnouncement,
                name + "_terrorized_title",
                name + "_terrorized_initial_announcement");
    loadMessage(troops.fFirstReminder,
                name + "_terrorized_title",
                name + "_terrorized_first_reminder");
    loadMessage(troops.fLastReminder,
                name + "_terrorized_title",
                name + "_terrorized_last_reminder");

    loadReason(troops.fEarlyReason,
               name + "_terrorized_early_reason");
    loadReason(troops.fComplyReason,
               name + "_terrorized_comply_reason");
    loadReason(troops.fTooLateReason,
               name + "_terrorized_too_late_reason");
    loadReason(troops.fRefuseReason,
               name + "_terrorized_refuse_reason");
    loadReason(troops.fLostBattleReason,
               name + "_terrorized_lost_battle_reason");

    if(name == "colony") {
        loadMessage(troops.fConquered,
                    name + "_devastated_by_monster_title",
                    name + "_devastated_by_monster_initial_announcement");
    } else if(name == "parentcity") {
        loadMessage(troops.fConquered,
                    "parent_city_destroyed_by_monster_title",
                    "parent_city_destroyed_by_monster_initial_announcement");
    } else {
        loadMessage(troops.fConquered,
                    name + "_destroyed_by_monster_title",
                    name + "_destroyed_by_monster_initial_announcement");
    }
}

void eMessages::load(eTroopsRequestedAttackMessages &troops,
                     const std::string &name,
                     std::string letter) {
    if(!letter.empty()) letter = "_" + letter;
    loadMessage(troops.fInitialAnnouncement,
                name + "_attacks_rival_title" + letter,
                name + "_attacks_rival_initial_announcement" + letter);
    loadMessage(troops.fFirstReminder,
                name + "_attacks_rival_title" + letter,
                name + "_attacks_rival_first_reminder" + letter);
    loadMessage(troops.fLastReminder,
                name + "_attacks_rival_title" + letter,
                name + "_attacks_rival_last_reminder" + letter);

    loadReason(troops.fEarlyReason,
               name + "_attacks_rival_early_reason" + letter);
    loadReason(troops.fComplyReason,
               name + "_attacks_rival_comply_reason" + letter);
    loadReason(troops.fTooLateReason,
               name + "_attacks_rival_too_late_reason" + letter);
    loadReason(troops.fRefuseReason,
               name + "_attacks_rival_refuse_reason" + letter);
    loadReason(troops.fLostBattleReason,
               name + "_attacks_rival_lost_battle_reason" + letter);

    loadMessage(troops.fConquered,
                name + "_conquers_rival_title",
                name + "_conquers_rival_initial_announcement");
}

bool eMessages::loadImpl() {
    if(mLoaded) return false;
    mLoaded = true;

    const std::string path = eGameDir::path("Model/Zeus eventmsg.txt");
    eLoadTextHelper::load(path, fMessages);

    fPop100.fFull.fTitle = loadMessage("PHRASE_population_100_title");
    fPop100.fFull.fText = loadMessage("PHRASE_population_100_initial_announcement");
    fPop100.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_100_initial_announcement");

    fPop500.fFull.fTitle = loadMessage("PHRASE_population_500_title");
    fPop500.fFull.fText = loadMessage("PHRASE_population_500_initial_announcement");
    fPop500.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_500_initial_announcement");

    fPop1000.fFull.fTitle = loadMessage("PHRASE_population_1000_title");
    fPop1000.fFull.fText = loadMessage("PHRASE_population_1000_initial_announcement");
    fPop1000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_1000_initial_announcement");

    fPop2000.fFull.fTitle = loadMessage("PHRASE_population_2000_title");
    fPop2000.fFull.fText = loadMessage("PHRASE_population_2000_initial_announcement");
    fPop2000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_2000_initial_announcement");

    fPop3000.fFull.fTitle = loadMessage("PHRASE_population_3000_title");
    fPop3000.fFull.fText = loadMessage("PHRASE_population_3000_initial_announcement");
    fPop3000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_3000_initial_announcement");

    fPop5000.fFull.fTitle = loadMessage("PHRASE_population_5000_title");
    fPop5000.fFull.fText = loadMessage("PHRASE_population_5000_initial_announcement");
    fPop5000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_5000_initial_announcement");

    fPop10000.fFull.fTitle = loadMessage("PHRASE_population_10000_title");
    fPop10000.fFull.fText = loadMessage("PHRASE_population_10000_initial_announcement");
    fPop10000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_10000_initial_announcement");

    fPop15000.fFull.fTitle = loadMessage("PHRASE_population_15000_title");
    fPop15000.fFull.fText = loadMessage("PHRASE_population_15000_initial_announcement");
    fPop15000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_15000_initial_announcement");

    fPop20000.fFull.fTitle = loadMessage("PHRASE_population_20000_title");
    fPop20000.fFull.fText = loadMessage("PHRASE_population_20000_initial_announcement");
    fPop20000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_20000_initial_announcement");

    fPop25000.fFull.fTitle = loadMessage("PHRASE_population_25000_title");
    fPop25000.fFull.fText = loadMessage("PHRASE_population_25000_initial_announcement");
    fPop25000.fCondensed.fText = loadMessage("PHRASE_CONDENSED_population_25000_initial_announcement");

    fHappinessMonument.fFull.fTitle = loadMessage("PHRASE_happiness_monument_title");
    fHappinessMonument.fFull.fText = loadMessage("PHRASE_happiness_monument_initial_announcement");
    fHappinessMonument.fCondensed.fText = loadMessage("PHRASE_CONDENSED_happiness_monument_initial_announcement");

    fFire.fFull.fTitle = loadMessage("PHRASE_fire_title");
    fFire.fFull.fText = loadMessage("PHRASE_fire_initial_announcement");
    fFire.fCondensed.fText = loadMessage("PHRASE_CONDENSED_fire_initial_announcement");

    fCollapse.fFull.fTitle = loadMessage("PHRASE_collapsed_building_title");
    fCollapse.fFull.fText = loadMessage("PHRASE_collapsed_building_initial_announcement");
    fCollapse.fCondensed.fText = loadMessage("PHRASE_CONDENSED_collapsed_building_initial_announcement");

    fPlagueStrikes.fFull.fTitle = loadMessage("PHRASE_plague_title");
    fPlagueStrikes.fFull.fText = loadMessage("PHRASE_plague_initial_announcement");
    fPlagueStrikes.fCondensed.fText = loadMessage("PHRASE_CONDENSED_plague_initial_announcement");

    load(fAphrodite, "aphrodite");
    load(fApollo, "apollo");
    load(fAres, "ares");
    load(fArtemis, "artemis");
    load(fAthena, "athena");
    load(fAtlas, "atlas");
    load(fDemeter, "demeter");
    load(fDionysus, "dionysus");
    load(fHades, "hades");
    load(fHephaestus, "hephaestus");
    load(fHera, "hera");
    load(fHermes, "hermes");
    load(fPoseidon, "poseidon");
    load(fZeus, "zeus");

    fZeusTradeResumes.fFull.fTitle = loadMessage("PHRASE_zeus_invasion_trade_resumes_title");
    fZeusTradeResumes.fFull.fText = loadMessage("PHRASE_zeus_invasion_trade_resumes_initial_announcement");
    fZeusTradeResumes.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_zeus_invasion_trade_resumes_title");
    fZeusTradeResumes.fCondensed.fText = loadMessage("PHRASE_CONDENSED_zeus_invasion_trade_resumes_initial_announcement");

    fPoseidonTradeResumes.fFull.fTitle = loadMessage("PHRASE_poseidon_invasion_trade_resumes_title");
    fPoseidonTradeResumes.fFull.fText = loadMessage("PHRASE_poseidon_invasion_trade_resumes_initial_announcement");
    fPoseidonTradeResumes.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_poseidon_invasion_trade_resumes_title");
    fPoseidonTradeResumes.fCondensed.fText = loadMessage("PHRASE_CONDENSED_poseidon_invasion_trade_resumes_initial_announcement");

    fHermesTradeResumes.fFull.fTitle = loadMessage("PHRASE_hermes_invasion_trade_resumes_title");
    fHermesTradeResumes.fFull.fText = loadMessage("PHRASE_hermes_invasion_trade_resumes_initial_announcement");
    fHermesTradeResumes.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_hermes_invasion_trade_resumes_title");
    fHermesTradeResumes.fCondensed.fText = loadMessage("PHRASE_CONDENSED_hermes_invasion_trade_resumes_initial_announcement");

    load(fAchilles, "achilles");
    load(fAtalanta, "atalanta");
    load(fBellerophon, "bellerophon");
    load(fHercules, "hercules");
    load(fJason, "jason");
    load(fOdysseus, "odysseus");
    load(fPerseus, "perseus");
    load(fTheseus, "theseus");

    load(fCalydonianBoar, "calydonian_boar", "calydonianboar");
    load(fCerberus, "cerberus", "cerberus");
    load(fChimera, "chimera", "chimera");
    load(fCyclops, "cyclops", "cyclops");
    load(fDragon, "dragon", "dragon");
    load(fEchidna, "echidna", "echidna");
    load(fHarpies, "harpies", "harpies");
    load(fHector, "hector", "hector");
    load(fHydra, "hydra", "hydra");
    load(fKraken, "kraken", "kraken");
    load(fMaenads, "maenads", "maenads");
    load(fMedusa, "medusa", "medusa");
    load(fMinotaur, "minotaur", "minotaur");
    load(fScylla, "scylla", "scylla");
    load(fSphinx, "sphinx", "sphinx");
    load(fTalos, "talos", "talos");

    fHomeGames.fBegin.fFull.fTitle = loadMessage("PHRASE_home_games_begin_title");
    fHomeGames.fBegin.fFull.fText = loadMessage("PHRASE_home_games_begin_initial_announcement");
    fHomeGames.fBegin.fCondensed.fText = loadMessage("PHRASE_CONDENSED_home_games_begin_initial_announcement");
    fHomeGames.fNoPart.fFull.fTitle = loadMessage("PHRASE_home_games_cancelled_title");
    fHomeGames.fNoPart.fFull.fText = loadMessage("PHRASE_home_games_cancelled_initial_announcement");
    fHomeGames.fNoPart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_home_games_cancelled_initial_announcement");
    fHomeGames.fWon.fFull.fTitle = loadMessage("PHRASE_home_games_conclude_title");
    fHomeGames.fWon.fFull.fText = loadMessage("PHRASE_home_games_conclude_initial_announcement_won");
    fHomeGames.fWon.fCondensed.fText = loadMessage("PHRASE_CONDENSED_home_games_conclude_initial_announcement_won");
    fHomeGames.fSecond.fFull.fTitle = loadMessage("PHRASE_home_games_conclude_title");
    fHomeGames.fSecond.fFull.fText = loadMessage("PHRASE_home_games_conclude_initial_announcement_second");
    fHomeGames.fSecond.fCondensed.fText = loadMessage("PHRASE_CONDENSED_home_games_conclude_initial_announcement_second");
    fHomeGames.fLost.fFull.fTitle = loadMessage("PHRASE_home_games_conclude_title");
    fHomeGames.fLost.fFull.fText = loadMessage("PHRASE_home_games_conclude_initial_announcement_lost");
    fHomeGames.fLost.fCondensed.fText = loadMessage("PHRASE_CONDENSED_home_games_conclude_initial_announcement_lost");

    fOlympianGames.fBegin.fFull.fTitle = loadMessage("PHRASE_olympian_games_begin_title");
    fOlympianGames.fBegin.fFull.fText = loadMessage("PHRASE_olympian_games_begin_initial_announcement_go");
    fOlympianGames.fBegin.fCondensed.fText = loadMessage("PHRASE_CONDENSED_olympian_games_begin_initial_announcement_go");
    fOlympianGames.fNoPart.fFull.fTitle = loadMessage("PHRASE_olympian_games_cant_participate_title");
    fOlympianGames.fNoPart.fFull.fText = loadMessage("PHRASE_olympian_games_cant_participate_initial_announcement");
    fOlympianGames.fNoPart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_olympian_games_cant_participate_initial_announcement");
    fOlympianGames.fWon.fFull.fTitle = loadMessage("PHRASE_olympian_games_conclude_title");
    fOlympianGames.fWon.fFull.fText = loadMessage("PHRASE_olympian_games_conclude_initial_announcement_won");
    fOlympianGames.fWon.fCondensed.fText = loadMessage("PHRASE_CONDENSED_olympian_games_conclude_initial_announcement_won");
    fOlympianGames.fSecond.fFull.fTitle = loadMessage("PHRASE_olympian_games_conclude_title");
    fOlympianGames.fSecond.fFull.fText = loadMessage("PHRASE_olympian_games_conclude_initial_announcement_second");
    fOlympianGames.fSecond.fCondensed.fText = loadMessage("PHRASE_CONDENSED_olympian_games_conclude_initial_announcement_second");
    fOlympianGames.fLost.fFull.fTitle = loadMessage("PHRASE_olympian_games_conclude_title");
    fOlympianGames.fLost.fFull.fText = loadMessage("PHRASE_olympian_games_conclude_initial_announcement_lost");
    fOlympianGames.fLost.fCondensed.fText = loadMessage("PHRASE_CONDENSED_olympian_games_conclude_initial_announcement_lost");

    fPythianGames.fBegin.fFull.fTitle = loadMessage("PHRASE_pythian_games_begin_title");
    fPythianGames.fBegin.fFull.fText = loadMessage("PHRASE_pythian_games_begin_initial_announcement_go");
    fPythianGames.fBegin.fCondensed.fText = loadMessage("PHRASE_CONDENSED_pythian_games_begin_initial_announcement_go");
    fPythianGames.fNoPart.fFull.fTitle = loadMessage("PHRASE_pythian_games_cant_participate_title");
    fPythianGames.fNoPart.fFull.fText = loadMessage("PHRASE_pythian_games_cant_participate_initial_announcement");
    fPythianGames.fNoPart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_pythian_games_cant_participate_initial_announcement");
    fPythianGames.fWon.fFull.fTitle = loadMessage("PHRASE_pythian_games_conclude_title");
    fPythianGames.fWon.fFull.fText = loadMessage("PHRASE_pythian_games_conclude_initial_announcement_won");
    fPythianGames.fWon.fCondensed.fText = loadMessage("PHRASE_CONDENSED_pythian_games_conclude_initial_announcement_won");
    fPythianGames.fSecond.fFull.fTitle = loadMessage("PHRASE_pythian_games_conclude_title");
    fPythianGames.fSecond.fFull.fText = loadMessage("PHRASE_pythian_games_conclude_initial_announcement_second");
    fPythianGames.fSecond.fCondensed.fText = loadMessage("PHRASE_CONDENSED_pythian_games_conclude_initial_announcement_second");
    fPythianGames.fLost.fFull.fTitle = loadMessage("PHRASE_pythian_games_conclude_title");
    fPythianGames.fLost.fFull.fText = loadMessage("PHRASE_pythian_games_conclude_initial_announcement_lost");
    fPythianGames.fLost.fCondensed.fText = loadMessage("PHRASE_CONDENSED_pythian_games_conclude_initial_announcement_lost");

    fIsthmianGames.fBegin.fFull.fTitle = loadMessage("PHRASE_isthmian_games_begin_title");
    fIsthmianGames.fBegin.fFull.fText = loadMessage("PHRASE_isthmian_games_begin_initial_announcement_go");
    fIsthmianGames.fBegin.fCondensed.fText = loadMessage("PHRASE_CONDENSED_isthmian_games_begin_initial_announcement_go");
    fIsthmianGames.fNoPart.fFull.fTitle = loadMessage("PHRASE_isthmian_games_cant_participate_title");
    fIsthmianGames.fNoPart.fFull.fText = loadMessage("PHRASE_isthmian_games_cant_participate_initial_announcement");
    fIsthmianGames.fNoPart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_isthmian_games_cant_participate_initial_announcement");
    fIsthmianGames.fWon.fFull.fTitle = loadMessage("PHRASE_isthmian_games_conclude_title");
    fIsthmianGames.fWon.fFull.fText = loadMessage("PHRASE_isthmian_games_conclude_initial_announcement_won");
    fIsthmianGames.fWon.fCondensed.fText = loadMessage("PHRASE_CONDENSED_isthmian_games_conclude_initial_announcement_won");
    fIsthmianGames.fSecond.fFull.fTitle = loadMessage("PHRASE_isthmian_games_conclude_title");
    fIsthmianGames.fSecond.fFull.fText = loadMessage("PHRASE_isthmian_games_conclude_initial_announcement_second");
    fIsthmianGames.fSecond.fCondensed.fText = loadMessage("PHRASE_CONDENSED_isthmian_games_conclude_initial_announcement_second");
    fIsthmianGames.fLost.fFull.fTitle = loadMessage("PHRASE_isthmian_games_conclude_title");
    fIsthmianGames.fLost.fFull.fText = loadMessage("PHRASE_isthmian_games_conclude_initial_announcement_lost");
    fIsthmianGames.fLost.fCondensed.fText = loadMessage("PHRASE_CONDENSED_isthmian_games_conclude_initial_announcement_lost");

    fNemeanGames.fBegin.fFull.fTitle = loadMessage("PHRASE_nemean_games_begin_title");
    fNemeanGames.fBegin.fFull.fText = loadMessage("PHRASE_nemean_games_begin_initial_announcement_go");
    fNemeanGames.fBegin.fCondensed.fText = loadMessage("PHRASE_CONDENSED_nemean_games_begin_initial_announcement_go");
    fNemeanGames.fNoPart.fFull.fTitle = loadMessage("PHRASE_nemean_games_cant_participate_title");
    fNemeanGames.fNoPart.fFull.fText = loadMessage("PHRASE_nemean_games_cant_participate_initial_announcement");
    fNemeanGames.fNoPart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_nemean_games_cant_participate_initial_announcement");
    fNemeanGames.fWon.fFull.fTitle = loadMessage("PHRASE_nemean_games_conclude_title");
    fNemeanGames.fWon.fFull.fText = loadMessage("PHRASE_nemean_games_conclude_initial_announcement_won");
    fNemeanGames.fWon.fCondensed.fText = loadMessage("PHRASE_CONDENSED_nemean_games_conclude_initial_announcement_won");
    fNemeanGames.fSecond.fFull.fTitle = loadMessage("PHRASE_nemean_games_conclude_title");
    fNemeanGames.fSecond.fFull.fText = loadMessage("PHRASE_nemean_games_conclude_initial_announcement_second");
    fNemeanGames.fSecond.fCondensed.fText = loadMessage("PHRASE_CONDENSED_nemean_games_conclude_initial_announcement_second");
    fNemeanGames.fLost.fFull.fTitle = loadMessage("PHRASE_nemean_games_conclude_title");
    fNemeanGames.fLost.fFull.fText = loadMessage("PHRASE_nemean_games_conclude_initial_announcement_lost");
    fNemeanGames.fLost.fCondensed.fText = loadMessage("PHRASE_CONDENSED_nemean_games_conclude_initial_announcement_lost");

    fInvasionInitial.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasionInitial.fFull.fText = loadMessage("PHRASE_invasion_initial_announcement");
    fInvasionInitial.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasionInitial.fCondensed.fText = loadMessage("PHRASE_CONDENSED_invasion_initial_announcement");

    fInvasion24.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasion24.fFull.fText = loadMessage("PHRASE_invasion_2year_reminder");
    fInvasion24.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasion24.fCondensed.fText = loadMessage("PHRASE_CONDENSED_invasion_2year_reminder");

    fInvasion12.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasion12.fFull.fText = loadMessage("PHRASE_invasion_1year_reminder");
    fInvasion12.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasion12.fCondensed.fText = loadMessage("PHRASE_CONDENSED_invasion_1year_reminder");

    fInvasion6.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasion6.fFull.fText = loadMessage("PHRASE_invasion_6month_warning");
    fInvasion6.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasion6.fCondensed.fText = loadMessage("PHRASE_CONDENSED_invasion_6month_warning");

    fInvasion1.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasion1.fFull.fText = loadMessage("PHRASE_invasion_1month_Warning");
    fInvasion1.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasion1.fCondensed.fText = loadMessage("PHRASE_invasion_1month_Warning");

    fInvasion.fFull.fTitle = loadMessage("PHRASE_invasion_title");
    fInvasion.fFull.fText = loadMessage("PHRASE_invasion_city_attacked_alert");
    fInvasion.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_invasion_title");
    fInvasion.fCondensed.fText = loadMessage("PHRASE_CONDENSED_invasion_city_attacked_alert");

    fInvasionBribed.fFull.fTitle = loadMessage("PHRASE_players_city_saved_bribery_title");
    fInvasionBribed.fFull.fText = loadMessage("PHRASE_players_city_saved_bribery_initial_announcement");

    fInvasionBribed.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_players_city_saved_bribery_title");
    fInvasionBribed.fCondensed.fText = loadMessage("PHRASE_CONDENSED_players_city_saved_bribery_initial_announcement");

    fInvasionBribed.fFull.fTitle = loadMessage("PHRASE_players_city_saved_bribery_title");
    fInvasionBribed.fFull.fText = loadMessage("PHRASE_players_city_saved_bribery_initial_announcement");

    fInvasionBribed.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_players_city_saved_bribery_title");
    fInvasionBribed.fCondensed.fText = loadMessage("PHRASE_CONDENSED_players_city_saved_bribery_initial_announcement");

    fInvasionDefeat.fFull.fTitle = loadMessage("PHRASE_enemy_victorious_title");
    fInvasionDefeat.fFull.fText = loadMessage("PHRASE_enemy_victorious_initial_announcement");

    fInvasionDefeat.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_enemy_victorious_title");
    fInvasionDefeat.fCondensed.fText = loadMessage("PHRASE_CONDENSED_enemy_victorious_initial_announcement");

    fInvasionVictory.fFull.fTitle = loadMessage("PHRASE_players_city_saved_title");
    fInvasionVictory.fFull.fText = loadMessage("PHRASE_players_city_saved_initial_announcement");

    fInvasionVictory.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_players_city_saved_title");
    fInvasionVictory.fCondensed.fText = loadMessage("PHRASE_CONDENSED_players_city_saved_initial_announcement");

    fInvasionVictoryMonument.fFull.fTitle = loadMessage("PHRASE_players_city_saved_title");
    fInvasionVictoryMonument.fFull.fText = loadMessage("PHRASE_players_city_saved_init_announcment_comm_monument");

    fInvasionVictoryMonument.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_players_city_saved_title");
    fInvasionVictoryMonument.fCondensed.fText = loadMessage("PHRASE_CONDENSED_players_city_saved_init_announcment_comm_monument");

    fTributePaidGranted.fFull.fTitle = loadMessage("PHRASE_tribute_paid_title");
    fTributePaidGranted.fFull.fText = loadMessage("PHRASE_tribute_paid_granted");
    fTributePaidGranted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_tribute_paid_title");
    fTributePaidGranted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_tribute_paid_granted");

    fTributePaidAccepted.fFull.fTitle = loadMessage("PHRASE_tribute_paid_title");
    fTributePaidAccepted.fFull.fText = loadMessage("PHRASE_tribute_paid_accepted");
    fTributePaidAccepted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_tribute_paid_title");
    fTributePaidAccepted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_tribute_paid_accepted");

    fTributePaidRefused.fFull.fTitle = loadMessage("PHRASE_tribute_paid_title");
    fTributePaidRefused.fFull.fText = loadMessage("PHRASE_tribute_paid_refused");
    fTributePaidRefused.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_tribute_paid_title");
    fTributePaidRefused.fCondensed.fText = loadMessage("PHRASE_CONDENSED_tribute_paid_refused");

    fTributePostponed.fFull.fTitle = loadMessage("PHRASE_tribute_paid_title");
    fTributePostponed.fFull.fText = loadMessage("PHRASE_tribute_paid_postponed");
    fTributePostponed.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_tribute_paid_title");
    fTributePostponed.fCondensed.fText = loadMessage("PHRASE_CONDENSED_tribute_paid_postponed");

    fRequestGranted.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestGranted.fFull.fText = loadMessage("PHRASE_request_granted_granted");
    fRequestGranted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestGranted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_granted");

    fRequestPartialSpace.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestPartialSpace.fFull.fText = loadMessage("PHRASE_request_granted_partial_space");
    fRequestPartialSpace.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestPartialSpace.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_partial_space");

    fRequestInsufficientSpace.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestInsufficientSpace.fFull.fText = loadMessage("PHRASE_request_granted_insufficient_space");
    fRequestInsufficientSpace.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestInsufficientSpace.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_insufficient_space");

    fRequestLastChance.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestLastChance.fFull.fText = loadMessage("PHRASE_request_granted_last_chance");
    fRequestLastChance.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestLastChance.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_last_chance");

    fRequestAccepted.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestAccepted.fFull.fText = loadMessage("PHRASE_request_granted_accepted");
    fRequestAccepted.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestAccepted.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_accepted");

    fRequestPostponed.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestPostponed.fFull.fText = loadMessage("PHRASE_request_granted_postponed");
    fRequestPostponed.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestPostponed.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_postponed");

    fRequestRefused.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestRefused.fFull.fText = loadMessage("PHRASE_request_granted_refused");
    fRequestRefused.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestRefused.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_refused");

    fRequestForfeited.fFull.fTitle = loadMessage("PHRASE_request_granted_title");
    fRequestForfeited.fFull.fText = loadMessage("PHRASE_request_granted_forfeited");
    fRequestForfeited.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_request_granted_title");
    fRequestForfeited.fCondensed.fText = loadMessage("PHRASE_CONDENSED_request_granted_forfeited");

    fDemandRefused.fFull.fTitle = loadMessage("PHRASE_demand_refused_title");
    fDemandRefused.fFull.fText = loadMessage("PHRASE_demand_refused_initial_announcement");
    fDemandRefused.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_demand_refused_title");
    fDemandRefused.fCondensed.fText = loadMessage("PHRASE_CONDENSED_demand_refused_initial_announcement");

    fGiftReceivedDrachmas.fFull.fTitle = loadMessage("PHRASE_gift_received_money_title");
    fGiftReceivedDrachmas.fFull.fText = loadMessage("PHRASE_gift_received_money_initial_announcement");
    fGiftReceivedDrachmas.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_gift_received_money_title");
    fGiftReceivedDrachmas.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_received_money_initial_announcement");

    fGiftReceivedNeeded.fFull.fTitle = loadMessage("PHRASE_gift_received_needs_title");
    fGiftReceivedNeeded.fFull.fText = loadMessage("PHRASE_gift_received_needs_initial_announcement");
    fGiftReceivedNeeded.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_gift_received_needs_title");
    fGiftReceivedNeeded.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_received_needs_initial_announcement");

    fGiftReceivedSells.fFull.fTitle = loadMessage("PHRASE_gift_received_produces_title");
    fGiftReceivedSells.fFull.fText = loadMessage("PHRASE_gift_received_produces_initial_announcement");
    fGiftReceivedSells.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_gift_received_produces_title");
    fGiftReceivedSells.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_received_produces_initial_announcement");

    fGiftReceivedNotNeeded.fFull.fTitle = loadMessage("PHRASE_gift_received_doesnt_need_title");
    fGiftReceivedNotNeeded.fFull.fText = loadMessage("PHRASE_gift_received_doesnt_need_initial_announcement");
    fGiftReceivedNotNeeded.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_gift_received_doesnt_need_title");
    fGiftReceivedNotNeeded.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_received_doesnt_need_initial_announcement");

    fGiftReceivedRefuse.fFull.fTitle = loadMessage("PHRASE_gift_refused_title");
    fGiftReceivedRefuse.fFull.fText = loadMessage("PHRASE_gift_refused_initial_announcement");
    fGiftReceivedRefuse.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_gift_refused_title");
    fGiftReceivedRefuse.fCondensed.fText = loadMessage("PHRASE_CONDENSED_gift_refused_initial_announcement");

    loadGeneralRequest(fGeneralRequestAllyS, "S");
    loadGeneralRequest(fGeneralRequestRivalD, "D");
    loadGeneralRequest(fGeneralRequestSubjectP, "P");
    loadGeneralRequest(fGeneralRequestParentR, "R");

    loadGeneralRequest(fFamineAllyS, "S", "famine_relief_relief");
    loadGeneralRequest(fFamineRivalD, "D", "famine_relief_relief");
    loadGeneralRequest(fFamineSubjectP, "P", "famine_relief_relief");
    loadGeneralRequest(fFamineParentR, "R", "famine_relief_relief");

    loadGeneralRequest(fProjectAllyS, "S", "building_project");
    loadGeneralRequest(fProjectRivalD, "D", "building_project");
    loadGeneralRequest(fProjectSubjectP, "P", "building_project");
    loadGeneralRequest(fProjectParentR, "R", "building_project");

    loadGeneralRequest(fFestivalAllyS, "S", "great_festival");
    loadGeneralRequest(fFestivalRivalD, "D", "great_festival");
    loadGeneralRequest(fFestivalSubjectP, "P", "great_festival");
    loadGeneralRequest(fFestivalParentR, "R", "great_festival");

    loadGeneralRequest(fFinancialWoesAllyS, "S", "financial_woes");
    loadGeneralRequest(fFinancialWoesRivalD, "D", "financial_woes");
    loadGeneralRequest(fFinancialWoesSubjectP, "P", "financial_woes");
    loadGeneralRequest(fFinancialWoesParentR, "R", "financial_woes");

    loadGeneralRequest(fTributeRequest, "D", "tribute_due");

    loadResourceGranted(fGiftGranted, "gift");

    fSupportTroopsArrive.fFull.fTitle = loadMessage("PHRASE_support_troops_arrive_title");
    fSupportTroopsArrive.fFull.fText = loadMessage("PHRASE_support_troops_arrive_initial_announcement");
    fSupportTroopsArrive.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_support_troops_arrive_title");
    fSupportTroopsArrive.fCondensed.fText = loadMessage("PHRASE_CONDENSED_support_troops_arrive_initial_announcement");

    fSupportTroopsDepart.fFull.fTitle = loadMessage("PHRASE_support_troops_depart_title");
    fSupportTroopsDepart.fFull.fText = loadMessage("PHRASE_support_troops_depart_initial_announcement");
    fSupportTroopsDepart.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_support_troops_depart_title");
    fSupportTroopsDepart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_support_troops_depart_initial_announcement");

    fSupportTroopsDefeated.fFull.fTitle = loadMessage("PHRASE_support_troops_defeated_title");
    fSupportTroopsDefeated.fFull.fText = loadMessage("PHRASE_support_troops_defeated_initial_announcement");
    fSupportTroopsDefeated.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_support_troops_defeated_title");
    fSupportTroopsDefeated.fCondensed.fText = loadMessage("PHRASE_CONDENSED_support_troops_defeated_initial_announcement");

    fStrikeDepart.fFull.fTitle = loadMessage("PHRASE_strike_force_sets_forth_title");
    fStrikeDepart.fFull.fText = loadMessage("PHRASE_strike_force_sets_forth_initial_announcement");
    fStrikeDepart.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_strike_force_sets_forth_title");
    fStrikeDepart.fCondensed.fText = loadMessage("PHRASE_CONDENSED_strike_force_sets_forth_initial_announcement");

    fStrikeSuccessful.fFull.fTitle = loadMessage("PHRASE_strike_successful_title");
    fStrikeSuccessful.fFull.fText = loadMessage("PHRASE_strike_successful_initial_announcement");
    fStrikeSuccessful.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_strike_successful_title");
    fStrikeSuccessful.fCondensed.fText = loadMessage("PHRASE_CONDENSED_strike_successful_initial_announcement");

    fStrikeUnsuccessul.fFull.fTitle = loadMessage("PHRASE_strike_unsuccessful_title");
    fStrikeUnsuccessul.fFull.fText = loadMessage("PHRASE_strike_unsuccessful_initial_announcement");
    fStrikeUnsuccessul.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_strike_unsuccessful_title");
    fStrikeUnsuccessul.fCondensed.fText = loadMessage("PHRASE_CONDENSED_strike_unsuccessful_initial_announcement");

    fRivalArmyAway.fFull.fTitle = loadMessage("PHRASE_rival_away_title");
    fRivalArmyAway.fFull.fText = loadMessage("PHRASE_rival_away_initial_announcement");
    fRivalArmyAway.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_rival_away_title");
    fRivalArmyAway.fCondensed.fText = loadMessage("PHRASE_CONDENSED_rival_away_initial_announcement");

    fIncreasedFavor.fFull.fTitle = loadMessage("PHRASE_favor_change_title_I");
    fIncreasedFavor.fFull.fText = loadMessage("PHRASE_favor_change_initial_announcement_I");
    fIncreasedFavor.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_favor_change_title_I");
    fIncreasedFavor.fCondensed.fText = loadMessage("PHRASE_CONDENSED_favor_change_initial_announcement_I");

    fDecreasedFavor.fFull.fTitle = loadMessage("PHRASE_favor_change_title_D");
    fDecreasedFavor.fFull.fText = loadMessage("PHRASE_favor_change_initial_announcement_D");
    fDecreasedFavor.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_favor_change_title_D");
    fDecreasedFavor.fCondensed.fText = loadMessage("PHRASE_CONDENSED_favor_change_initial_announcement_D");

    fUnemployment.fFull.fTitle = loadMessage(";PHRASE_unemployment_title");
    fUnemployment.fFull.fText = loadMessage(";PHRASE_unemployment_initial_announcement");
    fUnemployment.fCondensed.fTitle = loadMessage(";PHRASE_CONDENSED_unemployment_title");
    fUnemployment.fCondensed.fText = loadMessage(";PHRASE_CONDENSED_unemployment_initial_announcement");

    fEmployees.fFull.fTitle = loadMessage("PHRASE_employees_needed_title");
    fEmployees.fFull.fText = loadMessage("PHRASE_employees_needed_initial_announcement");
    fEmployees.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_employees_needed_title");
    fEmployees.fCondensed.fText = loadMessage("PHRASE_CONDENSED_employees_needed_initial_announcement");

    fPlague.fFull.fTitle = loadMessage("PHRASE_plague_title");
    fPlague.fFull.fText = loadMessage("PHRASE_plague_initial_announcement");
    fPlague.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_plague_title");
    fPlague.fCondensed.fText = loadMessage("PHRASE_CONDENSED_plague_initial_announcement");

    fCityConqueredByPlayer.fFull.fTitle = loadMessage("PHRASE_city_conquered_by_player_title");
    fCityConqueredByPlayer.fFull.fText = loadMessage("PHRASE_city_conquered_by_player_initial_announcement");
    fCityConqueredByPlayer.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_city_conquered_by_player_title");
    fCityConqueredByPlayer.fCondensed.fText = loadMessage("PHRASE_CONDENSED_city_conquered_by_player_initial_announcement");

    fCityConquerFailedByPlayer.fFull.fTitle = loadMessage("PHRASE_players_conquest_fails_title");
    fCityConquerFailedByPlayer.fFull.fText = loadMessage("PHRASE_players_conquest_fails_initial_announcement");
    fCityConquerFailedByPlayer.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_players_conquest_fails_title");
    fCityConquerFailedByPlayer.fCondensed.fText = loadMessage("PHRASE_CONDENSED_players_conquest_fails_initial_announcement");

    fCityRaidFailedByPlayer.fFull.fTitle = loadMessage("PHRASE_raiding_party_wiped_out_title");
    fCityRaidFailedByPlayer.fFull.fText = loadMessage("PHRASE_raiding_party_wiped_out_initial_announcement");
    fCityRaidFailedByPlayer.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_raiding_party_wiped_out_title");
    fCityRaidFailedByPlayer.fCondensed.fText = loadMessage("PHRASE_CONDENSED_raiding_party_wiped_out_initial_announcement");

    loadResourceGranted(fRaidGranted, "raiding_party_returns");

    fAllyAttackedByPlayer.fFull.fTitle = loadMessage("PHRASE_allies_hate_you_title");
    fAllyAttackedByPlayer.fFull.fText = loadMessage("PHRASE_allies_hate_you_initial_announcement");
    fAllyAttackedByPlayer.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_allies_hate_you_title");
    fAllyAttackedByPlayer.fCondensed.fText = loadMessage("PHRASE_CONDENSED_allies_hate_you_initial_announcement");

    fArmyReturns.fFull.fTitle = loadMessage("PHRASE_army_returns_title");
    fArmyReturns.fFull.fText = loadMessage("PHRASE_army_returns_initial_announcement");
    fArmyReturns.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_army_returns_title");
    fArmyReturns.fCondensed.fText = loadMessage("PHRASE_CONDENSED_army_returns_initial_announcement");

    fMilitaryBuildup.fFull.fTitle = loadMessage("PHRASE_military_change_title_I");
    fMilitaryBuildup.fFull.fText = loadMessage("PHRASE_military_change_initial_announcement_I");
    fMilitaryBuildup.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_military_change_title_I");
    fMilitaryBuildup.fCondensed.fText = loadMessage("PHRASE_CONDENSED_military_change_initial_announcement_I");

    fMilitaryDecline.fFull.fTitle = loadMessage("PHRASE_military_change_title_D");
    fMilitaryDecline.fFull.fText = loadMessage("PHRASE_military_change_initial_announcement_D");
    fMilitaryDecline.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_military_change_title_D");
    fMilitaryDecline.fCondensed.fText = loadMessage("PHRASE_CONDENSED_military_change_initial_announcement_D");

    fEconomicProsperity.fFull.fTitle = loadMessage("PHRASE_economic_change_title_I");
    fEconomicProsperity.fFull.fText = loadMessage("PHRASE_economic_change_initial_announcement_I");
    fEconomicProsperity.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_economic_change_title_I");
    fEconomicProsperity.fCondensed.fText = loadMessage("PHRASE_CONDENSED_economic_change_initial_announcement_I");

    fEconomicDecline.fFull.fTitle = loadMessage("PHRASE_economic_change_title_D");
    fEconomicDecline.fFull.fText = loadMessage("PHRASE_economic_change_initial_announcement_D");
    fEconomicDecline.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_economic_change_title_D");
    fEconomicDecline.fCondensed.fText = loadMessage("PHRASE_CONDENSED_economic_change_initial_announcement_D");

    load(fAllyTroopsRequest, "ally", "S");
    load(fVassalTroopsRequest, "vassal", "P");
    load(fColonyTroopsRequest, "colony", "P");
    load(fParentCityTroopsRequest, "parentcity", "R");

    load(fAllyTroopsMonsterRequest, "ally");
    load(fVassalTroopsMonsterRequest, "vassal");
    load(fColonyTroopsMonsterRequest, "colony");
    load(fParentCityTroopsMonsterRequest, "parentcity");

    loadMessage(fTroopsMonsterRequestAttackAverted,
                "monster_scared_off_title",
                "monster_scared_off_initial_announcement");

    load(fAllyTroopsAttackRequest, "ally", "");
    load(fVassalTroopsAttackRequest, "vassal", "P");

    loadMessage(fEarthqaukeNoReason,
                "quake_title",
                "quake_phrase_no_reason");
    loadMessage(fEarthqaukeGod,
                "quake_title",
                "quake_phrase_by_god");

    loadMessage(fTidalWaveNoReason,
                "flood_title",
                "flood_phrase_no_reason");
    loadMessage(fTidalWaveGod,
                "flood_title",
                "flood_phrase_by_god");

    loadMessage(fLavaFlowNoReason,
                "lava_title",
                "lava_phrase_no_reason");
    loadMessage(fLavaFlowGod,
                "lava_title",
                "lava_phrase_by_god");

    loadMessage(fSinkLandNoReason,
                "sinkland_title",
                "sinkland_phrase_no_reason");
    loadMessage(fSinkLandGod,
                "sinkland_title",
                "sinkland_phrase_by_god");

    loadMessage(fLandSlide,
                "landslide_title",
                "landslide_initial_announcement");

    loadMessage(fModestPyramidComplete1,
                "pyramid1_complete_title",
                "pyramid1_complete_initial_announcement");
    loadMessage(fPyramidComplete2,
                "pyramid2_complete_title",
                "pyramid2_complete_initial_announcement");
    loadMessage(fGreatPyramidComplete3,
                "pyramid3_complete_title",
                "pyramid3_complete_initial_announcement");
    loadMessage(fMajesticPyramidComplete4,
                "pyramid4_complete_title",
                "pyramid4_complete_initial_announcement");

    loadMessage(fSmallMonumentToTheSkyComplete5,
                "pyramid5_complete_title",
                "pyramid5_complete_initial_announcement");
    loadMessage(fMonumentToTheSkyComplete6,
                "pyramid6_complete_title",
                "pyramid6_complete_initial_announcement");
    loadMessage(fGrandMonumentToTheSkyComplete7,
                "pyramid7_complete_title",
                "pyramid7_complete_initial_announcement");

    loadMessage(fMinorShrineComplete8,
                "pyramid8_complete_title",
                "pyramid8_complete_initial_announcement");
    loadMessage(fShrineComplete9,
                "pyramid9_complete_title",
                "pyramid9_complete_initial_announcement");
    loadMessage(fMajorShrineComplete10,
                "pyramid10_complete_title",
                "pyramid10_complete_initial_announcement");


    loadMessage(fPyramidOfThePantheonComplete11,
                "pyramid11_complete_title",
                "pyramid11_complete_initial_announcement");
    loadMessage(fAltarOfOlympusComplete12,
                "pyramid12_complete_title",
                "pyramid12_complete_initial_announcement");
    loadMessage(fTempleOfOlympusComplete13,
                "pyramid13_complete_title",
                "pyramid13_complete_initial_announcement");
    loadMessage(fObservatoryKosmikaComplete14,
                "pyramid14_complete_title",
                "pyramid14_complete_initial_announcement");
    loadMessage(fMuseumAtlantikaComplete15,
                "pyramid15_complete_title",
                "pyramid15_complete_initial_announcement");

    fCityBecomesAlly.fFull.fTitle = loadMessage("PHRASE_becomes_ally_title");
    fCityBecomesAlly.fFull.fText = loadMessage("PHRASE_becomes_ally_initial_announcement");
    fCityBecomesAlly.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_becomes_ally_title");
    fCityBecomesAlly.fCondensed.fText = loadMessage("PHRASE_CONDENSED_becomes_ally_initial_announcement");

    fCityBecomesRival.fFull.fTitle = loadMessage("PHRASE_becomes_rival_title");
    fCityBecomesRival.fFull.fText = loadMessage("PHRASE_becomes_rival_initial_announcement");
    fCityBecomesRival.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_becomes_rival_title");
    fCityBecomesRival.fCondensed.fText = loadMessage("PHRASE_CONDENSED_becomes_rival_initial_announcement");

    fCityBecomesVassal.fFull.fTitle = loadMessage("PHRASE_becomes_vassal_title");
    fCityBecomesVassal.fFull.fText = loadMessage("PHRASE_becomes_vassal_initial_announcement");
    fCityBecomesVassal.fCondensed.fTitle = loadMessage("PHRASE_CONDENSED_becomes_vassal_title");
    fCityBecomesVassal.fCondensed.fText = loadMessage("PHRASE_CONDENSED_becomes_vassal_initial_announcement");

    loadMessage(fCityBecomesActive,
                "city_active_title",
                "city_active_initial_announcement");
    loadMessage(fCityBecomesInactive,
                "city_inactive_title",
                "city_inactive_initial_announcement");

    loadMessage(fCityBecomesVisible,
                "city_appears_title",
                "city_appears_initial_announcement");
    loadMessage(fCityBecomesInvisible,
                "city_disappears_title",
                "city_disappears_initial_announcement");

    loadMessage(fTributeSuspended,
                "tribute_suspended_title",
                "tribute_suspended_initial_announcement");
    loadMessage(fTributeResumed,
                "tribute_resumed_title",
                "tribute_resumed_initial_announcement");

    loadMessage(fCityRebellion,
                "rebellion_tribute_suspended_title",
                "rebellion_tribute_suspended_initial_announcement");
    loadMessage(fCityRebellionQuelled,
                "rebellion_quelled_tribute_resumed_title",
                "rebellion_quelled_tribute_resumed_initial_announcement");
    loadMessage(fCityRebellionOver,
                "rebellion_over_tribute_resumed_title",
                "rebellion_quelled_tribute_resumed_initial_announcement");

    loadMessage(fRivalConqueredByAlly,
                "ally_conquers_rival_title",
                "ally_conquers_rival_initial_announcement");
    loadMessage(fRivalConqueredByVassal,
                "vassal_conquers_rival_title",
                "vassal_conquers_rival_initial_announcement");

    loadMessage(fAllyConqueredByRival,
                "ally_conquered_by_rival_title",
                "ally_conquered_by_rival_initial_announcement");
    loadMessage(fVassalConqueredByRival,
                "vassal_conquered_by_rival_title",
                "vassal_conquered_by_rival_initial_announcement");
    loadMessage(fColonyConqueredByRival,
                "colony_conquered_by_rival_title",
                "colony_conquered_by_rival_initial_announcement");
    loadMessage(fParentConqueredByRival,
                "parent_city_falls_title",
                "parent_city_falls_initial_announcement");

    loadMessage(fTroopsRequestAttackAverted,
                "attack_averted_title",
                "attack_averted_initial_announcement");

    loadMessage(fColonyRestored,
                "colony_restored_title",
                "colony_restored_initial_announcement");

    loadMessage(fDebtAnniversary,
                "still_in_debt_title",
                "still_in_debt_initial_announcement");

    loadMessage(fColonyMonument,
                "colony_monument_title",
                "colony_monument_initial_announcement");

    loadMessage(fPriceIncrease,
                "price_change_title_I",
                "price_change_initial_announcement_I");
    loadMessage(fPriceDecrease,
                "price_change_title_D",
                "price_change_initial_announcement_D");

    loadMessage(fDemandIncrease,
                "demand_change_title_I",
                "demand_change_initial_announcement_I");
    loadMessage(fDemandDecrease,
                "demand_change_title_D",
                "demand_change_initial_announcement_D");

    loadMessage(fSupplyIncrease,
                "supply_change_title_I",
                "supply_change_initial_announcement_I");
    loadMessage(fSupplyDecrease,
                "supply_change_title_D",
                "supply_change_initial_announcement_D");

    loadMessage(fTradeShutsDown,
                "trade_shuts_down_title",
                "trade_shuts_down_initial_announcement");
    loadMessage(fTradeOpensUp,
                "trade_opens_up_title",
                "trade_opens_up_initial_announcement");

    loadMessage(fWageIncrease,
                "wage_change_title_I",
                "wage_change_initial_announcement_I");
    loadMessage(fWageDecrease,
                "wage_change_title_D",
                "wage_change_initial_announcement_D");

    loadMessage(fAreaCutOff,
                "road_rome_blocked_title",
                "road_rome_blocked_initial_announcement");

    return true;
}

bool eMessages::load() {
    return instance.loadImpl();
}

bool eMessages::loaded() {
    return instance.mLoaded;
}

void eMessages::reload() {
    if(!instance.mLoaded) return;
    instance.mLoaded = false;
    instance.fMessages.clear();
    instance.loadImpl();
}

std::string eMessages::message(const std::string& key) {
    return instance.loadMessage(key);
}

eMessageType eMessages::favorMessage(const eMessageType& reason) {
    const auto& ifavor = instance.fIncreasedFavor;
    eMessageType msg = ifavor;
    auto& text = msg.fFull.fText;
    eStringHelpers::replaceAll(text, "[reason_phrase]",
                               reason.fFull.fText);
    return msg;
}

eMessageType eMessages::dfavorMessage(const eMessageType& reason) {
    const auto& ifavor = instance.fDecreasedFavor;
    eMessageType msg = ifavor;
    auto& text = msg.fFull.fText;
    eStringHelpers::replaceAll(text, "[reason_phrase]",
                               reason.fFull.fText);
    return msg;
}

eMessageType eMessages::invasionMessage(const eMessageType& baseMsg,
                                        const std::string& reason,
                                        const int months) {
    eMessageType msg = baseMsg;
    auto& text = msg.fFull.fText;
    if(reason.empty()) {
        const auto& inst = eMessages::instance;
        eStringHelpers::replaceAll(text, "[reason_phrase]",
                                   inst.fInvasionNoReason);
    } else {
        eStringHelpers::replaceAll(text, "[reason_phrase]",
                                   reason);
    }
    eStringHelpers::replaceAll(text, "[time_until_attack]",
                               std::to_string(months));
    return msg;
}

const eGodMessages* eMessages::godMessages(const eGodType godType) const {
    switch(godType) {
    case eGodType::aphrodite:
        return &fAphrodite;
    case eGodType::apollo:
        return &fApollo;
    case eGodType::ares:
        return &fAres;
    case eGodType::artemis:
        return &fArtemis;
    case eGodType::athena:
        return &fAthena;
    case eGodType::atlas:
        return &fAtlas;
    case eGodType::demeter:
        return &fDemeter;
    case eGodType::dionysus:
        return &fDionysus;
    case eGodType::hades:
        return &fHades;
    case eGodType::hephaestus:
        return &fHephaestus;
    case eGodType::hera:
        return &fHera;
    case eGodType::hermes:
        return &fHermes;
    case eGodType::poseidon:
        return &fPoseidon;
    case eGodType::zeus:
        return &fZeus;
    }
    return nullptr;
}

const eHeroMessages* eMessages::heroMessages(const eHeroType heroType) const {
    switch(heroType) {
    case eHeroType::achilles:
        return &fAchilles;
    case eHeroType::atalanta:
        return &fAtalanta;
    case eHeroType::bellerophon:
        return &fBellerophon;
    case eHeroType::hercules:
        return &fHercules;
    case eHeroType::jason:
        return &fJason;
    case eHeroType::odysseus:
        return &fOdysseus;
    case eHeroType::perseus:
        return &fPerseus;
    case eHeroType::theseus:
        return &fTheseus;
    }
    return nullptr;
}

const eMonsterMessages* eMessages::monsterMessages(const eMonsterType monsterType) const {
    switch(monsterType) {
    case eMonsterType::calydonianBoar:
        return &fCalydonianBoar;
    case eMonsterType::cerberus:
        return &fCerberus;
    case eMonsterType::chimera:
        return &fChimera;
    case eMonsterType::cyclops:
        return &fCyclops;
    case eMonsterType::dragon:
        return &fDragon;
    case eMonsterType::echidna:
        return &fEchidna;
    case eMonsterType::harpies:
        return &fHarpies;
    case eMonsterType::hector:
        return &fHector;
    case eMonsterType::hydra:
        return &fHydra;
    case eMonsterType::kraken:
        return &fKraken;
    case eMonsterType::maenads:
        return &fMaenads;
    case eMonsterType::medusa:
        return &fMedusa;
    case eMonsterType::minotaur:
        return &fMinotaur;
    case eMonsterType::scylla:
        return &fScylla;
    case eMonsterType::sphinx:
        return &fSphinx;
    case eMonsterType::talos:
        return &fTalos;
    case eMonsterType::satyr:
        return nullptr;
    }
    return nullptr;
}
