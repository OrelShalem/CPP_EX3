// orel8155@gmail.com

#include "Game.hpp"
#include "Player.hpp"
#include "GameExceptions.hpp"
#include "Roles/General.hpp"
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "Roles/Merchant.hpp"
#include <iostream>
#include "Player.hpp"
using namespace std;
namespace coup
{

    Role Game::turn() const // to get the current player
    {

        if (players_.empty())
        {
            throw GameNotOverException("Game has not started yet");
        }

        return players_[current_player_index_]->role();
    }

    shared_ptr<Player> Game::createPlayer(const string &name, const Role &role)
    {
        if (game_started_)
        {
            throw GameException("Game already started");
        }
        switch (role)
        {
        case Role::GENERAL:
        {
            auto player = make_shared<General>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        case Role::GOVERNOR:
        {
            auto player = make_shared<Governor>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        case Role::SPY:
        {
            auto player = make_shared<Spy>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        case Role::BARON:
        {
            auto player = make_shared<Baron>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        case Role::JUDGE:
        {
            auto player = make_shared<Judge>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        case Role::MERCHANT:
        {
            auto player = make_shared<Merchant>(*this, name, role);
            players_.push_back(player);
            return player;
        }
        default:
        {
            throw GameException("Invalid role");
        }
        }
    }

    vector<string> Game::players() const // to get the players
    {
        vector<string> active_players;
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                active_players.push_back(player->name());
            }
        }
        return active_players;
    }

    string Game::winner() const // to get the winner
    {
        if (!isGameOver())
        {
            throw GameNotOverException("Game has not ended yet");
        }
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                return player->name();
            }
        }

        throw GameException("No active players found");
    }

    void Game::removePlayer()
    {
        auto player = getPlayer();
        player->setActive(false);
    }

    bool Game::isPlayerActive()
    {
        try
        {
            auto player = getPlayer();
            return player->isActive();
        }
        catch (const PlayerNotFound &)
        {
            return false;
        }
    }

    void Game::advanceTurn()
    {
        if (players_.empty())
        {
            throw GameException("No players in the game");
        }

        if (!game_started_)
        {
            game_started_ = true;
        }

        do
        {
            current_player_index_ = (current_player_index_ + 1) % players_.size(); // to advance the turn to the next player
        } while (!players_[current_player_index_]->isActive());
    }

    shared_ptr<Player> &Game::getPlayer()
    {
        if (players_.empty())
        {
            throw GameException("No players in the game");
        }
        return players_[current_player_index_];
    }

    size_t Game::getPlayerIndex(const string &name) const
    {
        for (size_t i = 0; i < players_.size(); ++i)
        {
            if (players_[i]->name() == name)
            {
                return i;
            }
        }
        throw PlayerNotFound("Player not found: " + name);
    }

    bool Game::isGameStarted() const
    {
        return game_started_;
    }

    bool Game::isGameOver() const
    {
        int active_count = 0;
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                active_count++;
            }
        }
        return active_count <= 1 && game_started_;
    }

}