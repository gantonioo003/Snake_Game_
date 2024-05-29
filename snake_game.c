#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "cli-lib/include/keyboard.h"
#include "cli-lib/include/screen.h"
#include "cli-lib/include/timer.h"

#define MAXX 80
#define MAXY 24

struct Position {
    int x, y;
};

struct SnakeNode {
    struct Position pos;
    struct SnakeNode *next;
};

struct Snake {
    struct SnakeNode *head;
    int length;
};

struct Food {
    struct Position pos;
};

struct Game {
    struct Snake *snake;
    struct Food *food;
    int score;
    int gameOver;
    char direction;
    int topScore;
};

void initGame(struct Game *game);
void drawGame(struct Game *game);
void updateGame(struct Game *game);
void endGame(struct Game *game);
void saveTopScore(int score);
int loadTopScore();

int main() {
    static int ch = 0;
    struct Game game;
    srand(time(0));

    game.topScore = loadTopScore();  
    initGame(&game);

    screenInit(1);
    keyboardInit();
    timerInit(100); 

    while (!game.gameOver) {
        drawGame(&game);

        if (keyhit()) {
            ch = readch();
            switch (ch) {
                case 'w':
                case 'a':
                case 's':
                case 'd':
                    game.direction = ch;
                    break;
                case 'q':
                    game.gameOver = 1;
                    break;
            }
        }

        if (timerTimeOver() == 1) {
            updateGame(&game);
        }
    }

    endGame(&game);

    if (game.score > game.topScore) {
        saveTopScore(game.score);
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

void initGame(struct Game *game) {
    game->snake = (struct Snake *)malloc(sizeof(struct Snake));
    game->snake->head = (struct SnakeNode *)malloc(sizeof(struct SnakeNode));
    game->snake->head->pos.x = MAXX / 2;
    game->snake->head->pos.y = MAXY / 2;
    game->snake->head->next = NULL;
    game->snake->length = 3; 

    struct SnakeNode *current = game->snake->head;
    for (int i = 1; i < game->snake->length; ++i) {
        current->next = (struct SnakeNode *)malloc(sizeof(struct SnakeNode));
        current->next->pos.x = game->snake->head->pos.x - i; 
        current->next->pos.y = game->snake->head->pos.y; 
        current->next->next = NULL;
        current = current->next;
    }

    game->food = (struct Food *)malloc(sizeof(struct Food));


    game->food->pos.x = rand() % (MAXX - 4) + 2; 
    game->food->pos.y = rand() % (MAXY - 4) + 2; 

    game->score = 0;
    game->gameOver = 0;
    game->direction = 'd'; 
}

void drawGame(struct Game *game) {
    screenClear();

    usleep(10000); 

    screenGotoxy(0, 0);
    for (int i = 0; i < MAXX; ++i) {
        printf("-");
    }

    for (int i = 1; i < MAXY - 1; ++i) {
        screenGotoxy(0, i);
        printf("|");
        screenGotoxy(MAXX - 1, i);
        printf("|");
    }

    screenGotoxy(0, MAXY - 1);
    for (int i = 0; i < MAXX; ++i) {
        printf("-");
    }

    struct SnakeNode *current = game->snake->head;
    while (current != NULL) {
        screenGotoxy(current->pos.x, current->pos.y);
        printf("O");
        current = current->next;
    }
    screenGotoxy(game->food->pos.x, game->food->pos.y);
    printf("*");

    char score[50];
    sprintf(score, "Score: %d  Top Score: %d", game->score, game->topScore);
    screenGotoxy(0, 0);
    printf("%s", score);

    screenUpdate(); 

    usleep(10000); 
}

void updateGame(struct Game *game) {
    struct Position newPos = game->snake->head->pos;
    switch (game->direction) {
        case 'w':
            newPos.y--;
            break;
        case 's':
            newPos.y++;
            break;
        case 'a':
            newPos.x--;
            break;
        case 'd':
            newPos.x++;
            break;
    }

    if (newPos.x < 1 || newPos.x >= MAXX - 1 || newPos.y < 1 || newPos.y >= MAXY - 1) {
        game->gameOver = 1;
        return;
    }

    struct SnakeNode *newHead = (struct SnakeNode *)malloc(sizeof(struct SnakeNode));
    newHead->pos = newPos;
    newHead->next = game->snake->head;

    game->snake->head = newHead;

    if (newPos.x == game->food->pos.x && newPos.y == game->food->pos.y) {
        game->score++;

        game->food->pos.x = rand() % (MAXX - 2) + 1;
        game->food->pos.y = rand() % (MAXY - 2) + 1;
    } else {
        struct SnakeNode *current = game->snake->head;
        while (current->next->next != NULL) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    }

    struct SnakeNode *current = game->snake->head->next;
    while (current != NULL) {
        if (newPos.x == current->pos.x && newPos.y == current->pos.y) {
            game->gameOver = 1;
            return;
        }
        current = current->next;
    }
}

void endGame(struct Game *game) {
    struct SnakeNode *current = game->snake->head;
    while (current != NULL) {
        struct SnakeNode *temp = current;
        current = current->next;
        free(temp);
    }
    free(game->snake);
    free(game->food);
}

void saveTopScore(int score) {
    FILE *file = fopen("top_score.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

int loadTopScore() {
    int score = 0;
    FILE *file = fopen("top_score.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &score);
        fclose(file);
    }
    return score;
}
