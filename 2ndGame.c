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
extern const byte titlescreen_pal[16];
extern const byte titlescreen_rle[];
extern const byte background_pal[16];
extern const byte background_rle[];
//#link "titlescreen.s"
//#link "background.s"

struct Actor{
  char label[30]; // What kind of actor is it?
  byte x; // Current x-location
  byte y; // Current y-location
  sbyte dx; // Delta-x
  sbyte dy; // Delta-y
  bool is_alive; // Is the actor supposed to be 'alive' right now?
  byte lives; // How many lives does it have?
};

void fade_in() {
  byte vb;
  for (vb=0; vb<=4; vb++) {
    // set virtual bright value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}

void show_title_screen(const byte* pal, const byte* rle) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  pal_bright(0);
  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
  fade_in();
}

  

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0D,			// screen color

  0x14,0x24,0x34,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x16,0x35,0x24,0x00,	// sprite palette 0
  0x00,0x37,0x25,0x00,	// sprite palette 1
  0x0D,0x2D,0x3A,0x00,	// sprite palette 2
  0x0D,0x28,0x2A	// sprite palette 3
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
  
  show_title_screen(background_pal, background_rle);
  show_title_screen(titlescreen_pal, titlescreen_rle); 
while(1)
  
  // Establish Major Enemies
  for(i=0; i<2; i++){
    strcpy(major_enemies[i].label, "Major");
    major_enemies[i].x = 70 + (50 * i);
    major_enemies[i].y = 10;
    major_enemies[i].dx = 1;
    major_enemies[i].dy = 0;
    major_enemies[i].is_alive = true;
    major_enemies[i].lives = 1;
  }
  
  // Establish Minor Enemies
  for(i=0; i<2; i++){
    strcpy(minor_enemies[i].label, "Minor");
    minor_enemies[i].x = 70 + (50 * i);
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
    oam_id = 0;
    
    // Track movements for each actor type
    // Major Enemies
    for(i=0; i<2; i++){
      if(major_enemies[i].x > 220){ // Major enemies lower slower, but speed up on dx.
        major_enemies[i].y += 15;
        major_enemies[i].dx++;
        major_enemies[i].dx = -major_enemies[i].dx; 
      }
      else if(major_enemies[i].x < 10){
        major_enemies[i].y += 15;
        major_enemies[i].dx = -major_enemies[i].dx;
      }
      major_enemies[i].x += major_enemies[i].dx;
      major_enemies[i].y += major_enemies[i].dy;
    }
    // Minor Enemies
    for(i=0; i<2; i++){
      if(minor_enemies[i].x > 220){ // Minor enemies lower faster, but do not speed up on dx.
        minor_enemies[i].y += 40;
        minor_enemies[i].dx = -minor_enemies[i].dx;
      }
      else if(minor_enemies[i].x < 10){
        minor_enemies[i].y += 40;
        minor_enemies[i].dx = -minor_enemies[i].dx;
      }
      minor_enemies[i].x += minor_enemies[i].dx;
      minor_enemies[i].y += minor_enemies[i].dy;
    }
    // Player
    if (pad & PAD_LEFT && player.x>10){
      player.dx=-2;
      oam_clear();
    }
    else if (pad & PAD_RIGHT && player.x<230){
      player.dx=2;
      oam_clear();
    }
    else{
      player.dx=0;
    }
    
    // Player Attack
    if (pad & PAD_A && !bullet_exists){
      // Bullet must spawn in front of player
      bullet_player.x = player.x;
      bullet_player.y = player.y - 12;
      bullet_exists = true;
    }
    // If the player's bullet exists...
    if (bullet_exists){
      // Check for enemy collision
      for(i=0; i<2; i++){
        if(major_enemies[i].is_alive && abs(bullet_player.x - major_enemies[i].x) < 9 && abs(bullet_player.y - major_enemies[i].y) < 9){
          major_enemies[i].is_alive = false;
          bullet_exists = false;
          oam_clear();
          break;
        }
      }
      for(i=0; i<2; i++){
        if(minor_enemies[i].is_alive && abs(bullet_player.x - minor_enemies[i].x) < 9 && abs(bullet_player.y - minor_enemies[i].y) < 9){
          minor_enemies[i].is_alive = false;
          bullet_exists = false;
          oam_clear();
          break;
        }
      }
      // And the bullet must fly forward
      bullet_player.y += bullet_player.dy;
      oam_id = oam_meta_spr(bullet_player.x, bullet_player.y, oam_id, bullet);
      if (bullet_player.y < 1 || bullet_player.y > 190){
        bullet_exists = false;
      }
    }
    
    // Check for enemy collision with player
    // Major Enemies first
    for(i=0; i<2; i++){
      if(major_enemies[i].is_alive && abs(major_enemies[i].x - player.x) < 8 && abs(major_enemies[i].y - player.y) < 8){
        player.is_alive = false;
        oam_clear();
      }
    }
    // Then Minor Enemies
    for(i=0; i<2; i++){
      if(minor_enemies[i].is_alive && abs(minor_enemies[i].x - player.x) < 8 && abs(minor_enemies[i].y - player.y) < 8){
        player.is_alive = false;
        oam_clear();
      }
    }
    
    // Render actor positions
    // Major and Minor Enemies
    for(i=0; i<2; i++){
      if(major_enemies[i].is_alive){
        oam_id = oam_meta_spr(major_enemies[i].x, major_enemies[i].y, oam_id, majorEnemy);
      }
    }
    for(i=0; i<2; i++){
      if(minor_enemies[i].is_alive){
        oam_id = oam_meta_spr(minor_enemies[i].x, minor_enemies[i].y, oam_id, minorEnemy);
      }
    }
    // Player
    if(player.is_alive){
      player.x += player.dx;
      oam_id = oam_meta_spr(player.x, player.y, oam_id, Player);
    }
    else{
      // Stop all enemies when player loses.
      for(i=0; i<2; i++){
        major_enemies[i].dx = 0;
        major_enemies[i].dy = 0;
        minor_enemies[i].dx = 0;
        minor_enemies[i].dy = 0;
      }
      
      for(i=0; i<50; i++) {
        ppu_wait_frame();
      }
      
      ppu_wait_nmi();
      
      // write text to name table
      vram_adr(NTADR_A(11,11));		// set address
      vram_write("Game Over!", 10);	// write bytes to video RAM
    }
    
    // wait for next frame (sprites)
    ppu_wait_nmi();
  }
}