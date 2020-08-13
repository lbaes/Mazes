#ifndef MMAZE_CELL_H
#define MAZE_CELL_H
#include <iostream>

enum class CELL_COLOR { NONE, RED, GREEN, YELLOW, BLUE };

class MazeCell {
  private:
    bool visited;
    int data;
    CELL_COLOR color;

  public:
    MazeCell();
    ~MazeCell();
    void setVisited(bool);
    int getVisited() const;

    void setData(int);
    int getData() const;

    void setColor(CELL_COLOR);
    CELL_COLOR getColor() const;

    friend std::ostream &operator<<(std::ostream &out, const MazeCell &m);

    friend class Maze;
};

MazeCell::MazeCell() {
    visited = 0;
    data = 0;
    color = CELL_COLOR::NONE;
}

MazeCell::~MazeCell() {}

void MazeCell::setVisited(bool newBool) { visited = newBool; }

int MazeCell::getVisited() const { return visited; }

void MazeCell::setData(int newData) { data = newData; }

int MazeCell::getData() const { return data; }

void MazeCell::setColor(CELL_COLOR newColor) { color = newColor; }

CELL_COLOR MazeCell::getColor() const { return color; }

std::ostream &operator<<(std::ostream &out, const MazeCell &m) {
    out << m.data;
    return out;
}

#endif // MAZE_CELL_H