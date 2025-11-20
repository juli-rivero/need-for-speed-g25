#include "common/timer_iterator.h"

#include <thread>

TimerIterator::TimerIterator(const seconds interval)
    : iteration(0), interval(interval), start(clock::now()) {}

size_t TimerIterator::next() {
    const time now = clock::now();
    const size_t previos_iteration = iteration;
    iteration = 1 + (now - start) / interval;
    const auto wake_time = start + iteration * interval;
    std::this_thread::sleep_until(wake_time);
    return iteration - previos_iteration;
}

size_t TimerIterator::get_iteration() const { return iteration; }
