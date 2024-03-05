#include "Leaderboard.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
    if(head_leaderboard_entry == nullptr || new_entry->score > head_leaderboard_entry->score){
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry= new_entry;
    }else{
        LeaderboardEntry *temp = head_leaderboard_entry;
        while (temp->next_leaderboard_entry != nullptr && temp->next_leaderboard_entry->score >= new_entry->score) {
            temp = temp->next_leaderboard_entry;
        }
        new_entry->next_leaderboard_entry = temp->next_leaderboard_entry;
        temp->next_leaderboard_entry = new_entry;
    }
    LeaderboardEntry * temp = head_leaderboard_entry;
    int length = 1;
    while(length<10 && temp->next_leaderboard_entry != nullptr){
        temp = temp->next_leaderboard_entry;
        ++length;
    }
    if(temp->next_leaderboard_entry != nullptr){
        delete temp->next_leaderboard_entry;
        temp->next_leaderboard_entry = nullptr;
    }

}

void Leaderboard::write_to_file(const string& filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
    LeaderboardEntry * temp = head_leaderboard_entry;
    std::ofstream file(filename);
    while(temp != nullptr){
        std::string time_value = to_string(static_cast<long long>(temp->last_played));
        file << temp->score << " " << time_value << " " << temp->player_name << "\n";
        temp = temp->next_leaderboard_entry;
    }
}

void Leaderboard::read_from_file(const string& filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.
    std::ifstream file(filename);
    string line;
    vector<vector<string>> players;
    while(getline(file, line)){
        vector<string> player;
        std::istringstream iss(line);
        std::string component;
        while (iss >> component) {
            player.push_back(component);
        }
        players.push_back(player);
    }
    file.close();
    for (auto & player : players) {
        unsigned long score = std::stoul(player[0]);
        unsigned long time_value = std::stoul(player[1]);
        string player_name = player[2];
        LeaderboardEntry * entry = new LeaderboardEntry(score,time_value,player_name);
        insert_new_entry(entry);
    }
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    LeaderboardEntry * temp = head_leaderboard_entry;
    int n = 1;
    std::cout << "Leaderboard\n-----------" << endl;
    while(temp != nullptr){
        char buffer[80];
        struct tm* timeinfo;
        timeinfo = localtime(&temp->last_played);
        strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", timeinfo);
        std::cout << n << ". "<< temp->player_name << " " << temp->score << " " << buffer <<std::endl;
        temp = temp->next_leaderboard_entry;
        ++n;
    }
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    while(head_leaderboard_entry != nullptr){
        LeaderboardEntry * temp = head_leaderboard_entry;
        head_leaderboard_entry = head_leaderboard_entry->next_leaderboard_entry;
        temp->next_leaderboard_entry = nullptr;
        delete temp;
    }
}


