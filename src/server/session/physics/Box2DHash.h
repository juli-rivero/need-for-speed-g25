
#ifndef TALLER_TP_BOX2DHASH_H
#define TALLER_TP_BOX2DHASH_H

#include <box2d/box2d.h>
#include <cstddef>
#include <functional>

struct b2BodyIdHasher {
    std::size_t operator()(const b2BodyId id) const noexcept {
        // reinterpretar como puntero entero para hashing estable
        return std::hash<uint64_t>()((uint64_t)(uintptr_t)id.index1);
    }
};

struct b2BodyIdEqual {
    bool operator()(const b2BodyId& a, const b2BodyId& b) const noexcept {
        return a.index1 == b.index1 && a.world0 == b.world0;
    }
};
#endif //TALLER_TP_BOX2DHASH_H