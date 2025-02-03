#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#define WIDTH 40
#define HEIGHT 20
#define PACMAN 'C'
#define WALL '#'
#define FOOD '.'
#define EMPTY ' '
#define DEMON 'X'
#define BONUS '$'
#define ENEMY 'E'

typedef struct {
    char type;
    bool isBonusActive;
    int bonusCounter;
} Cell;

int res = 0;
int score = 0;
int pacman_x, pacman_y;
Cell board[HEIGHT][WIDTH];
int food = 0;
int curr = 0;
bool isComputerPlaying = false;
int bonusCounter = 0;

void initialize() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || j == WIDTH - 1 || j == 0 || i == HEIGHT - 1) {
                board[i][j].type = WALL;
            } else {
                board[i][j].type = EMPTY;
            }
            board[i][j].isBonusActive = false;
            board[i][j].bonusCounter = 0;
        }
    }

    int count = 50;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j].type != WALL && board[i][j].type != PACMAN) {
            board[i][j].type = WALL;
            count--;
        }
    }

    int val = 5;
    while (val--) {
        int row = (rand() % (HEIGHT + 1));
        for (int j = 3; j < WIDTH - 3; j++) {
            if (board[row][j].type != WALL && board[row][j].type != PACMAN) {
                board[row][j].type = WALL;
            }
        }
    }

    count = 10;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j].type != WALL && board[i][j].type != PACMAN) {
            board[i][j].type = DEMON;
            count--;
        }
    }

    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x].type = PACMAN;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 && j % 2 == 0 && board[i][j].type != WALL && board[i][j].type != DEMON && board[i][j].type != PACMAN) {
                board[i][j].type = FOOD;
                food++;
            }
        }
    }

    count = 5;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j].type != WALL && board[i][j].type != DEMON && board[i][j].type != PACMAN && board[i][j].type != FOOD) {
            board[i][j].type = BONUS;
            count--;
        }
    }

    count = 3;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j].type != WALL && board[i][j].type != DEMON && board[i][j].type != PACMAN && board[i][j].type != FOOD && board[i][j].type != BONUS) {
            board[i][j].type = ENEMY;
            count--;
        }
    }
}

void draw() {
    system("cls");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", board[i][j].type);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
    if (bonusCounter > 0) {
        printf("Bonus Moves Left: %d\n", bonusCounter);
    }
}

void move(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x].type != WALL) {
        if (board[y][x].type == FOOD) {
            score++;
            food--;
            curr++;
            if (food == 0) {
                res = 2;
                return;
            }
        } else if (board[y][x].type == DEMON || board[y][x].type == ENEMY) {
            res = 1;
        } else if (board[y][x].type == BONUS) {
            bonusCounter = 10;
        }

        board[pacman_y][pacman_x].type = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x].type = PACMAN;
    }
}

void saveGame() {
    FILE *file = fopen("savegame.dat", "wb");
    if (file == NULL) {
        printf("Error saving game!\n");
        return;
    }
    fwrite(&pacman_x, sizeof(int), 1, file);
    fwrite(&pacman_y, sizeof(int), 1, file);
    fwrite(&score, sizeof(int), 1, file);
    fwrite(&food, sizeof(int), 1, file);
    fwrite(&curr, sizeof(int), 1, file);
    fwrite(&bonusCounter, sizeof(int), 1, file);
    fwrite(board, sizeof(Cell), HEIGHT * WIDTH, file);
    fclose(file);
}

void loadGame() {
    FILE *file = fopen("savegame.dat", "rb");
    if (file == NULL) {
        printf("No saved game found!\n");
        return;
    }
    fread(&pacman_x, sizeof(int), 1, file);
    fread(&pacman_y, sizeof(int), 1, file);
    fread(&score, sizeof(int), 1, file);
    fread(&food, sizeof(int), 1, file);
    fread(&curr, sizeof(int), 1, file);
    fread(&bonusCounter, sizeof(int), 1, file);
    fread(board, sizeof(Cell), HEIGHT * WIDTH, file);
    fclose(file);
}

void computerMove() {
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    int dir = rand() % 4;
    move(directions[dir][0], directions[dir][1]);
}

void moveEnemies() {
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j].type == ENEMY) {
                int dir = rand() % 4;
                int x = j + directions[dir][0];
                int y = i + directions[dir][1];

                if (board[y][x].type == EMPTY || board[y][x].type == FOOD || board[y][x].type == PACMAN) {
                    if (board[y][x].type == PACMAN) {
                        res = 1;
                    }
                    board[i][j].type = EMPTY;
                    board[y][x].type = ENEMY;
                }
            }
        }
    }
}

int main() {
    srand(time(NULL));
    char ch;
    printf("Do you want to load the saved game? (Y/N): ");
    ch = getch();
    if (ch == 'Y' || ch == 'y') {
        loadGame();
    } else {
        initialize();
    }

    printf("Do you want to play against the computer? (Y/N): ");
    ch = getch();
    if (ch == 'Y' || ch == 'y') {
        isComputerPlaying = true;
    }

    food -= 35;
    int totalFood = food;

    while (1) {
        draw();
        printf("Total Food count: %d\n", totalFood);
        printf("Total Food eaten: %d\n", curr);
        if (res == 1) {
            system("cls");
            printf("Game Over! Dead by Demon or Enemy\n Your Score: %d\n", score);
            return 1;
        }

        if (res == 2) {
            system("cls");
            printf("You Win! \n Your Score: %d\n", score);
            return 1;
        }

        if (isComputerPlaying) {
            for (int i = 0; i < 5; i++) {
                computerMove();
            }
            Sleep(100);
        } else {
            ch = getch();
            switch (ch) {
                case 'w':
                    move(0, -1);
                    if (bonusCounter > 0) {
                        move(0, -1);
                        bonusCounter--;
                    }
                    break;
                case 's':
                    move(0, 1);
                    if (bonusCounter > 0) {
                        move(0, 1);
                        bonusCounter--;
                    }
                    break;
                case 'a':
                    move(-1, 0);
                    if (bonusCounter > 0) {
                        move(-1, 0);
                        bonusCounter--;
                    }
                    break;
                case 'd':
                    move(1, 0);
                    if (bonusCounter > 0) {
                        move(1, 0);
                        bonusCounter--;
                    }
                    break;
                case 'q':
                    printf("Game Over! Your Score: %d\n", score);
                    return 0;
                case 'p':
                    saveGame();
                    printf("Game saved!\n");
                    break;
            }
        }

        moveEnemies();
        Sleep(200);
    }

    return 0;
}
