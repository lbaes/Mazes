#ifndef MAZE_H
#define MAZE_H
#include "mazecell.h"
#include <cstdlib>
#include <locale.h>
#include <memory>
#include <stack>
#include <string>
#include <time.h>
#include <vector>
#include <wchar.h>

struct MazeDirection {
    static const int N = 1;
    static const int E = 2;
    static const int S = 4;
    static const int W = 8;

    static int getOppositeDirection(int dir) {
        if (dir == N)
            return S;
        else if (dir == E)
            return W;
        else if (dir == S)
            return N;
        else if (dir == W)
            return E;
        else
            return 0;
    }
};

class Maze {
  private:
    int rows, cols;
    std::unique_ptr<MazeCell[]> grid;
    static int directions[];
    static int deltaX[];
    static int deltaY[];
    static wchar_t chars[];

    inline int index(int i, int j) const;
    void genMaze(int i, int j);
    std::vector<int> getUnvisited(int i, int j);
    void genMazeStack();
    
  public:
    Maze(int, int);
    ~Maze();
    int getRows();
    int getCols();
    MazeCell *getGrid() const;
    void renderMaze();
};

int Maze::directions[] = {MazeDirection::N, MazeDirection::E, MazeDirection::S,
                          MazeDirection::W};
int Maze::deltaX[] = {0, 1, 0, -1};
int Maze::deltaY[] = {1, 0, -1, 0};
wchar_t Maze::chars[] = {L' ', L'╨', L'╞', L'╚', L'╥', L'║', L'╔', L'╠',
                         L'╡', L'╝', L'═', L'╩', L'╗', L'╣', L'╦', L'╬'};

Maze::Maze(int rows, int cols) : grid(new MazeCell[rows * cols]) {
    this->rows = rows;
    this->cols = cols;

    // top
    for (int i = 0; i < cols; i++)
        grid[index(0, i)].visited = true;
    // right
    for (int i = 0; i < rows; i++)
        grid[index(i, cols - 1)].visited = true;
    // bottom
    for (int i = 0; i < cols; i++)
        grid[index(rows - 1, i)].visited = true;
    // left
    for (int i = 0; i < rows; i++)
        grid[index(i, 0)].visited = true;

    srand(time(NULL));
    // genMaze(1, 1);
    genMazeStack();
}

Maze::~Maze() {}

MazeCell *Maze::getGrid() const { return grid.get(); }

int Maze::getRows() { return rows; }

int Maze::getCols() { return cols; }

inline int Maze::index(int i, int j) const { return i * cols + j; }

std::vector<int> Maze::getUnvisited(int i, int j) {
    std::vector<int> unvisitedCellDirections;

    for (unsigned int dir = 0; dir < 4; dir++)
        if ((grid[index(i + deltaY[dir], j + deltaX[dir])]).visited == false) {
            unvisitedCellDirections.push_back(dir);
        }

    return std::move(unvisitedCellDirections);
}

// recursive
void Maze::genMaze(int i, int j) {

    if (i > rows - 2 || i <= 0)
        return;
    if (j > cols - 2 || j <= 0)
        return;

    auto p = &grid[index(i, j)];
    p->visited = true;

    std::vector<int> unvisitedDirs;
    int randomDir;
    while ((unvisitedDirs = getUnvisited(i, j)).size() > 0) {

        randomDir = unvisitedDirs[rand() % unvisitedDirs.size()];

        if (directions[randomDir] == MazeDirection::N ||
            directions[randomDir] == MazeDirection::S) {
            grid[index(i + deltaY[randomDir], j + deltaX[randomDir])].data +=
                directions[randomDir];

            grid[index(i, j)].data +=
                MazeDirection::getOppositeDirection((directions[randomDir]));

        } else {
            grid[index(i + deltaY[randomDir], j + deltaX[randomDir])].data +=
                MazeDirection::getOppositeDirection((directions[randomDir]));
            grid[index(i, j)].data += directions[randomDir];
        }
        genMaze(i + deltaY[randomDir], j + deltaX[randomDir]);
    }
}

// iterative
void Maze::genMazeStack() {
    int i, j, rdir;
    std::vector<int> ud;
    std::stack<std::pair<int, int>> s;
    i = 1;
    j = 1;
    grid[index(i, j)].visited = true;
    s.push(std::pair<int, int>(i, j));
    while (!s.empty()) {
        i = s.top().first;
        j = s.top().second;
        s.pop();
        ud = getUnvisited(i, j);
        if (!ud.empty()) {
            s.push(std::pair<int, int>(i, j));
            rdir = ud[rand() % ud.size()];
            if (directions[rdir] == MazeDirection::N ||
                directions[rdir] == MazeDirection::S) {

                grid[index(i + deltaY[rdir], j + deltaX[rdir])].data +=
                    directions[rdir];

                grid[index(i, j)].data +=
                    MazeDirection::getOppositeDirection((directions[rdir]));
            } else {
                grid[index(i + deltaY[rdir], j + deltaX[rdir])].data +=
                    MazeDirection::getOppositeDirection((directions[rdir]));
                grid[index(i, j)].data += directions[rdir];
            }
            grid[index(i + deltaY[rdir], j + deltaX[rdir])].visited = true;
            s.push(std::pair<int, int>(i + deltaY[rdir], j + deltaX[rdir]));
        }
    }
}

void Maze::renderMaze() {
    setlocale(LC_ALL, "en_US.UTF-8");
    fwide(stdout, 1);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int v = grid[index(i, j)].data;
            wprintf(L"%lc", Maze::chars[v]);
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}
#endif // MAZE_H
