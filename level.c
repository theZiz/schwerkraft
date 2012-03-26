/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License"); you may not use this file except in           
 compliance with the License. You may obtain a copy of the License at       
 http://www.mozilla.org/MPL/                                                
                                                                            
 Software distributed under the License is distributed on an "AS IS"        
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the    
 License for the specific language governing rights and limitations         
 under the License.                                                         
                                                                            
 Alternatively, the contents of this file may be used under the terms       
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the  
 provisions of LGPL License are applicable instead of those                  
 above.                                                                     
                                                                            
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/

#include "level.h"
#include <fcntl.h>
#include <SDL.h>
#include <SDL_image.h>
#include "font.h"
#include "trace.h"

#define STAR_COUNT 200
#define BULLET_ACCURACY 11

SDL_Surface* planet_texture[10];
spModelPointer ship[2];
spModelPointer gun;
spModelPointer bullet;
Sint32 levelTime = 0;
tLevel level;
Sint32 momPlayer = 0;
Sint32 game_mode = 0; //0 targeting, 1 shooting
Sint32 countdown = 20000; //20 seconds for targeting and 20 seconds of flying throw space
SDL_Surface* starSurface[5];
struct {Sint32 x,y,kind;} star[STAR_COUNT];

void initLevel()
{
  int rfd = open("/dev/urandom", O_RDONLY);
  int d;
  read(rfd,&d,sizeof(d));
  srand(d);
  close(rfd);
  
  int i;
  for (i = 0; i < 10;i++)
  {
    char buffer[256];
    sprintf(buffer,"./data/planet%02i.png",i+1);
    printf("Loading %s\n",buffer);
    planet_texture[i] = spLoadSurface(buffer);
  }
  for (i = 0; i < 5;i++)
  {
    char buffer[256];
    sprintf(buffer,"./data/star%i.png",i+1);
    printf("Loading %s\n",buffer);
    starSurface[i] = IMG_Load(buffer);
  }
  ship[0] = spMeshLoadObjSize("./data/ship.obj",NULL,12345,1<<SP_ACCURACY-3);
  ship[1] = spMeshLoadObjSize("./data/ship.obj",NULL,23456,1<<SP_ACCURACY-3);
  //Mirroring ship[1]
  for (i = 0; i < ship[1]->pointCount; i++)
    ship[1]->point[i].x = -ship[1]->point[i].x;
  gun = spMeshLoadObjSize("./data/gun.obj",NULL,65535,1<<SP_ACCURACY-4);
  bullet = spMeshLoadObj("./data/bullet.obj",NULL,65535);
}

#define MIN_PLANETS 1
#define MAX_PLANETS 5

void createRandomLevel()
{
  deleteLevel();
  deleteAllTracePoints();
  level.width = rand()%(1<<SP_ACCURACY)+(1<<SP_ACCURACY); //width between 1 and 2
  level.firstPlanet = NULL;
  //Creating random planets
  int planet_count = rand()%(MAX_PLANETS+1-MIN_PLANETS)+MIN_PLANETS;
  int i;
  for (i = 0; i < planet_count; i++)
  {
    pPlanet planet = (pPlanet)malloc(sizeof(tPlanet));
    planet->radius = rand()%(1<<SP_ACCURACY-1)+(1<<SP_ACCURACY-4);
    int too_near = 1;
    int count = 0;
    while (too_near)
    {
      count++;
      if (count > 100)
        break;
      planet->x = rand()%(level.width*2-planet->radius*2+1)-level.width+planet->radius;
      planet->y = rand()%((1<<SP_ACCURACY)*2-planet->radius*2+1)-(1<<SP_ACCURACY)+planet->radius;
      too_near = 0;
      pPlanet other = level.firstPlanet;
      while (other)
      {
        if ((planet->x-other->x >> (SP_ACCURACY-12))*(planet->x-other->x >> (SP_ACCURACY-12)) +
            (planet->y-other->y >> (SP_ACCURACY-12))*(planet->y-other->y >> (SP_ACCURACY-12)) <
            (planet->radius+other->radius >> (SP_ACCURACY-13))*(planet->radius+other->radius >> (SP_ACCURACY-13)))
        too_near = 1;
        other = other->next;
      }
    }
    if (count > 100)
    {
      printf("Finished infinite loop\n");
      free(planet);
      continue;
    }
    planet->mass = rand()%(spMul(planet->radius*planet->radius,planet->radius));
    planet->kind = rand()%2;
    if (planet->kind == PLANET_NORMAL)
      planet->mesh = spMeshLoadObjSize("./data/planet.obj",planet_texture[rand()%6],65535,planet->radius);
    else
      planet->mesh = spMeshLoadObjSize("./data/planet.obj",planet_texture[rand()%4+6],65535,planet->radius);
    planet->rx = 0;
    planet->ry = 0;
    planet->rz = 0;
    planet->dx = rand()%33-16<<(SP_ACCURACY-16);
    planet->dy = rand()%33-16<<(SP_ACCURACY-16);
    planet->dz = rand()%33-16<<(SP_ACCURACY-16);
    planet->satellite = 0;
    planet->next = level.firstPlanet;
    level.firstPlanet = planet;
    
  }
  
  //Setting random positions for the ships
  level.ship[0].y = rand()%((1<<SP_ACCURACY+1)+1-(1<<SP_ACCURACY-1))-(1<<SP_ACCURACY)+(1<<SP_ACCURACY-2); //position between -1 and 1
  level.ship[1].y = rand()%((1<<SP_ACCURACY+1)+1-(1<<SP_ACCURACY-1))-(1<<SP_ACCURACY)+(1<<SP_ACCURACY-2); //position between -1 and 1
  level.ship[0].direction =   SP_PI/2;
  level.ship[1].direction = 3*SP_PI/2;
  level.ship[0].energy = 1<<SP_ACCURACY;
  level.ship[1].energy = 1<<SP_ACCURACY;

  for (i = 0; i < STAR_COUNT; i++)
  {
    star[i].x = rand()%(level.width*6)-level.width*3;
    star[i].y = rand()%(6<<SP_ACCURACY)-(3<<SP_ACCURACY);
    switch (rand()%10)
    {
      case 0:case 1:case 2:case 3:
        star[i].kind = 4;
        break;
      case 4:case 5:case 6:
        star[i].kind = 3;
        break;
      case 7:case 8:
        star[i].kind = 2;
        break;
      case 9:
        star[i].kind = 1;
        break;
    }
  }
  level.bullet.x = 0;
  level.bullet.y = 0;
  level.bullet.dx = 0;
  level.bullet.dy = 0;

  momPlayer = 0;
  countdown = 20000;
  game_mode = 0;
}

void deleteLevel()
{
  pPlanet planet = level.firstPlanet;
  while (planet != NULL)
  {
    spMeshDelete(planet->mesh);
    pPlanet next = planet->next;
    free(planet);
    planet = next;
  }
  level.firstPlanet = NULL;
}

void drawLevel()
{
  SDL_Surface *screen = spGetWindowSurface();

  spSetZSet(0);
  spSetZTest(0);
  
  
  Sint32 matrix[16];
  Sint32 *project = spGetProjectionMatrix();
  
  Sint32 z_width = spDiv(spMul(project[0],level.width+(6<<SP_ACCURACY-4)),project[11]);
  Sint32 z_height = spDiv(spMul(project[5],(1<<SP_ACCURACY)),project[11]);
  Sint32 z_min = spMin(z_width,z_height);
  if (game_mode == 1)
  {
    Sint32 z_bulletx = spDiv(spMul(project[0],abs(level.bullet.x>>BULLET_ACCURACY)),project[11])-(1<<SP_ACCURACY-2);
    z_min = spMin(z_min,z_bulletx);
    Sint32 z_bullety = spDiv(spMul(project[5],abs(level.bullet.y>>BULLET_ACCURACY)),project[11])-(1<<SP_ACCURACY-2);
    z_min = spMin(z_min,z_bullety);
  }
  
  
  spTranslate(0,0,z_min);

  int i;
  for (i = 0; i < 100;i++)
    spBlit3D(star[i].x,star[i].y,-1,starSurface[star[i].kind]);

  drawTracePoints();

  spSetZSet(1);
  spSetZTest(1);

  //Game Rectangle
  spRectangleBorder3D(0,0,0,level.width*4,4<<SP_ACCURACY,1<<SP_ACCURACY-5,1<<SP_ACCURACY-5,spGetHSV(0,0,64));

  char buffer[256];

  //Drawing the left ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(-level.width-(1<<SP_ACCURACY-2),level.ship[0].y,0);
  spRotateY(SP_PI/2);
  spRotateX(level.ship[0].direction-SP_PI/2);
  spMesh3D(gun,0);
  if (game_mode == 0 && momPlayer == 0)
  {
    spSetZSet(0);
    spSetZTest(0);
    spRotateY(-SP_PI/2);
    spQuad3D((1<<SP_ACCURACY-4)+0,+1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+0,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[0].energy/16,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[0].energy/16,+1<<SP_ACCURACY-6,0,63488);
    spSetZSet(1);
    spSetZTest(1);
  }
  //spMesh3D(ship[0],0);
  memcpy(spGetMatrix(),matrix,64); //"glPop"
  
  //Drawing the right ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(+level.width+(1<<SP_ACCURACY-2),level.ship[1].y,0);
  spRotateY(SP_PI/2);
  spRotateX(level.ship[1].direction-SP_PI/2);
  spMesh3D(gun,0);
  if (game_mode == 0 && momPlayer == 1)
  {
    spSetZSet(0);
    spSetZTest(0);
    spRotateY(-SP_PI/2);
    spQuad3D((1<<SP_ACCURACY-4)+0,+1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+0,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[1].energy/16,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[1].energy/16,+1<<SP_ACCURACY-6,0,63488);
    spSetZSet(1);
    spSetZTest(1);
  }
  //spMesh3D(ship[1],0);
  memcpy(spGetMatrix(),matrix,64); //"glPop"
  
  pPlanet planet = level.firstPlanet;
  for (; planet != NULL; planet = planet->next)
  {
    Sint32 matrix[16];
    memcpy(matrix,spGetMatrix(),64);
    spTranslate(planet->x,planet->y,0);
    spRotateX(planet->rx);
    spRotateY(planet->ry);
    spRotateZ(planet->rz);
    spMesh3D(planet->mesh,0);
    memcpy(spGetMatrix(),matrix,64);
  }
  
  if (game_mode == 1)
  {
    spEllipse3D(level.bullet.x>>BULLET_ACCURACY,level.bullet.y>>BULLET_ACCURACY,0,1<<SP_ACCURACY-5,1<<SP_ACCURACY-5,63488);
  }
  
  spSetZSet(0);
  spSetZTest(0);  
  sprintf(buffer,"%is left",(countdown+999)/1000);
  spFontDrawMiddle(screen->w/2,screen->h-getFont(0)->maxheight,-1,buffer,getFont(0));  
}

void calcLevel(Sint32 steps)
{
  levelTime += steps;
  countdown -= steps;
  pPlanet planet = level.firstPlanet;
  while (planet)
  {
    planet->rx += planet->dx*steps;
    planet->ry += planet->dy*steps;
    planet->rz += planet->dz*steps;
    planet = planet-> next;
  }
  
  if (game_mode == 0)
  {
    //ship control
    if (!momPlayer)
    {
      if (spGetInput()->axis[1]<0)
      {
        level.ship[momPlayer].direction+=steps<<SP_ACCURACY-10;
        if (level.ship[momPlayer].direction >= SP_PI)
          level.ship[momPlayer].direction = SP_PI-(1<<SP_ACCURACY-10);
      }
      if (spGetInput()->axis[1]>0)
      {
        level.ship[momPlayer].direction-=steps<<SP_ACCURACY-10;
        if (level.ship[momPlayer].direction <= 0)
          level.ship[momPlayer].direction = (1<<SP_ACCURACY-10);
      }
    }
    else
    {
      if (spGetInput()->axis[1]>0)
      {
        level.ship[momPlayer].direction+=steps<<SP_ACCURACY-9;
        if (level.ship[momPlayer].direction >= 2*SP_PI)
          level.ship[momPlayer].direction = 2*SP_PI-(1<<SP_ACCURACY-10);
      }
      if (spGetInput()->axis[1]<0)
      {
        level.ship[momPlayer].direction-=steps<<SP_ACCURACY-9;
        if (level.ship[momPlayer].direction <= SP_PI)
          level.ship[momPlayer].direction = SP_PI+(1<<SP_ACCURACY-10);
      }
    }
    if ((!momPlayer && spGetInput()->axis[0]<0) || (momPlayer && spGetInput()->axis[0]>0))
    {
      level.ship[momPlayer].energy-=steps<<SP_ACCURACY-10;
      if (level.ship[momPlayer].energy < (1<<SP_ACCURACY-1))
        level.ship[momPlayer].energy = 1<<SP_ACCURACY-1;
    }
    if ((!momPlayer && spGetInput()->axis[0]>0) || (momPlayer && spGetInput()->axis[0]<0))
    {
      level.ship[momPlayer].energy+=steps<<SP_ACCURACY-10;
      if (level.ship[momPlayer].energy > (2<<SP_ACCURACY))
        level.ship[momPlayer].energy = 2<<SP_ACCURACY;
    }
    if (spGetInput()->button[SP_BUTTON_A])
    {
      spGetInput()->button[SP_BUTTON_A] = 0;
      game_mode = 1;
      countdown = 20000;
      if (momPlayer == 0)
        level.bullet.x  = -level.width-(1<<SP_ACCURACY-2);
      else
        level.bullet.x  =  level.width+(1<<SP_ACCURACY-2);
      level.bullet.y  = level.ship[momPlayer].y;
      level.bullet.x += spMul(spSin(level.ship[momPlayer].direction),(3<<SP_ACCURACY-4));
      level.bullet.y += spMul(spCos(level.ship[momPlayer].direction),(3<<SP_ACCURACY-4));
      level.bullet.x  = level.bullet.x << BULLET_ACCURACY;
      level.bullet.y  = level.bullet.y << BULLET_ACCURACY;
      level.bullet.dx = spMul(spSin(level.ship[momPlayer].direction),level.ship[momPlayer].energy);
      level.bullet.dy = spMul(spCos(level.ship[momPlayer].direction),level.ship[momPlayer].energy);
      nextTrace();
    }
    if (countdown <= 0)
    {
      momPlayer = 1-momPlayer;
      countdown = 20000;
    }
  }
  else
  {
    int i;
    for (i = 0; i < steps; i++)
    {
      level.bullet.x += level.bullet.dx;
      level.bullet.y += level.bullet.dy;
      addTracePoint(level.bullet.x>>BULLET_ACCURACY,level.bullet.y>>BULLET_ACCURACY);
    }
    int hit = 0;
    int out = 0;
    if ((level.bullet.x >> BULLET_ACCURACY) < -level.width *2 ||
        (level.bullet.x >> BULLET_ACCURACY) >  level.width *2  ||
        (level.bullet.y >> BULLET_ACCURACY) < -(2<<SP_ACCURACY) ||
        (level.bullet.y >> BULLET_ACCURACY) >  (2<<SP_ACCURACY))
      out = 1;
    if (countdown <= 0 || hit || out)
    {
      momPlayer = 1-momPlayer;
      game_mode = 0;
      countdown = 20000;
    }
  }
}

void quitLevel()
{
  deleteLevel();
  int i;
  for (i = 0; i < 10;i++)
    SDL_FreeSurface(planet_texture[i]);
  for (i = 0; i < 5;i++)
    SDL_FreeSurface(starSurface[i]);
  spMeshDelete(ship[0]);
  spMeshDelete(ship[1]);
  spMeshDelete(gun);
  spMeshDelete(bullet);
}
