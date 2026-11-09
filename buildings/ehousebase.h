#ifndef EHOUSEBASE_H
#define EHOUSEBASE_H

#include "ebuilding.h"

class ePopulationData;

enum class eHouseMissing {
    water,
    food,
    fleece,
    oil,
    venues,
    appeal,
    wine,
    arms,
    horse,
    nothing
};

class eHouseBase : public eBuilding {
public:
    eHouseBase(eGameBoard& board,
               const eBuildingType type,
               const int sw, const int sh,
               const std::vector<int>& maxPeople,
               const eCityId cid);
    ~eHouseBase();

    virtual eHouseMissing missing() const = 0;

    void timeChanged(const int by) override;

    int paidTaxes() const { return mPaidTaxes; }
    int paidTaxesLastMonth() const { return mPaidTaxesLastMonth; }

    int food() const { return mFood; }
    int fleece() const { return mFleece; }
    int oil() const { return mOil; }

    int philosophersInventors() const { return mPhilosophers; }
    int actorsAstronomers() const { return mActors; }
    int athletesScholars() const { return mAthletes; }
    int competitorsCurators() const { return mCompetitors; }
    int allCultureScience() const;

    void levelUp();
    void levelDown();

    // Whether the appeal is enough for the level, as Zeus decides it: a
    // house keeps a level on less than it needs to rise to it.
    bool appealAllows(const double appeal, const int level) const;

    int level() const { return mLevel; }
    int people() const { return mPeople; }

    int kill(int k);
    int moveIn(int c);
    void leave();

    int vacancies() const;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
protected:
    void setLevel(const int l);
    int evict();
    void setPeople(const int p);

    int mPaidTaxes = 0;
    int mPaidTaxesLastMonth = 0;

    int mLevel{0};
    int mPeople{0};

    int mFood = 0;
    int mFleece = 0;
    int mOil = 0;

    int mUpdateCulture = 0;

    int mPhilosophers = 0;
    int mActors = 0;
    int mAthletes = 0;
    int mCompetitors = 0;
private:
    ePopulationData& popData();

    const std::vector<int> mMaxPeople;
};

#endif // EHOUSEBASE_H
