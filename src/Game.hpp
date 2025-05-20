#pragma once

#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include <stdexcept>

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

        template <typename PlayerType, typename... Args>
        shared_ptr<PlayerType> createPlayer(Args &&...args)
        {
            if (game_started_)
            {
                throw runtime_error("Game already started");
            }
            auto player = make_shared<PlayerType>(*this, forward<Args>(args)...);
            players_.push_back(player);
            return player;
        }

        // helper functions

        void removePlayer(const string &player);       // to remove a player from the game. the parameter is const because we don't want to change the player
        bool isPlayerActive(const string &name) const; // to check if the player is active. the parameter is const because we don't want to change the name and the function is const because it doesn't change the game
        void advanceTurn();                            // to advance the turn to the next player

        // getters
        shared_ptr<Player> getPlayer(const string &name) const;
        size_t getPlayerIndex(const string &name) const;

        // check if the game has started correctly
        bool isGameStarted() const;
        bool isGameOver() const;

    private:
        void addPlayer(shared_ptr<Player> player);
    };

}