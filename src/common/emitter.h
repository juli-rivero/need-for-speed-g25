#pragma once

#include <condition_variable>
#include <mutex>
#include <unordered_set>
#include <utility>

template <typename T>
class Emitter final {
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
    template <typename F, typename... Args>
    void emit(F func, Args&&... args) {
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
    Emitter<Parent>& emitter;

   public:
    explicit Listener(Emitter<Parent>& emitter) : emitter(emitter) {
        emitter.add_listener(static_cast<Parent*>(this));
    }
    virtual ~Listener() { emitter.unsubscribe(static_cast<Parent*>(this)); }
};
