#pragma once

#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include "Player.hpp"
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
        shared_ptr<Player> current_player_;  // to store the current player

    public:
        Game() : current_player_index_(0), game_started_(false), current_player_(nullptr) {} // constructor
        ~Game() = default;                                                                   // destructor

        // basic functions
        Role turn() const;              // to get the current player
        vector<string> players() const; // to get the players
        string winner() const;          // to get the winner

        // template <typename PlayerType, typename... Args>
        // shared_ptr<PlayerType> createPlayer(Args &&...args)
        // {
        //     if (game_started_)
        //     {
        //         throw runtime_error("Game already started");
        //     }
        //     auto player = make_shared<PlayerType>(*this, forward<Args>(args)...);
        //     players_.push_back(player);
        //     return player;
        // }

        shared_ptr<Player> createPlayer(const string &name, const Role &role);
        // helper functions

        void removePlayer();   // to remove a player from the game. the parameter is const because we don't want to change the player
        bool isPlayerActive(); // to check if the player is active. the parameter is const because we don't want to change the name and the function is const because it doesn't change the game
        void advanceTurn();    // to advance the turn to the next player

        // getters
        shared_ptr<Player> &getPlayer();
        size_t getPlayerIndex(const string &name) const;

        // check if the game has started correctly
        bool isGameStarted() const;
        bool isGameOver() const;
    };

}