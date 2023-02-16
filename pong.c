#include <stdio.h>
#include <curses.h>
#include <unistd.h>

// graphics
#define WALL "|"
#define FLOOR "-"
#define GRID "~"
#define BALL "o"

// features
#define HEIGHT 25
#define WEIGHT 81
#define WIN 10

// controls player 1
#define UP1 'a'
#define DOWN1 'z'
#define UP1_H 'A'
#define DOWN1_H 'Z'
// controls player 2
#define UP2 'k'
#define DOWN2 'm'
#define UP2_H 'K'
#define DOWN2_H 'M'


// functions
void f_scorebar(int s1, int s2, int w);
void f_render(int h, int w, int r1_x, int r1_y, int r2_x, int r2_y, int b_x, int b_y);
void f_first_racket_physics(int* r1_y, char command);
void f_second_racket_physics(int* r2_y, char command);
int f_ball_physics(int* ball_x, int* ball_y, int* score1, int* score2, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y, int* impuls);
void f_whoStart(int* score1, int* score2, int* impuls);

int main(void) {
    // score
    int score1 = 0, score2 = 0;
    // x and y position
    int r1_y = 13, r2_y = 13, r1_x = 10, r2_x = WEIGHT - 10, ball_x = 41, ball_y = 13, f_gr = 0, impuls, f = 1;

    initscr();
    nodelay(stdscr, TRUE);
    int speed = 30000;
    while (score1 < WIN || score2 < WIN) {
        if (f == 1) {
            f_whoStart(&score1, &score2, &impuls);
            ball_x = 41;
            ball_y = 13;
        }
        usleep(speed * 4);
        clear();

        f_scorebar(score1, score2, WEIGHT);
        f_render(HEIGHT, WEIGHT, r1_x, r1_y, r2_x, r2_y, ball_x, ball_y);

        char c1 = getchar();
        char c2 = getchar();

        f_first_racket_physics(&r1_y, c1);
        f_second_racket_physics(&r2_y, c2);

        f = f_ball_physics(&ball_x, &ball_y, &score1, &score2, &f_gr, r1_x, r1_y, r2_x, r2_y, &impuls);
    }
    
    if (score1 >= 21) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 1 WON!\n");
    }
    if (score2 >= 21) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 2 WON!\n");
    }

    nodelay(stdscr, FALSE);
    endwin();

    return 0;
}

void f_scorebar(int s1, int s2, int w) {
    printf("SCORE_1 - %i", s1);
    for (int i = 1; i <= w - 22; i++) {
        printf(" ");
    }
    printf("SCORE_2 - %i\n", s2);
}

void f_render(int h, int w, int r1_x, int r1_y, int r2_x, int r2_y, int b_x, int b_y) {
    for (int y = h; 0 < y; y--) {
        for (int x = w; 0 < x; x--) {
            // FLOOR
            if (y == 1 || y == h) {
                printf(FLOOR);
                continue;
            }
            // WALL
            if ((x == 1 && (y != 1 || y != h) ) || (x == w && (y != 1 || y != h) )) {
                printf(WALL);
                continue;
            }
            // RACKET 1
            if (((y == r1_y) && (x == r1_x)) || ((y == r1_y - 1) && (x == r1_x)) || ((y == r1_y + 1) && (x == r1_x))) {
                printf(WALL);
                continue;
            }
            // RACKET 2
            if (((y == r2_y) && (x == r2_x)) || ((y == r2_y - 1) && (x == r2_x)) || ((y == r2_y + 1) && (x == r2_x))) {
                printf(WALL);
                continue;
            }
            // BALL
            if ((y == b_y) && (x == b_x)) {
                printf(BALL);
                continue;
            }
            // GRID
            if ((x == WEIGHT/2 + 1) && ((x != b_x) || (y != b_y))) {
                printf(GRID);
                continue;
            }

            //FIELD
            printf(" ");
        }
        printf("\n");
    }
}

void f_first_racket_physics(int* r1_y, char command) {
    if ((command == UP1 || command == UP1_H) && (*r1_y < HEIGHT - 1)) {
        *r1_y += 1;
    }
    if ((command == DOWN1 || command == DOWN1_H) && (*r1_y > 2)) {
        *r1_y -= 1;
    }
}

void f_second_racket_physics(int* r2_y, char command) {
    if ((command == UP2 || command == UP2_H) && (*r2_y < HEIGHT - 1)) {
        *r2_y += 1;
    }
    if ((command == DOWN2 || command == DOWN2_H) && (*r2_y > 2)) {
        *r2_y -= 1;
    }
}

int f_ball_physics(int* ball_x, int* ball_y, int* score1, int* score2, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y, int* impuls) {
    if ((*ball_x + *impuls == r1_x && *ball_y == r1_y) || (*ball_x + *impuls == r2_x && *ball_y == r2_y)) {
        *f_gr = 1;
    }
    if ((*ball_x + *impuls == r1_x && *ball_y == r1_y - 1) || (*ball_x + *impuls == r2_x && *ball_y == r2_y - 1) || (*ball_y == HEIGHT - 1)) {
        *f_gr = 2;
    }
    if ((*ball_x + *impuls == r1_x && *ball_y == r1_y + 1) || (*ball_x + *impuls == r2_x && *ball_y == r2_y + 1) || (*ball_y == 2)) {
        *f_gr = 3;
    }
    if (*ball_y == 2 && ((*ball_y == r2_y - 1 && *ball_x == r2_x + *impuls) || (*ball_y == r1_y - 1 && *ball_x == r1_x + *impuls))) {
        *f_gr = 4;
    }
    if (*ball_y == HEIGHT - 1 && ((*ball_y == r2_y + 1 && *ball_x == r2_x + *impuls) || (*ball_y == r1_y + 1 && *ball_x == r1_x + *impuls))) {
        *f_gr = 5;
    }
    if (*ball_x == 2 || *ball_x == WEIGHT - 1) {
        if (*ball_x == 2) {
            *score2 = *score1 + 1;
        } else {
            *score1 = *score2 + 1;
        }
        return 1;
    }

    if (*f_gr <= 1) {
        if (f_gr != 0) {
            *impuls = -(*impuls);
        }
        *ball_x += *impuls;
    }
    if (*f_gr == 2) {
        if ((*ball_x + *impuls == r1_x && *ball_y == r1_y - 1) || (*ball_x + *impuls == r2_x && *ball_y == r2_y - 1)) {
            *impuls = -(*impuls);
            *ball_x += *impuls;
            *ball_y -= 1;
        } else {
            *ball_x += *impuls;
            *ball_y -= 1;
        }
    }
    if (*f_gr == 3) {
        if ((*ball_x + *impuls == r1_x && *ball_y == r1_y + 1) || (*ball_x + *impuls == r2_x && *ball_y == r2_y + 1)) {
            *impuls = -(*impuls);
            *ball_x += *impuls;
            *ball_y += 1;
        } else {
            *ball_x += *impuls;
            *ball_y += 1;
        }
    }
    if (*f_gr == 4) {
        *impuls = -(*impuls);
        *ball_y += 1;
        *ball_x += *impuls;
    }
    if (*f_gr == 5) {
        *impuls = -(*impuls);
        *ball_y -= 1;
        *ball_x += *impuls;
    }
    return 0;
}

void f_whoStart(int* score1, int* score2, int* impuls) {
    if (*score1 > *score2) {
        *impuls = 1;
    }
    if (*score1 < *score2) {
        *impuls = -1;
    }
    if (*score1 == *score2) {
        *impuls = 1;
    }
}

//bySAO