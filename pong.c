#include <stdio.h>
#include <curses.h>
#include <unistd.h>

// graphics
#define WALL '|'
#define FLOOR '-'
#define GRID '~'
#define BALL 'o'

// gamemode
#define EASY 30000
#define NORMAL 25000
#define HARD 15000
#define IMPOSIBLE 5000

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
void f_first_racket_physics(int* r1_y, char command, char command1);
void f_second_racket_physics(int* r2_y, char command, char command1);
int f_ball_physics(int* ball_x, int* ball_y, int* score1, int* score2, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y, int* impuls);
void f_whoStart(int* score1, int* score2, int* impuls);
int f_menu();
void f_help();
void f_chose_mode(int* mode);


int main(void) {
    initscr();

    curs_set(0);
    noecho();

    int f_m = 1;

    while ((f_m = f_menu()) != 3) {
        

        // score
        int score1 = 0, score2 = 0;
        // x and y position
        int r1_y = 13, r2_y = 13, r1_x = 10, r2_x = WEIGHT - 10, ball_x = 41, ball_y = 13, f_gr = 0, impuls;

        int f = 1;

        if (f_m == 2) {
            f_help();
        }

        int mode = NORMAL;
        f_chose_mode(&mode);
        int speed = mode;

        // nodelay, kill getch, no-stop
        nodelay(stdscr, TRUE);
        char c1, c2;
        do {
            if (f == 1) {
                f_whoStart(&score1, &score2, &impuls);
                ball_x = 41;
                ball_y = 13;
            }
            usleep(speed * 4);
            
            c1 = getch();
            f_first_racket_physics(&r1_y, c1, c2);
            fflush(stdin);

            if ((c1 == UP1) || (c1 == DOWN1) || (c1 == DOWN1_H) || (c1 == UP1_H)) {
                while ((c2 == UP1) || (c2 == DOWN1) || (c2 == DOWN1_H) || (c2 == UP1_H)) {
                    c2 = getch();
                    fflush(stdin);
                }
            }
            if ((c1 == UP2) || (c1 == DOWN2) || (c1 == DOWN2_H) || (c1 == UP2_H)) {
                while ((c2 == UP1) || (c2 == DOWN1) || (c2 == DOWN1_H) || (c2 == UP1_H)) {
                    c2 = getch();
                    fflush(stdin);
                }
            }
            f_second_racket_physics(&r2_y, c2, c1);

            clear();
            f_scorebar(score1, score2, WEIGHT);
            f_render(HEIGHT, WEIGHT, r1_x, r1_y, r2_x, r2_y, ball_x, ball_y);

            f = f_ball_physics(&ball_x, &ball_y, &score1, &score2, &f_gr, r1_x, r1_y, r2_x, r2_y, &impuls);
        } while (score1 < WIN && score2 < WIN && c1 != 27 && c2 != 27);
        nodelay(stdscr, FALSE);
        
        if (score1 >= WIN) {
            do {
                clear();
                f_scorebar(score1, score2, WEIGHT);
                for (int j = 1; j < 3; j++) {
                    for (int i = 1; i < WEIGHT/2 - 13; i++) {
                        addch(' ');
                        if (j == 1 && i == WEIGHT/2 - 13 - 1) {
                            printw("PLAYER_1 WON!\n");
                        }
                        if (j == 2 && i == WEIGHT/2 - 14 - 1) {
                            printw("PRESS q TO EXIT");
                        }
                    }
                }
            } while(getch() != 'q');
        }
        if (score2 >= WIN) {
            do {
                clear();
                f_scorebar(score1, score2, WEIGHT);
                for (int j = 1; j < 3; j++) {
                    for (int i = 1; i < WEIGHT/2 - 13; i++) {
                        addch(' ');
                        if (j == 1 && i == WEIGHT/2 - 13 - 1) {
                            printw("PLAYER_2 WON!\n");
                        }
                        if (j == 2 && i == WEIGHT/2 - 14 - 1) {
                            printw("PRESS q TO EXIT");
                        }
                    }
                }
            } while(getch() != 'q');
        }
    }
    endwin();
    return 0;
}

void f_scorebar(int s1, int s2, int w) {
    printw("SCORE_1 - %i", s1);
    for (int i = 1; i <= w - 22; i++) {
        printw(" ");
    }
    printw("SCORE_2 - %i\n", s2);
}

void f_render(int h, int w, int r1_x, int r1_y, int r2_x, int r2_y, int b_x, int b_y) {
    for (int y = h; 0 < y; y--) {
        for (int x = w; 0 < x; x--) {
            // FLOOR
            if (y == 1 || y == h) {
                addch(FLOOR | A_NORMAL);
                continue;
            }
            // WALL
            if ((x == 1 && (y != 1 || y != h) ) || (x == w && (y != 1 || y != h) )) {
                addch(WALL);
                continue;
            }
            // RACKET 1
            if (((y == r1_y) && (x == r1_x)) || ((y == r1_y - 1) && (x == r1_x)) || ((y == r1_y + 1) && (x == r1_x))) {
                addch(WALL);
                continue;
            }
            // RACKET 2
            if (((y == r2_y) && (x == r2_x)) || ((y == r2_y - 1) && (x == r2_x)) || ((y == r2_y + 1) && (x == r2_x))) {
                addch(WALL);
                continue;
            }
            // BALL
            if ((y == b_y) && (x == b_x)) {
                addch(BALL);
                continue;
            }
            // GRID
            if ((x == WEIGHT/2 + 1) && ((x != b_x) || (y != b_y))) {
                addch(GRID);
                continue;
            }

            //FIELD
            addch(' ');
        }
        addch('\n');
    }
}

void f_first_racket_physics(int* r1_y, char command, char command1) {
    if ((command == UP1 || command == UP1_H) && (*r1_y < HEIGHT - 2) || (command1 == UP1 || command1 == UP1_H) && (*r1_y < HEIGHT - 2)) {
        *r1_y += 1;
    }
    if ((command == DOWN1 || command == DOWN1_H) && (*r1_y > 3) || (command1 == DOWN1 || command1 == DOWN1_H) && (*r1_y > 3)) {
        *r1_y -= 1;
    }
}

void f_second_racket_physics(int* r2_y, char command, char command1) {
    if ((command == UP2 || command == UP2_H) && (*r2_y < HEIGHT - 2) || (command1 == UP2 || command1 == UP2_H) && (*r2_y < HEIGHT - 2)) {
        *r2_y += 1;
    }
    if ((command == DOWN2 || command == DOWN2_H) && (*r2_y > 3) || (command1 == DOWN2 || command1 == DOWN2_H) && (*r2_y > 3)) {
        *r2_y -= 1;
    }
}

int f_ball_physics(int* ball_x, int* ball_y, int* score2, int* score1, int* f_gr, int r1_x, int r1_y, int r2_x, int r2_y, int* impuls) {
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
        *impuls = -1;
    }
    if (*score1 < *score2) {
        *impuls = +1;
    }
    if (*score1 == *score2) {
        *impuls = 1;
    }
}

int f_menu() {
    clear();
    int f = 1;
    char c;

    do {
        clear();
        if ((c == 's' || c == 'S') && f < 3) {
            f++;
        }
        if ((c == 'w' || c == 'W') && f > 1) {
            f--;
        }

        for (int y = HEIGHT; 0 < y; y--) {
            for (int x = WEIGHT; 0 < x; x--) {
                // FLOOR
                if (y == 1 || y == HEIGHT) {
                    addch(FLOOR);
                    continue;
                }
                // WALL
                if ((x == 1 && (y != 1 || y != HEIGHT) ) || (x == WEIGHT && (y != 1 || y != HEIGHT) )) {
                    addch(WALL);
                    continue;
                }
                // PONG
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 4) {
                    printw("| PONG |");
                    x -= 8;
                }
                // PLAY
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 + 1 &&  f != 1) {
                    printw("PLAY");
                    x -= 4;
                }
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 1 && f == 1) {
                    printw("> PLAY <");
                    x -= 8;  
                }
                // HELP
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 &&  f != 2) {
                    printw("HELP");
                    x -= 4;
                } 
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 && f == 2) {
                    printw("> HELP <");
                    x -= 8;
                }
                // EXIT
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 - 1 &&  f != 3) {
                    printw("EXIT");
                    x -= 4;
                }
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 - 1 && f == 3) {
                    printw("> EXIT <");
                    x -= 8;
                }
                // VERSION
                if (y == 2 && x == WEIGHT - 2) {
                    printw("v1.0");
                    x -= 4;
                }
                // AUTHOR
                if (y == 2 && x == 10) {
                    printw("byAllEXE");
                    x -= 8;
                }
                //FIELD
                addch(' ');
            }
            addch('\n');
        }
            
        c = getch();
    } while (c != '\n' && c != 27);

    if (c == 27) {
        f = 3;
    }

    return f;
}

void f_chose_mode(int* mode) {
    clear();
    int f = 1;
    char c;

    do {
        clear();
        if ((c == 's' || c == 'S') && f < 3) {
            f++;
        }
        if ((c == 'w' || c == 'W') && f > 1) {
            f--;
        }

        for (int y = HEIGHT; 0 < y; y--) {
            for (int x = WEIGHT; 0 < x; x--) {
                // FLOOR
                if (y == 1 || y == HEIGHT) {
                    addch(FLOOR);
                    continue;
                }
                // WALL
                if ((x == 1 && (y != 1 || y != HEIGHT) ) || (x == WEIGHT && (y != 1 || y != HEIGHT) )) {
                    addch(WALL);
                    continue;
                }
                // PONG
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 4) {
                    printw("| MODE |");
                    x -= 8;
                }
                // EASY
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 + 1 &&  f != 1) {
                    printw("EASY");
                    x -= 4;
                }
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 1 && f == 1) {
                    printw("> EASY <");
                    x -= 8;
                    *mode = EASY; 
                }
                // NORMAL
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 &&  f != 2) {
                    printw("NORMAL");
                    x -= 6;
                } 
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 && f == 2) {
                    printw("> NORMAL <");
                    x -= 10;
                    *mode = NORMAL;
                }
                // HARD
                if (x == WEIGHT/2 + 2 && y == HEIGHT/2 - 1 &&  f != 3) {
                    printw("HARD");
                    x -= 4;
                }
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 - 1 && f == 3) {
                    printw("> HARD <");
                    x -= 8;
                    *mode = HARD;
                }
                // VERSION
                if (y == 2 && x == WEIGHT - 2) {
                    printw("v1.0");
                    x -= 4;
                }
                // AUTHOR
                if (y == 2 && x == 10) {
                    printw("byAllEXE");
                    x -= 8;
                }
                //FIELD
                addch(' ');
            }
            addch('\n');
        }   
        c = getch();
        if (c == '~') {
            *mode = IMPOSIBLE;
            f = 1;
            do {
                clear();
                if ((c == 's' || c == 'S') && f < 3) {
                    f++;
                }
                if ((c == 'w' || c == 'W') && f > 1) {
                    f--;
                }
                for (int y = HEIGHT; 0 < y; y--) {
                    for (int x = WEIGHT; 0 < x; x--) {
                        // FLOOR
                        if (y == 1 || y == HEIGHT) {
                            addch(FLOOR);
                            continue;
                        }
                        // WALL
                        if ((x == 1 && (y != 1 || y != HEIGHT) ) || (x == WEIGHT && (y != 1 || y != HEIGHT) )) {
                            addch(WALL);
                            continue;
                        }
                        // ???
                        if (x == WEIGHT/2 + 9 && y == HEIGHT/2 + 4) {
                            printw("| READY TO HELL? |");
                            x -= 18;
                        }
                        // 666
                        if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 1 && f == 1) {
                            printw(" > YES < ");
                            x -= 9;
                        }
                        if (x == WEIGHT/2 + 2 && y == HEIGHT/2 + 1 && f != 1) {
                            printw(" YES ");
                            x -= 5;
                        }
                        /// &&&
                        if (x == WEIGHT/2 + 4 && y == HEIGHT/2 && f == 2) {
                            printw(" > YES < ");
                            x -= 9;
                        }
                        if (x == WEIGHT/2 + 2 && y == HEIGHT/2 && f != 2) {
                            printw(" YES ");
                            x -= 5;
                        }
                        //FIELD
                        addch('6');
                    }
                    addch('\n');
                }   
                c = getch();
            } while (c != '\n');
            c = '\n';
        }
    } while (c != '\n');
}

void f_help() {
    clear();
    int f = 1;
    char c;

    do {
        clear();
        if ((c == 's' || c == 'S') && f < 3) {
            f++;
        }
        if ((c == 'w' || c == 'W') && f > 1) {
            f--;
        }

        for (int y = HEIGHT; 0 < y; y--) {
            for (int x = WEIGHT; 0 < x; x--) {
                // FLOOR
                if (y == 1 || y == HEIGHT) {
                    addch(FLOOR);
                    continue;
                }
                // WALL
                if ((x == 1 && (y != 1 || y != HEIGHT) ) || (x == WEIGHT && (y != 1 || y != HEIGHT) )) {
                    addch(WALL);
                    continue;
                }
                // HELP
                if (x == WEIGHT/2 + 4 && y == HEIGHT/2 + 4) {
                    printw("| HELP |");
                    x -= 8;
                }
                // PLAYER_1
                if (x == WEIGHT/2 + 20 && y == HEIGHT/2 + 1) {
                    printw("PLAYER_1 USE \"A\" FOR UP AND \"Z\" FOR DOWN");
                    x -= 40;
                }
                // PLAYER_2
                if (x == WEIGHT/2 + 20 && y == HEIGHT/2) {
                    printw("PLAYER_2 USE \"K\" FOR UP AND \"M\" FOR DOWN");
                    x -= 40;  
                }
                // ESC
                if (x == WEIGHT/2 + 14 && y == HEIGHT/2 - 1) {
                    printw("PLAYERS USE \"ESC\" FOR ESCAPE");
                    x -= 28;  
                }
                // ENTER
                if (x == WEIGHT/2 + 14 && y == HEIGHT/2 - 3) {
                    printw("PRESS \"ENTER\" TO CONTINUE");
                    x -= 25;  
                }
                //FIELD
                addch(' ');
            }
            addch('\n');
        }
            
        c = getch();
    } while (c != '\n');
}


//byAllExe