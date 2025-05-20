#pragma once

#include <vector>
#include <string>
#include <memory>

using namespace std;

namespace coup
{

    class Player; // forward declaration

    class Game
    {
    private:
        vector<shared_ptr<Player>> players_; // to store the players
        size_t current_player_index_;        // to know the current player
        bool game_started_;                  // to check if the game has started

    public:
        Game();
        ~Game() = default; // destructor

        // basic functions
        string turn() const;            // to get the current player
        vector<string> players() const; // to get the players
        string winner() const;          // to get the winner

        // helper functions
        void addPlayer(shared_ptr<Player> player);
        void removePlayer(const string &player);       // to remove a player from the game. the parameter is const because we don't want to change the player
        bool isPlayerActive(const string &name) const; // to check if the player is active. the parameter is const because we don't want to change the name and the function is const because it doesn't change the game
        void advanceTurn();                            // to advance the turn to the next player

        // getters
        shared_ptr<Player> getPlayer(const string &name) const;
        size_t getPlayerIndex(const string &name) const;

        // check if the game has started correctly
        bool isGameStarted() const;
        bool isGameOver() const;
    };

}