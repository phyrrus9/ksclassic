/*
 Key Seeker Classic Copyright © 2012 Ethan Laur (phyrrus9) <phyrrus9@gmail.com>
 compile with $ cc main.c -lncurses -o ksclassic
 */

#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define HEIGHT 10
#define WIDTH 10
#define LEVELS 3

char map[LEVELS][WIDTH][HEIGHT] =
{
    /*
     defining a new map is really simple, just take this format:
     {
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
     }
     use the following characters:
     k = key
     c = coin
     a = atk bonus
     d = def bonus
     t = teleport
     z = zombie
     NEVER USE SLOT 0!
     Make sure you adjust the LEVELS macro as well.
     */
    {
        {'~', '~', 'k', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', 'c', '~', '~', '~', 'd', '~', '~'},
        {'~', 'z', '~', '~', '~', 'a', '~', '~', '~', '~'},
        {'~', '~', '~', '~', 'd', '~', '~', 'a', '~', '~'},
        {'~', 'k', 'k', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'d', '~', '~', 'a', 'z', 'k', 'k', 'k', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', 'z', '~', '~', 'c', '~', 'c', 'z', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', 't'},
    },
    {
        {'~', '~', '~', 'c', '~', '~', '~', 'd', '~', '~'},
        {'~', 'z', '~', '~', '~', 'a', '~', '~', '~', '~'},
        {'~', 'k', 'k', '~', '~', '~', '~', 'z', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'d', '~', '~', 'a', '~', 'k', 'k', 'k', '~', '~'},
        {'~', '~', 'k', '~', '~', '~', '~', '~', '~', '~'},
        {'~', 'z', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', 'c', '~', 'c', '~', '~'},
        {'~', '~', '~', '~', 'd', 'z', '~', 'a', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', 't'},
    },
    {
        {'~', '~', '~', 'c', '~', '~', '~', 'd', '~', '~'},
        {'~', '~', '~', '~', '~', 'a', '~', '~', '~', '~'},
        {'~', 'k', 'k', 'z', '~', '~', '~', 'z', '~', '~'},
        {'z', '~', '~', '~', '~', '~', '~', '~', '~', 't'},
        {'~', '~', '~', '~', 'd', '~', '~', 'a', '~', '~'},
        {'~', '~', 'z', '~', '~', '~', '~', '~', '~', '~'},
        {'d', '~', '~', 'a', '~', 'k', 'k', 'k', '~', '~'},
        {'~', '~', 'k', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', '~', '~', '~', '~', '~'},
        {'~', '~', '~', '~', '~', 'c', '~', 'c', '~', '~'},
    }
};

/*begin global definitions*/
WINDOW *s;
WINDOW *d;
WINDOW *o;
WINDOW *m;

int display_num = 0;
char displaystr[3][50] = { "","",""};
/*end global definitions*/

struct player
{
    int x, y;
    int health;
    int lives;
    int score;
    int keys;
    int coins;
    int atk;
    int def;
    int level;
};

struct xy
{
    int x, y;
};

typedef enum { N, S, E, W } direction;

void push_display_string(const char * str)
{
    char * ostr = (char *)malloc(strlen(str) + 15);
    sprintf(ostr, "[%d] %s", display_num, str);
    strcpy(displaystr[0], displaystr[1]);
    strcpy(displaystr[1], displaystr[2]);
    strcpy(displaystr[2], ostr);
    display_num++;
}

void player_init(struct player *t)
{
    t->x = t->y = 0;
    t->health = 100;
    t->lives = 3;
    t->score = 0;
    t->keys = 0;
    t->coins = 0;
    t->atk = 1;
    t->def = 1;
    t->level = 0;
}

void display(struct player p)
{
    wclear(s);
    wclear(d);
    wclear(o);
    wprintw(s, "Health: %3d Lives: %2d Score: %4d Keys: %3d Coins: %3d ATK: %2d DEF: %2d X: %2d y: %2d", p.health, p.lives, p.score, p.keys, p.coins, p.atk, p.def, p.x, p.y);
    wprintw(d, "%s\n%s\n%s\n", displaystr[0], displaystr[1], displaystr[2]);
    wrefresh(s);
    wrefresh(d);
    wrefresh(o);
}

void display_help_screen(void)
{
    wclear(s);
    wclear(d);
    wclear(o);
    wrefresh(s);
    wrefresh(d);
    wrefresh(o);
    WINDOW *h = newwin(15, 105, 0, 0);
    wprintw(h,
            "Key Seeker Classic: Help\n"
            "This game is really nothing new, but more just a rebuild of KS4200 (2010) with a few new"
            "aditions to it.\n"
            "Commands are really simple and easy to get there are four modes real, move, use, and option\n"
            "Real mode has no real purpose, except to parse all of the other modes, so moving on\n"
            "Move mode is just that, it allows your character to move around the screen freely. To use\n"
            "move mode, just use w/a/s/d, and watch the output section for any notes or errors\n"
            "Use mode is what you will use to pick things up and attack zombies. It is also the mode to\n"
            "use for killing zombies\n"
            "Option mode is what you will use to save(s), restore(r), and quit the game.\n"
            "Here are the keys for using the game:\n"
            "m - enter move mode\n"
            "u - enter use mode\n"
            "o - enter option mode\n"
            "h - show help\n"
            "escape - enter real mode\n"
            "q - quit\n"
            "Press any key to continue...");
    wrefresh(h);
    getch();
    wclear(h);
    wrefresh(h);
    delwin(h);
}

void die(struct player p)
{
    display(p);
    push_display_string("You died!");
    getch();
    endwin();
    exit(1);
}

struct xy get_new_position_from_direction(direction d, struct player *p)
{
    int x = p->x;
    int y = p->y;
    struct xy pt;
    switch (d)
    {
        case N:
            y = p->y - 1;
            break;
        case S:
            y = p->y + 1;
            break;
        case E:
            x = p->x + 1;
            break;
        case W:
            x = p->x - 1;
            break;
    }
    pt.x = x;
    pt.y = y;
    return pt;
}

void print_savecode(struct player p)
{
    wclear(o);
    wrefresh(o);
    wprintw(o, "Save code: %d:%d:%d:%d:%d:%d:%d:%d:%d:%d;\n", p.x, p.y, p.health, p.lives, p.score, p.keys, p.coins, p.atk, p.def, p.level);
    wrefresh(o);
    getch();
}

void restore_savecode(struct player *p)
{
    wclear(o);
    wrefresh(o);
    wprintw(o, "Paste the savecode here, you will not see anything until you press enter.\n");
    wrefresh(o);
    scanf("%d:%d:%d:%d:%d:%d:%d:%d:%d:%d;", &p->x, &p->y, &p->health, &p->lives, &p->score, &p->keys, &p->coins, &p->atk, &p->def, &p->level);
    wrefresh(o);
    getch();
    push_display_string("Game restored!");
}

void do_zombie_damage(direction d, struct player *p)
{
    int x, y;
    struct xy pt = get_new_position_from_direction(d, p);
    x = pt.x;
    y = pt.y;
    push_display_string("Zombie attack!");
    if (p->atk > (y + x) / 2)
    {
        p->score += (x + y) / 2;
    }
    else
    {
        p->health -= (x + y - p->def) / 3;
    }
}

void player_move(direction d, struct player *p)
{
    int x, y;
    struct xy pt = get_new_position_from_direction(d, p);
    x = pt.x;
    y = pt.y;
    if ((x < 10 && y < 10) && (x >= 0 && y >= 0))
    {
        if (map[p->level][y][x] != '~')
        {
            if (map[p->level][y][x] == 'z')
            {
                push_display_string("Zombie!!!");
                p->health -= x + y;
                if (p->health <= 0)
                {
                    p->lives--;
                    if (p->lives <= 0)
                    {
                        die(*p);
                    }
                    p->health = 100;
                }
            }
            push_display_string("There is something here!");
        }
        else
        {
            //push_display_string("Move successful!");
            p->x = x;
            p->y = y;
        }
    }
    else
    {
        push_display_string("You found a wall!");
    }
}

void player_use(direction d, struct player *p)
{
    int x, y;
    struct xy pt = get_new_position_from_direction(d, p);
    x = pt.x;
    y = pt.y;
    char teleport = 0;
    if ((x < 10 && y < 10) && (x >= 0 && y >= 0))
    {
        //block
        {
            switch (map[p->level][y][x])
            {
                    case '~':
                        push_display_string("Nothing to use");
                        break;
                    case 'c':
                        push_display_string("Coin added to inventory");
                        p->coins++;
                        p->score += 5;
                        break;
                    case 'k':
                        push_display_string("Key added to inventory");
                        p->keys++;
                        p->score += 10;
                        break;
                    case 'a':
                        push_display_string("Attack increase!");
                        p->atk++;
                        p->score += 2;
                        break;
                    case 'd':
                        push_display_string("Defense increase!");
                        p->def++;
                        p->score += 2;
                        break;
                    case 'z':
                        do_zombie_damage(d, p);
                        break;
                    case 't':
                        push_display_string("Teleport!");
                        p->level++;
                        if (p->level >= LEVELS)
                        {
                            push_display_string("Game Over!");
                            p->level--;
                            die(*p);
                        }
                        p->score += 35;
                        teleport = 1;
                        p->x = 0;
                        p->y = 0;
                    break;
            }
            if (!teleport)
                map[p->level][y][x] = '~';
        }
    }
    else
    {
        push_display_string("Error! Outside range!");
    }
}

void player_look(direction d, struct player *p)
{
    int x, y;
    struct xy pt = get_new_position_from_direction(d, p);
    x = pt.x;
    y = pt.y;
    if ((x < 10 && y < 10) && (x >= 0 && y >= 0))
    {
        //block
        {
            switch (map[p->level][y][x])
            {
                case '~':
                    push_display_string("You see nothing");
                    break;
                case 'c':
                    push_display_string("You see a coin!");
                    p->coins++;
                    p->score += 5;
                    break;
                case 'k':
                    push_display_string("You see a key!");
                    p->keys++;
                    p->score += 10;
                    break;
                case 'a':
                    push_display_string("You see a suprise!");
                    break;
                case 'd':
                    push_display_string("You see a suprise!");
                    break;
                case 'z':
                    push_display_string("You see a zombie!");
                    break;
                case 't':
                    push_display_string("You see the teleport!");
                    break;
            }
        }
    }
    else
    {
        push_display_string("Error! Outside range!");
    }
}

int main(int argc, const char * argv[])
{
    void (*module)(direction,struct player *);
    struct player p;
    player_init(&p);
    initscr();
    refresh();
    noecho();
    if (COLS < 135)
    {
        endwin();
        printf("Error! Terminal not wide enough\n");
        exit(-1);
    }
    s = newwin(1, 105, 0, 0);
    d = newwin(3, 105, 1, 0);
    o = newwin(6, 105, 4, 0);
    m = newwin(1, 15, 0, 105);
    int c = 0;
    display_help_screen();
    while (c != 'q')
    {
        display(p);
        wclear(m);
        wrefresh(m);
        c = getch();
        char enable_module = 0;
        char options_mode = 0;
        if (c == 'm')
        {
            wprintw(m, "move mode");
            module = player_move;
            enable_module = 1;
        }
        if (c == 'u')
        {
            wprintw(m, "use mode");
            module = player_use;
            enable_module = 1;
        }
        if (c == 'l')
        {
            wprintw(m, "look mode");
            module = player_look;
            enable_module = 1;
        }
        if (c == 'o')
        {
            wprintw(m, "options mode");
            options_mode = 1;
        }
        if (c == 'h')
        {
            display_help_screen();
        }
        wrefresh(m);
        if (c == 'q')
        {
            continue;
        }
        while (enable_module)
        {
            display(p);
            c = getch();
            if (c == 27)
            {
                enable_module = 0;
            }
            switch (c)
            {
                case 'w':
                    module(N, &p);
                    break;
                case 'a':
                    module(W, &p);
                    break;
                case 's':
                    module(S, &p);
                    break;
                case 'd':
                    module(E, &p);
                    break;
            }
            if (module == player_use)
            {
                enable_module = 0; //use one time
            }
        }
        while (options_mode)
        {
            display(p);
            wprintw(o,  "Options:\n"
                        "a. quit game\n"
                        "s. save game\n"
                        "r. restore game\n");
            wrefresh(o);
            c = getch();
            if (c == 27)
            {
                options_mode = 0;
            }
            switch (c)
            {
                    case 'a':
                    endwin();
                    exit(2);
                    break;
                    case 's':
                    print_savecode(p);
                    break;
                    case 'r':
                    restore_savecode(&p);
                    break;
            }
        }
    }
    endwin();
}