#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x, y) wprintf(L"\033[%d;%dH", (y), (x))

#define WALL 0
#define N 1
#define E 2
#define S 4
#define W 8

#define DELAY 100

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

const char *color_codes[] = {ANSI_COLOR_RESET,  ANSI_COLOR_RED,
                             ANSI_COLOR_YELLOW, ANSI_COLOR_GREEN,
                             ANSI_COLOR_BLUE,   ANSI_COLOR_MAGENTA};

void print_maze(int **maze, int maze_size);
void render_maze(int **maze, int **colors, int maze_size);
void free_maze(int **maze, int maze_size);

const wchar_t chars[] = {L' ', L'╨', L'╞', L'╚', L'╥', L'║', L'╔', L'╠',
                         L'╡', L'╝', L'═', L'╩', L'╗', L'╣', L'╦', L'╬'};

int directions[] = {0, N, E, S, W};
int deltaX[] = {0, 0, 1, 0, -1};
int deltaY[] = {0, 1, 0, -1, 0};

typedef struct node_t {
    int n;
    struct node_t *next;
} node_t;

void insert_after(node_t *node, int num) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->n = num;
    new_node->next = node->next;
    node->next = new_node;
}
node_t *insert_at_head(node_t *head, int num) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->n = num;
    new_node->next = head;
    return new_node;
}
void remove_after(node_t *node) {
    node_t *temp = node->next;
    node->next = NULL;
    free(temp);
}

node_t *remove_node(node_t *head, node_t *node) {
    node_t *cursor1, *cursor2;
    if (node == head) {
        cursor1 = head->next;
        free(head);
        return cursor1;
    }

    cursor1 = cursor2 = head;
    while (cursor2 != node && cursor2 != NULL) {
        cursor1 = cursor2;
        cursor2 = cursor2->next;
    }

    if (cursor2 != NULL) {
        cursor1->next = cursor2->next;
        free(cursor2);
    }
    return head;
}
int list_size(node_t *head) {
    int counter = 0;
    node_t *cursor = head;
    while (cursor != NULL) {
        counter++;
        cursor = cursor->next;
    }
    return counter;
}

node_t *list_at(node_t *head, int index) {
    int counter = 0;
    node_t *cursor = head;
    while (counter < index && cursor != NULL) {
        cursor = cursor->next;
        counter++;
    }
    return cursor;
}

node_t *find(node_t *head, int num) {
    node_t *cursor = head;
    while (cursor != NULL && cursor->n != num) {
        cursor = cursor->next;
    }
    return cursor;
}

void delete_list(node_t **head) {
    node_t *cursor;
    while (*head != NULL) {
        cursor = *head;
        *head = (*head)->next;
        free(cursor);
    }
}

int opposite(int dir) {
    switch (dir) {
    case N:
        return S;
    case E:
        return W;
    case S:
        return N;
    case W:
        return E;
    default:
        return 0;
    }
}

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

node_t *unvisited(int **visits, int maze_size, int i, int j) {
    node_t *h = NULL;
    for (unsigned int dir = 1; dir <= 4; dir++) {
        if ((visits[i + deltaY[dir]][j + deltaX[dir]]) == 0) {
            h = insert_at_head(h, dir);
        }
    }
    return h;
}

static bool draw = false;

void makePathR(int **maze, int **visits, int **colors, int maze_size, int i,
               int j) {

    if (draw)
        render_maze(maze, colors, maze_size);

    if (i > maze_size - 2 || i <= 0)
        return;
    if (j > maze_size - 2 || j <= 0)
        return;

    visits[i][j] = 1;
    int dir_size;
    int dir;
    node_t *dirs, *temp;
    dirs = temp = NULL;

    while ((dirs = unvisited(visits, maze_size, i, j)) != NULL) {
        colors[i][j] = 1;
        dir_size = list_size(dirs);
        temp = list_at(dirs, (rand() % dir_size));
        dir = temp->n;
        if (directions[dir] == N || directions[dir] == S) {
            maze[i][j] += opposite(directions[dir]);
            maze[i + deltaY[dir]][j + deltaX[dir]] += directions[dir];
        } else {
            maze[i][j] += directions[dir];
            maze[i + deltaY[dir]][j + deltaX[dir]] += opposite(directions[dir]);
        }
        makePathR(maze, visits, colors, maze_size, i + deltaY[dir],
                  j + deltaX[dir]);
        delete_list(&dirs);
    }
    colors[i][j] = 4;
    if (draw)
        render_maze(maze, colors, maze_size);
}

void gen_maze(int **maze, int maze_size) {
    int **visits = malloc(maze_size * sizeof(int *));
    for (int i = 0; i < maze_size; i++) {
        visits[i] = malloc(maze_size * sizeof(int));
    }

    int **colors = malloc(maze_size * sizeof(int *));
    for (int i = 0; i < maze_size; i++) {
        colors[i] = malloc(maze_size * sizeof(int));
    }

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            colors[i][j] = visits[i][j] = maze[i][j] = WALL;
        }
    }

    for (int i = 0; i < maze_size; i++) {
        maze[0][i] = WALL;
        visits[0][i] = 1;
    }

    for (int i = 0; i < maze_size; i++) {
        maze[i][0] = WALL;
        visits[i][0] = 1;
    }

    for (int i = 0; i < maze_size; i++) {
        maze[i][maze_size - 1] = WALL;
        visits[i][maze_size - 1] = 1;
    }

    for (int i = 0; i < maze_size; i++) {
        maze[maze_size - 1][i] = WALL;
        visits[maze_size - 1][i] = 1;
    }

    makePathR(maze, visits, colors, maze_size, 1, 1);
    render_maze(maze, colors, maze_size);
    free_maze(visits, maze_size);
    free_maze(colors, maze_size);
}

void create_maze(int maze_size) {
    int **maze = malloc(maze_size * sizeof(int *));
    for (int i = 0; i < maze_size; i++) {
        maze[i] = malloc(maze_size * sizeof(int));
    }

    srand(time(NULL));
    gen_maze(maze, maze_size);
    free_maze(maze, maze_size);
}

void free_maze(int **maze, int maze_size) {
    for (int i = 0; i < maze_size; i++) {
        free(maze[i]);
    }
    free(maze);
}

void print_maze(int **maze, int maze_size) {
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            if (maze[i][j] != 0)
                wprintf(L"%s[%02d]%s", ANSI_COLOR_YELLOW, maze[i][j],
                        ANSI_COLOR_RESET);
            else
                wprintf(L"%s[%02d]%s", ANSI_COLOR_RESET, maze[i][j],
                        ANSI_COLOR_RESET);
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}

void render_maze(int **maze, int **colors, int maze_size) {
    setlocale(LC_ALL, "");
    fwide(stdout, 1);
    clear();
    gotoxy(0, 0);
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            int v = maze[i][j];
            wprintf(L"%s%lc%s", color_codes[colors[i][j]], chars[v],
                    ANSI_COLOR_RESET);
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
    msleep(DELAY);
}

int main(int argc, char const *argv[]) {
    int **maze;
    int maze_size;
    if (argc < 3) {
        printf("%s requires 2 arguments\n", argv[0]);
        printf("Usage: %s <maze_size> <draw?>\n", argv[0]);
        return EXIT_FAILURE;
    }
    draw = strtol(argv[2], NULL, 10);
    maze_size = strtol(argv[1], NULL, 10);
    if (maze_size == 0L) {
        printf("Invalid maze size %s", argv[1]);
    }

    create_maze(maze_size);

    return 0;
}
