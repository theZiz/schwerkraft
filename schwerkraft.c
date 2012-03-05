#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* planet_texture[10];
spModelPointer planet[10];
Sint32 rotation = 0;

void draw_function(void)
{
  spResetZBuffer();
  spClearTarget(0);
  spIdentity();
  
  spBindTexture(planet_texture[0]);
  
  spTranslate(0,0,-8<<SP_ACCURACY);
  int i;
  for (i = 0; i < 10; i++)
  {
    Sint32 matrix[16];
    memcpy(matrix,spGetMatrix(),64);
    spRotateZ(rotation+i*SP_PI/5);
    Sint32 c = abs(spCos(rotation+i*SP_PI/5));
    spTranslate((5<<SP_ACCURACY-1)+c,0,0);
    spRotateX(rotation);
    spRotateY(rotation);
    spRotateZ(rotation);
    spMesh3D(planet[i],0);
    memcpy(spGetMatrix(),matrix,64);
  }
  spFlip();
}


int calc_function(Uint32 steps)
{
  rotation+=steps*32;
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
}


int main(int argc, char **argv)
{
  spInitCore();
  
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  int i;
  for (i = 0; i < 10;i++)
  {
    char buffer[256];
    sprintf(buffer,"./data/planet%02i.png",i+1);
    printf("Loading %s\n",buffer);
    planet_texture[i] = spLoadSurface(buffer);
    planet[i] = spMeshLoadObjSize("./data/planet.obj",planet_texture[i],65535,3<<SP_ACCURACY-2);
  }
    
  spSetAlphaTest(0);
  spSetLight(1);
  
  spLoop(draw_function,calc_function,10,resize);
  
  for (i = 0; i < 10;i++)
  {
    spMeshDelete(planet[i]);
    SDL_FreeSurface(planet_texture[i]);
  }
  spQuitCore();
  return 0;
}
