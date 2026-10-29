#ifndef ESICKDISGRUNTLEDACTION_H
#define ESICKDISGRUNTLEDACTION_H

#include "eactionwithcomeback.h"

class eHouseBase;

// One walk out of a house and back, used by the sick and disgruntled
// of common housing and by the nobles of elite housing.
class eSickDisgruntledAction : public eActionWithComeback {
public:
    eSickDisgruntledAction(eCharacter* const c,
                           eHouseBase* const ch);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
protected:
    void patrol();
    void goBackDecision(const stdsptr<eWalkableObject>& w =
                            eWalkableObject::sCreateRoadAvenue());
private:
    eHouseBase* mBuilding = nullptr;

    bool mGoBackNext = false;
};

#endif // ESICKDISGRUNTLEDACTION_H
