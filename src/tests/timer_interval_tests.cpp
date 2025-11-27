#include <gtest/gtest.h>

#include "common/timer_iterator.h"

TEST(TimerIteratorTest, FirstSleepUpdatesIteration) {
    using namespace std::chrono;

    TimerIterator timer(seconds(1));
    std::this_thread::sleep_for(milliseconds(10));

    timer.next();

    EXPECT_EQ(timer.get_iteration(), 1);
}

TEST(TimerIteratorTest, IterationAdvancesOverTime) {
    using namespace std::chrono;

    TimerIterator timer(seconds(1));

    // Esperar 2500ms = debería estar en la tercera iteración
    std::this_thread::sleep_for(milliseconds(2500));
    timer.next();

    EXPECT_EQ(timer.get_iteration(), 3);
}

TEST(TimerIteratorTest, ActuallySleepsUntilNextInterval) {
    using namespace std::chrono;

    TimerIterator timer(seconds(1));

    const auto before = steady_clock::now();
    timer.next();
    const auto after = steady_clock::now();

    const auto slept = after - before;

    EXPECT_LT(slept, milliseconds(1100));
    EXPECT_GT(slept, milliseconds(900));
}

TEST(TimerIteratorTest, SkipsIterationsWhenLate) {
    using namespace std::chrono;

    TimerIterator timer(seconds(1));

    // Simular atraso de >3 intervalos
    std::this_thread::sleep_for(milliseconds(3500));

    const auto before = steady_clock::now();
    timer.next();
    const auto after = steady_clock::now();

    const auto slept = after - before;

    // Si está muy atrasado, no debe esperar 1s entero.
    EXPECT_LT(slept, milliseconds(800));

    // Pero por tu requisito debe dormir algo.
    EXPECT_GT(slept, milliseconds(300));
}
