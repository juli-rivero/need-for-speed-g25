#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include "Wall.h"
#include "Track.h"

struct SpawnPoint {
    float x;
    float y;
    float angle;
};

class Map {
private:
    std::string name;
    std::vector<Wall> walls;
    std::vector<SpawnPoint> spawnPoints;
    Track track;

public:
    Map(const std::string& name = "") : name(name) {}

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    void addWall(const Wall& wall) { walls.push_back(wall); }
    const std::vector<Wall>& getWalls() const { return walls; }

    void addSpawnPoint(float x, float y, float angle) {
        spawnPoints.push_back({x, y, angle});
    }
    const std::vector<SpawnPoint>& getSpawnPoints() const { return spawnPoints; }

    void setTrack(const Track& t) { track = t; }
    const Track& getTrack() const { return track; }
};

#endif
