#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
    if (head_leaderboard_entry == nullptr){ // indicating an absence of high scores
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry* current = head_leaderboard_entry;
        LeaderboardEntry* previous = nullptr;

        // find the place to insert
        while (current != nullptr && new_entry->score <= current->score) {
            previous = current;
            current = current->next_leaderboard_entry;
        }

        // insert to head
        if (previous == nullptr) {
            new_entry->next_leaderboard_entry = head_leaderboard_entry;
            head_leaderboard_entry = new_entry;
        } else {
            // insert middle or tail
            previous->next_leaderboard_entry = new_entry;
            new_entry->next_leaderboard_entry = current;
        }

    }

    // adjust the size of the linked list to MAX_LEADERBOARD_SIZE
    int n =0;
    LeaderboardEntry *temp = head_leaderboard_entry;
    while (temp != nullptr){
        temp = temp->next_leaderboard_entry;
        n++;
    }

    if (n>MAX_LEADERBOARD_SIZE){
        LeaderboardEntry *temp2 = head_leaderboard_entry;
        for (int i = 1; i < MAX_LEADERBOARD_SIZE; ++i) {
            temp2=temp2->next_leaderboard_entry;
        }
        LeaderboardEntry* temp3 = temp2->next_leaderboard_entry;
        temp2->next_leaderboard_entry = nullptr;

        // delete rest nodes
        LeaderboardEntry* nextNode;
        while (temp3 != nullptr) {
            nextNode = temp3->next_leaderboard_entry;
            delete temp3;
            temp3 = nextNode;
        }
    }

}

void Leaderboard::write_to_file(const string& filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
    std::ofstream outputLeaderboardFile(filename, std::ios::out | std::ios::trunc);
    LeaderboardEntry *tempEntry=head_leaderboard_entry;
    while (tempEntry != nullptr){
        outputLeaderboardFile << tempEntry->score <<" "<< tempEntry->last_played<<" "<<tempEntry->player_name<<std::endl;
        tempEntry=tempEntry->next_leaderboard_entry;
    }


}

void Leaderboard::read_from_file(const string& filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.
    std::ifstream leaderBoardFile(filename);
    // if file can not open, stop the program with exit code -1
    if (leaderBoardFile.is_open()) {
        std::string line;

        LeaderboardEntry *tempEntry = nullptr; // to travers on the linked list
        while (std::getline(leaderBoardFile, line)) {
            std::istringstream iss(line);
            LeaderboardEntry *entry = new LeaderboardEntry(0, 0, ""); // initialize with zero values, will fill up with real values
            if (iss >> entry->score >> entry->last_played >> entry->player_name) {
                if (head_leaderboard_entry == nullptr) {
                    head_leaderboard_entry = entry;
                    tempEntry = head_leaderboard_entry;
                } else{
                    tempEntry->next_leaderboard_entry = entry;
                    tempEntry = entry;
                }
            } else{
                // invalid
                delete entry;
            }

        }
        leaderBoardFile.close();
    }else{
        // indicating an absence of high scores
    }
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    LeaderboardEntry *temp = head_leaderboard_entry;
    int n=1;
    std::cout<<"Leaderboard"<<std::endl;
    std::cout<<"-----------"<<std::endl;
    while (temp!= nullptr){
        struct tm *localTime = localtime(&temp->last_played);
        char formattedTime[100];

        strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S/%d.%m.%Y", localTime);
        std::cout<< n << ". " << temp->player_name << " " << temp->score << " " << formattedTime << std::endl;
        temp=temp->next_leaderboard_entry;
        n++;
    }

}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    while (head_leaderboard_entry!= nullptr){
        LeaderboardEntry *temp = head_leaderboard_entry;
        head_leaderboard_entry = head_leaderboard_entry->next_leaderboard_entry;
        delete temp;
    }
}
