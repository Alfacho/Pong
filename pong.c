#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>

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
#define UP2 'a'
#define DOWN2 'z'
#define UP2_H 'A'
#define DOWN2_H 'Z'
// controls player 2
#define UP1 'k'
#define DOWN1 'm'
#define UP1_H 'K'
#define DOWN1_H 'M'


// functions
void f_scorebar(int s1, int s2, int w);
void f_render(int h, int w, int r1_x, int r1_y, int r2_x, int r2_y, int b_x, int b_y);
void f_first_racket_physics(int* r1_y, char command);
void f_second_racket_physics(int* r2_y, char command);
int f_ball_physics(int* ball_x, int* ball_y, int* score1, int* score2, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y, int* impuls);
void f_whoStart(int* score1, int* score2, int* impuls);
int mygetch();

int main(void) {
    // score
    int score1 = 0, score2 = 0;
    // x and y position
    int r1_y = 13, r2_y = 13, r1_x = 10, r2_x = WEIGHT - 10, ball_x = 41, ball_y = 13, f_gr = 0, impuls, f = 1;

    int speed = 25000;

    
    while (score1 < WIN && score2 < WIN) {
        char c1, c2;

        if (f == 1) {
            f_whoStart(&score1, &score2, &impuls);
            ball_x = 41;
            ball_y = 13;
        }
        usleep(speed * 4);
        
        c1 = mygetch();

        f_first_racket_physics(&r1_y, c1);
        f_second_racket_physics(&r2_y, c2);

        f_scorebar(score1, score2, WEIGHT);
        f_render(HEIGHT, WEIGHT, r1_x, r1_y, r2_x, r2_y, ball_x, ball_y);

        f = f_ball_physics(&ball_x, &ball_y, &score1, &score2, &f_gr, r1_x, r1_y, r2_x, r2_y, &impuls);
    }
    
    if (score1 >= WIN) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 1 WON!\n");
        return 0;
    }
    if (score2 >= WIN) {
        f_scorebar(score1, score2, WEIGHT);
        printf("PLAYER 2 WON!\n");
        return 0;
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

void f_first_racket_physics(int* r1_y, char command) {
    if ((command == UP1 || command == UP1_H) && (*r1_y < HEIGHT - 2)) {
        *r1_y += 1;
    }
    if ((command == DOWN1 || command == DOWN1_H) && (*r1_y > 3)) {
        *r1_y -= 1;
    }
}

void f_second_racket_physics(int* r2_y, char command) {
    if ((command == UP2 || command == UP2_H) && (*r2_y < HEIGHT - 2)) {
        *r2_y += 1;
    }
    if ((command == DOWN2 || command == DOWN2_H) && (*r2_y > 3)) {
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
            *score2 = *score2 + 1;
            *f_gr = 0;
        } else {
            *score1 = *score1 + 1;
            *f_gr = 0;
        }
        return 1;
    }

    if (*f_gr <= 1) {
        if ((*ball_x + *impuls == r1_x && *ball_y == r1_y) || (*ball_x + *impuls == r2_x && *ball_y == r2_y)) {
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

int mygetch() {
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}

//bySAO