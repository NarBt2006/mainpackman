#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WIDTH 40
#define HEIGHT 20
#define PACMAN 'C'
#define WALL '#'
#define FOOD '.'
#define EMPTY ' '
#define DEMON 'X'
#define SPEED_BONUS '$'

int res = 0;
int score = 0;
int pacman_x, pacman_y;
char board[HEIGHT][WIDTH];
int food = 0;
int curr = 0;
int double_moves_remaining = 0; // Counter for double moves

void initialize() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || j == WIDTH - 1 || j == 0 || i == HEIGHT - 1) {
                board[i][j] = WALL;
            } else {
                board[i][j] = EMPTY;
            }
        }
    }

    int count = 50;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j] != WALL && board[i][j] != PACMAN) {
            board[i][j] = WALL;
            count--;
        }
    }

    int val = 5;
    while (val--) {
        int row = (rand() % (HEIGHT + 1));
        for (int j = 3; j < WIDTH - 3; j++) {
            if (board[row][j] != WALL && board[row][j] != PACMAN) {
                board[row][j] = WALL;
            }
        }
    }

    count = 10;
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j] != WALL && board[i][j] != PACMAN) {
            board[i][j] = DEMON;
            count--;
        }
    }

    // Place speed bonus items
    count = 5; // Number of speed bonus items
    while (count != 0) {
        int i = (rand() % (HEIGHT + 1));
        int j = (rand() % (WIDTH + 1));

        if (board[i][j] != WALL && board[i][j] != DEMON && board[i][j] != PACMAN && board[i][j] != FOOD) {
            board[i][j] = SPEED_BONUS;
            count--;
        }
    }

    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x] = PACMAN;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 && j % 2 == 0 && board[i][j] != WALL && board[i][j] != DEMON && board[i][j] != PACMAN && board[i][j] != SPEED_BONUS) {
                board[i][j] = FOOD;
                food++;
            }
        }
    }
}

void draw() {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
    if (double_moves_remaining > 0) {
        printf("Double Moves Remaining: %d\n", double_moves_remaining);
    }
}

void move(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x] != WALL) {
        if (board[y][x] == FOOD) {
            score++;
            food--;
            curr++;
            if (food == 0) {
                res = 2;
                return;
            }
        } else if (board[y][x] == DEMON) {
            res = 1;
        } else if (board[y][x] == SPEED_BONUS) {
            double_moves_remaining = 10; // Activate double moves
        }

        board[pacman_y][pacman_x] = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x] = PACMAN;

        if (double_moves_remaining > 0) {
            double_moves_remaining--;
            if (double_moves_remaining == 0) {
                // No need to display any message, just return to normal
            }
        }
    }
}

void saveGame() {
    FILE *file = fopen("saved_game.dat", "wb");
    if (file == NULL) {
        printf("Error saving game!\n");
        return;
    }

    fwrite(&score, sizeof(int), 1, file);
    fwrite(&pacman_x, sizeof(int), 1, file);
    fwrite(&pacman_y, sizeof(int), 1, file);
    fwrite(&food, sizeof(int), 1, file);
    fwrite(&curr, sizeof(int), 1, file);
    fwrite(&double_moves_remaining, sizeof(int), 1, file);
    fwrite(board, sizeof(char), HEIGHT * WIDTH, file);

    fclose(file);
    printf("Game saved successfully!\n");
}

int loadGame() {
    FILE *file = fopen("saved_game.dat", "rb");
    if (file == NULL) {
        return 0; // No saved game
    }

    fread(&score, sizeof(int), 1, file);
    fread(&pacman_x, sizeof(int), 1, file);
    fread(&pacman_y, sizeof(int), 1, file);
    fread(&food, sizeof(int), 1, file);
    fread(&curr, sizeof(int), 1, file);
    fread(&double_moves_remaining, sizeof(int), 1, file);
    fread(board, sizeof(char), HEIGHT * WIDTH, file);

    fclose(file);
    return 1; // Game loaded successfully
}

void computerMove() {
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // Up, Down, Left, Right
    int randomIndex = rand() % 4;
    move(directions[randomIndex][0], directions[randomIndex][1]);
}

int main() {
    srand(time(NULL)); // Seed the random number generator
    char ch;
    int totalFood;
    int computerMode = 0;

    printf("Do you want to continue the saved game? (Y/N): ");
    ch = getch();
    if (ch == 'Y' || ch == 'y') {
        if (loadGame()) {
            printf("Game loaded successfully!\n");
        } else {
            printf("No saved game found. Starting a new game.\n");
            initialize();
        }
    } else {
        initialize();
    }

    printf("Do you want to play manually or let the computer play? (M for manual, C for computer): ");
    ch = getch();
    if (ch == 'C' || ch == 'c') {
        computerMode = 1;
        printf("Computer is playing...\n");
    } else {
        printf("Manual mode. Use w(up), a(left), d(right), s(down), q(quit), p(save and quit)\n");
    }

    totalFood = food;

    while (1) {
        draw();
        printf("Total Food count: %d\n", totalFood);
        printf("Total Food eaten: %d\n", curr);
        if (res == 1) {
            system("cls");
            printf("Game Over! Dead by Demon\n Your Score: %d\n", score);
            return 1;
        }

        if (res == 2) {
            system("cls");
            printf("You Win! \n Your Score: %d\n", score);
            return 1;
        }

        if (computerMode) {
            computerMove();
            Sleep(1000); // Wait for 1 second
        } else {
            if (_kbhit()) {
                ch = _getch();

                switch (ch) {
                    case 'w':
                        move(0, -1);
                        if (double_moves_remaining > 0) {
                            move(0, -1); // Double move
                        }
                        break;
                    case 's':
                        move(0, 1);
                        if (double_moves_remaining > 0) {
                            move(0, 1); // Double move
                        }
                        break;
                    case 'a':
                        move(-1, 0);
                        if (double_moves_remaining > 0) {
                            move(-1, 0); // Double move
                        }
                        break;
                    case 'd':
                        move(1, 0);
                        if (double_moves_remaining > 0) {
                            move(1, 0); // Double move
                        }
                        break;
                    case 'q':
                        printf("Game Over! Your Score: %d\n", score);
                        return 0;
                    case 'p':
                        saveGame();
                        printf("Game saved. Exiting...\n");
                        return 0;
                }
            }
        }
    }

    return 0;
}
