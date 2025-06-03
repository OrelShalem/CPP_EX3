//orel8155@gmail.com
/**
 * Header file containing custom exception classes for the Coup game
 * This file defines several exception types that are used throughout the game
 * to handle various error conditions in a structured way
 */
#pragma once  // Ensures this header file is included only once during compilation

#include <stdexcept>  // Include standard runtime error classes

namespace coup
{
    /**
     * Base exception class for all game-related exceptions
     * Inherits from std::runtime_error to provide standard error functionality
     */
    class GameException : public std::runtime_error
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of the error that occurred
         */
        explicit GameException(const std::string &message) : std::runtime_error(message) {}
    };

    /**
     * Exception thrown when a player attempts to act out of turn
     * Used to enforce the turn-based rules of the game
     */
    class InvalidTurn : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of the invalid turn action
         */
        InvalidTurn(const std::string &message) : GameException(message) {}
    };

    /**
     * Exception thrown when a player attempts an invalid game operation
     * For example, using an ability that their role doesn't have
     */
    class InvalidOperation : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of the invalid operation
         */
        InvalidOperation(const std::string &message) : GameException(message) {}
    };

    /**
     * Exception thrown when a player doesn't have enough coins for an action
     * Used when actions require a minimum number of coins to perform
     */
    class NotEnoughCoins : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of the insufficient coins error
         */
        NotEnoughCoins(const std::string &message) : GameException(message) {}
    };

    /**
     * Exception thrown when trying to find a player that doesn't exist
     * Used when operations reference players by name that aren't in the game
     */
    class PlayerNotFound : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of which player wasn't found
         */
        PlayerNotFound(const std::string &message) : GameException(message) {}
    };

    /**
     * Exception thrown when trying to continue a game that is already over
     * Used to prevent actions after a winner has been determined
     */
    class GameOverException : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of the game over condition
         */
        GameOverException(const std::string &message) : GameException(message) {}
    };

    /**
     * Exception thrown when trying to determine a winner before the game is over
     * Used to prevent premature declaration of winners
     */
    class GameNotOverException : public GameException
    {
    public:
        /**
         * Constructor that takes an error message
         * @param message The description of why the game is not over
         */
        GameNotOverException(const std::string &message) : GameException(message) {}
    };

}