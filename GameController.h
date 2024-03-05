#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay

private:
    string printGrid(Block & block, vector<vector<int>> grid, unsigned long score, unsigned long hight_score, vector<int> block_position, bool x);
    bool move(Block & block, vector<vector<int>> & grid, vector<int> & block_position, int direction);
    bool rotate(Block & block, vector<vector<int>> & grid, vector<int> & block_position, int direction);
    void drop(Block & block, vector<vector<int>> & grid, vector<int> & block_position, unsigned long & score, vector<vector<bool>> power_up, bool isGravityOn);
    void gameOver(unsigned long & score, string player_name, Leaderboard & leaderboard, string file_name);
    void gravity(vector<vector<int>> &grid);
    bool is_power_up(vector<std::vector<int>>& grid, vector<std::vector<bool>>& power_up);
};

#endif //PA2_GAMECONTROLLER_H
