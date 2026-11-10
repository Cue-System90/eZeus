#ifndef EPATROLBUILDINGBASE_H
#define EPATROLBUILDINGBASE_H

#include "eemployingbuilding.h"
#include "engine/epatrolguide.h"
#include "characters/actions/epatrolaction.h"
#include "enumbers.h"

class ePatrolBuildingBase : public eEmployingBuilding {
    friend class eGuidedMovePathTask;
public:
    using eCharGenerator =  std::function<stdsptr<eCharacter>()>;
    using eActGenerator =  std::function<stdsptr<eCharacterAction>(
                                eCharacter* const c,
                                ePatrolBuildingBase* const b,
                                const std::vector<eOrientation>& path,
                                const stdsptr<eDirectionTimes>& dirTimes)>;
    ePatrolBuildingBase(eGameBoard& board,
                        const eCharGenerator& charGen,
                        const eActGenerator& actGen,
                        const eBuildingType type,
                        const int sw, const int sh,
                        const int maxEmployees,
                        const eCityId cid);
    ePatrolBuildingBase(eGameBoard& board,
                        const eCharGenerator& charGen,
                        const eBuildingType type,
                        const int sw, const int sh,
                        const int maxEmployees,
                        const eCityId cid);
    ~ePatrolBuildingBase();

    static stdsptr<eCharacterAction> sDefaultActGenerator(
        eCharacter* const c,
        ePatrolBuildingBase* const b,
        const std::vector<eOrientation> &path,
        const stdsptr<eDirectionTimes>& dirTimes);

    void timeChanged(const int by) override;

    using ePatrolGuides = std::vector<ePatrolGuide>;
    ePatrolGuides& patrolGuides() { return mPatrolGuides; };
    ePatrolGuides reversePatrolGuides() const;
    void setPatrolGuides(const ePatrolGuides& g);
    bool bothDirections() const { return mBothDirections; }
    void setBothDirections(const bool both);

    void setSpawnPatrolers(const bool s);
    bool spawnPatrolers() const { return mSpawnPatrolers; }

    int maxDistance() const { return mMaxDistance; }
    void setMaxDistance(const int d) { mMaxDistance = d; }
    void setWaitTime(const int w) { mWaitTime = w; }

    bool spawnsPatrolers() const;

    const stdsptr<eDirectionTimes>& dirTimes() const
    { return mDirTimes; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    bool updatePathIfNeeded();
    using eAction = std::function<void()>;
    bool updatePath(const eAction &finish = nullptr);

    const std::vector<eOrientation>& path() const { return mPath; }
    const std::vector<eOrientation>& reversePath() const { return mReversePath; }
private:
    bool spawn();

    const eCharGenerator mCharGenerator;
    const eActGenerator mActGenerator;

    void setPath(const std::vector<eOrientation>& path,
                 const std::vector<eOrientation>& reversePath);

    bool mBothDirections = false;
    bool mLastDirection = false;
    int mSpawnRoadId = 0;
    stdsptr<eDirectionTimes> mDirTimes =
            std::make_shared<eDirectionTimes>();

    bool mSpawnOnPathSet = false;
    int mPathState = 0;
    std::vector<eOrientation> mPath;
    std::vector<eOrientation> mReversePath;

    bool mSpawnPatrolers = true;

    int mMaxDistance = eNumbers::sPatrolerMaxDistance;

    int mWaitTime = eNumbers::sPatrolerWaitTime;
    double mSpawnTime = 1000000;
    stdptr<eCharacter> mChar;

    ePatrolGuides mPatrolGuides;
};

#endif // EPATROLBUILDINGBASE_H
