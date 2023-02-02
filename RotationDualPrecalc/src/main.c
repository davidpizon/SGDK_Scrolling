#include <genesis.h>
#include "resources.h"
#include "rotation.h"

/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
#define PLANE_MAX_TILE 64

s16 hScrollA[224];
s16 vScrollA[20];
s16 vScrollUpperA[20];
s16 vScrollLowerA[20];

s16 vScrollB[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
s16 planeBDeltas[20] = {9, 9, 7, 7, 5, 5, 5, 4, 3, 2, 2, 3, 4, 5, 5, 5, 7, 7, 9, 9};



/////////////////////////////////////////////////////////////////////
//  Interrupt handlers
//
static vu16  lineDisplay   = 0;             // line position on display screen

HINTERRUPT_CALLBACK HIntHandler()
{
  if( lineDisplay == 120  ) {
    // set vertical rotation component for lwoer part of BG_A
    memcpy( vScrollA, vScrollLowerA, sizeof(vScrollLowerA));
    VDP_setVerticalScrollTile(BG_A, 0, vScrollA, 20, DMA);
  }
  // Count raster lines
  lineDisplay++;

}
void VBlankHandler()
{
  // Reset to line 0
  lineDisplay = 0;

  // set vertical rotation component for upper part of BG_A
  memcpy( vScrollA, vScrollUpperA, sizeof(vScrollUpperA));
  VDP_setVerticalScrollTile(BG_A, 0, vScrollA, 20, DMA);
}






int main(bool hard)
{
  VDP_setScreenWidth320();
  // set colors
  PAL_setPalette( PAL0, planea_pal.data, CPU );
  PAL_setPalette( PAL1, crosshairs_pal.data, CPU );

  // set scrolling mode to LINE for horizontal and TILE for vertical
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

  // get tile positions in VRAM.
  int ind = TILE_USER_INDEX;
  int indexA = ind;
  // Load the plane tiles into VRAM
  VDP_loadTileSet(planea.tileset, ind, DMA);

  // setup the tiles
  VDP_setTileMapEx(BG_A, planea.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, indexA),
      0,               // Plane X destination
      0,               // plane Y destination
      0,               // Region X start position
      0,               // Region Y start position
      PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
      28,             // height
      CPU);


  u8 ticks = 0;

  s16 currUpperAngle = 0;
  s16 upperStepDir = 1;
  s16 xUpperOffset = 0;
  s16 yUpperOffset = 0;
  s16 yUpperOffsetDir = 1;

  s16 currLowerAngle = 0;
  s16 lowerStepDir = 1;
  s16 xLowerOffset = 0;
  s16 yLowerOffset = -10;
  s16 yLowerOffsetDir = 1;


  for( int row = 0; row < 224; ++row ) {
    hScrollA[row] = -40;
  }


  // Setup interrupt handlers
  SYS_disableInts();
  {
    SYS_setVBlankCallback(VBlankHandler);
    SYS_setHIntCallback(HIntHandler);
    VDP_setHIntCounter(0);
    VDP_setHInterrupt(1);
  }
  SYS_enableInts();

  // SPRITES
  SPR_init();


  Sprite * lgun_sprite = NULL;
  int lgun_pos_x = lgun[currUpperAngle*2]-8;
  int lgun_pos_y = lgun[currUpperAngle*2+1]-8;
  lgun_sprite = SPR_addSprite( &crosshairs, lgun_pos_x, lgun_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * mgun_sprite = NULL;
  int mgun_pos_x = mgun[currUpperAngle*2]-8;
  int mgun_pos_y = mgun[currUpperAngle*2+1]-8;
  mgun_sprite = SPR_addSprite( &crosshairs, mgun_pos_x, mgun_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * rgun_sprite = NULL;
  int rgun_pos_x = rgun[currUpperAngle*2]-8;
  int rgun_pos_y = rgun[currUpperAngle*2+1]-8;
  rgun_sprite = SPR_addSprite( &crosshairs, rgun_pos_x, rgun_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * lvent_sprite = NULL;
  int lvent_pos_x = lvent[currUpperAngle*2]-8;
  int lvent_pos_y = lvent[currUpperAngle*2+1]-8;
  lvent_sprite = SPR_addSprite( &crosshairs, lvent_pos_x, lvent_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * rvent_sprite = NULL;
  int rvent_pos_x = rvent[currUpperAngle*2]-8;
  int rvent_pos_y = rvent[currUpperAngle*2+1]-8;
  rvent_sprite = SPR_addSprite( &crosshairs, rvent_pos_x, rvent_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));



Sprite * larray_sprite = NULL;
  int larray_pos_x = larray[currLowerAngle*2]-8;
  int larray_pos_y = larray[currLowerAngle*2+1]-8;
  larray_sprite = SPR_addSprite( &crosshairs, larray_pos_x, larray_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * marray_sprite = NULL;
  int marray_pos_x = marray[currLowerAngle*2]-8;
  int marray_pos_y = marray[currLowerAngle*2+1]-8;
  marray_sprite = SPR_addSprite( &crosshairs, marray_pos_x, marray_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));

  Sprite * rarray_sprite = NULL;
  int rarray_pos_x = rarray[currLowerAngle*2]-8;
  int rarray_pos_y = rarray[currLowerAngle*2+1]-8;
  rarray_sprite = SPR_addSprite( &crosshairs, rarray_pos_x, rarray_pos_y,  TILE_ATTR( PAL1, 1, FALSE, FALSE ));


  while (TRUE)
  {

    ++ticks;
    if( ticks % 6 == 0 ) {
      currUpperAngle += upperStepDir;
      if( currUpperAngle >= lower_SCROLL_COUNT ) {
        upperStepDir = -1;
        currUpperAngle = 9;
      }else if (currUpperAngle <0 ) {
        upperStepDir = 1;
        currUpperAngle = 1;
      }
    }
    if( ticks % 3 == 0 ) {
      yUpperOffset += yUpperOffsetDir;
      if( yUpperOffset > 5) {
        yUpperOffsetDir = -1;
      }else if( yUpperOffset < -15 ) {
        yUpperOffsetDir = 1;
      }
      if( currUpperAngle < 4) {
        xUpperOffset+=2;
      } else if ( currUpperAngle > 6) {
        xUpperOffset-=2;
      }
    }

    if( ticks % 9  == 0 ) {
      currLowerAngle += lowerStepDir;
      if( currLowerAngle >= upper_SCROLL_COUNT ) {
        lowerStepDir = -1;
        currLowerAngle = 9;
      }else if (currLowerAngle < 0 ) {
        lowerStepDir = 1;
        currLowerAngle = 1;
      }
    }
    if( ticks % 12 == 0 ) {
      yLowerOffset += yLowerOffsetDir;
      if( yLowerOffset > -10) {
        yLowerOffsetDir = -1;
      }else if( yLowerOffset < -25 ) {
        yLowerOffsetDir = 1;
      }
      if( currLowerAngle < 4) {
        xLowerOffset+=1;
      } else if ( currLowerAngle > 6) {
        xLowerOffset-=1;
      }
    }


    // could unroll loops to eliminate some overhead
    s16 startUpperHScroll = upper_START_ROW_A - yUpperOffset;
    s16 stopUpperRows = upper_END_ROW_A - yUpperOffset;
    if( startUpperHScroll < 0 ) {
      stopUpperRows = upper_START_ROW_A + upper_ROWS_A + startUpperHScroll;
      startUpperHScroll = 0;
    }
    for(int i=startUpperHScroll, offset=0; i < stopUpperRows; ++i, ++offset ) {
      hScrollA[ i ] = upper_hScroll[ currUpperAngle * upper_ROWS_A + offset] + xUpperOffset;
    }

    s16 startLowerHScroll = lower_START_ROW_A - yLowerOffset;
    s16 stopLowerHScroll = lower_END_ROW_A - yLowerOffset;
    if( stopLowerHScroll > 223 ) {
      stopLowerHScroll =223;
    }
    for(int i=startLowerHScroll, offset=0; i <= stopLowerHScroll; ++i, ++offset ) {
      hScrollA[ i ] = lower_hScroll[ currLowerAngle * lower_ROWS_A + offset] + xLowerOffset;
    }


    //
    for (int i = 0; i < upper_COLS_A; ++i)
    {
      vScrollUpperA[i] = upper_vScroll[currUpperAngle * upper_COLS_A + i] + yUpperOffset;
    }
    for (int i = 0; i < lower_COLS_A; ++i)
    {
      vScrollLowerA[i] = lower_vScroll[currLowerAngle * lower_COLS_A + i] + yLowerOffset;
    }



    // set SGDK scrolling functions to fake the rotation.
    VDP_setHorizontalScrollLine(BG_A, 0, hScrollA, 224, DMA);


    lgun_pos_x = lgun[currUpperAngle * 2]-8 + xUpperOffset;
    lgun_pos_y = lgun[currUpperAngle * 2 + 1]-8 - yUpperOffset;
    SPR_setPosition(lgun_sprite, lgun_pos_x, lgun_pos_y);

    mgun_pos_x = mgun[currUpperAngle * 2]-8 + xUpperOffset;
    mgun_pos_y = mgun[currUpperAngle * 2 + 1]-8 - yUpperOffset;
    SPR_setPosition(mgun_sprite, mgun_pos_x, mgun_pos_y);

    rgun_pos_x = rgun[currUpperAngle * 2]-8 + xUpperOffset;
    rgun_pos_y = rgun[currUpperAngle * 2 + 1]-8 - yUpperOffset;
    SPR_setPosition(rgun_sprite, rgun_pos_x, rgun_pos_y);

    lvent_pos_x = lvent[currUpperAngle * 2]-8 + xUpperOffset;
    lvent_pos_y = lvent[currUpperAngle * 2 + 1]-8 - yUpperOffset;
    SPR_setPosition(lvent_sprite, lvent_pos_x, lvent_pos_y);

    rvent_pos_x = rvent[currUpperAngle * 2]-8 + xUpperOffset;
    rvent_pos_y = rvent[currUpperAngle * 2 + 1]-8 - yUpperOffset;
    SPR_setPosition(rvent_sprite, rvent_pos_x, rvent_pos_y);



  larray_pos_x = larray[currLowerAngle * 2]-8 + xLowerOffset;
    larray_pos_y = larray[currLowerAngle * 2 + 1]-8 - yLowerOffset;
    SPR_setPosition(larray_sprite, larray_pos_x, larray_pos_y);

    marray_pos_x = marray[currLowerAngle * 2]-8 + xLowerOffset;
    marray_pos_y = marray[currLowerAngle * 2 + 1]-8 - yLowerOffset;
    SPR_setPosition(marray_sprite, marray_pos_x, marray_pos_y);

    rarray_pos_x = rarray[currLowerAngle * 2]-8 + xLowerOffset;
    rarray_pos_y = rarray[currLowerAngle * 2 + 1]-8 - yLowerOffset;
    SPR_setPosition(rarray_sprite, rarray_pos_x, rarray_pos_y);



    SPR_update();





    // let SGDK do its thing
    SYS_doVBlankProcess();
  }
  return 0;
}
