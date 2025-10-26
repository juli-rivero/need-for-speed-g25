#pragma once

#include <exception>

/** Clase que encapsula un "gai" error. @see getaddrinfo() */
class ResolverError : public std::exception {
    int gai_errno;

   public:
    explicit ResolverError(int gai_errno);

    const char* what() const noexcept override;

    /*
     * Siempre que heredemos debemos hacer a nuestros
     * destructores polimórficos (`virtual`)
     *
     * Esto garantiza que será llamado.
     * */
    ~ResolverError() override;
};
