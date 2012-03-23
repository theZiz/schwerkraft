#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>
#include "font.h"

SDL_Surface* screen;

void draw_function(void)
{
  spResetZBuffer();
  spClearTarget(3);
  spIdentity();

  drawLevel();

  spSetZSet(0);
  spSetZTest(0);
  
  spFlip();
}

int calc_function(Uint32 steps)
{
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  calcLevel(steps);
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
  reloadFont();
}


int main(int argc, char **argv)
{
  spInitCore();
  
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  spSetAlphaTest(1);
  spSetLight(1);
  
  initLevel();
  createRandomLevel();
  spLoop(draw_function,calc_function,10,resize);
  quitLevel();
  
  quitFont();
  
  spQuitCore();
  return 0;
}
