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

#define NUM_ACTOR_TYPES 3
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
};
void main(void)
{
  char i; // Actor Index
  char oam_id; // Sprite ID
  
  setup_graphics();
  setupActors();
  
  // infinite loop
  while(1) {
    // Track movements for each actor type
    for(i=0; i<2; i++){
    actor_x[i] += actor_dx[i];
    actor_y[i] += actor_dy[i];
      }
    // Track enemy positions
    for(i=0; i<9; i++){
      if(i==0 || i==1 || i==2 || i==3){
        oam_id = oam_meta_spr(actor_x[0] + (20 * i), actor_y[0], oam_id, majorEnemy);
      }
      else if(i==4 || i==5 || i==6 || i==7){
        oam_id = oam_meta_spr(actor_x[1] + (20 * (i-4)), actor_y[1], oam_id, minorEnemy);
      }
      else if(i==8){
        oam_id = oam_meta_spr(actor_x[2], actor_y[2], oam_id, player);
      }
    }
    
    // wait for next frame
    ppu_wait_nmi();
  }
}
