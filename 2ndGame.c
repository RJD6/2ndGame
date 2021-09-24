#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

struct Actor{
  char label[30]; // What kind of actor is it?
  byte x; // Current x-location
  byte y; // Current y-location
  sbyte dx; // Delta-x
  sbyte dy; // Delta-y
  bool is_alive; // Is the actor supposed to be 'alive' right now?
  byte lives; // How many lives does it have?
};

// Wait frame to prevent text break or flickering
// A wait frame does 5 frames per call so...
// 50 frames = 1 sec
void ppuPause(){
  ppu_wait_frame();
  ppu_wait_frame();
  ppu_wait_frame();
  ppu_wait_frame();
  ppu_wait_frame();
}

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0D,			// screen color

  0x1E,0x30,0x2D,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x16,0x35,0x24,0x00,	// sprite palette 0
  0x00,0x37,0x25,0x00,	// sprite palette 1
  0x0D,0x2D,0x3A,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
  // enable rendering
  ppu_on_all();
}

// Metasprite Stuff
// define a 2x2 metasprite
#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        0,      8,      (code)+1,   pal, \
        8,      0,      (code)+2,   pal, \
        8,      8,      (code)+3,   pal, \
        128};

// Make metasprite instances
DEF_METASPRITE_2x2(majorEnemy, 0xe0, 0); //$07
DEF_METASPRITE_2x2(minorEnemy, 0xdc, 0); //$06
DEF_METASPRITE_2x2(Player, 0xd8, 0); // $05; Capitalized to prevent confusion with player struct
DEF_METASPRITE_2x2(bullet, 0xe4, 0); // $08

void main(void)
{
  char i; // Actor Index
  char oam_id; // Sprite ID
  bool bullet_exists = false; // Is the player's shot currently traveling?
  struct Actor major_enemies[4];
  struct Actor minor_enemies[4];
  struct Actor player;
  struct Actor bullet_player;

  
  // Establish Major Enemies
  for(i=0; i<4; i++){
    strcpy(major_enemies[i].label, "Major");
    major_enemies[i].x = 80 + (20 * i);
    major_enemies[i].y = 10;
    major_enemies[i].dx = 1;
    major_enemies[i].dy = 0;
    major_enemies[i].is_alive = true;
    major_enemies[i].lives = 1;
  }
  
  // Establish Minor Enemies
  for(i=0; i<4; i++){
    strcpy(minor_enemies[i].label, "Minor");
    minor_enemies[i].x = 80 + (20 * i);
    minor_enemies[i].y = 30;
    minor_enemies[i].dx = 1;
    minor_enemies[i].dy = 0;
    minor_enemies[i].is_alive = true;
    minor_enemies[i].lives = 1;
  }
  
  // Establish Player
  strcpy(player.label, "Player");
  player.x = 120;
  player.y = 190;
  player.dx = 0;
  player.dy = 0;
  player.is_alive = true;
  player.lives = 3;
  
  // Establish Player Bullet
  strcpy(bullet_player.label, "Bullet-Player");
  bullet_player.x = 0;
  bullet_player.y = 0;
  bullet_player.dx = 0;
  bullet_player.dy = -4;
  
  setup_graphics(); // Setup graphics
  
  // infinite loop
  while(1) {
    char pad = pad_poll(0);
    
    // Track movements for each actor type
    // Major and Minor Enemies
    for(i=0; i<4; i++){
      major_enemies[i].x += major_enemies[i].dx;
      major_enemies[i].y += major_enemies[i].dy;
      minor_enemies[i].x += minor_enemies[i].dx;
      minor_enemies[i].y += minor_enemies[i].dy;
    }
    // Player
    if (pad & PAD_LEFT && player.x>10){
      player.dx=-2;
      
    }
    else if (pad & PAD_RIGHT && player.x<230){
      player.dx=2;
    }
    else{
      player.dx=0;
    }
    // Player Attack
    if (pad & PAD_A && !bullet_exists){
      // Bullet must spawn in front of player
      bullet_player.x = player.x;
      bullet_player.y = player.y - 12;
      oam_id = oam_meta_spr(bullet_player.x, bullet_player.y, oam_id, bullet);
      bullet_exists = true;
    }
    if (bullet_exists){
      // Bullet must fly forward
      bullet_player.x += bullet_player.dx;
      bullet_player.y += bullet_player.dy;
      oam_id = oam_meta_spr(bullet_player.x, bullet_player.y, oam_id, bullet);
      if (bullet_player.y < 1 || bullet_player.y > 190){
        bullet_exists = false;
      }
    }
    
    // Track actor positions
    // Major and Minor Enemies
    for(i=0; i<4; i++){
      if(major_enemies[i].is_alive){
        oam_id = oam_meta_spr(major_enemies[i].x, major_enemies[i].y, oam_id, majorEnemy);
      }
      if(minor_enemies[i].is_alive){
        oam_id = oam_meta_spr(minor_enemies[i].x, minor_enemies[i].y, oam_id, minorEnemy);
      }
    }
    // Player
    if(player.is_alive){
      player.x += player.dx;
      player.y += player.dy;
      oam_id = oam_meta_spr(player.x, player.y, oam_id, Player);
    }
    
    // wait for next frame (sprites)
    ppu_wait_nmi();
  }
}