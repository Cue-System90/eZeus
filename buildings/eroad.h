#ifndef EROAD_H
#define EROAD_H

#include "ebuilding.h"

class eGatehouse;
class eAgoraBase;
class eHippodromePiece;

class eRoad : public eBuilding {
public:
    eRoad(eGameBoard& board, const eCityId cid);

    // Above this appeal a road is drawn paved. The original's value is not
    // known, this one paves the better quarters of its cities.
    static const int sPavedAppeal = 20;

    void erase() override;

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

    int getHippodromeTextureId() const;

    void setUnderAgora(eAgoraBase* const a);
    eAgoraBase* underAgora() const { return mUnderAgora; }

    void setUnderGatehouse(eGatehouse* const g);
    eGatehouse* underGatehouse() const { return mUnderGatehouse; }

    void setAboveHippodrome(eHippodromePiece* const h);
    eHippodromePiece* aboveHippodrome() const { return mAboveHippodrome; }

    bool isBridge() const;
    bool isRoadblock() const;
    void setRoadblock(const bool rb);

    void setCharacterAltitude(const char a) { mCharacterAltitude = a; }
    char characterAltitude() const { return mCharacterAltitude; }

    void bridgeConnectedTiles(std::vector<eTile*>& tiles) const;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
private:
    eGatehouse* mUnderGatehouse = nullptr;
    eAgoraBase* mUnderAgora = nullptr;
    eHippodromePiece* mAboveHippodrome = nullptr;
    bool mRoadblock = false;
    char mCharacterAltitude = 0;
};

#endif // EROAD_H
