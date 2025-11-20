#pragma once

#include <chrono>

class TimerIterator {
    using clock = std::chrono::steady_clock;
    using time = std::chrono::time_point<clock>;
    using seconds = std::chrono::duration<double>;

    size_t iteration;
    const seconds interval;
    const time start;

   public:
    explicit TimerIterator(seconds interval);

    /**
     * @brief Duerme hasta el próximo intervalo programado.
     *
     * Calcula qué iteración debería ejecutarse según el tiempo actual
     * y duerme hasta ese instante usando `sleep_until`.
     * Si el llamado está atrasado, saltea una o más iteraciones, pero
     * siempre duerme al menos un pequeño tiempo antes de retornar.
     *
     * @return Cantidad de iteraciones avanzadas desde el último llamado.
     *         - Es imposible que retorne 0, ya que siempre avanza
     *         - Retorna 1 si se pasó al siguiente tick
     *         - Retorna 2 o más si se saltearon ticks
     */
    size_t next();

    /**
     * @brief Devuelve la iteración lógica actual del temporizador.
     *
     * Este valor se actualiza únicamente cuando se llama a `next()`,
     * y representa cuántos intervalos fijos
     * han transcurrido desde el inicio del temporizador.
     *
     * @return Número de iteración actual.
     */
    size_t get_iteration() const;
};
