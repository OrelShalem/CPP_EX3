// orel8155@gmail.com

#include "Game.hpp"
#include "Player.hpp"
#include "GameExceptions.hpp"

using namespace std;
namespace coup
{

    Game::Game() : current_player_index_(0), game_started_(false) {} // constructor

    string Game::turn() const // to get the current player
    {

        if (players_.empty())
        {
            throw GameNotOverException("Game has not started yet");
        }

        return players_[current_player_index_]->name();
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

    void Game::addPlayer(shared_ptr<Player> player)
    {
        if (game_started_)
        {
            throw GameException("Game has already started");
        }
        players_.push_back(player);
    }

    void Game::removePlayer(const string &name)
    {
        auto player = getPlayer(name);
        player->setActive(false);
    }

    bool Game::isPlayerActive(const string &name) const
    {
        try
        {
            auto player = getPlayer(name);
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

    shared_ptr<Player> Game::getPlayer(const string &name) const
    {
        for (const auto &player : players_)
        {
            if (player->name() == name)
            {
                return player;
            }
        }
        throw PlayerNotFound("Player not found: " + name);
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