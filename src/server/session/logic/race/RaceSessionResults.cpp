#include "RaceSession.h"

std::vector<PlayerResult> RaceSession::makeResults() const {
    std::vector<PlayerResult> out;
    out.reserve(players.size());

    for (auto* p : players) {
        PlayerResult r;
        r.id = p->getId();
        r.rawTime = p->getRawTime();
        r.penalty = p->getPenalty();
        r.netTime = p->getNetTime();
        r.dnf = p->isDQ() || !p->isFinished();

        out.push_back(r);
    }

    std::sort(out.begin(), out.end(),
              [](const PlayerResult& a, const PlayerResult& b) {
                  if (a.dnf != b.dnf) return !a.dnf;
                  return a.netTime < b.netTime;
              });

    return out;
}
