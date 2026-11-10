#include "emainwindow.h"
#include "efspath.h"

#include "widgets/emainmenu.h"
#include "widgets/esettingsmenu.h"
#include "widgets/egamewidget.h"
#include "widgets/egameloadingwidget.h"
#include "widgets/egamemenu.h"
#include "widgets/emenuloadingwidget.h"
#include "widgets/eworldwidget.h"
#include "widgets/echoosegameeditmenu.h"
#include "widgets/eselectcolonywidget.h"

#include "audio/emusic.h"

#include "pak/zeussavefile.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "buildings/eagorabase.h"
#include "buildings/epatroltarget.h"
#include "buildings/evendor.h"
#include "characters/echaracter.h"

#include "engine/ethreadpool.h"
#include "engine/emapstats.h"
#include "engine/emapgenerator.h"

#include "egamedir.h"

#include "fileIO/ereadstream.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include "widgets/efilewidget.h"
#include "elanguage.h"
#include "emessages.h"

#include "evectorhelpers.h"

#include "widgets/eeventbackground.h"
#include "widgets/eepisodeintroductionwidget.h"
#include "widgets/eepisodelostwidget.h"
#include "widgets/erosterofleaders.h"
#include <cmath>
#include <set>

eMainWindow::eMainWindow() {}

eMainWindow::~eMainWindow() {
    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    setWidget(nullptr);
}

bool eMainWindow::initialize(const eSettings& settings) {
    const auto& res = settings.fRes;
    const int w = res.width();
    const int h = res.height();
    const auto window = SDL_CreateWindow("eZeus",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         w, h, SDL_WINDOW_SHOWN);

    if(!window) {
        printf("Window could not be created! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }
    const Uint32 flags = SDL_RENDERER_ACCELERATED/* |
                         SDL_RENDERER_PRESENTVSYNC*/;
    const auto renderer = SDL_CreateRenderer(window, -1, flags);
    if(!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    mSdlWindow = window;
    mSdlRenderer = renderer;
    setResolution(res);
    setFullscreen(settings.fFullscreen);
    mSettings = settings;
    fitResolutionToWindow();
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_StartTextInput(); // needed for SDL_TEXTINPUT events

    const std::string icoPath = eGameDir::path("zeus.ico");
    const auto icon = IMG_Load(icoPath.c_str());
    SDL_SetWindowIcon(window, icon);
    eGameTextures::setSettings(mSettings);
    return true;
}

void eMainWindow::setWidget(eWidget* const w) {
    if(mWidget) {
        if(mWidget != mGW && mWidget != mWW) {
            mWidget->deleteLater();
        }
    }
    mWidget = w;
}

eWidget* eMainWindow::takeWidget() {
    const auto w = mWidget;
    mWidget = nullptr;
    return w;
}

void eMainWindow::addSlot(const eSlot& slot) {
    mSlots.push_back(slot);
}

void eMainWindow::setResolution(const eResolution& res) {
    if(mSettings.fRes == res && !mFirstFullscrenSetting) return;
    mFirstResolutionSetting = false;
    mSettings.fRes = res;
    const int w = res.width();
    const int h = res.height();
    SDL_SetWindowSize(mSdlWindow, w, h);
}

void eMainWindow::setFullscreen(const bool f) {
    if(mSettings.fFullscreen == f && !mFirstFullscrenSetting) return;
    mFirstFullscrenSetting = false;
    mSettings.fFullscreen = f;
    SDL_SetWindowFullscreen(mSdlWindow, f ? SDL_WINDOW_FULLSCREEN : 0);
}

void eMainWindow::fitResolutionToWindow() {
    const int w = mSettings.fRes.width();
    const int h = mSettings.fRes.height();
    if(mSettings.fFullscreen) {
        // A fullscreen window gets the closest mode the screen offers,
        // which is not the chosen one when the screen lacks it. A TV
        // without 16:10 modes turns 1440x900 into 1600x900, and the game
        // laid out for 1440 left a strip of bare map next to the panel.
        SDL_DisplayMode mode;
        if(SDL_GetWindowDisplayMode(mSdlWindow, &mode) != 0) return;
        if(mode.w == w && mode.h == h) return;
        mSettings.fRes = eResolution(mode.w, mode.h);
    } else {
        // leaving fullscreen restores the windowed size, which may still be
        // the one chosen before a fullscreen mode replaced it
        SDL_SetWindowSize(mSdlWindow, w, h);
    }
}

void eMainWindow::startGameAction(eGameBoard* const board,
                                  const eGameWidgetSettings& settings) {
    const auto show = [this, board, settings]() {
        showGame(board, settings);
    };
    startGameAction(show);
}

void eMainWindow::startGameAction(const stdsptr<eCampaign>& c,
                                  const eGameWidgetSettings& settings) {
    const auto show = [this, c, settings]() {
        showGame(c, settings);
    };
    startGameAction(show);
}

void eMainWindow::startGameAction(const eAction& a) {
    clearWidgets();
    const auto l = new eGameLoadingWidget(this);
    l->resize(width(), height());
    l->setDoneAction(a);
    setWidget(l);
    l->initialize();
}

void eMainWindow::showEpisodeIntroduction(
        const stdsptr<eCampaign>& c) {
    clearWidgets();
    if(c) mCampaign = c;
    const auto e = new eEpisodeIntroductionWidget(this);
    const auto proceedA = [this]() {
        mCampaign->startEpisode();
        const auto dir = leaderSaveDir();
        saveGame(dir + "autosave replay.ez");
        startGameAction([this]() {
            eGameWidgetSettings settings;
            settings.fPaused = true;
            showGame(mCampaign, settings);
        });
    };
    e->resize(width(), height());
    const auto ee = mCampaign->currentEpisode();

    const auto path = mCampaign->currentEpisodeAudioFilePath(true);
    const bool played = eMusic::playCampaignVoice(path);
    if(!played) eMusic::playMissionIntroMusic();

    e->initialize(mCampaign,
                  mCampaign->titleText(),
                  ee->fIntroduction,
                  ee->fGoals,
                  proceedA,
                  eEpisodeIntroType::intro);
    setWidget(e);
}

std::string eMainWindow::leaderSaveDir() const {
    return eGameDir::saveDir() + mLeader + "/";
}

bool eMainWindow::importOriginalSave(const std::string& sav,
                                     const std::string& out) {
    const auto save = std::make_shared<ZeusSaveFile>(sav);
    if(!save->load()) {
        printf("Could not read the saved game '%s'\n", sav.c_str());
        return false;
    }
    // the adventure lies in Adventures, the original ones with a character
    // in front of the name, the others in a folder of their own
    const auto pakName = save->adventure() + ".pak";
    std::string pak;
    const auto dir = eFsPath::sPath(eGameDir::pakAdventuresDir());
    for(const auto& e : std::filesystem::recursive_directory_iterator(dir)) {
        if(e.is_directory()) continue;
        if(eFsPath::sUtf8(e.path().filename()) != pakName) continue;
        pak = eFsPath::sUtf8(e.path());
        break;
    }
    if(pak.empty()) {
        printf("Could not find the adventure '%s'\n", pakName.c_str());
        return false;
    }

    const auto c = std::make_shared<eCampaign>();
    c->readPak(save->adventure(), pak);
    c->startEpisode();
    const auto board = c->currentEpisode()->fBoard;
    // same rule as ZeusFile::loadBoard, a colony has another map size
    const int mapsize = save->mapsize();
    int boardW = mapsize/2;
    if(boardW % 2) boardW++;
    int boardH = mapsize + 1;
    if(boardH % 2) boardH++;
    if(!board || board->width() != boardW || board->height() != boardH) {
        printf("'%s' is not the parent city of '%s', colonies are not "
               "supported yet\n", sav.c_str(), pakName.c_str());
        return false;
    }
    mCampaign = c;

    startGameAction([this, save, sav, out]() {
        eGameWidgetSettings settings;
        settings.fPaused = true;
        showGame(mCampaign, settings);
        const auto report = mGW->importOriginalSave(*save);
        printf("Imported '%s': %s\n", sav.c_str(), report.c_str());
        if(!out.empty()) {
            const bool r = saveGame(out);
            if(!r) printf("Could not save '%s'\n", out.c_str());
            mQuit = true;
        }
    });
    return true;
}

void eMainWindow::setStartupImport(const std::string& sav,
                                   const std::string& out) {
    mImportSav = sav;
    mImportOut = out;
}

void eMainWindow::autoplay() {
    const bool inGame = mGW && mBoard && mWidget == mGW;
    if(mAutoplayMonth != -1 && !inGame) {
        // another screen took over, like the end of an episode
        printf("autoplay: the game left the city, ending\n");
        mQuit = true;
        return;
    }
    if(!inGame) return;
    // the viewed city is known after the first frame
    if(mGW->viewedCity() == eCityId::neutralFriendly) return;
    // messages stop the time, a decision there is left to a player
    const auto open = mGW->closeMessage();
    if(!open.empty() && open != mAutoplayWaiting) {
        printf("waiting for a decision: %s\n", open.c_str());
        fflush(stdout);
    }
    mAutoplayWaiting = open;
    const auto& d = mBoard->date();
    const int month = 12*d.year() + static_cast<int>(d.month());
    if(mAutoplayMonth == -1) {
        mGW->runAtMaxSpeed();
        mAutoplayMonth = month;
        printCitySummary();
        return;
    }
    if(month == mAutoplayMonth) {
        // a month takes about a second at the highest speed
        if(++mAutoplayStillFrames == 3000) {
            const auto by = mGW->timeStoppedBy();
            printf("autoplay: no month passed, time stopped by %s, ending\n",
                   by.empty() ? "nothing known" : by.c_str());
            mQuit = true;
        }
        return;
    }
    mAutoplayStillFrames = 0;
    mAutoplayMonth = month;
    printCitySummary();
    if(--mAutoplayMonths == 0) mQuit = true;
}

void eMainWindow::printCitySummary() const {
    const auto& b = *mBoard;
    const auto cid = mGW->viewedCity();
    std::map<int, int> common;
    std::map<int, int> elite;
    int people = 0;
    int hoplites = 0;
    int horsemen = 0;
    const auto houses = b.buildings(cid, [](eBuilding* const bb) {
        const auto t = bb->type();
        return t == eBuildingType::commonHouse ||
               t == eBuildingType::eliteHousing;
    });
    // what keeps the houses from the next level
    std::map<eHouseMissing, int> missing;
    for(const auto bb : houses) {
        const auto h = static_cast<eHouseBase*>(bb);
        people += h->people();
        if(h->people() > 0) missing[h->missing()]++;
        if(h->type() == eBuildingType::commonHouse) {
            common[h->level()]++;
        } else {
            elite[h->level()]++;
            int ho;
            int hm;
            static_cast<eEliteHousing*>(h)->soldiers(ho, hm);
            hoplites += ho;
            horsemen += hm;
        }
    }
    std::map<eCharacterType, int> chars;
    for(const auto c : b.characters()) {
        chars[c->type()]++;
    }
    const auto levels = [](const std::map<int, int>& m) {
        std::string s;
        for(const auto& l : m) {
            s += " " + std::to_string(l.first) + ":" + std::to_string(l.second);
        }
        return s;
    };
    const char* const missingNames[] = {"water", "food", "fleece", "oil",
                                        "venues", "appeal", "wine", "arms",
                                        "horse", "nothing"};
    std::string missingStr;
    for(const auto& m : missing) {
        missingStr += std::string(" ") + missingNames[static_cast<int>(m.first)] +
                      ":" + std::to_string(m.second);
    }
    const auto emp = mBoard->employmentData(cid);
    const auto& d = b.date();
    // real time since the last report, to compare the speed of the game
    static auto last = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
    last = now;
    printf("[%lld ms, board %d x %d] ", static_cast<long long>(ms.count()),
           b.width(), b.height());
    printf("%d/%02d people %d drachmas %d | common%s | elite%s | missing%s | "
           "workers %d jobs %d | "
           "soldiers %d hoplites %d horsemen | nobles %d watchmen %d "
           "disgruntled %d\n",
           d.year(), static_cast<int>(d.month()) + 1, people,
           b.drachmas(b.personPlayer()), levels(common).c_str(),
           levels(elite).c_str(), missingStr.c_str(),
           emp ? emp->employable() : 0, emp ? emp->totalJobVacancies() : 0,
           hoplites, horsemen,
           chars[eCharacterType::eliteCitizen],
           chars[eCharacterType::watchman],
           chars[eCharacterType::disgruntled]);
    // what the city has in stock of the goods its houses use up, and what
    // its trade partners still sell or buy this year
    const auto pid = b.personPlayer();
    const auto stock = [&](const eResourceType t) {
        return std::to_string(b.resourceCount(cid, t));
    };
    printf("  stock fleece %s oil %s wine %s food %s\n",
           stock(eResourceType::fleece).c_str(),
           stock(eResourceType::oliveOil).c_str(),
           stock(eResourceType::wine).c_str(),
           stock(eResourceType::food).c_str());
    // the houses that lack a good the agoras sell
    {
        std::string lack;
        for(const auto bb : houses) {
            const auto h = static_cast<eHouseBase*>(bb);
            if(h->people() <= 0) continue;
            const auto m = h->missing();
            const char* n = nullptr;
            if(m == eHouseMissing::food) n = "food";
            else if(m == eHouseMissing::fleece) n = "fleece";
            else if(m == eHouseMissing::oil) n = "oil";
            else if(m == eHouseMissing::wine) n = "wine";
            if(!n) continue;
            const auto ct = bb->centerTile();
            lack += std::string(" ") + n + "@" +
                    std::to_string(ct ? ct->x() : -1) + "," +
                    std::to_string(ct ? ct->y() : -1);
        }
        if(!lack.empty()) printf("  lacking%s\n", lack.c_str());
        // how much fleece the common houses hold, to compare with
        // the houses of a saved game of the original
        std::map<int, int> held;
        for(const auto bb : houses) {
            const auto h = static_cast<eHouseBase*>(bb);
            if(h->people() <= 0) continue;
            if(h->type() != eBuildingType::commonHouse) continue;
            held[h->fleece()]++;
        }
        std::string hs;
        for(const auto& hh : held) {
            hs += " " + std::to_string(hh.first) + ":" +
                  std::to_string(hh.second);
        }
        printf("  fleece in common houses%s\n", hs.c_str());
    }
    // culture: houses without each kind, and what the venues do
    {
        int noPh = 0, noAc = 0, noAt = 0, noCo = 0, inh = 0;
        for(const auto bb : houses) {
            const auto h = static_cast<eHouseBase*>(bb);
            if(h->people() <= 0) continue;
            inh++;
            if(h->philosophersInventors() <= 0) noPh++;
            if(h->actorsAstronomers() <= 0) noAc++;
            if(h->athletesScholars() <= 0) noAt++;
            if(h->competitorsCurators() <= 0) noCo++;
        }
        printf("  culture houses %d without philosophers %d actors %d "
               "athletes %d competitors %d\n", inh, noPh, noAc, noAt, noCo);
        // podiums, theatres and stadiums that no philosopher, actor or
        // athlete has reached, so they send nobody to the houses
        int targets = 0;
        std::string idle;
        const auto cb = b.buildings(cid, [](eBuilding* const bb) {
            const auto t = bb->type();
            return t == eBuildingType::stadium ||
                   t == eBuildingType::podium ||
                   t == eBuildingType::theater;
        });
        for(const auto bb : cb) {
            const auto pt = dynamic_cast<ePatrolTarget*>(bb);
            if(!pt) continue;
            targets++;
            if(pt->available()) continue;
            const auto ct = bb->centerTile();
            idle += " " + eBuilding::sNameForBuilding(bb) + "@" +
                    std::to_string(ct ? ct->x() : -1) + "," +
                    std::to_string(ct ? ct->y() : -1);
        }
        printf("  venues %d, idle:%s\n", targets,
               idle.empty() ? " none" : idle.c_str());
    }
    // what the stalls of each agora hold, in hundredths of a load
    std::string stalls;
    for(const auto a : b.agoras(cid)) {
        std::string s;
        for(int i = 0; i < a->nPts(); i++) {
            const auto v = dynamic_cast<eVendor*>(a->building(i));
            if(!v) continue;
            const auto vt = v->type();
            const char kind = vt == eBuildingType::foodVendor ? 'f' :
                              vt == eBuildingType::fleeceVendor ? 'w' :
                              vt == eBuildingType::oilVendor ? 'o' :
                              vt == eBuildingType::wineVendor ? 'v' :
                              vt == eBuildingType::armsVendor ? 'a' : 'h';
            s += (s.empty() ? "" : "/") + std::string(1, kind) +
                 std::to_string(v->peddlerResource()) +
                 (v->enabled() ? "" : "x");
        }
        const auto t = a->centerTile();
        if(!s.empty()) {
            stalls += " " + std::to_string(t->x()) + "," +
                      std::to_string(t->y()) + (a->enabled() ? "" : "x") +
                      "[" + s + "]";
        }
    }
    printf("  stalls%s\n", stalls.c_str());
    for(const auto& c : b.world().cities()) {
        if(c->cityId() == cid || !c->trades()) continue;
        const auto trades = [&](const std::vector<eResourceTrade>& ts) {
            std::string s;
            for(const auto& t : ts) {
                s += " " + eResourceTypeHelpers::typeName(t.fType) + " " +
                     std::to_string(t.used(pid)) + "/" +
                     std::to_string(t.fMax);
            }
            return s;
        };
        printf("  trade %s%s | sells%s | buys%s\n", c->name().c_str(),
               c->waterTrade(cid) ? " (sea)" : "",
               trades(c->sells()).c_str(), trades(c->buys()).c_str());
    }
    fflush(stdout);
}

void eMainWindow::clearWidgets() {
    if(mGW && mWidget != mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }
    if(mWW && mWidget != mWW) {
        mWW->deleteLater();
        mWW = nullptr;
    }
}

void eMainWindow::episodeFinished() {
    clearWidgets();
    if(!mCampaign) return;
    mCampaign->episodeFinished();
    const bool f = mCampaign->finished();
    if(f) return adventureComplete();
    const auto n = mCampaign->currentEpisodeType();
    if(n == eEpisodeType::parentCity) {
        showEpisodeIntroduction();
    } else {
        const auto w = new eSelectColonyWidget(this);
        const auto sel = mCampaign->remainingColonies();
        const auto selA = [this](const stdsptr<eWorldCity>& c) {
            int cid = 0;
            const auto& eps = mCampaign->colonyEpisodes();
            for(const auto& e : eps) {
                if(e->fCity == c) break;
                cid++;
            }
            mCampaign->setCurrentColonyEpisode(cid);
            showEpisodeIntroduction();
        };
        w->resize(width(), height());
        w->initialize(sel, selA, &mCampaign->worldBoard());
        setWidget(w);
    }
}

void eMainWindow::adventureComplete() {
    clearWidgets();
    if(!mCampaign) return;
    const auto e = new eEpisodeIntroductionWidget(this);
    const auto proceedA = [this]() {
        showMainMenu();
    };
    e->resize(width(), height());

    const auto path = mCampaign->adventureVictoryAudioFilePath();
    const bool played = eMusic::playCampaignVoice(path);
    if(!played) eMusic::playCampaignVictoryMusic();

    e->initialize(mCampaign,
                  eLanguage::zeusText(62, 0),
                  mCampaign->completeText(),
                  {},
                  proceedA,
                  eEpisodeIntroType::campaingVictory);
    setWidget(e);
}

void eMainWindow::episodeLost() {
    clearWidgets();
    const auto e = new eEpisodeLostWidget(this);
    const auto proceedA = [this]() {
        showMainMenu();
    };
    e->resize(width(), height());
    e->initialize(proceedA);
    setWidget(e);
}

bool eMainWindow::saveGame(const std::string& path) {
    const auto fsp = eFsPath::sPath(path);
    const auto fspd = fsp.parent_path();
    std::filesystem::create_directories(fspd);
    std::ofstream file(eFsPath::sPath(path), std::ios::out | std::ios::binary |
                       std::ios::trunc);
    if(!file) return false;
    eWriteTarget target(&file);
    eWriteStream dst(target);
    dst.writeFormat("eZeus.ez");
    if(mGW) {
        const auto s = mGW->settings();
        s.write(dst);
    } else {
        eGameWidgetSettings s;
        s.fPaused = true;
        s.write(dst);
    }
    mCampaign->write(dst);
    file.close();
    return true;
}

bool eMainWindow::loadGame(const std::string& path) {
    std::ifstream file(eFsPath::sPath(path), std::ios::in | std::ios::binary);
    if(!file) return false;
    eReadSource source(&file);
    eReadStream src(source);
    src.readFormat();
    const auto& format = src.format();
    const int version = src.formatVersion();
    if(format != "eZeus.ez") {
        printf("Invalid file '%s' format '%s', expected 'eZeus.ez'.\n",
               path.c_str(), format.c_str());
        return false;
    }
    if(version > eFileFormat::version) {
        printf("Attempting to read '%s' format '%s' version '%i' newer than the executable.\n",
               path.c_str(), format.c_str(), version);
    }
    eGameWidgetSettings s;
    s.read(src);
    const auto c = std::make_shared<eCampaign>();
    c->read(src);
    c->loadStrings();
    c->loadNumbers();
    src.handlePostFuncs();
    file.close();

    if(mResizeW <= 0 && mSaveOut.empty()) {
        startGameAction(c, s);
        return true;
    }
    startGameAction([this, c, s]() {
        showGame(c, s);
        if(mResizeW > 0 && mResizeH > 0 && mBoard) {
            const auto t0 = std::chrono::steady_clock::now();
            mBoard->resize(mResizeW, mResizeH);
            mGW->updateViewBoxSize();
            mGW->updateTopBottomAltitude();
            mGW->updateMinMaxAltitude();
            mGW->updateMaps(true);
            const auto t1 = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
            printf("Resized the board to %d x %d in %lld ms\n",
                   mBoard->width(), mBoard->height(),
                   static_cast<long long>(ms.count()));
            mResizeW = 0;
            mResizeH = 0;
        }
        if(!mSaveOut.empty()) {
            const auto t0 = std::chrono::steady_clock::now();
            const bool r = saveGame(mSaveOut);
            const auto t1 = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
            if(r) {
                printf("Saved '%s' in %lld ms\n", mSaveOut.c_str(),
                       static_cast<long long>(ms.count()));
            } else {
                printf("Could not save '%s'\n", mSaveOut.c_str());
            }
            mSaveOut.clear();
            if(mAutoplayMonths <= 0) mQuit = true;
        }
    });
    return true;
}

void eMainWindow::closeGame() {
    if(!mGW) return;
    if(mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }
    if(mWW) {
        mWW->deleteLater();
        mWW = nullptr;
    }
    showMainMenu();
}

void eMainWindow::showRosterOfLeaders() {
    clearWidgets();
    eMusic::playMenuMusic();
    const auto rol = new eRosterOfLeaders(this);
    rol->resize(width(), height());
    rol->initialize();
    setWidget(rol);
}

void eMainWindow::showMenuLoading() {
    const auto mlw = new eMenuLoadingWidget(this);
    mlw->setDoneAction([this]() {
        if(!mMapStats.empty()) {
            const auto path = mMapStats == "Adventures/" ?
                                  eGameDir::pakAdventuresDir() : mMapStats;
            eMapStats::sRun(path);
            mMapStats.clear();
            mQuit = true;
            return;
        }
        if(mGenW > 0 && mGenH > 0) {
            const auto c = std::make_shared<eCampaign>();
            c->initialize("generated");
            auto& board = c->parentCityBoard();
            board.initialize(mGenW, mGenH);
            eMapGenerator g(board);
            eMapGeneratorSettings sett;
            sett.fSeed = mGenSeed;
            const auto t0 = std::chrono::steady_clock::now();
            g.generate(sett);
            const auto t1 = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
            printf("Generated a %d x %d map in %lld ms\n", mGenW, mGenH,
                   static_cast<long long>(ms.count()));
            eMapStats::sPrintHeader();
            eMapStats::sPrint("generated", "parent", board);
            const bool img = eMapStats::sWriteImage(board, "generated.ppm");
            if(img) printf("Wrote generated.ppm\n");
            mGenW = 0;
            mGenH = 0;
            mQuit = true;
            return;
        }
        if(!mImportSav.empty()) {
            const bool r = importOriginalSave(mImportSav, mImportOut);
            mImportSav.clear();
            if(r) return;
            if(!mImportOut.empty()) {
                mQuit = true;
                return;
            }
        }
        const auto ls = eRosterOfLeaders::sLeaders();
        if(ls.size() == 1) setLeader(ls[0]);
        if(!mLoad.empty()) {
            const bool r = loadGame(mLoad);
            if(!r) printf("Could not load '%s'\n", mLoad.c_str());
            mLoad.clear();
            if(r) return;
        }
        if(mLeader.empty()) {
            showRosterOfLeaders();
        } else {
            showMainMenu();
        }
    });
    mlw->initialize();
    mlw->resize(width(), height());
    setWidget(mlw);
}

void eMainWindow::showMainMenu() {
    mCampaign = nullptr;
    clearWidgets();
    eMusic::playMenuMusic();

    const auto mm = new eMainMenu(this);
    mm->resize(width(), height());
    setWidget(mm);

    const auto newGameAction = [this]() {
        showChooseGameMenu();
    };

    const auto loadGameAction = [this, mm]() {
        const auto fw = new eFileWidget(this);
        const auto func = [this](const std::string& path) {
            return loadGame(path);
        };
        const auto closeAct = [mm, fw]() {
            mm->removeWidget(fw);
            fw->deleteLater();
        };
        const auto dir = leaderSaveDir();
        fw->intialize(eLanguage::zeusText(1, 3),
                      dir, func, closeAct);
        mm->addWidget(fw);
        fw->align(eAlignment::center);
    };

    const auto editGameAction = [this]() {
        showChooseGameEditMenu();
    };

    const auto settingsAction = [this]() {
        showSettingsMenu();
    };

    const auto quitAction = [this]() {
        mQuit = true;
    };

    const auto leaderAction = [this]() {
        showRosterOfLeaders();
    };

    mm->initialize(newGameAction,
                   loadGameAction,
                   editGameAction,
                   settingsAction,
                   quitAction,
                   leaderAction);
}

void eMainWindow::showSettingsMenu() {
    const auto esm = new eSettingsMenu(mSettings, this);
    esm->resize(width(), height());

    const auto applyA = [this](const eSettings& settings) {
        const auto oldRes = mSettings.fRes;
        const bool languageChanged = settings.fLanguage != mSettings.fLanguage;
        setResolution(settings.fRes);
        setFullscreen(settings.fFullscreen);
        mSettings = settings;
        fitResolutionToWindow();
        const bool loadNeeded = mSettings.fRes != oldRes;
        mSettings.write();
        if(languageChanged) {
            // the code page of the original game's text files depends on
            // the language, so the game texts are reread as well
            mSettings.apply();
            eLanguage::reload();
            eMessages::reload();
        }
        if(!mSettings.fTinyTextures &&
           !mSettings.fSmallTextures &&
           !mSettings.fMediumTextures &&
           !mSettings.fLargeTextures) {
            mSettings.fSmallTextures = true;
        }
        eGameTextures::setSettings(mSettings);
        if(loadNeeded) showMenuLoading();
        else showMainMenu();
    };
    const auto fullscrennA = [this](const bool f) {
        setFullscreen(f);
    };
    esm->initialize(applyA, fullscrennA);
    setWidget(esm);
}

void eMainWindow::showChooseGameMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(false);
    setWidget(gem);
}

void eMainWindow::showChooseGameEditMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(true);
    setWidget(gem);
}

void eMainWindow::showGame(const stdsptr<eCampaign>& c,
                           const eGameWidgetSettings& settings) {
    mCampaign = c;
    const auto e = c->currentEpisode();
    showGame(e->fBoard, settings);
}

void eMainWindow::showGame(eGameBoard* b,
                           const eGameWidgetSettings& settings) {
    if(!b) b = mBoard;

    if(b == mBoard && mGW) {
        return setWidget(mGW);
    }

    if(mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }

    mBoard = b;
    if(mBoard) {
        mBoard->updateAppealMapIfNeeded();
        mBoard->waitUntilFinished();
    }

    eMusic::playRandomMusic();
    mGW = new eGameWidget(this);
    mGW->setBoard(b);
    mGW->resize(width(), height());
    mGW->initialize();
    mGW->setSettings(settings);
    setWidget(mGW);
}

void eMainWindow::showWorld() {
    if(mWidget == mWW) return;
    if(!mCampaign) return;
    if(!mWW) {
        mWW = new eWorldWidget(this);
        mWW->resize(width(), height());
        mWW->initialize();
        mWW->setBoard(mBoard);
    } else {
        mWW->update();
    }
    setWidget(mWW);
}

void eMainWindow::execDialog(
        eWidget* const d, const bool closable,
        const eAction &closeFunc,
        eWidget* const parent) {
    if(!mWidget) return;
    const auto bg = new eEventBackground(this);
    if(closeFunc) {
        bg->initialize(parent ? parent : mWidget, d, closable, closeFunc);
    } else {
        const auto closeFunc = [d]() {
            d->deleteLater();
        };
        bg->initialize(parent ? parent : mWidget, d, closable, closeFunc);
    }
}

class eTooltip {
public:
    eTooltip(eMainWindow& w) : mWindow(w) {}

    void update() {
        const auto txt = eWidget::sTooltip();
        const bool updateTxt = mText != txt;
        if(updateTxt) {
            mText = txt;
        }

        const auto& res = mWindow.resolution();
        const int fontSize = res.verySmallFontSize();
        const bool updateFont = mFontSize != fontSize;
        if(updateFont) {
            mFontSize = fontSize;
            mFont = eFonts::defaultFont(mFontSize);
        }

        const bool updateTexture = updateTxt || updateFont;
        if(updateTexture) {
            const auto r = mWindow.renderer();
            if(mText.empty()) {
                mTexture->reset();
            } else {
                mTexture->loadText(r, mText, eFontColor::light, *mFont, 50*fontSize);
            }
        }
    }

    void paint(const int x, const int y, ePainter& p) {
        const int pp = padding();
        SDL_Rect rect{x, y, width(), height()};
        p.fillRect(rect, SDL_Color{16, 108, 144, 255});
        p.drawRect(rect, SDL_Color{0, 32, 32, 255}, 1);
        p.drawTexture(x + pp, y + pp, mTexture);
    }

    int width() const { return mTexture->width() + 2*padding(); }
    int height() const { return mTexture->height() + 2*padding(); }

    bool empty() const { return mText.empty(); }
private:
    int padding() const { return mFontSize/2; }

    eMainWindow& mWindow;
    int mFontSize = -1;
    TTF_Font* mFont = nullptr;
    std::string mText;
    stdsptr<eTexture> mTexture = std::make_shared<eTexture>();
};

int eMainWindow::exec() {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    showMenuLoading();

    eMouseButton button{eMouseButton::none};
    eMouseButton buttons{eMouseButton::none};

    SDL_Event e;
    eTooltip tooltip(*this);

    const bool showFPS = false;
    const double fpsClamp = 20;

    int c = 0;
    int fpsVal = 0;
    bool resetRenderTargets = false;
    while(!mQuit) {
        const auto fpsStart = high_resolution_clock::now();
        if(mAutoplayMonths) autoplay();

        while(SDL_PollEvent(&e)) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            const bool shift = mShiftPressed > 0;
            const bool ctrl = mCtrlPressed > 0;
            if(e.type == SDL_QUIT) {
                mQuit = true;
            } else if(e.type == SDL_WINDOWEVENT) {
                const auto we = e.window.event;
                // a run without a player goes on while minimized
                if(we == SDL_WINDOWEVENT_MINIMIZED && !mAutoplayMonths) {
                    resetRenderTargets = true;
                    while(SDL_WaitEvent(&e)) {
                        if(e.window.event == SDL_WINDOWEVENT_RESTORED) {
                            break;
                        }
                    }
                } else if(we == SDL_WINDOWEVENT_EXPOSED) {
                    resetRenderTargets = true;
                }
            } else if(e.type == SDL_RENDER_TARGETS_RESET ||
                      e.type == SDL_RENDER_DEVICE_RESET) {
                resetRenderTargets = true;
            } else if(e.type == SDL_MOUSEMOTION) {
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseMove(me);
            } else if(e.type == SDL_MOUSEBUTTONDOWN) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = button | buttons;

                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mousePress(me);
            } else if(e.type == SDL_MOUSEBUTTONUP) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = buttons & ~button;
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseRelease(me);
            } else if(e.type == SDL_MOUSEWHEEL) {
                const eMouseWheelEvent me(x, y, shift, ctrl, buttons, e.wheel.y);
                if(mWidget) mWidget->mouseWheel(me);
            } else if(e.type == SDL_KEYDOWN) {
                const auto k = e.key.keysym.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed++;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed++;
                }
                const eKeyPressEvent ke(x, y, shift, ctrl, buttons, k);
                if(mWidget) mWidget->keyPress(ke);
            } else if(e.type == SDL_TEXTINPUT) {
                // SDL has already applied the keyboard layout and any dead
                // keys, so this is the only place characters outside the
                // US layout, umlauts among them, can be read from
                if(mWidget) mWidget->textInput(e.text.text);
            } else if(e.type == SDL_KEYUP) {
                const auto k = e.key.keysym.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed--;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed--;
                }
            }
        }

        if(resetRenderTargets) {
            resetRenderTargets = false;
            if(mWidget) mWidget->renderTargetsReset();
        }

        SDL_SetRenderDrawColor(mSdlRenderer, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(mSdlRenderer);

        ePainter p(mSdlRenderer);

        eMusic::incTime();
        if(mWidget) {
            mWidget->paint(p);
            tooltip.update();
            if(!tooltip.empty()) {
                const auto& res = resolution();
                const int pp = 25*res.multiplier();
                const int wtt = tooltip.width();
                const int htt = tooltip.height();
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                int xtt;
                int ytt;
                if(mx > width()/2) {
                    xtt = mx - wtt;
                } else {
                    xtt = mx;
                }
                if(my > height()/2) {
                    ytt = my - htt - pp;
                } else {
                    ytt = my + pp;
                }
                tooltip.paint(xtt, ytt, p);
            }
        }

        if(showFPS) {
            p.setFont(eFonts::defaultFont(resolution()));
            p.drawText(0, 0, std::to_string(fpsVal), eFontColor::dark);
        }

        SDL_RenderPresent(mSdlRenderer);

        for(const auto& s : mSlots) {
            s();
        }
        mSlots.clear();

        const auto fpsEnd = high_resolution_clock::now();
        const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
        const duration<double, std::milli> fpsDuration(1000./fpsClamp);
        const duration<double, std::milli> fpsSleep(fpsDuration - fpsElapsed);
        std::this_thread::sleep_for(fpsSleep);

        if(showFPS) {
            c++;
            if(c % 25 == 0) {
                const auto fpsEnd = high_resolution_clock::now();
                const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
                fpsVal = (int)std::round(1000./fpsElapsed.count());
            }
        }
    }

    return 0;
}
