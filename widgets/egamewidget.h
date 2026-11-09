#ifndef EGAMEWIDGET_H
#define EGAMEWIDGET_H

#include <deque>

#include "emainwidget.h"
#include "eframedlabel.h"

#include "engine/etile.h"

#include "textures/eterraintextures.h"
#include "textures/ebuildingtextures.h"

#include "buildings/epatrolbuilding.h"

#include "widgets/egamemenu.h"
#include "widgets/earmymenu.h"
#include "egamemainmenu.h"
#include "etopbarwidget.h"

#include "eviewmode.h"
#include "emessage.h"
#include "echeckbox.h"

#include "engine/eeventdata.h"

class eTerrainEditMenu;
class eDomesticatedAnimal;
class ZeusSaveFile;
struct eSanctBlueprint;
class eWorldWidget;
struct eGodMessages;
struct eHeroMessages;
class eMessageBox;
class eGameBoard;
class eAgoraBase;
class eInfoWidget;
class eFramedButton;

enum class eAgoraOrientation;
enum class eGodType;
enum class eHeroType;
enum class eGodQuestId;
enum class eWorldDirection;

using eBuildingCreator = std::function<stdsptr<eBuilding>()>;

struct eSavedMessage {
    eEventData fEd;
    eMessage fMsg;
};

struct eGameWidgetSettings {
    bool fPaused = false;
    int fSpeedId = 1;
    int fSpeed = 10;
    int fDX = 0;
    int fDY = 0;
    eTileSize fTileSize = eTileSize::s30;
    eWorldDirection fDir = eWorldDirection::N;
    std::map<int, std::pair<int, int>> fBookmarks;

    void read(eReadStream& src) {
        src >> fPaused;
        src >> fSpeedId;
        src >> fSpeed;
        src >> fDX;
        src >> fDY;
        src >> fTileSize;
        src >> fDir;

        int n;
        src >> n;
        for(int i = 0; i < n; i++) {
            int id;
            src >> id;
            auto& b = fBookmarks[id];
            src >> b.first;
            src >> b.second;
        }
    }

    void write(eWriteStream& dst) const {
        dst << fPaused;
        dst << fSpeedId;
        dst << fSpeed;
        dst << fDX;
        dst << fDY;
        dst << fTileSize;
        dst << fDir;

        dst << fBookmarks.size();
        for(const auto& b : fBookmarks) {
            dst << b.first;
            dst << b.second.first;
            dst << b.second.second;
        }
    }
};

class eGameWidget : public eMainWidget {
public:
    eGameWidget(eMainWindow* const window);
    ~eGameWidget();

    void initialize();

    void pixToId(const int pixX, const int pixY,
                 int& idX, int& idY) const;

    void setViewMode(const eViewMode m);
    eViewMode viewMode() const { return mViewMode; }

    void viewFraction(const double fx, const double fy);
    void viewTile(eTile* const tile);
    eTile* viewedTile() const;
    bool tileVisible(eTile* const tile) const;
    eCityId viewedCity() const;

    void showBuyCity(const eCityId cid);
    void hideBuyCity();

    void setBoard(eGameBoard* const board);

    eGameWidgetSettings settings() const;
    void setSettings(const eGameWidgetSettings& s);

    void updateRequestButtons();
    void showTip(const ePlayerCityTarget& target,
                 const std::string& tip);
    void showQuestion(const std::string& title,
                      const std::string& q,
                      const eAction& action);

    void updateViewBoxSize();
    void updateTopBottomAltitude();
    void updateMinMaxAltitude();
    void updateMaps(const bool totalUpdate);
    void updateMaps(const std::vector<eTile*>& tiles);
    void updateCitiesOnBoard();

    void setWorldDirection(const eWorldDirection dir);

    // Rebuilds the city of a saved game of the original game on the board,
    // which has to come from the same adventure. Returns a report of what
    // could not be taken over.
    std::string importOriginalSave(const ZeusSaveFile& save);
    // unpauses at the highest speed, for runs without a player
    void runAtMaxSpeed();
    // Closes the message box on screen, which stops the time, unless it
    // asks for a decision. Returns the title of the box left open.
    std::string closeMessage();
    // what keeps the time from running, empty when it runs
    std::string timeStoppedBy() const;

    void centerDialog(eWidget* const d);
    void openDialog(eWidget* const d) override;
protected:
    void paintEvent(ePainter& p) override;

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseWheelEvent(const eMouseWheelEvent& e) override;
private:
    void renderTargetsReset() override;
    void initializeNumbers();

    void drawXY(int tx, int ty,
                double& rx, double& ry,
                const int wSpan, const int hSpan,
                const int a);

    void showGoals();

    void setDX(const int dx);
    void setDY(const int dy);
    void clampViewBox();

    void setBookmark(const int id);
    void viewBookmark(const int id);

    using eApply = std::function<void(eTile* const)>;
    eApply editFunc();
    bool buildMouseRelease();

    using eTileAction = std::function<void(eTile* const)>;
    void iterateOverVisibleTiles(const eTileAction& a);

    void setTileSize(const eTileSize size);

    using eSpecialRequirement = std::function<bool(eTile*)>;
    bool canBuildVendor(const int tx, const int ty,
                        const eResourceType resType) const;
    bool canBuildFishery(const int tx, const int ty,
                         eDiagonalOrientation& o) const;
    bool waterTileHasAccessToSea(const int tx, const int ty) const;
    bool canBuildTriremeWharf(const int tx, const int ty,
                              eDiagonalOrientation& o) const;
    bool canBuildPier(const int tx, const int ty,
                      eDiagonalOrientation& o, const eCityId cid,
                      const ePlayerId pid, const bool forestAllowed) const;

    std::vector<eTile*> agoraBuildPlaceBR(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceTL(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceBL(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceTR(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceIter(
            eTile* const tile, const bool grand,
            eAgoraOrientation& bt,
            const eCityId cid,
            const ePlayerId pid) const;

    std::vector<ePatrolGuide>::iterator
        findGuide(const int tx, const int ty);

    void handleEvent(const eEvent e, eEventData& ed);
    void handleGodQuestEvent(eEventData& ed,
                             const bool fulfilled);
    void handleGodVisitEvent(eEventData& ed);
    void handleGodInvasionEvent(eEventData& ed);
    void handleGodHelpEvent(eEventData& ed);
    void handleSanctuaryComplete(eEventData& ed);
    void handleMonsterUnleashEvent(eEventData& ed);
    void handleMonsterInvasionInitialEvent(eEventData& ed);
    void handleMonsterInvasion24Event(eEventData& ed);
    void handleMonsterInvasion12Event(eEventData& ed);
    void handleMonsterInvasion6Event(eEventData& ed);
    void handleMonsterInvasion1Event(eEventData& ed);
    void handleMonsterInvasionEvent(eEventData& ed);
    void handleMonsterSlainEvent(eEventData& ed);
    void handleHeroArrivalEvent(eEventData& ed);

    void handleMonsterInCityEvent(eEventData& ed);

    void mapDimensions(int& mdx, int& mdy) const;
    void viewBoxSize(double& fx, double& fy) const;
    void viewedFraction(double& fx, double& fy) const;
    void tileViewFraction(eTile* const tile,
                          double& xf, double& yf) const;

    void updateMinimap();

    int rotationId() const;
    int hippodromeId() const;
    void updateHippodromeIds();

    void showMessage(eEventData& ed, const eMessage& msg,
                     const bool prepend = false);
    void showMessage(eEventData& ed, const eMessageType& msg,
                     const bool prepend = false);
    void showMessage(eEventData& ed, const eEventMessageType& msg,
                     const bool prepend = false);

    void updateTipPositions();

    bool roadPath(std::vector<eOrientation>& path);
    bool columnPath(std::vector<eOrientation>& path);
    bool bridgeTiles(eTile* const t, const eTerrain terr,
                     std::vector<eTile*>& tiles,
                     bool& rotated);
    bool canBuildAvenue(eTile* const t, const eCityId cid,
                        const ePlayerId pid,
                        const bool forestAllowed) const;

    bool inErase(const int tx, const int ty);
    bool inErase(const SDL_Rect& rect);
    bool inErase(eAgoraBase* const a);
    bool inErase(eBuilding* const b);

    bool inPatrolBuildingHover(eBuilding* const b);

    void setArmyMenuVisible(const bool v);

    void scheduleConnectedTerrainUpdate(eTile* const startTile);
    void updateTerrainTextures(eTile* const tile,
                               const eTerrainTextures& trrTexs,
                               const eBuildingTextures& builTexs);
    void updateTerrainTextures();

    void updatePatrolPath();
    void setPatrolBuilding(ePatrolBuildingBase* const pb);

    eInfoWidget* openInfoWidget(eBuilding* const b);
    eInfoWidget* openInfoWidget(const std::vector<eCharacter *> chars);

    void switchPause();

    stdsptr<eTexture> getBasementTexture(
            const int tx, const int ty, eBuilding* const d,
            const eTerrainTextures& trrTexs, const eWorldDirection dir,
            const int boardw, const int boardh);

    std::vector<eTile*> selectedTiles() const;

    eMouseButton mPressedButtons = eMouseButton::none;

    bool mEditorMode = false;
    // while a saved game is imported, without tips and sounds
    bool mImporting = false;
    bool mEditorShowBuildings = false;
    bool mTerrainEditMode = false;

    bool mRotate = false;
    int mRotateId = 0;

    const int sSpeeds[6] = {2, 10, 25, 50, 100, 100};
    const int sMaxSpeedId = int(std::size(sSpeeds)) - 1;

    bool mPaused = false;
    bool mLocked = false;
    int mFrame{0};
    int mRotateFrame{0};
    std::vector<int> mValiableHippodromePieces;
    int mTime{0};
    int mSpeedId = 1;
    int mSpeed = sSpeeds[mSpeedId];
    std::map<int, std::pair<int, int>> mBookmarks;

    int mWheel = 0;

    int mMinAltitude = 0;
    int mMaxAltitude = 0;

    int mTopMinAltitude = 0;
    int mBottomMaxAltitude = 0;

    int mDX = 0;
    int mDY = 0;

    bool mLeftPressed = false;
    bool mMovedSincePress = false;

    int mHoverX = -1;
    int mHoverY = -1;
    int mHoverTX = -1;
    int mHoverTY = -1;
    int mPressedX = -1;
    int mPressedY = -1;
    int mPressedTX = -1;
    int mPressedTY = -1;
    int mLastX = -1;
    int mLastY = -1;

    eViewMode mViewMode = eViewMode::defaultView;

    eTileSize mTileSize = eTileSize::s30;
    int mTileW = 60;
    int mTileH = 30;

    int mUpdateRect = 0;
    std::vector<SDL_Rect> mUpdateRects;
    stdptr<eGameBoard> mBoard;

    bool mDrawElevation = true;
    stdptr<ePatrolBuildingBase> mPatrolBuilding;
    std::vector<eTile*> mPatrolPath;
    std::vector<eTile*> mExcessPatrolPath;
    std::vector<eTile*> mPatrolPath1;
    std::vector<eTile*> mExcessPatrolPath1;
    eWidget* mPatrolPathWid = nullptr;
    std::vector<ePatrolGuide> mSavedGuides;

    eFramedLabel* mPausedLabel = nullptr;

    eTopBarWidget* mTopBar = nullptr;
    eInfoWidget* mInfoWidget = nullptr;
    eMessageBox* mMsgBox = nullptr;
    std::deque<eSavedMessage> mSavedMsgs;
    eTerrainEditMenu* mTem = nullptr;
    eGameMenu* mGm = nullptr;
    eArmyMenu* mAm = nullptr;

    eWorldWidget* mWW = nullptr;

    struct eTip {
        ePlayerCityTarget fTarget;
        std::string fText;
        eWidget* fWid = nullptr;
        int fLastFrame = 0;
    };

    std::deque<eTip> mTips;

    std::map<eTileSize, std::vector<stdsptr<eTexture>>> mNumbers;
    std::vector<eTile*> mInflTiles;
    std::vector<eTile*> mHoverTiles;

    eTile* mViewedTile = nullptr;
    eCityId mViewedCityId = eCityId::neutralFriendly;
    bool mUpdateViewedTileScheduled = true;

    eWidget* mBuyCityWidget = nullptr;
    eLabel* mBuyCityName = nullptr;
    eLabel* mBuyCityPrice = nullptr;
    eFramedButton* mBuyCityButton = nullptr;
};

#endif // EGAMEWIDGET_H
