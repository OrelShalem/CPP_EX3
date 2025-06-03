//orel8155@gmail.com
#pragma once  // Ensures this header file is included only once during compilation

#include <cstddef>       // For size_t
#include <vector>        // For vector container
#include <string>        // For string class
#include <memory>        // For shared_ptr
#include "Player.hpp"    // Include Player class
#include <climits>       // For INT_MAX and other limits
using namespace std;     // Using standard namespace

namespace coup
{
    // Forward declaration to resolve circular dependency
    class Player;  // Player class will be defined elsewhere

    /**
     * Game class that manages the Coup game logic
     * Handles player management, turns, and game state
     */
    class Game
    {
    private:
        vector<shared_ptr<Player>> players_;           // Vector storing all players in the game
        size_t current_player_index_;                  // Index of the current player in the players vector
        size_t previous_player_index_;                 // Index of the previous player in the players vector
        bool game_started_;                            // Flag indicating if the game has started
        shared_ptr<Player> current_player_;            // Pointer to the current player
        shared_ptr<Player> previous_player_;           // Pointer to the previous player
        shared_ptr<Player> arrested_player_;           // Pointer to a player who has been arrested
        string player_get_arrested;                    // Name of the player who was arrested
        shared_ptr<Player> last_player_couped;         // Pointer to the last player who was eliminated via coup
        int bank_balance_;                             // Total coins in the bank

    public:
        /**
         * Constructor - Initializes a new game with default values
         */
        Game() : current_player_index_(0), previous_player_index_(0), game_started_(false), 
                current_player_(nullptr), previous_player_(nullptr), arrested_player_(nullptr), 
                player_get_arrested(""), last_player_couped(nullptr), bank_balance_(1000000) {}
        
        /**
         * Destructor - Uses default implementation
         */
        ~Game() = default;

        // Basic game functions
        /**
         * Returns the role of the current player
         * @return Role enum value of the current player
         */
        Role turn() const;
        
        /**
         * Returns a vector of all player names
         * @return Vector of strings containing player names
         */
        vector<string> players() const;
        
        /**
         * Returns the name of the winner if the game is over
         * @return String with winner's name
         */
        string winner() const;

        /**
         * Creates a new player and adds them to the game
         * @param name The player's name
         * @param role The role the player will play
         * @return Shared pointer to the created player
         */
        shared_ptr<Player> createPlayer(const string &name, const Role &role);
        
        // Helper functions
        /**
         * Removes a player from the game
         * @param player_name Name of the player to remove
         */
        void removePlayer(const string &player_name);
        
        /**
         * Checks if the current player is active
         * @return True if player is active, false otherwise
         */
        bool isPlayerActive();
        
        /**
         * Advances the turn to the next active player
         */
        void advanceTurn();

        // Getters
        /**
         * Gets the current player
         * @return Reference to shared pointer of current player
         */
        shared_ptr<Player> &getPlayer();
        
        /**
         * Gets the index of a player by name
         * @param name Name of the player to find
         * @return Index of the player in the players vector
         */
        size_t getPlayerIndex(const string &name) const;
        
        /**
         * Gets a player by their name
         * @param name Name of the player to find
         * @return Shared pointer to the player
         */
        shared_ptr<Player> getPlayerByName(const string &name) const;
        
        /**
         * Gets all players in the game
         * @return Reference to vector of player pointers
         */
        vector<shared_ptr<Player>> &getPlayers() { return players_; };
        
        /**
         * Gets the arrested player
         * @return Reference to shared pointer of arrested player
         */
        shared_ptr<Player> &getArrestedPlayer() { return arrested_player_; };
        
        /**
         * Gets the current player
         * @return Reference to shared pointer of current player
         */
        shared_ptr<Player> &getCurrentPlayer() { return current_player_; };
        
        /**
         * Gets the previous player
         * @return Reference to shared pointer of previous player
         */
        shared_ptr<Player> &getPreviousPlayer() { return previous_player_; };
        
        /**
         * Gets the index of the previous player
         * @return Index of the previous player
         */
        size_t getPreviousPlayerIndex() { return previous_player_index_; };
        
        /**
         * Gets the index of the current player
         * @return Index of the current player
         */
        size_t getCurrentPlayerIndex() { return current_player_index_; };

        // Methods for handling arrested player
        /**
         * Sets the name of the arrested player
         * @param name Name of the player who was arrested
         */
        void setArrestedPlayerName(const string &name) { player_get_arrested = name; }
        
        /**
         * Gets the name of the arrested player
         * @return Name of the arrested player
         */
        string getArrestedPlayerName() const { return player_get_arrested; }

        /**
         * Checks if the game has started correctly
         * Game is considered started if game_started_ flag is true and player count is valid
         * @return True if game has started correctly, false otherwise
         */
        bool isGameStarted() const
        {
            // Check if the game has started and if the number of players is valid (between 2 and 6)
            return game_started_ && isValidPlayerCount();
        }
        
        /**
         * Checks if the game is over
         * @return True if the game is over, false otherwise
         */
        bool isGameOver() const;

        /**
         * Checks if the number of players is valid (between 2 and 6)
         * @return True if player count is valid, false otherwise
         */
        bool isValidPlayerCount() const
        {
            size_t player_count = players_.size();
            return player_count >= 2 && player_count <= 6;
        }

        /**
         * Gets the last player who was eliminated via coup
         * @return Reference to shared pointer of the last couped player
         */
        shared_ptr<Player> &getLastPlayerCouped() { return last_player_couped; }
        
        /**
         * Adds coins to the bank
         * @param amount Number of coins to add
         */
        void addCoinsToBank(int amount) {bank_balance_ += amount; }
        
        /**
         * Removes coins from the bank
         * @param amount Number of coins to remove
         */
        void removeCoinsFromBank(int amount) {bank_balance_ -= amount; }
    };
}