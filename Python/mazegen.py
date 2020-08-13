import numpy as np
import random
import sys

if len(sys.argv) < 3:
    print("Not engough arguments!!")
    exit(1)

rows = int(sys.argv[1]) + 1
cols = int(sys.argv[2]) + 1

drawing_chars = [' ', '╨', '╞', '╚', '╥', '║', '╔', '╠',
                 '╡', '╝', '═', '╩', '╗', '╣', '╦', '╬']

directions = [1, 2, 4, 8]
deltaX = [0, 1, 0, -1]
deltaY = [1, 0, -1, 0]
grid = []

def index(i, j):
    return i * cols + j

def makeCell():
    return [0, 0]

def opposite(direction):
    if direction == 1:
        return 4
    elif direction == 2:
        return 8
    elif direction == 4:
        return 1
    elif direction == 8:
        return 2

def markVisited(i, j):
    grid[index(i, j)][0] = 1

def checkNeighbours(i, j):
    unvisited = []
    for dir in range(4):
        if grid[index(i + deltaY[dir], j + deltaX[dir])][0] == 0:
            unvisited.append(dir)
    return unvisited

def breakWall(i, j, k, l, direction):
    if (direction == 1 or direction == 4):
        grid[index(i, j)][1] += opposite(direction)
        grid[index(k, l)][1] += direction
    else:
        grid[index(i, j)][1] += direction
        grid[index(k, l)][1] += opposite(direction)

def genMaze():
    stack = []
    i, j = 1, 1
    markVisited(i, j)
    stack.append((i, j))
    while len(stack) > 0:
        i, j = stack.pop()
        ud = checkNeighbours(i, j)
        if (len(ud) > 0):
            stack.append((i, j))
            uv = ud[random.randint(0, len(ud)-1)]
            breakWall(i, j, i + deltaY[uv], j + deltaX[uv], directions[uv])
            markVisited(i + deltaY[uv], j + deltaX[uv])
            stack.append((i + deltaY[uv], j + deltaX[uv]))

def render():
    for i in range (rows):
        for j in range(cols):
            print(f"{drawing_chars[grid[index(i, j)][1]]}", end="")
        print()

for i in range(rows*cols):
    grid.append(makeCell())
grid = np.array(grid)

# mark all borders as visited
# top
for i in range(cols):
    grid[index(0, i)][0] = 1
# right
for i in range(rows):
    grid[index(i, cols - 1)][0] = 1
# bottom
for i in range(cols):
    grid[index(rows - 1, i)][0] = 1
# left
for i in range(rows):
    grid[index(i, 0)][0] = 1
genMaze()
render()