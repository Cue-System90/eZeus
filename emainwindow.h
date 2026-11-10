#ifndef EMAINWINDOW_H
#define EMAINWINDOW_H

#include "widgets/ewidget.h"
#include "widgets/eresolution.h"
#include "textures/eterraintextures.h"
#include "textures/egodtextures.h"
#include "textures/ebuildingtextures.h"
#include "textures/echaractertextures.h"
#include "widgets/esettingsmenu.h"

using eSlot = std::function<void()>;

class eGameBoard;
class eGameWidget;
class eWorldWidget;
class eCampaign;

struct eGameWidgetSettings;

class eMainWindow {
public:
    eMainWindow();
    ~eMainWindow();

    bool initialize(const eSettings& settings);
public:

    void setWidget(eWidget* const w);
    eWidget* takeWidget();

    int exec();

    void addSlot(const eSlot& slot);

    int width() const { return resolution().width(); }
    int height() const { return resolution().height(); }
    const eResolution& resolution() const { return mSettings.fRes; }
    SDL_Window* window() const { return mSdlWindow; }
    SDL_Renderer* renderer() const { return mSdlRenderer; }

    void setResolution(const eResolution& res);
    void setFullscreen(const bool f);

    void startGameAction(eGameBoard* const board,
                         const eGameWidgetSettings& settings);
    void startGameAction(const stdsptr<eCampaign>& c,
                         const eGameWidgetSettings& settings);
    void startGameAction(const eAction& a);
    void episodeFinished();
    void adventureComplete();
    void episodeLost();

    bool saveGame(const std::string& path);
    bool loadGame(const std::string& path);
    void closeGame();

    void showRosterOfLeaders();
    void showMenuLoading();
    void showMainMenu();
    void showSettingsMenu();
    void showChooseGameMenu();
    void showChooseGameEditMenu();
    void showGame(const stdsptr<eCampaign>& c,
                  const eGameWidgetSettings& settings);
    void showGame(eGameBoard* b,
                  const eGameWidgetSettings& settings);
    void showWorld();

    eWidget* currentWidget() const { return mWidget; }
    eWorldWidget* worldWidget() const { return mWW; }

    const eSettings& settings() const { return mSettings; }

    void execDialog(eWidget* const d,
                    const bool closable = true,
                    const eAction& closeFunc = nullptr,
                    eWidget* const parent = nullptr);

    void showEpisodeIntroduction(const stdsptr<eCampaign>& c = nullptr);
    const stdsptr<eCampaign>& campaign() const { return mCampaign; }

    const std::string& leader() const { return mLeader; }
    void setLeader(const std::string& leader) { mLeader = leader; }
    std::string leaderSaveDir() const;

    // Starts the city of a saved game of the original game (.sav). With an
    // output path the city is saved there as .ez and the program ends.
    bool importOriginalSave(const std::string& sav,
                            const std::string& out = "");
    // imports right after the start instead of showing the main menu
    void setStartupImport(const std::string& sav,
                          const std::string& out);
    // loads a saved game right after the start
    void setStartupLoad(const std::string& path) { mLoad = path; }
    // changes the size of the board of the loaded game, tiles in dtile
    // columns and rows (the largest map of the original is 114 x 230)
    void setStartupResize(const int w, const int h)
    { mResizeW = w; mResizeH = h; }
    // saves the loaded game right away; without --autoplay the program ends
    void setStartupSave(const std::string& path) { mSaveOut = path; }
    // prints what the maps of the adventures (directory or .pak) are
    // made of, then ends the program
    void setStartupMapStats(const std::string& path) { mMapStats = path; }
    // generates a map of the size with the default settings, prints its
    // statistics like --map-stats and ends the program
    void setStartupGenerate(const int w, const int h, const int seed)
    { mGenW = w; mGenH = h; mGenSeed = seed; }
    // lets the game run on its own for months after loading or importing,
    // reports the city every month and ends the program
    void setStartupAutoplay(const int months) { mAutoplayMonths = months; }
private:
    void clearWidgets();
    void fitResolutionToWindow();
    void autoplay();
    void printCitySummary() const;

    eSettings mSettings;

    std::string mLeader;

    std::string mImportSav;
    std::string mImportOut;
    std::string mLoad;
    std::string mSaveOut;
    std::string mMapStats;
    int mGenW = 0;
    int mGenH = 0;
    int mGenSeed = 0;
    int mResizeW = 0;
    int mResizeH = 0;
    int mAutoplayMonths = 0;
    int mAutoplayMonth = -1;
    int mAutoplayStillFrames = 0;
    std::string mAutoplayWaiting;

    bool mQuit = false;
    bool mFirstFullscrenSetting = true;
    bool mFirstResolutionSetting = true;

    std::vector<eSlot> mSlots;

    int mShiftPressed = 0;
    int mCtrlPressed = 0;

    stdsptr<eCampaign> mCampaign;
    eGameBoard* mBoard = nullptr;
    eGameWidget* mGW = nullptr;
    eWorldWidget* mWW = nullptr;

    eWidget* mWidget = nullptr;
    SDL_Window* mSdlWindow = nullptr;
    SDL_Renderer* mSdlRenderer = nullptr;
};

#endif // EMAINWINDOW_H
