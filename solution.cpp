
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <numeric> // needed for accumulate in compute_score

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for (const auto &row : board) {
        for (auto val : row) {
            if (val == 0) cout << ".\t";
            else cout << val << "\t";
        }
        cout << "\n";
    }
    cout << endl;
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
    vector<vector<int>> empty_space;
    for (int r=0;r<4;r++)
        for (int c=0;c<4;c++)
            if (board[r][c]==0) {
                empty_space.push_back({r,c});
            }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, empty_space.size()-1);

    vector<int> pos = empty_space[dist(gen)];

    uniform_int_distribution<> tile_dist(1,10);

    if (tile_dist(gen) == 4) {
        board[pos[0]][pos[1]] = 4;
    } else {
        board[pos[0]][pos[1]] = 2;
    }

}

vector<int> compress_left(const vector<int>& row){
    vector<int> compressed;

    copy_if(row.begin(), row.end(), back_inserter(compressed), [](int x){ return x!=0; });
    while (compressed.size()<4) {
        compressed.push_back(0);
    }
    return compressed;
    
}

vector<int> merge_left(vector<int>& row){
    for (int i=0; i < 3; i++){
        if (row[i] == row[i+1]){
            row[i] *= 2;
            row[i+1] = 0;
        }
    }
    return compress_left(row);
}

vector<int> compress_right(const vector<int>& row){
    vector<int> compressed;

    copy_if(row.rbegin(), row.rend(), back_inserter(compressed), [](int x){ return x!=0; });
    while (compressed.size()<4) {
        compressed.push_back(0);
    }
    reverse(compressed.begin(), compressed.end());
    return compressed;
    
}

vector<int> merge_right(vector<int>& row){
    for (int i=3; i > 0; i--){
        if (row[i] == row[i-1]){
            row[i] *= 2;
            row[i-1] = 0;
        }
    }
    return compress_right(row);
}


// TODO: implement using STL algorithms
bool move_left(vector<vector<int>>& board){
    bool moved = false;
    for (vector<int> &row : board){
        vector<int> compressed = compress_left(row);
        vector<int> merged = merge_left(compressed);
        if (row != merged) {
            moved = true;
        }
        row = merged;
    }
    return moved;
}


bool move_right(vector<vector<int>>& board){
    bool moved = false;
    for (vector<int> &row : board){
        vector<int> compressed = compress_right(row);
        vector<int> merged = merge_right(compressed);
        if (row != merged) {
            moved = true;
        }
        row = merged;
    }
    return moved;
}

void flip_board(vector<vector<int>>& board) {
    for (int r = 0; r < 4; r++){
        for (int c = r + 1; c < 4; c++) {
            swap(board[r][c], board[c][r]);
        }
    }
}

bool move_up(vector<vector<int>>& board){
    flip_board(board);
    bool moved = false;
    for (vector<int> &row : board){
        vector<int> compressed = compress_left(row);
        vector<int> merged = merge_left(compressed);
        if (row != merged) {
            moved = true;
        }
        row = merged;
    }
    flip_board(board);
    return moved;
}


bool move_down(vector<vector<int>>& board){
    flip_board(board);
    bool moved = false;
    for (vector<int> &row : board){
        vector<int> compressed = compress_right(row);
        vector<int> merged = merge_right(compressed);
        if (row != merged) {
            moved = true;
        }
        row = merged;
    }
    flip_board(board);
    return moved;
}

// TODO: compute score by summing board values
int compute_score(const vector<vector<int>>& board) {
    int score = 0;
    for (const auto &row : board) {
        score += accumulate(row.begin(), row.end(), 0);
    }
    return score;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout << "Score: " << compute_score(board) << "\n";
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: Implement undo handling here using the history stack
            if (!history.empty()) {
                board = history.top();
                history.pop();
            }
            print_board(board,false);
            continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: push prev to history stack for undo
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
