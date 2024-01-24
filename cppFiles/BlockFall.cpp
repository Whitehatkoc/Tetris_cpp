
#include "BlockFall.h"

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    std::ifstream blockFile(input_file);
    // if file can not open, stop the program with exit code -1
    if (!blockFile.is_open()) {
        std::cerr << "Can not open file!" << std::endl;
        exit(-1);
    }

    // read the blocks from the input file
    vector<vector<vector<int>>> allBlocks = splitMatrix(blockFile);

    // initialize the initial_block
    initial_block = new Block;
    // deep copy to shape variable
    for (const vector<int>& x: allBlocks[0]) {
        vector<bool> row;
        for (int y:x) {
            if(y) row.push_back(true);
            else row.push_back(false);
        }
        initial_block->shape.push_back(row);
    }

    //initialize the active_rotation variable
    active_rotation = initial_block;

    // to travel in linked list with a pointer
    initial_block->next_block = new Block;
    Block* temp = initial_block->next_block;
    // then initialize all other blocks without their multilevel linked list
    for (int i = 1; i < allBlocks.size()-1; ++i) {
        // deep copy to shape variable
        for (const vector<int>& x: allBlocks[i]) {
            vector<bool> row;
            for (int y:x) {
                if(y) row.push_back(true);
                else row.push_back(false);
            }
            temp->shape.push_back(row);
        }
        if(i!=allBlocks.size()-2){
            temp->next_block = new Block;
            temp=temp->next_block;
        }else {};
    }

    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    temp = initial_block;
    while (temp!=nullptr){
        createCircularDoublyLinkedList(temp,temp);
        temp = temp->next_block;
    }


    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
    for (const vector<int>& x: allBlocks.back()) {
        vector<bool> row;
        for (int y:x) {
            if(y) row.push_back(true);
            else row.push_back(false);
        }
        power_up.push_back(row);
    }
    blockFile.close();
}

void BlockFall::initialize_grid(const string &input_file) {
    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main

    std::ifstream gridFile(input_file);
    // if file can not open, stop the program with exit code -1
    if (!gridFile.is_open()) {
        std::cerr << "Can not open file!" << std::endl;
        exit(-1);
    }
    // initialize "rows" member variable
    rows = 0;
    std::string line; // to keep each line to read
    while (std::getline(gridFile, line)) {
        rows++; // to find total row values in input file

        std::istringstream lineStream(line); // to read data on the related line
        std::vector<int> rowVector; // to keep 0 and 1 values of one line from the input file
        int cell; // to keep each 1 or 0 values to append to the 2D game grid named grid
        cols = 0; // initialize "cols" member variable
        while (lineStream >> cell) {
            cols++; // to find total column values in input file
            rowVector.push_back(cell);
        }
        grid.push_back(rowVector);
    }
    gridFile.close();
}

BlockFall::~BlockFall() {
    // TODO: Free dynamically allocated memory used for storing game blocks
    while (initial_block != nullptr) {
        Block* temp = initial_block;
        while (initial_block->right_rotation != initial_block){
            Block* temp2 = initial_block->right_rotation;
            initial_block->right_rotation = initial_block->right_rotation->right_rotation;
            delete temp2;
        }
        initial_block = initial_block->next_block;
        delete temp;
    }

 }



// extra help functions
vector<vector<vector<int>>> BlockFall::splitMatrix(std::ifstream & matrixFile){

    vector<vector<vector<int>>> allMatrixes;
    vector<vector<int>> matrix;
    std::string line;
    while (std::getline(matrixFile, line)) {
        if ((line.find('[') == 0 || line.find('0') == 0 ||line.find('1') == 0) && line.find(']') == std::string::npos) {
            vector<int> matrixRow;
            if (line.find('[') == 0) line.erase(line.begin());
            for (char x: line) {
                if ( x == '0' || x == '1' ){
                    matrixRow.push_back(x - '0');
                }
            }
            matrix.push_back(matrixRow);
        }
        else if(line.find_last_of(']') == line.length() - 1 && !line.empty()){
            vector<int> matrixRow;
            if (line.find('[') == 0) line.erase(line.begin());
            line.pop_back();
            for (char x: line) {
                if ( x == '0' || x == '1' ){
                    matrixRow.push_back(x - '0');
                }
            }
            matrix.push_back(matrixRow);
            allMatrixes.push_back(matrix);
            matrix.clear();
        }
        else {}// case of empty line
    }
    return allMatrixes;
}

void BlockFall::createCircularDoublyLinkedList(Block *temp, Block* head) {
    vector<vector<bool>> rightRotatedBlock=temp->rotationShape(false); // right rotation of block
    int n=0;
    if (rightRotatedBlock != head->shape){
        n++;
        temp->right_rotation = new Block;
        temp->right_rotation->shape = rightRotatedBlock;
        temp->right_rotation->left_rotation=temp;
        temp->right_rotation->next_block=temp->next_block;
        createCircularDoublyLinkedList(temp->right_rotation,head);
    }else {
        temp->right_rotation=head;
        head->left_rotation = temp;
    }
};


