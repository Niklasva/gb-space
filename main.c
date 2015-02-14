#include <gb/gb.h>
#include "tiles.c"
#include "map.c"
#include "sprites.c"

#define SCREEN_WIDTH 160
#define STARTING_POS_X 1
#define STARTING_POS_Y 70

#define PLAYER_SPRITE 0
#define BULLET_SPRITE 1
#define MAX_BULLETS 8

unsigned int player_x, player_y;

BOOLEAN bullet_active[MAX_BULLETS];
unsigned int bullet_x[MAX_BULLETS];
unsigned int bullet_y[MAX_BULLETS];
BOOLEAN shot;

BOOLEAN scroll;
BOOLEAN playing;

int i;

int scrolli;

void init_screen()
{
   // visa upp allt!
   SHOW_BKG;
   SHOW_SPRITES;
   DISPLAY_ON;
}
void init_intro()
{
   // init tiles intro
   set_bkg_data(0, 6, tile);
   set_bkg_tiles(0, 0, 32, 18, intro);
}

void init_game()
{
   DISPLAY_OFF;
   HIDE_BKG;
   // init tiles
   set_bkg_data(0, 6, tile);
   set_bkg_tiles(0, 0, 32, 32, map);

   // init sprites
   SPRITES_8x8;
   set_sprite_data(0, 7, sprites);
}

void init_player()
{
   set_sprite_tile(PLAYER_SPRITE, 0);
   player_x = 10;
   player_y = 70;
}

void init_bullets()
{
    for (i = 0; i < MAX_BULLETS; i++)
    {
        set_sprite_tile(BULLET_SPRITE + i, 1);
        bullet_active[i] = FALSE;

        bullet_x[i] = 0;
        bullet_y[i] = 0;
    }
}


void move_bkg_slow(int x, int y, int scrolltime)
{
    scrolli++;
    if (scrolli > scrolltime)
    {
        scroll_bkg(x, y);
        scrolli = 0;
    }
}

void scroll_bkg_slow()
{
    if (scroll)
        scroll_bkg(1, 0);
    scroll = !scroll;
}
void input()
{
    if (joypad() & J_UP && player_y)
    {
        if (player_y <= 16) // om spelaren försöker ta sig ut ur spelet
            player_y = 16; // se till att han inte flyr
        else
            player_y--; // annars flyttar vi bara på spelaren
    }
    else if (joypad() & J_DOWN)
    {
        if (player_y >= 150)
            player_y = 150; // flytta upp spelaren
        else
            player_y++;
    }

    if (joypad() & J_LEFT)
    {
        if (player_x <= 8)
            player_x = 8; // håll spelaren på plats
        else
            player_x--;
    }
    else if (joypad() & J_RIGHT)
    {
        if (player_x >= SCREEN_WIDTH)
            player_x = SCREEN_WIDTH; // håll spelaren på plats
        else
            player_x++;
    }


    if (joypad() & J_A && !shot)
    {
      for (i = 0; i < MAX_BULLETS; i++)
      {
         if (!bullet_active[i])
         {
            bullet_active[i] = TRUE;
            bullet_x[i] = player_x;
            bullet_y[i] = player_y;
            break;
         }
      }
      shot = TRUE;
   }
   else if (!(joypad() & J_A))
       shot = FALSE;

}

void move_bullets()
{
    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_active[i])
        {
            bullet_x[i] += 4;
            if (bullet_x[i] > SCREEN_WIDTH + 16)
                bullet_active[i] = FALSE;
        }
    }
}

void update_sprites()
{
    move_sprite(0, player_x, player_y);

    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_active[i])
        {
            move_sprite(BULLET_SPRITE + i, bullet_x[i], bullet_y[i]);
        }
    }
}

int main()
{
    init_intro();
    init_screen();

    // Update för startmenyn
    while (!playing)
    {
        wait_vbl_done();
        scroll_bkg_slow();

        if (joypad() & J_START)
            playing = TRUE;
    }

    // SPELAREN HAR TRYCKT PÅ START!! LADDA SPELET!!!
    init_game();
    init_player();
    init_bullets();
    init_screen();

    // Update för speldelen
    while(playing)
    {
        wait_vbl_done(); // Uppdatera endast när vblank är klar
        input();         // Hantera spelarens input
        update_sprites();
        move_bullets();
        scroll_bkg_slow();

        // Ändra scroll i y-led beroende på spelarens position på skärmen
        // Shit vad tufft det blir vem behöver parallax
        if (player_y < 60)
        {
            move_bkg_slow(0, -1, 2 + player_y/16);
        }
        if (player_y > 100)
        {
            move_bkg_slow(0, 1, 14 - player_y/16);
        }
    }
    return 0;
}
