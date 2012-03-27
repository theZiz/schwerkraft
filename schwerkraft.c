#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>
#include "menu.h"
#include "font.h"

SDL_Surface* screen;

void draw_game(void)
{
  spResetZBuffer();
  spClearTarget(3);
  spIdentity();

  drawLevel();

  spSetZSet(0);
  spSetZTest(0);
  
  spFlip();
}

int calc_game(Uint32 steps)
{
  if (spGetInput()->button[SP_BUTTON_START])
  {
    spGetInput()->button[SP_BUTTON_START] = 0;
    return 1;
  }
  return calcLevel(steps);
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
  
  init_menu_stuff();
  initLevel();
  int result = 1;
  while (result)
  {
    result = spLoop(draw_menu,calc_menu,10,resize);
    switch (result)
    {
      case 1:
        createRandomLevel(0);
        result = spLoop(draw_game,calc_game,10,resize);
        break;
      case 2:
        createRandomLevel(1);
        result = spLoop(draw_game,calc_game,10,resize);
        break;
      case 4:
        spLoop(draw_about,calc_about,10,resize);
        break;
      case 5:
        result = 0;
        break;
    }
  }
  quitLevel();
  quit_menu_stuff();
  quitFont();
  
  spQuitCore();
  return 0;
}
