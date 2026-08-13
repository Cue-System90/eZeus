#include "egameloadingwidget.h"

#include "textures/egametextures.h"
#include "audio/emusic.h"
#include "audio/esounds.h"
#include "emessages.h"
#include "buildings/sanctuaries/esanctuaryblueprint.h"
#include "emainwindow.h"
#include "elanguage.h"
#include "egamedir.h"

eGameLoadingWidget::eGameLoadingWidget(eMainWindow* const window) :
    eLoadingWidget(eGameTextures::gameSize(window->settings()) + 4,
                   [window](std::string& text) {
        const auto sett = window->settings();
        const bool r = eGameTextures::loadNextGame(sett, text);
        if(r) {
            const bool r = eMusic::loaded();
            if(r) {
                const bool r = eSounds::loaded();
                if(r) {
                    const bool r = eSanctBlueprints::loaded();
                    if(r) {
                        const bool r = eMessages::loaded();
                        if(r) return true;
                        text = eLanguage::text("loading_messages");
                        eMessages::load();
                        return false;
                    }
                    text = eLanguage::text("loading_blueprints");
                    eSanctBlueprints::load();
                    return false;
                } else {
                    text = eLanguage::text("loading_sounds");
                    eSounds::load();
                    return false;
                }
            } else {
                text = eLanguage::text("loading_music");
                eMusic::load();
                return false;
            }
        }
        return false;
    }, window) {}
