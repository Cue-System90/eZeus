#ifndef EGAMEMENU_H
#define EGAMEMENU_H

#include "egamemenubase.h"
#include "ebuildingmode.h"

class eCheckableButton;
class eTextureCollection;
class eInterfaceTextures;
class eButton;
class eGameBoard;
class ePopulationDataWidget;
class eEmploymentDataWidget;
class eAdminDataWidget;
class eStorageDataWidget;
class eAppealDataWidget;
class eHygieneSafetyDataWidget;
class eHusbandryDataWidget;
class eMythologyDataWidget;
class eCultureDataWidget;
class eScienceDataWidget;
class eMilitaryDataWidget;
class eOverviewDataWidget;
class eMiniMap;
class eGameWidget;
class eEventWidget;
class eBuildButton;
class eBuildWidget;
class eFramedLabel;
enum class eEvent;
struct eEventData;
class eRotateButton;
class eDataWidget;

struct eSubButtonData;

class eSubButton;

struct eSPR {
    eBuildingMode fMode;
    std::string fName;
    int fMarbleCost = 0;
    int fCity = -1;
};

class eGameMenu : public eGameMenuBase {
public:
    using eGameMenuBase::eGameMenuBase;
    ~eGameMenu();
    void initialize(eGameBoard* const b,
                    const eAction& goalsView);

    int tradeCityId() const { return mTradeCityId; }
    eBuildingMode mode() const { return mMode; }
    void setMode(const eBuildingMode mode);
    void clearMode() { mMode = eBuildingMode::none; }

    void setGameWidget(eGameWidget* const gw);

    eMiniMap* miniMap() const;

    void pushEvent(const eEvent e, const eEventData& ed);

    using eViewTileHandler = std::function<void(eTile*)>;
    void setViewTileHandler(const eViewTileHandler& h);

    void closeBuildWidget();
    void setBuildWidget(eBuildWidget* const bw);

    void updateButtonsVisibility();
    void viewedCityChanged();
    void openBuildWidget(const int cmx, const int cmy,
                         const std::vector<eSPR>& cs);

    void setModeChangedAction(const eAction& func);

    void updateRequestButtons();

    void setWorldDirection(const eWorldDirection dir);

    void update();

    void setShowAllPossibleBuildings(const bool b);
protected:
    bool mousePressEvent(const eMouseEvent& e);
private:
    using eButtonsDataVec = std::vector<eSubButtonData>;
    eWidget* createSubButtons(const int resoltuionMult,
                              const eButtonsDataVec& buttons);
    eBuildButton* createBuildButton(const eSPR& c);

    void displayPrice(const int price, const int loc);
    eWidget* createPriceWidget(const eInterfaceTextures& coll);

    eGameBoard* mBoard{nullptr};
    eGameWidget* mGW = nullptr;

    eBuildWidget* mBuildWidget = nullptr;

    eFramedLabel* mNameLabel = nullptr;

    eCheckableButton* mPopulationButton = nullptr;
    eCheckableButton* mHusbandryButton = nullptr;
    eCheckableButton* mIndustryButton = nullptr;
    eCheckableButton* mDistributionButton = nullptr;
    eCheckableButton* mHygieneSafetyButton = nullptr;
    eCheckableButton* mAdministrationButton = nullptr;
    eCheckableButton* mScienceButton = nullptr;
    eCheckableButton* mCultureButton = nullptr;
    eCheckableButton* mMythologyButton = nullptr;
    eCheckableButton* mMilitaryButton = nullptr;
    eCheckableButton* mAesthethicsButton = nullptr;
    eCheckableButton* mOverviewButton = nullptr;

    ePopulationDataWidget* mPopDataW = nullptr;
    eEmploymentDataWidget* mEmplDataW = nullptr;
    eHusbandryDataWidget* mHusbDataW = nullptr;
    eStorageDataWidget* mStrgDataW = nullptr;
    eAppealDataWidget* mApplDataW = nullptr;
    eHygieneSafetyDataWidget* mHySaDataW = nullptr;
    eAdminDataWidget* mAdminDataW = nullptr;
    eCultureDataWidget* mCultureDataW = nullptr;
    eScienceDataWidget* mScienceDataW = nullptr;
    eMythologyDataWidget* mMythDataW = nullptr;
    eMilitaryDataWidget* mMiltDataW = nullptr;
    eOverviewDataWidget* mOverDataW = nullptr;

    eRotateButton* mRotateButton = nullptr;
    eButton* mWorldButton = nullptr;

    eMiniMap* mMiniMap = nullptr;

    int mTradeCityId = -1;
    eBuildingMode mMode{eBuildingMode::none};

    std::vector<eWid> mWidgets;

    eEventWidget* mEventW = nullptr;

    std::vector<eWidget*> mPriceWidgets;
    std::vector<eLabel*> mPriceLabels;

    std::vector<eSubButton*> mSubButtons;

    eAction mModeChangeAct;

    bool mShowAllPossibleBuildings = false;
};

#endif // EGAMEMENU_H
