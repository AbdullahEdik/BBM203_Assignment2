#include "BlockFall.h"
#include <iostream>
#include <fstream>
#include <utility>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
    std::ifstream file(input_file);
    string line;
    vector<vector<string>> block_vec;
    while(getline(file, line)){
        if (!line.empty()) {
            // Store blocks as a string with their condition.
            if (line.front() == '[' && line.back() == ']'){
                block_vec.emplace_back();
                int length = line.length();
                line = line.substr(1);
                line = line.substr(0, length-2);
                block_vec.back().push_back(line);
            }
            else if (line.front() == '[') {
                block_vec.emplace_back();
                block_vec.back().push_back(line.substr(1));
            }
            else if (line.back() == ']'){
                block_vec.back().push_back(line.substr(0, line.length() - 1));
            }
            else{
                block_vec.back().push_back(line);
            }

        }
    }
    file.close();
    // Define blocks' shape and create new blocks with insertFront().
    // This part uses the loop in reverse.
    for (int i = (int) (block_vec.size() - 1); i >= 0; --i) {
        vector<vector<bool>> block_shape;
        for (const auto & j : block_vec[i]) {
            vector<bool> shape_col;
            for (char k : j) {
                shape_col.push_back(k == '1');
            }
            block_shape.push_back(shape_col);
        }




        if (i == block_vec.size() -1) {
            power_up = block_shape;
        }else{
            insertFront(block_shape);
        }
    }
    initial_block = active_rotation;

}

void BlockFall::initialize_grid(const string &input_file) {
    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main
    std::ifstream file(input_file);
    vector<string> grid_str;
    string line;
    rows = 0;
    while(getline(file, line)){
        ++rows;
        cols = (int) (line.length() + 1) / 2;
        grid_str.push_back(line);
    }
    file.close();
    grid.resize(rows, std::vector<int>(cols));
    vector<int> row(cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            line = grid_str[i];
            row[j] = (line[2 * j] == '1' ? 1 : 0);
        }
        grid[i] = row;
    }

}
// Adding blocks the beginning of linked list.
void BlockFall::insertFront(vector<vector<bool>> shape) {
    Block* newBlock = new Block;
    newBlock->shape = shape;
    newBlock->right_rotation = newBlock;
    newBlock->left_rotation = newBlock;
    newBlock->next_block = active_rotation;
    Block* rightBlock = new Block;
    Block* leftBlock;
    Block* oppositeBlock;
    rightBlock->shape = rotateClockwise(newBlock->shape);
    if(*newBlock == *rightBlock){
        delete rightBlock;
    }else{
        rightBlock->next_block = active_rotation;
        newBlock->right_rotation = rightBlock;
        newBlock->left_rotation = rightBlock;
        rightBlock->right_rotation = newBlock;
        rightBlock->left_rotation = newBlock;
        oppositeBlock = new Block;
        oppositeBlock->shape = rotateClockwise(rightBlock->shape);
        if(*newBlock == *oppositeBlock){
            delete oppositeBlock;
        }else{
            oppositeBlock->next_block = active_rotation;
            leftBlock = new Block;
            leftBlock->next_block = active_rotation;
            leftBlock->shape = rotateClockwise(oppositeBlock->shape);
            rightBlock->right_rotation = oppositeBlock;
            oppositeBlock->left_rotation = rightBlock;
            oppositeBlock->right_rotation = leftBlock;
            leftBlock->left_rotation = oppositeBlock;
            leftBlock->right_rotation = newBlock;
            newBlock->left_rotation = leftBlock;
        }
    }
    active_rotation = newBlock;
}
// Rotate the shape clockwise.
vector<std::vector<bool>> BlockFall::rotateClockwise(const vector<vector<bool>> &input) {
    int row = (int) input.size();
    int col = (int) input[0].size();
    std::vector<std::vector<bool>> rotated(col, std::vector<bool>(row, false));

    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            rotated[j][row - 1 - i] = input[i][j];
        }
    }
    return rotated;
}



BlockFall::~BlockFall() {
    // TODO: Free dynamically allocated memory used for storing game blocks
    active_rotation = initial_block;
    while(active_rotation != nullptr){
        Block* temp = active_rotation;
        active_rotation = active_rotation->next_block;
        if(*temp != *(temp->right_rotation)){
            if(*temp != *((temp->right_rotation)->right_rotation)){
                (temp->right_rotation->right_rotation)->right_rotation = nullptr;
                (temp->right_rotation->right_rotation)->left_rotation = nullptr;
                (temp->right_rotation->right_rotation)->next_block = nullptr;
                delete (temp->right_rotation)->right_rotation;

                (temp->left_rotation)->right_rotation = nullptr;
                (temp->left_rotation)->left_rotation = nullptr;
                (temp->left_rotation)->next_block = nullptr;
                delete temp->left_rotation;
            }
            (temp->right_rotation)->right_rotation = nullptr;
            temp->right_rotation->left_rotation = nullptr;
            temp->right_rotation->next_block = nullptr;
            delete temp->right_rotation;
        }
        temp->right_rotation = nullptr;
        temp->left_rotation = nullptr;
        temp->next_block = nullptr;
        delete temp;
    }
    initial_block = nullptr;
}
