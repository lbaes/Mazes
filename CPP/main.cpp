#include <iostream>
#include <locale.h>
#include "maze.h"

#define SUCCESS 1
#define FAILURE 0

using namespace std;

int test();

int main(int argc, char const *argv[])
{
    if (argc < 2){
        cout << "missing arguments" << endl;
        return 1;
    }
    MazeCell cell();
    Maze maze(atoi(argv[1]), atoi(argv[2]));

    maze.renderMaze();
    return 0;
}
