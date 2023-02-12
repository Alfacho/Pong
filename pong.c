#include <stdio.h>

// graphics
#define WALL "|"
#define FLOOR "-"
#define GRID "~"
#define BALL "o"

// features
#define HEIGHT 25
#define WEIGHT 81

// controls player 1
#define UP1 'a'
#define DOWN1 'z'
#define UP1_H 'A'
#define DOWN1_H 'z'
// controls player 2
#define UP2 'k'
#define DOWN2 'm'
#define UP2_H 'K'
#define DOWN2_H 'M'


// functions
void f_scorebar(int s1, int s2, int w);
void f_render(int h, int w, int r1_x, int r1_y, int r2_x, int r2_y, int b_x, int b_y);

int main(void) {
    // score
    int score1 = 0, score2 = 0;
    // x and y position
    int r1_y = 13, r2_y = 13, r1_x = 10, r2_x = WEIGHT - 10, ball_x = 41, ball_y = 13, f_gr = 0;
    
    f_scorebar(score1, score2, WEIGHT);
    f_render(HEIGHT, WEIGHT, r1_x, r1_y, r2_x, r2_y, ball_x, ball_y);
    
    if (score1 >= 21) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 1 WON!\n");
    }
    if (score2 >= 21) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 2 WON!\n");
    }

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

int f_first_racket_physics(int* r1_y, char command) {
    if ((command == UP1 || command == UP1_H) && (*r1_y < HEIGHT - 1)) {
        *r1_y++;
    }
    if ((command == DOWN1 || command == DOWN1_H) && (*r1_y > 2)) {
        *r1_y--;
    }

    return 0;
}

int f_second_racket_physics(int* r2_y, char command) {
    if ((command == UP2 || command == UP2_H) && (*r2_y < HEIGHT - 1)) {
        *r2_y++;
    }
    if ((command == DOWN2 || command == DOWN2_H) && (*r2_y > 2)) {
        *r2_y--;
    }

    return 0;
}

int f_ball_physics(int* ball_x, int* ball_y, int* score1, int* score2, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y) {
    if (((r1_x != *ball_x && r1_y != *ball_y) && (r1_x + 1 != *ball_x && r1_y != *ball_y) && (r1_x + 1 != *ball_x && r1_y != *ball_y)) 
        && ((r2_x != *ball_x && r2_y != ball_y) && (r2_x - 1 != *ball_x && r2_y != ball_y) && (r2_x + 1!= *ball_x && r2_y != ball_y))) {
        if ((*ball_y == 2 || *f_gr == 1)) {
            if (*ball_y != HEIGHT - 1) {
                *ball_y++;
                *ball_x = *ball_x + 1;
                *f_gr = 1;
                return 0;
            }
        }
        if ((*ball_y == HEIGHT - 1 || f_gr == 2)) {
            if (*ball_y != 2) {
                *ball_y--;
                *ball_x++;
                f_gr = 2;
                return 0;
            }
        }
    }
    if ((r1_x == *ball_x && r1_y == *ball_y) || (r1_x + 1 == *ball_x && r1_y == *ball_y) || (r1_x + 1 == *ball_x && r1_y == *ball_y) || f_gr == 3) {
        if (((r1_x == *ball_x && r1_y == *ball_y) || (f_gr == 3)) && ((r1_x + 1 != *ball_x && r1_y != *ball_y) && (r1_x + 1 != *ball_x && r1_y != *ball_y))) {
            //???
        }
    }
    *ball_x++;
}

//bySAO