#pragma once

#include <stdexcept>

namespace coup
{
    class GameException : public std::runtime_error
    {
    public:
        explicit GameException(const std::string &message) : std::runtime_error(message) {}
    };

    class InvalidTurn : public GameException
    {
    public:
        InvalidTurn(const std::string &message) : GameException(message) {}
    };

    class InvalidOperation : public GameException
    {
    public:
        InvalidOperation(const std::string &message) : GameException(message) {}
    };

    class NotEnoughCoins : public GameException
    {
    public:
        NotEnoughCoins(const std::string &message) : GameException(message) {}
    };

    class PlayerNotFound : public GameException
    {
    public:
        PlayerNotFound(const std::string &message) : GameException(message) {}
    };

    class GameOverException : public GameException
    {
    public:
        GameOverException(const std::string &message) : GameException(message) {}
    };

    class GameNotOverException : public GameException
    {
    public:
        GameNotOverException(const std::string &message) : GameException(message) {}
    };

}