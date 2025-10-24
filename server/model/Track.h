#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include <string>
#include "Checkpoint.h"

class Track {
private:
    std::string name;
    std::vector<Checkpoint> checkpoints;

public:
    Track(const std::string& name = "") : name(name) {}

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    void addCheckpoint(const Checkpoint& cp) { checkpoints.push_back(cp); }
    const std::vector<Checkpoint>& getCheckpoints() const { return checkpoints; }
};

#endif
