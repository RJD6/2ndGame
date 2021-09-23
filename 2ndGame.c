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
DEF_METASPRITE_2x2(majorEnemy, 0xe0, 0); //$07; actors[0]
DEF_METASPRITE_2x2(minorEnemy, 0xdc, 0); //$06; actors[1]
DEF_METASPRITE_2x2(player, 0xd8, 0); // $05; actors[2]
DEF_METASPRITE_2x2(bullet, 0xe4, 0); // $08

#define NUM_ACTOR_TYPES 4
// Actors' x/y positions
byte actor_x[NUM_ACTOR_TYPES];
byte actor_y[NUM_ACTOR_TYPES];
// Actors' delta x and delta y
sbyte actor_dx[NUM_ACTOR_TYPES];
sbyte actor_dy[NUM_ACTOR_TYPES];

// Place actors on screen
void setupActors(){
  // Major Enemy
  actor_x[0] = 80;
  actor_y[0] = 10;
  actor_dx[0] = 1;
  actor_dy[0] = 0;
  
  // Minor Enemy
  actor_x[1] = 80;
  actor_y[1] = 30;
  actor_dx[1] = 1;
  actor_dy[1] = 0;
  
  // Player
  actor_x[2] = 120;
  actor_y[2] = 190;
  actor_dx[2] = 0;
  actor_dy[2] = 0;
  
  // Bullets
  actor_x[3] = 0;
  actor_y[3] = 0;
  actor_dx[3] = 0;
  actor_dy[3] = -4;
};
void main(void)
{
  bool bullet_exists = false;
  char i; // Actor Index
  char oam_id; // Sprite ID
  
  setup_graphics(); // Setup graphics
  setupActors(); // Setup Actors
  
  // infinite loop
  while(1) {
    // Player Movement
    char pad = pad_poll(0);
    if (pad & PAD_LEFT && actor_x[2]>10){
      actor_dx[2]=-2;
    }
    else if (pad & PAD_RIGHT && actor_x[2]<230){
      actor_dx[2]=2;
    }
    else{
      actor_dx[2]=0;
    }
    // Player Attack
    if (pad & PAD_A && !bullet_exists){
      // Bullet must spawn in front of player
      actor_x[3] = actor_x[2];
      actor_y[3] = actor_y[2] - 12;
      oam_id = oam_meta_spr(actor_x[3], actor_y[3], oam_id, bullet);
      bullet_exists = true;
    }
    if (bullet_exists){
      // Bullet must fly forward
      actor_x[3] += actor_dx[3];
      actor_y[3] += actor_dy[3];
      oam_id = oam_meta_spr(actor_x[3], actor_y[3], oam_id, bullet);
      if (actor_y[3] < 1 || actor_y[3] > 190){
        bullet_exists = false;
      }
    }
    // Track movements for each actor type
    for(i=0; i<3; i++){
    actor_x[i] += actor_dx[i];
    actor_y[i] += actor_dy[i];
    }
    // Track actor positions
    for(i=0; i<9; i++){
      if(i==0 || i==1 || i==2 || i==3){
        oam_id = oam_meta_spr((actor_x[0]) + (20 * i), actor_y[0], oam_id, majorEnemy);
      }
      else if(i==4 || i==5 || i==6 || i==7){
        oam_id = oam_meta_spr(actor_x[1] + (20 * (i-4)), actor_y[1], oam_id, minorEnemy);
      }
      else if(i==8){
        // Player position and movement
        oam_id = oam_meta_spr(actor_x[2], actor_y[2], oam_id, player);
      }
    }
    
    // wait for next frame (sprites)
    ppu_wait_nmi();
  }
  
  // loop forever
  /*while (1) {
    // start with OAMid/sprite 0
    oam_id = 0;
    // set player 0/1 velocity based on controller
    for (i=0; i<2; i++) {
      // poll controller i (0-1)
      pad = pad_poll(i);
      // move actor[i] left/right
      if (pad&PAD_LEFT && actor_x[i]>0) actor_dx[i]=-2;
      else if (pad&PAD_RIGHT && actor_x[i]<240) actor_dx[i]=2;
      else actor_dx[i]=0;
      // move actor[i] up/down
      if (pad&PAD_UP && actor_y[i]>0) actor_dy[i]=-2;
      else if (pad&PAD_DOWN && actor_y[i]<212) actor_dy[i]=2;
      else actor_dy[i]=0;
    }
    
    // draw and move all actors
    for (i=0; i<NUM_ACTORS; i++) {
      byte runseq = actor_x[i] & 7;
      if (actor_dx[i] >= 0)
        runseq += 8;
      oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRunSeq[runseq]);
      actor_x[i] += actor_dx[i];
      actor_y[i] += actor_dy[i];
    }
    // hide rest of sprites
    // if we haven't wrapped oam_id around to 0
    if (oam_id!=0) oam_hide_rest(oam_id);
    */
}