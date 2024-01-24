#include "GameController.h"

#include <unordered_map>

bool GameController::play(BlockFall& game, const string& commands_file){

    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.
    //crate the player leaderboard

    // create the file stream to read the commands_file
    std::fstream commandsFile(commands_file); // take in the file
    // if file can not open, stop the program with exit code -1
    if (!commandsFile.is_open()) {
        std::cerr << "Can not open file!" << std::endl;
        exit(-1);
    }
    LeaderboardEntry *player;
    // start to process
    std::string line; // to keep the info of each line
    game.active_rotation = game.initial_block;

    int coordinateX = 0; // represent for columns
    int coordinateY= 0; // represent for rows
    if ( anyCollision(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game)) {
        return false;
    }else saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);

    while(std::getline(commandsFile,line)){
        int x= readCommand(line);
            switch (x) {
                case 1: // PRINT_GRID,
                                // PRİNT OPERATÖRÜN OVERLOAD ET YA DA OLUŞTURUDĞUM PRİNT FONKSİYONUNU DOLDUR
                    std::cout<<"Score: "<<game.current_score<<std::endl;
                    // is it first player or not
                    if (game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score < game.current_score){
                        std::cout<<"High Score: "<<game.current_score<<std::endl;
                    } else{
                        std::cout<<"High Score: "<<game.leaderboard.head_leaderboard_entry->score<<std::endl;
                    }
                    printGrid(game);
                    break;

                case 2: // DROP
                    //dropping
                    if (game.gravity_mode_on){

                        drop(coordinateX,coordinateY,game.active_rotation,game);
                        Block *cellOfBlock = new Block;
                        for (int i =int (game.active_rotation->shape.size())-1; i >=0 ; --i) {
                            for (int j = 0; j < game.active_rotation->shape.begin()->size(); ++j) {
                                if (game.grid[i+coordinateY][j+coordinateX]==1){
                                    cellOfBlock->shape = {{bool (game.grid[i+coordinateY][j+coordinateX])}};
                                    dropWithoutScore(std::pair<int, int> (j+coordinateX,i+coordinateY),cellOfBlock,game);
                                }
                            }
                        }
                        delete cellOfBlock;
                    }else  drop(coordinateX,coordinateY,game.active_rotation,game);

                    // after dropping, check is there any power-up block in the grid ?
                    if(anyPowerUp(game)){
                        std::cout<<"Before clearing:"<<std::endl;
                        printGrid(game);
                        clearTheGrid(game);

                    }else{ // finding none power_up then scan the grid for any completed rows
                        vector<int> completedRowsVector =anyCompletedRow(game);
                        if (! completedRowsVector.empty()){
                            slideRows(completedRowsVector,game);
                        }
                    }
                    // after checking the power-up block and any completed row
                    game.active_rotation = game.active_rotation->next_block;

                    if (game.active_rotation == nullptr) { // if no additional blocks remain available for gameplay
                        player= new LeaderboardEntry(game.current_score,std::time(nullptr),game.player_name);
                        // YOU WIN!
                        std::cout<<"YOU WIN!"<<std::endl<<"No more blocks."<<std::endl<<"Final grid and score:"<<std::endl;
                        std::cout<<"Score: "<<game.current_score<<std::endl;
                        // is it first player or not
                        if (game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score<game.current_score ){
                            std::cout<<"High Score: "<<game.current_score<<std::endl;
                        }else{
                            std::cout<<"High Score: "<<game.leaderboard.head_leaderboard_entry->score<<std::endl;
                        }
                        printGrid(game);

                        game.leaderboard.insert_new_entry(player);
                        game.leaderboard.write_to_file(game.leaderboard_file_name);
                        game.leaderboard.print_leaderboard();
                        //delete player;
                        player= nullptr;
                        return true;};
                    coordinateX = 0;
                    coordinateY = 0;
                    // if in the (0,0) coordinate of the grid, there is a collision, then game is over
                    //clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                    if ( anyCollision(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game)) {
                        //GAME OVER!
                        std::cout<<"GAME OVER!"<<std::endl<<"Next block that couldn't fit:"<<std::endl;
                        for (vector<bool>x:game.active_rotation->shape) {
                            for (bool y : x) {
                                if (y==1){
                                    //std::cout<<1<<" ";
                                    std::cout<<occupiedCellChar;
                                } else{
                                    //std::cout<<0<<" ";
                                    std::cout<<unoccupiedCellChar;
                                }
                            }std::cout<<std::endl;
                        }std::cout<<std::endl;
                        std::cout<<"Final grid and score:"<<std::endl<<"Score: "<<game.current_score<<std::endl;
                        // is it first or not
                        if (game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score<game.current_score ){
                            std::cout<<"High Score: "<<game.current_score<<std::endl;
                        }else{
                            std::cout<<"High Score: "<<game.leaderboard.head_leaderboard_entry->score<<std::endl;
                        }
                        printGrid(game);
                        player = new LeaderboardEntry(game.current_score,std::time(nullptr),game.player_name);
                        game.leaderboard.insert_new_entry(player);
                        game.leaderboard.write_to_file(game.leaderboard_file_name);
                        game.leaderboard.print_leaderboard();

                        //delete player;
                        return false;
                    }else saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);

                    break;

                case 3: // MOVE_RIGHT
                    if (coordinateX + game.active_rotation->shape.begin()->size() < game.cols){
                        clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        coordinateX += 1;
                        if (! anyCollision(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game)){
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }else{
                            coordinateX -= 1;
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }
                    }

                    break;

                case 4: // MOVE_LEFT
                    if (coordinateX !=0 ){
                        clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        coordinateX -= 1;
                        if (!anyCollision(std::pair<int, int> (coordinateX,coordinateY + 1),game.active_rotation,game)){
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }else{
                            coordinateX += 1;
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }
                    }

                    break;

                case 5: // ROTATE_RIGHT
                    // is there ends up outside the grid
                    if (coordinateX + game.active_rotation->right_rotation->shape.begin()->size() <= game.cols){
                        clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        // is there collision
                        if (!anyCollision(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation->right_rotation,game)){
                            game.active_rotation = game.active_rotation->right_rotation;
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }else{
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }
                    } else{
                        //nothing to do
                    }

                    break;

                case 6: // ROTATE_LEFT
                    // is there ends up outside the grid
                    if (coordinateX + game.active_rotation->left_rotation->shape.begin()->size() <= game.cols ){
                        clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        // is there collision
                        if (!anyCollision(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation->left_rotation,game)){
                            game.active_rotation = game.active_rotation->left_rotation;
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }else{
                            saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        }
                    } else{
                        //nothing to do
                    }
                    break;
                case 7: // GRAVITY_SWITCH
                    game.gravity_mode_on = !(game.gravity_mode_on); // Toggle gravity mode
                    //dropped block rearrange according to the new gravity mode
                    if (game.gravity_mode_on){
                        // clear the active block
                        clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                        // crate a temp Block to travers in grid
                        Block *cell = new Block;
                        for (int i = int (game.grid.size()-2) ; i >=0; --i) {
                            for (int j = 0; j < game.grid.begin()->size(); ++j) {
                                if (game.grid[i][j]==1){
                                    cell->shape = {{bool (game.grid[i][j])}};
                                    dropWithoutScore(std::pair<int, int> (j,i),cell,game);
                                }
                            }
                        }
                        delete cell;
                        vector<int> completedRows;
                        for (int i =0; i < game.rows; i++) {
                            int count =0;
                            for (int j = 0; j < game.cols; ++j) {
                                count+=game.grid[i][j];
                            }
                            if (count == game.cols){
                                game.current_score+=count;
                                completedRows.push_back(i);
                            }
                        }

                        if (! completedRows.empty()){
                            slideRows(completedRows,game);
                        }
                        // put the active block back
                        saveGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
                    }

                    break;
                default:
                    // error message but game will continue with the next command Unknown command: GIMME_POINTS
                    std::cout<<"Unknown command: " << line << std::endl;

                    break;
            }
    }
    commandsFile.close();
    clearBlock_fromSavedGrid(std::pair<int, int> (coordinateX,coordinateY),game.active_rotation,game);
    // if while is end then game is over means no more commands
    // GAME FINISHED!
    std::cout<<"GAME FINISHED!"<<std::endl<<"No more commands."<<std::endl<<"Final grid and score:"<<std::endl;
    std::cout<<"Score: "<<game.current_score<<std::endl;
    // is it first player or not
    if (game.leaderboard.head_leaderboard_entry == nullptr || game.leaderboard.head_leaderboard_entry->score<game.current_score ){
        std::cout<<"High Score: "<<game.current_score<<std::endl;
    } else{
        std::cout<<"High Score: "<<game.leaderboard.head_leaderboard_entry->score<<std::endl;
    }
    printGrid(game);
    player = new LeaderboardEntry(game.current_score,std::time(nullptr),game.player_name);
    game.leaderboard.insert_new_entry(player);
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    game.leaderboard.print_leaderboard();

    //delete player;
    return true; // no more commands
}

void GameController::printGrid(BlockFall& game){
/**
    for (vector<int>x:game.grid) {
        for (int y:x) {
            std::cout<<y<<" ";
        }std::cout<< std::endl;
    }std::cout<< std::endl;

*/
    for (vector<int>x:game.grid) {
        for (int y:x) {
            if (y==1){
                std::cout<<occupiedCellChar;
            } else std::cout<<unoccupiedCellChar;
        }std::cout<< std::endl;
    }std::cout<< std::endl;

}

bool GameController::anyCollision(std::pair<int, int> topLeft, Block *block,BlockFall& game) {

    int heightBlock = block->shape.size();
    int widthBlock = block->shape.begin()->size();
    for (int i = 0; i < heightBlock; ++i) {
        for (int j = 0; j < widthBlock; ++j) {
            if (game.grid[i+topLeft.second][j+topLeft.first] == 1 && block->shape[i][j] == 1){

                return true; //there is a collision
            }
        }
    }
    return false;
}

// read the command (which is "line") and return its correspondent integer for switch case operation
int GameController::readCommand(std::string line) {

    std::unordered_map<std::string, int> options = {
            {"PRINT_GRID", 1},
            {"DROP", 2},
            {"MOVE_RIGHT", 3},
            {"MOVE_LEFT", 4},
            {"ROTATE_RIGHT", 5},
            {"ROTATE_LEFT", 6},
            {"GRAVITY_SWITCH",7}
    };
    auto it = options.find(line);
    if (it == options.end()) {
        return -1;
    }
    return it->second;
}

void GameController::saveGrid(std::pair<int, int> topLeft, Block *block, BlockFall &game) {
    int heightBlock = block->shape.size();
    int widthBlock = block->shape.begin()->size();
    for (int i =0; i < heightBlock; ++i) {
        for (int j = 0; j < widthBlock; ++j) {
            if (game.grid[i+topLeft.second][j+topLeft.first] == 0)
            game.grid[i+topLeft.second][j+topLeft.first] = block->shape[i][j];
        }
    }
}

void GameController::clearBlock_fromSavedGrid(std::pair<int, int> topLeft, Block *block, BlockFall &game) { // temporarily
    int heightBlock = block->shape.size();
    int widthBlock = block->shape.begin()->size();
    for (int i =0; i < heightBlock; ++i) {
        for (int j =0; j < widthBlock; ++j) {
            if (block->shape[i][j] == 1) game.grid[i+topLeft.second][j+topLeft.first]=0; // clear the cells that only the block occupied
        }
    }
}

void GameController::drop(int& coordinateX,int& coordinateY, Block *block, BlockFall &game) {
    clearBlock_fromSavedGrid(pair<int, int>(coordinateX,coordinateY), block,game);
    // to calculate the score we want to know the amount of block cells
    int amountOfCell=0;
    for (vector<bool> x:block->shape) {
        for (bool y: x) {
            if (y){
                amountOfCell+=1;
            }
        }
    }
    while(  (coordinateY+block->shape.size() < game.rows)  && (!anyCollision(std::pair<int, int> (coordinateX,coordinateY+1),block,game)) ){
        coordinateY +=1 ;
        game.current_score+=amountOfCell;
    }

    saveGrid( pair<int, int>(coordinateX,coordinateY), block,game);
}

bool GameController::anyPowerUp(BlockFall &game) {
    for (int i = 0; i <= (game.rows) - (game.power_up.size()); ++i) {
        for (int j = 0; j <=(game.cols) - (game.power_up.begin()->size()) ; ++j) {
            vector<vector<bool>> powerUpBlockInGrid;
            for (int k = 0; k < game.power_up.size(); ++k) {
                vector<bool> row;
                for (int l = 0; l < game.power_up.begin()->size(); ++l) {
                    row.push_back(game.grid[i+k][j+l]);
                }powerUpBlockInGrid.push_back(row);
            }
            // compare these two block  powerUp shaped block in grid and active block
            if ( powerUpBlockInGrid == game.power_up ){
                // there is power up shape in grid
                game.current_score+=1000;
                return true;
            }
        }
    }
    return false;
}

void GameController::clearTheGrid(BlockFall &game) {
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols ; ++j) {
            if(game.grid[i][j]==1){
                game.current_score +=1;
                game.grid[i][j] = 0;
            }
        }
    }
}

void GameController::dropWithoutScore(pair<int, int> topLeft, Block *block, BlockFall &game) {
    clearBlock_fromSavedGrid(topLeft, block,game);
    // to calculate the score we want to know the amount of block cells
    while(  (topLeft.second+block->shape.size() < game.rows)  && (!anyCollision(std::pair<int, int> (topLeft.first,topLeft.second+1),block,game)) ){
        topLeft.second +=1 ;
    }
    saveGrid( topLeft, block,game);
}

vector<int> GameController::anyCompletedRow(BlockFall &game) {
    vector<int> completedRows;
    for (int i =0; i < game.rows; i++) {
        int count =0;
        for (int j = 0; j < game.cols; ++j) {
            count+=game.grid[i][j];
        }
        if (count == game.cols){
            game.current_score+=count;
            completedRows.push_back(i);
        }
    }
    // clean the completed rows
    if (! completedRows.empty()){
        std::cout<<"Before clearing:"<<std::endl;
        printGrid(game);
        for (int x: completedRows) {
            for (int k = 0; k < game.cols; ++k) {
                game.grid[x][k]=0;
            }
        }
    }

    return completedRows;
}

void GameController::slideRows(vector<int> completedRows, BlockFall &game) {
    // slide the rows above the cleaning row
    for (int completedRow: completedRows) {
        for (int i = completedRow; i >= 1; --i) {
            for (int j = 0; j < game.cols; ++j) {
                game.grid[i][j]=game.grid[i-1][j];
            }
        }
        // make zero all cell of the top row
        for (int j = 0; j < game.cols; ++j) {
            game.grid[0][j] = 0;
        }
    }
}









