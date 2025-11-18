
#pragma once

#include <box2d/box2d.h>

#include <cstddef>
#include <functional>

struct b2ShapeIdHasher {
    std::size_t operator()(const b2ShapeId id) const noexcept {
        return std::hash<uint64_t>()((static_cast<uintptr_t>(id.world0) << 32) ^
                                     id.index1);
    }
};

struct b2ShapeIdEqual {
    bool operator()(const b2ShapeId& a, const b2ShapeId& b) const noexcept {
        return a.world0 == b.world0 && a.index1 == b.index1;
    }
};
