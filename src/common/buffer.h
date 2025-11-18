#pragma once

#include <vector>

/** Util for buffering data and send it as a block.
 *
 * Reduces overload of packets as it buffers all data and, when it is ready,
 * sends all data as a block. This helps as it tries to maximize the payload of
 * the packet.
 */
class Buffer : std::vector<std::byte> {
   public:
    template <typename InputIt>
    void write_bytes(const InputIt& begin, const InputIt& end) {
        for (auto it = begin; it != end; ++it) write(*it);
    }

    template <typename T>
    void write(const T& value) {
        // https://stackoverflow.com/questions/5585532/c-int-to-byte-array
        // https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast
        const auto bytes =
            static_cast<const std::byte*>(static_cast<const void*>(&value));
        insert(end(), bytes, bytes + sizeof(T));
    }

    using std::vector<std::byte>::data;
    using std::vector<std::byte>::size;
    using std::vector<std::byte>::clear;
};
