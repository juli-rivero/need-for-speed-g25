#pragma once

#include <assert.h>

#include <condition_variable>
#include <mutex>
#include <unordered_set>
#include <utility>

namespace common {

template <typename T>
class Emitter {
    std::mutex mtx;
    std::condition_variable has_listeners;
    std::unordered_set<T*> listeners;

    template <typename>
    friend class Listener;

    void add_listener(T* listener) {
        std::lock_guard lock(mtx);
        listeners.insert(listener);
    }

    void unsubscribe(T* listener) {
        {
            std::lock_guard lock(mtx);
            listeners.erase(listener);
        }
        has_listeners.notify_all();
    }

   public:
    template <typename Method, typename... Args>
    void dispatch(Method func, Args&&... args) {
        std::lock_guard lock(mtx);
        for (T* listener : listeners) {
            (listener->*func)(std::forward<Args>(args)...);
        }
    }

    Emitter() = default;

    /// Awaits for all listeners to unsubscribe.
    ~Emitter() {
        std::unique_lock lock(mtx);
        has_listeners.wait(lock, [&] { return listeners.empty(); });
    }
};

template <typename Parent>
class Listener {
    Emitter<Parent>* emitter = nullptr;

   protected:
    void subscribe(Emitter<Parent>& new_emitter) {
        assert(emitter == nullptr);
        emitter = &new_emitter;
        emitter->add_listener(static_cast<Parent*>(this));
    }

    void unsubscribe() {
        assert(emitter != nullptr);
        emitter->unsubscribe(static_cast<Parent*>(this));
        emitter = nullptr;
    }

   public:
    virtual ~Listener() { assert(emitter == nullptr); }
};
}  // namespace common
