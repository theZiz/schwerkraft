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
#include "font.h"

SDL_Surface* planet_texture[10];
spModelPointer planet[10];
spModelPointer ship[2];
Sint32 levelTime = 0;
tLevel level;
Sint32 momPlayer = 0;

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
    planet[i] = spMeshLoadObjSize("./data/planet.obj",planet_texture[i],65535,3<<SP_ACCURACY-2);
  }
  ship[0] = spMeshLoadObjSize("./data/ship.obj",NULL,12345,1<<SP_ACCURACY-3);
  ship[1] = spMeshLoadObjSize("./data/ship.obj",NULL,23456,1<<SP_ACCURACY-3);
  //Mirroring ship[1]
  for (i = 0; i < ship[1]->pointCount; i++)
    ship[1]->point[i].x = -ship[1]->point[i].x;
}

void createRandomLevel()
{
  level.width = rand()%(1<<SP_ACCURACY)+(1<<SP_ACCURACY); //width between 1 and 2
  level.firstPlanet = NULL;
  level.ship[0].y = rand()%((1<<SP_ACCURACY+1)+1-(1<<SP_ACCURACY-1))-(1<<SP_ACCURACY)+(1<<SP_ACCURACY-2); //position between -1 and 1
  level.ship[1].y = rand()%((1<<SP_ACCURACY+1)+1-(1<<SP_ACCURACY-1))-(1<<SP_ACCURACY)+(1<<SP_ACCURACY-2); //position between -1 and 1
  level.ship[0].direction =   SP_PI/2;
  level.ship[1].direction = 3*SP_PI/2;
  level.ship[0].energy = 1<<SP_ACCURACY-1; //1/2
  level.ship[1].energy = 1<<SP_ACCURACY-1; //1/2
  momPlayer = 0;
}

void drawLevel()
{
  Sint32 matrix[16];
  //TODO: More accurate zoom
  spTranslate(0,0,(-6<<SP_ACCURACY-2)-level.width);

  //Game Rectangle
  //spRectangleBorder3D(0,0,0,level.width*2,2<<SP_ACCURACY,1<<SP_ACCURACY-5,1<<SP_ACCURACY-5,spGetHSV(0,0,64));

  char buffer[256];

  //Drawing the left ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(-level.width,level.ship[0].y,0);
  Sint32 x = spMul(spSin(level.ship[0].direction),level.ship[0].energy/2);
  Sint32 y = spMul(spCos(level.ship[0].direction),level.ship[0].energy/2);
  spLine3D(0,0,0,x,y,0,32768);
  spRotateX(SP_PI/2);
  spMesh3D(ship[0],0);
  memcpy(spGetMatrix(),matrix,64); //"glPop"
  
  //Drawing the right ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(+level.width,level.ship[1].y,0);
  x = spMul(spSin(level.ship[1].direction),level.ship[1].energy/2);
  y = spMul(spCos(level.ship[1].direction),level.ship[1].energy/2);
  spLine3D(0,0,0,x,y,0,32768);
  spRotateX(SP_PI/2);
  spMesh3D(ship[1],0);
  memcpy(spGetMatrix(),matrix,64); //"glPop"
  
  /*int i;
  for (i = 0; i < 10; i++)
  {
    Sint32 matrix[16];
    memcpy(matrix,spGetMatrix(),64);
    spRotateZ(levelTime+i*SP_PI/5);
    Sint32 c = abs(spCos(levelTime+i*SP_PI/5));
    spTranslate((5<<SP_ACCURACY-1)+c,0,0);
    spRotateX(levelTime);
    spRotateY(levelTime);
    spRotateZ(levelTime);
    spMesh3D(planet[i],0);
    memcpy(spGetMatrix(),matrix,64);
  }*/
}

void calcLevel(Sint32 steps)
{
  levelTime+=steps;
  
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
    if (level.ship[momPlayer].energy < (1<<SP_ACCURACY-3))
      level.ship[momPlayer].energy = 1<<SP_ACCURACY-3;
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
    momPlayer = 1-momPlayer;
  }
  
}

void quitLevel()
{
  int i;
  for (i = 0; i < 10;i++)
  {
    spMeshDelete(planet[i]);
    SDL_FreeSurface(planet_texture[i]);
  }
  spMeshDelete(ship[0]);
  spMeshDelete(ship[1]);
}
