#include "GameController.h"
#include <fstream>
#include <iostream>

bool GameController::play(BlockFall& game, const string& commands_file){

    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.
    std::ifstream file(commands_file);
    string line; //Line is for commands.
    string result = "";
    vector<int> block_position = {0, 0}; // Vector to keep position of block.
    while(getline(file, line)) { // Loop for commands.
        // Print grid condition that calls printGrid function.
        if(line == "PRINT_GRID"){
            unsigned long high_score;
            if(game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score < game.current_score){
                high_score = game.current_score;
            }else{
                high_score = game.leaderboard.head_leaderboard_entry->score;
            }
            std::cout << printGrid(*game.active_rotation, game.grid, game.current_score, high_score, block_position, true);
        }
        // Move right condition that calls move function with direction.
        else if(line == "MOVE_RIGHT"){
            if(move(*game.active_rotation, game.grid, block_position, 1)){
                block_position[1] = block_position[1] + 1;
            }
        }
        // Move left condition that calls move function with direction.
        else if(line == "MOVE_LEFT"){
            if(move(*game.active_rotation, game.grid, block_position, -1)){
                block_position[1] = block_position[1] - 1;
            }
        }
        // Rotate right condition that calls rotate function with direction.
        else if(line == "ROTATE_RIGHT"){
            if(rotate(*game.active_rotation, game.grid, block_position, 1)){
                game.active_rotation = game.active_rotation->right_rotation;
            }
        }
        // Rotate left condition that calls rotate function with direction.
        else if(line == "ROTATE_LEFT"){
            if(rotate(*game.active_rotation, game.grid, block_position, -1)){
                game.active_rotation = game.active_rotation->left_rotation;
            }
        }
        // Gravity switch condition that change gravity_mode_on and call gravity function.
        // If there is full row, that calculates new score and remove the row.
        else if (line == "GRAVITY_SWITCH"){
            game.gravity_mode_on = !game.gravity_mode_on;
            if(game.gravity_mode_on){
                gravity(game.grid);
                vector<int> cleared_rows;
                for(int i = (int) game.grid.size() - 1; i >= 0; --i){
                    bool clearRow = true;
                    for (int j : game.grid[i]) {
                        clearRow = (j == 1);
                        if(!clearRow) break;
                    }
                    if(clearRow){
                        cleared_rows.push_back(i);
                    }
                }
                if(!cleared_rows.empty()){
                    cout << "Before clearing\n";
                    string result;
                    for (auto & j : game.grid) {
                        for (int k : j) {
                            result += (k == 1 ? occupiedCellChar : unoccupiedCellChar);
                            result += " ";
                        }
                        result.substr(0, result.length() - 1);
                        result += "\n";
                    }
                    cout <<result + "\n";
                }
                int x=0;
                for(int i = 0; i < cleared_rows.size(); ++i){
                    game.current_score += game.grid[cleared_rows[i]].size();
                    game.grid.erase(game.grid.begin() + cleared_rows[i] + x);
                    game.grid.insert(game.grid.begin(), std::vector<int>(game.grid[0].size(), 0));
                    ++x;
                }

            }
            continue;
        }
        //Drop condition that calls drop function.
        else if(line == "DROP"){
            drop(*game.active_rotation, game.grid, block_position, game.current_score, game.power_up, game.gravity_mode_on);
            unsigned long high_score;
            if(game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score < game.current_score){
                high_score = game.current_score;
            }else{
                high_score = game.leaderboard.head_leaderboard_entry->score;
            }
            block_position[0]= 0; block_position[1]= 0;
            if(game.active_rotation->next_block != nullptr){
                bool isGameOver = false;
                for (int i = 0; i < game.active_rotation->next_block->shape.size(); ++i) {
                    for (int j = 0; j < game.active_rotation->next_block->shape[i].size(); ++j) {
                        if (game.active_rotation->next_block->shape[i][j]) {
                            isGameOver = game.grid[i][j] == 1;
                            if (isGameOver) break;
                        }
                    }
                    if (isGameOver) break;
                }
                if (isGameOver) {
                    result = "GAME OVER!\nNext block that couldn't fit:\n";
                    for (auto &i: game.active_rotation->shape) {
                        for (auto &&j: i) {
                            result += (j ? occupiedCellChar : unoccupiedCellChar);
                        }
                        result += "\n";
                    }
                    cout << result;
                    cout << "\nFinal grid and score:\n\n";
                    cout << printGrid(*game.active_rotation, game.grid, game.current_score, high_score,
                                      block_position, false);
                    gameOver(game.current_score, game.player_name, game.leaderboard, game.leaderboard_file_name);
                    return false;
                }
            }else{
                cout << "YOU WIN!\nNo more blocks.\nFinal grid and score:\n\n";
                cout << printGrid(*game.initial_block, game.grid, game.current_score, high_score,
                                  block_position, false);
                gameOver(game.current_score, game.player_name, game.leaderboard, game.leaderboard_file_name);
                return true;
            }
            game.active_rotation = game.active_rotation->next_block;
        }
        // If the given command is unexpected.
        else{
            std::cerr << "Unknown command: " + line << endl;
        }
    }
    //If the commands run out, the result of game will be printed.
    file.close();
    unsigned long high_score;
    if(game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score < game.current_score){
        high_score = game.current_score;
    }else{
        high_score = game.leaderboard.head_leaderboard_entry->score;
    }
    cout << "GAME FINISHED!\nNo more commands.";
    cout << "\n\nFinal grid and score:\n\n";
    cout << printGrid(*game.active_rotation, game.grid, game.current_score, high_score,
                      block_position, false);
    gameOver(game.current_score, game.player_name, game.leaderboard, game.leaderboard_file_name);
    return true;
}

// Print grid function that prints current grid with active_rotation block.
string GameController::printGrid(Block & block, vector<vector<int>> grid, unsigned long score, unsigned long high_score, vector<int> block_position, bool x){
    string result = "\nScore: " + std::to_string(score);
    result += "\nHigh Score: " + std::to_string(high_score) + "\n";
    if(x){
        for (int i = 0; i < block.shape.size(); ++i) {
            for (int j = 0; j < block.shape[i].size(); ++j) {
                if (block.shape[i][j]) {
                    grid[i + block_position[0]][j + block_position[1]] = 1;
                }
            }
        }
    }
    for (auto & i : grid) {
        for (int j : i) {
            result += (j == 1 ? occupiedCellChar: unoccupiedCellChar);
        }
        result += "\n";
    }
    return result + "\n";
}

// Move function that return if block can move to the given direction.
bool GameController::move(Block & block, vector<vector<int>> & grid, vector<int> & block_position, int direction){
    bool canMove = true;
    for (int i = 0; i < block.shape.size(); ++i) {
        for (int j = 0; j < block.shape[i].size(); ++j) {
            if(block.shape[i][j] && i + block_position[0] < grid.size() && j + block_position[1] + direction < grid[0].size()){
                canMove = (grid[i + block_position[0]][j + block_position[1] + direction] == 0);
                if (!canMove){
                    return false;
                }
            }else if(block.shape[i][j]){
                return false;
            }
        }
    }
    return true;
}

// Rotate function that return if block can rotate the given direction.
bool GameController::rotate(Block &block, vector<vector<int>> & grid, vector<int> &block_position, int direction) {
    bool canRotate = true;
    Block * rotatedBlock = (direction == 1 ? block.right_rotation : block.left_rotation);
    for (int i = 0; i < rotatedBlock->shape.size(); ++i) {
        for (int j = 0; j < rotatedBlock->shape[i].size(); ++j) {
            if(rotatedBlock->shape[i][j] && (i + block_position[0]) < grid.size() && (j + block_position[1]) < grid[0].size()){
                canRotate = (grid[i + block_position[0]][j + block_position[1]] == 0);
            }else if (rotatedBlock->shape[i][j]){
                canRotate = false;
            }
            if(!canRotate) break;
        }
        if(!canRotate) break;
    }
    return canRotate;
}

// Drop function nearly do everything about drop and score.
void GameController::drop(Block &block, vector<vector<int>> &grid, vector<int> &block_position, unsigned long & score, vector<vector<bool>> power_up, bool isGravityOn) {
    int length = grid.size();

    while(length >= block.shape.size() + block_position[0]){
        bool canDrop = true;
        for (int i = 0; i < block.shape.size(); ++i) {
            for (int j = 0; j < block.shape[i].size(); ++j) {
                if(block.shape[i][j]){
                    canDrop = (grid[length - (block.shape.size()- i)][j + block_position[1]] == 0);
                    if(!canDrop) break;
                }
            }
            if(!canDrop) break;
        }
        if(canDrop){
            break;
        }else{
            length -= 1;
        }
    }
    int numOfCells = 0;
    for (int i = 0; i < block.shape.size(); ++i) {
        for (int j = 0; j < block.shape[i].size(); ++j) {
            if(block.shape[i][j]){
                ++numOfCells;
                grid[length - (block.shape.size()- i)][j + block_position[1]] = 1;
            }
        }
    }
    int numOfGrid = 0;
    for (auto & i : grid) {
        for (int j : i) {
            if(j == 1){
                ++numOfGrid;
            }
        }
    }
    if(isGravityOn){
        gravity(grid);
    }
    // Call is_power_up function. If it is power_up, prints and clear current grid.
    if(is_power_up(grid, power_up)){
        cout << "Before clearing\n";
        string result;
        for (auto & j : grid) {
            for (int k : j) {
                result += (k == 1 ? occupiedCellChar : unoccupiedCellChar);
                result += " ";
            }
            result.substr(0, result.length() - 1);
            result += "\n";
        }
        cout <<result + "\n";
        score += 1000 + numOfGrid;
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                grid[i][j] = 0;
            }
        }
    }
    // Checking whether is there any full row.
    score += (numOfCells * (int)(length - block.shape.size()));
    vector<int> cleared_rows;
    for(int i = (int) grid.size() - 1; i >= 0; --i){
        bool clearRow = true;
        for (int j : grid[i]) {
            clearRow = (j == 1);
            if(!clearRow) break;
        }
        if(clearRow){
            cleared_rows.push_back(i);
        }
    }
    // If there is full rows, remove that rows and adding new rows beginning of grid.
    if(cleared_rows.size() != 0){
        cout << "Before clearing\n";
        string result;
        for (auto & j : grid) {
            for (int k : j) {
                result += (k == 1 ? occupiedCellChar : unoccupiedCellChar);
                result += " ";
            }
            result.substr(0, result.length() - 1);
            result += "\n";
        }
        cout <<result + "\n";
    }
    int x=0;
    for(int i = 0; i < cleared_rows.size(); ++i){
        score += grid[cleared_rows[i]].size();
        grid.erase(grid.begin() + cleared_rows[i] + x);
        grid.insert(grid.begin(), std::vector<int>(grid[0].size(), 0));
        ++x;
    }
}

// It is just for updating and printing new leaderboard.
void GameController::gameOver(unsigned long & score, string player_name, Leaderboard & leaderboard, string file_name) {
    time_t current_time;
    LeaderboardEntry * entry = new LeaderboardEntry(score, time(&current_time),player_name);
    leaderboard.insert_new_entry(entry);
    leaderboard.print_leaderboard();
    leaderboard.write_to_file(file_name);
}

// Gravity function for gravity switch.
void GameController::gravity(vector<vector<int>> &grid) {
    for (int i = 0; i < grid[0].size(); ++i) {
        int destRow = grid.size() - 1;

        for (int j = grid.size() - 1; j >= 0; --j) {
            if (grid[j][i] == 1) {
                grid[destRow][i] = 1;

                if (destRow != j) {
                    grid[j][i] = 0;
                }
                --destRow;
            }
        }
    }
}

// Power up function to check is there any power_up on the grid.
bool GameController::is_power_up(vector<std::vector<int>>& grid, vector<std::vector<bool>>& power_up) {
    int gridRows = grid.size();
    int gridCols = grid[0].size();
    int pRows = power_up.size();
    int pCols = power_up[0].size();

    for (int i = 0; i <= gridRows - pRows; ++i) {
        for (int j = 0; j <= gridCols - pCols; ++j) {
            bool isPowerUp = true;
            for (int k = 0; k < pRows; ++k) {
                for (int l = 0; l < pCols; ++l) {
                    if (grid[i + k][j + l] != power_up[k][l]) {
                        isPowerUp = false;
                        break;
                    }
                }
                if (!isPowerUp) break;
            }
            if (isPowerUp) return true;
        }
    }
    return false;
}