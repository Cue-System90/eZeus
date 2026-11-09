#ifndef ETHREADBOARDHANDLER_H
#define ETHREADBOARDHANDLER_H

#include <atomic>
#include <mutex>

#include "engine/thread/ethreadboard.h"

enum class eStateRelevance;
class eBoardCity;

class eThreadBoardHandler {
public:
    eThreadBoardHandler();

    void initialize(const int w, const int h);

    void update(eGameBoard& board, const eCityId cid,
                const eStateRelevance rel);

    void updateBoard();

    eThreadBoard& board() { return mBoard; }
private:
    void update(const std::vector<eTile*>& tiles);
    void updateSanctuary(const std::vector<eTile*>& tiles);
    void updateAll(eGameBoard& board, const eBoardCity &c);
    void updateFinishedMonuments(const eBoardCity& c);
    void updateAgoras(const eBoardCity& c);

    bool mInitialized = false;
    eThreadBoard mBoard;
    std::atomic_bool mTmpChanged{false};
    eThreadBoard mTmpBoard;
    std::mutex mTmpBoardMutex;
};

#endif // ETHREADBOARDHANDLER_H
