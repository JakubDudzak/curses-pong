#define _POSIX_C_SOURCE 200201L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include <time.h>
#include <string.h>
struct timespec ts = {
        .tv_sec = 0,                    // nr of secs
        .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
};
const int width = 3; //sirka hraca
const int rows=31; //vyska ihriska
const int columns=50; //sirka ihriska
const int start_x = 5;
const int start_y = rows/2 + 4;
const int win_score = 3;
typedef struct{
    int x;
    int y;
    int direction;
        //-2 kazdym x ide hore o 1
        //-1 kazdym 2x ide hore o 1
        //0 ide rovno
        //1 kazdym 2x ide dole o 1
        //2 kazdym x ide dole o 1
    bool right;
}ball;
typedef struct{
    unsigned int score;
    int x;
    int y;
}player;



void game_field();
int pong(player player1,player player2,ball ball1, bool singleplayer, int max_score);
int main(int argc, char *argv[]){
    srand(time(NULL));

    // initialize the library
    initscr();
    // set implicit modes
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    // invisible cursor, visibility of cursor (0,1,2)
    curs_set(FALSE);
    // getch() will be non-blocking
    nodelay(stdscr, TRUE);
    //start colors
    start_color();

    // your code goes here
    if(argc < 2){
        printf("Not enough arguments! usage: ./main \"NUMBER_OF_PLAYERS (1/2)\"\n");
        return EXIT_FAILURE;
    }
    bool singleplayer;
    int choice = 0;
    int len = strlen(argv[1]);
    do{
        choice*= 10;
        choice+= argv[1][len-1] - '0';
        len--;
    } while(len > 0);
    if(choice!=2 && choice!=1){
        printf("Wrong arguments! NUMBER_OF_PLAYERS should be 1-2\n");
        return EXIT_FAILURE;
    }
    if (choice==1)
        singleplayer=true;
    else
        singleplayer=false;
    int max_score=0;

    ball ball1;
    player player1, player2;
    ball1.x = (columns*3)/2 +3;ball1.y = start_y;
    player1.x = start_x; player1.y = start_y;player1.score = 0;
    player2.x = start_x+columns*3-4;player2.y = start_y;player2.score = 0;
    pong(player1,player2,ball1,singleplayer,max_score);

    getchar();

    // end curses
    endwin();
    return EXIT_SUCCESS;
}
void print_entity(char ball){

    init_pair(3, COLOR_RED, COLOR_RED);
    init_pair(8, COLOR_WHITE, COLOR_WHITE);
    init_pair(15, COLOR_WHITE, COLOR_RED);



    switch (ball) {
        case '!':
            attron(COLOR_PAIR(3));
            printw(" %c ",ball);
            attroff(COLOR_PAIR(3));
            break;
        case '#':
            attron(COLOR_PAIR(15));
            printw(" %c ",ball);
            attroff(COLOR_PAIR(15));
            break;
        case '-':
            attron(COLOR_PAIR(8));
            printw(" %c ",ball);
            attroff(COLOR_PAIR(8));
            break;
        case '|':
            attron(COLOR_PAIR(8));
            printw(" %c ",ball);
            attroff(COLOR_PAIR(8));
            break;
        default:
            printw(" %c ",ball);
            break;
    }
}
void game_field(){
    //horny okraj
    printw("\n\n\n     ");
    for (int i = 0; i < columns-1; ++i) {
        print_entity('-');
    }

    printw("\n");
    //hracie pole
    for (int j = 0; j < rows; j++) {
        printw("     ");

        for (int i = 1; i < columns; ++i) {
            //sietka alebo antuka
            i == columns / 2 ? print_entity('#') : print_entity('!');
        }

        printw("\n");
    }
    printw("     ");
    //dolna stena
    for (int i = 0; i < columns-1; ++i) {
        print_entity('-');
    }
    printw("\n");
    refresh();
}
void print_ball(int x, int y){
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    attron(COLOR_PAIR(8));
    mvprintw(y,x,"O");
    attroff(COLOR_PAIR(8));
    refresh();
}
void print_player(int x, int y){
    init_pair(2, COLOR_WHITE, COLOR_WHITE);
    attron(COLOR_PAIR(2));
    for(int i = -2; i<=width-1;i++) {
        mvprintw(y+i, x, "|");
    }
    attroff(COLOR_PAIR(2));
    refresh();
}
player update_player(const bool up, player playerx){
    attron(COLOR_PAIR(3));
    if(up){
        mvprintw(playerx.y+width-1,playerx.x,"!");
        playerx.y--;
        print_player(playerx.x,playerx.y);
    } else{
        mvprintw(playerx.y-width+1,playerx.x,"!");
        playerx.y+=1;
        print_player(playerx.x,playerx.y);
    }
    attroff(COLOR_PAIR(3));
    refresh();
    return playerx;
}
ball update_ball(ball ballx){
    attron(COLOR_PAIR(1));
    bool isincenter = (ballx.x==(columns*3)/2 +3);
    int colour;
    if(isincenter)
        colour = 15;
    else
        colour = 3;
    attron(COLOR_PAIR(colour));
    switch (ballx.direction) {
        case -2:
            mvprintw(ballx.y,ballx.x,isincenter? "#" :  "!" );
            ballx.right ? ballx.x++:ballx.x--;
            ballx.y--;
            print_ball(ballx.x,ballx.y);
            break;
        case -1:
            mvprintw(ballx.y,ballx.x,isincenter? "#" :  "!" );
            ballx.right ? ballx.x++:ballx.x--;
            if(ballx.x%2==0)
                ballx.y--;
            print_ball(ballx.x,ballx.y);
            break;
        case 0:
            mvprintw(ballx.y,ballx.x,isincenter? "#" :  "!" );
            ballx.right ? ballx.x++:ballx.x--;
            print_ball(ballx.x,ballx.y);
            break;
        case 1:
            mvprintw(ballx.y,ballx.x,isincenter? "#" :  "!" );
            ballx.right ? ballx.x++:ballx.x--;
            if(ballx.x%2==0)
                ballx.y++;
            print_ball(ballx.x,ballx.y);
            break;
        case 2:
            mvprintw(ballx.y,ballx.x,isincenter? "#" :  "!" );
            ballx.right ? ballx.x++:ballx.x--;
            ballx.y++;
            print_ball(ballx.x,ballx.y);
            break;
        }
    attroff(COLOR_PAIR(colour));
    refresh();
    return ballx;
}
int pong(player player1,player player2,ball ball1, bool singleplayer, int max_score){
    srand(time(NULL));
    bool end = false;
    int seconds;
    int minutes;
    int score = 0;
    unsigned int speed = 32;
    int ball_speed;
    new_round:
    ball_speed = 1000;
    score = 0;
    seconds = 0;
    minutes = 0;
    move(0,0);
    game_field();
    print_ball(ball1.x, ball1.y);
    print_player(player1.x, player1.y);
    print_player(player2.x, player2.y);
    mvprintw(rows+6,(columns*3)/2,"%d | %d", player1.score,player2.score);
    mvprintw(rows+7,(columns*3)/2,"top score: %d", max_score);
    ball1.direction = 0;
    for(unsigned int i=0;!end;nanosleep(&ts, NULL),i++){
        if(i%ball_speed==0) {
            seconds += 1;
            if(seconds%10 == 0 && ball_speed > 500)
                ball_speed-=50;
            if (seconds==60){
                seconds=0;
                minutes++;
            }
        }
        mvprintw(rows+9,(columns*3)/2,"%d : %d ", minutes,seconds);\

        if (i%speed==0) {

            ball1 = update_ball(ball1);
            //odraz od hraca
            if (ball1.x == player1.x + 1) {
                if(player1.y==ball1.y){
                    ball1.right = true;
                    ball1.direction = 0;
                }
                else if(player1.y==ball1.y+1){
                    ball1.right = true;
                    ball1.direction = -1;
                }
                else if(player1.y==ball1.y+2){
                    ball1.right = true;
                    ball1.direction = -2;
                }
                else if(player1.y==ball1.y-2){
                    ball1.right = true;
                    ball1.direction = 2;
                }
                else if(player1.y==ball1.y-1){
                    ball1.right = true;
                    ball1.direction = 1;
                }
                else{
                    player2.score++;
                    player1.x = start_x; player1.y = start_y;
                    player2.x = start_x+columns*3-4;player2.y = start_y;
                    ball1.x = (columns*3)/2 +3;ball1.y = start_y;
                    if(score>max_score)
                        max_score=score;
                    ball1.right = false;
                    goto new_round;
                }
                score++;
            }
            if (ball1.x == player2.x - 1) {
                if(singleplayer){
                    ball1.direction = rand() % 4 - 2;
                    ball1.right = false;
                }
                else if(!singleplayer){
                    if(player2.y==ball1.y){
                        ball1.right = false;
                        ball1.direction = 0;
                    }
                    else if(player2.y==ball1.y+1){
                        ball1.right = false;
                        ball1.direction = -1;
                    }
                    else if(player2.y==ball1.y+2){
                        ball1.right = false;
                        ball1.direction = -2;
                    }
                    else if(player2.y==ball1.y-2){
                        ball1.right = false;
                        ball1.direction = 2;
                    }
                    else if(player2.y==ball1.y-1){
                        ball1.right = false;
                        ball1.direction = 1;
                    }
                    else{
                        player1.score++;
                        player1.x = start_x; player1.y = start_y;
                        player2.x = start_x+columns*3-4;player2.y = start_y;
                        ball1.x = (columns*3)/2 +3;ball1.y = start_y;
                        if(score>max_score)
                            max_score=score;
                        ball1.right = true;
                        goto new_round;
                    }
                    score++;

                }
            }

            //odraz od steny
            if(ball1.y==4){
                if(ball1.direction == -2)
                    ball1.direction = 2;
                if (ball1.direction == -1)
                    ball1.direction = 1;
            }
            if(ball1.y==3+rows){
                if(ball1.direction == 2)
                    ball1.direction = -2;
                if (ball1.direction == 1)
                    ball1.direction = -1;
            }

            mvprintw(rows+8,(columns*3)/2,"score: %d     ", score);
        }
        //vstup od hracov
        switch (getch()) {
            case 'q':
            case 'Q': {
                refresh();
                getchar();
                mvprintw(rows+9,(columns*3)/2,"Game has benn terminated with the top score of: %d",max_score);
                refresh();
                return max_score;
            }
            case 'u':
                update_ball(ball1);
                break;
            case 'w':
            case 'W':
                if(player1.y-width+2!=width+2){
                    player1 = update_player(true, player1);
                }
                break;

            case 's':
            case 'S':
                if(player1.y-width+2!=rows){
                    player1 = update_player(false, player1);
                 }
                break;
            case KEY_UP:
                if(player2.y-width+2!=width+2){
                    player2 = update_player(true, player2);
                }
                break;
            case KEY_DOWN:
                if(player2.y-width+2!=rows){
                    player2 = update_player(false, player2);
                }
                break;
        }
    if((!singleplayer && player2.score==win_score) || (player1.score==win_score)){
        end = true;
        mvprintw(rows+9,(columns*3)/2,"Congratulations! player %d won", player1.score > player2.score ? 1 : 2);
        refresh();
        }
    }

    return max_score;
}
