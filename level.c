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
#define MAX_COUNTDOWN_TARGETING 20000
#define MAX_COUNTDOWN_FLYING 40000

SDL_Surface* planet_texture[10];
spModelPointer ship[2];
spModelPointer gun;
spModelPointer bullet;
Sint32 levelTime = 0;
tLevel level;
Sint32 momPlayer = 0;
Sint32 game_mode = 0; //0 targeting, 1 shooting, 2 winner
Sint32 winner;
Sint32 countdown = MAX_COUNTDOWN_TARGETING; //20 seconds for targeting and 40 seconds of flying throw space
SDL_Surface* starSurface[5];
struct {Sint32 x,y,kind;} star[STAR_COUNT];
int ki;
Sint32 ki_from,ki_to;
Sint32 ki_old_from,ki_old_to;


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

#define MIN_PLANETS 2
#define MAX_PLANETS 8

void createRandomLevel(int player_2_is_ki)
{
  ki = player_2_is_ki;
  if (ki)
    printf("Start game vs computer\n");
  else
    printf("Start game vs human\n");
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
    planet->mass = spMul(planet->radius*planet->radius,planet->radius)>>8;
    planet->kind = (rand()%3) & 1;
    if (planet->kind == PLANET_NORMAL)
      planet->mesh = spMeshLoadObjSize("./data/planet.obj",planet_texture[rand()%6],65535,planet->radius);
    else
    {
      planet->mesh = spMeshLoadObjSize("./data/planet.obj",planet_texture[rand()%4+6],65535,planet->radius);
      planet->mass /= 4;
    }
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
  level.ship[0].allEnergy = 10<<SP_ACCURACY;
  level.ship[1].allEnergy = 10<<SP_ACCURACY;

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
  countdown = MAX_COUNTDOWN_TARGETING;
  game_mode = 0;
  ki_from = SP_PI+(1<<SP_ACCURACY-10);
  ki_to = 2*SP_PI-(1<<SP_ACCURACY-10);
  ki_old_from = 0;
  ki_old_to = 0;
}

#define KI_STEP 1

Sint32 ki_getDistanceFromPlayer1(Sint32 direction)
{
  int hit = 0;
  int out = 0;
  int selfHit = 0;
  int enemyHit = 0;
  Sint32 bx  = level.width+(1<<SP_ACCURACY-2);
  Sint32 by  = level.ship[momPlayer].y;
  bx += spMul(spSin(direction),(3<<SP_ACCURACY-4));
  by += spMul(spCos(direction),(3<<SP_ACCURACY-4));
  bx  = bx << BULLET_ACCURACY;
  by  = by << BULLET_ACCURACY;
  Sint32 bdx = spMul(spSin(direction),level.ship[1].energy);
  Sint32 bdy = spMul(spCos(direction),level.ship[1].energy);
  Sint32 nearest = 1<<SP_ACCURACY+8;
  int i;
  for (i = 0; i < MAX_COUNTDOWN_FLYING/KI_STEP; i++)
  {
    if (hit || out || selfHit || enemyHit)
      break;
    Sint32 gx = 0;
    Sint32 gy = 0;
    
    pPlanet planet = level.firstPlanet;
    while (planet)
    {
      Sint32 vx = planet->x-(bx >> BULLET_ACCURACY);
      Sint32 vy = planet->y-(by >> BULLET_ACCURACY);
      Sint32 vlength = spSqrt(spMulHigh(vx,vx)+spMulHigh(vy,vy));
      if (planet->kind == PLANET_NORMAL && vlength < (planet->radius+(1<<SP_ACCURACY-5)))
        hit = 1;
      Sint32 vlength_3 = spMulHigh(spMulHigh(vlength,vlength),vlength);
      if (vlength_3!=0)
      {
        gx += spDivHigh(spMulHigh(planet->mass,vx),vlength_3);
        gy += spDivHigh(spMulHigh(planet->mass,vy),vlength_3);
      }
      planet = planet->next;
    }
    
    bdx += gx*KI_STEP>>15;
    bdy += gy*KI_STEP>>15;
    
    //addTracePoint(bx>>BULLET_ACCURACY,by>>BULLET_ACCURACY);
    bx += bdx*KI_STEP;
    by += bdy*KI_STEP;
    if ((bx >> BULLET_ACCURACY) < -level.width *5/2 ||
        (bx >> BULLET_ACCURACY) >  level.width *5/2  ||
        (by >> BULLET_ACCURACY) < -(5<<SP_ACCURACY-1) ||
        (by >> BULLET_ACCURACY) >  (5<<SP_ACCURACY-1))
      out = 1;
    if (spMul((bx >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)),
              (bx >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)))+
        spMul((by >> BULLET_ACCURACY)-(level.ship[0].y),
              (by >> BULLET_ACCURACY)-(level.ship[0].y)) <
        spMul(4<<SP_ACCURACY-5,4<<SP_ACCURACY-5))
      enemyHit = 1;
    if (spMul((bx >> BULLET_ACCURACY)-(level.width+(1<<SP_ACCURACY-2)),
              (bx >> BULLET_ACCURACY)-(level.width+(1<<SP_ACCURACY-2)))+
        spMul((by >> BULLET_ACCURACY)-(level.ship[1].y),
              (by >> BULLET_ACCURACY)-(level.ship[1].y)) <
        spMul(4<<SP_ACCURACY-5,4<<SP_ACCURACY-5))
      selfHit = 1;
    Sint32 distance = spMul((bx >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)),
              (bx >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)))+
        spMul((by >> BULLET_ACCURACY)-(level.ship[0].y),
              (by >> BULLET_ACCURACY)-(level.ship[0].y));
    if (distance < nearest)
      nearest = distance;
  }
  return nearest;
}

#define KI_TRIES 5

void ki_search_best()
{
  if (ki_to == ki_old_to && ki_from == ki_old_from)
    level.ship[1].energy = rand()%(2<<SP_ACCURACY);
  ki_old_from = ki_from;
  ki_old_to = ki_to;
  Sint32 ki_step = (ki_to-ki_from)/(KI_TRIES+1);
  Sint32 best = ki_from;
  Sint32 best_value = ki_getDistanceFromPlayer1(best);
  Sint32 second = ki_to;
  Sint32 second_value = ki_getDistanceFromPlayer1(second);
  Sint32 ki_try;
  for (ki_try = ki_from+ki_step; ki_try <= ki_to-ki_step; ki_try+=ki_step)
  {
    Sint32 value = ki_getDistanceFromPlayer1(ki_try);
    if (value < best_value && ki_try!=second)
    {
      best = ki_try;
      best_value = value;
    }
    else
    if (value < second_value && ki_try!=best)
    {
      second = ki_try;
      second_value = value;
    }
  }
  if (best < second)
  {
    ki_from = best;
    ki_to = second;
  }
  else
  {
    ki_from = second;
    ki_to = best;
  }
  level.ship[1].direction = best;
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
  for (i = 0; i < STAR_COUNT;i++)
    spBlit3D(star[i].x,star[i].y,-1,starSurface[star[i].kind]);

  drawTracePoints();

  spSetZSet(1);
  spSetZTest(1);

  //Game Rectangle
  spRectangleBorder3D(0,0,0,level.width*5,5<<SP_ACCURACY,1<<SP_ACCURACY-4,1<<SP_ACCURACY-4,spGetHSV(0,0,64));

  char buffer[256];

  //Drawing the left ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(-level.width-(1<<SP_ACCURACY-2),level.ship[0].y,0);
  spRotateY(SP_PI/2);
  spRotateX(level.ship[0].direction-SP_PI/2);
  spMesh3D(gun,0);
  spSetZSet(0);
  spSetZTest(0);
  spEllipseBorder3D(0,0,0,3<<SP_ACCURACY-5,3<<SP_ACCURACY-5,3<<SP_ACCURACY-7,3<<SP_ACCURACY-7,2016);
  if (game_mode == 0 && momPlayer == 0)
  {
    spRotateY(-SP_PI/2);
    spQuad3D((1<<SP_ACCURACY-4)+0,+1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+0,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[0].energy/16,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[0].energy/16,+1<<SP_ACCURACY-6,0,63488);
  }
  spSetZSet(1);
  spSetZTest(1);
  //spMesh3D(ship[0],0);
  memcpy(spGetMatrix(),matrix,64); //"glPop"
  
  //Drawing the right ship, direction and energy
  //TODO: 20% cooler
  memcpy(matrix,spGetMatrix(),64); //"glPush"
  spTranslate(+level.width+(1<<SP_ACCURACY-2),level.ship[1].y,0);
  spRotateY(SP_PI/2);
  spRotateX(level.ship[1].direction-SP_PI/2);
  spMesh3D(gun,0);
  spSetZSet(0);
  spSetZTest(0);
  spEllipseBorder3D(0,0,0,3<<SP_ACCURACY-5,3<<SP_ACCURACY-5,3<<SP_ACCURACY-7,3<<SP_ACCURACY-7,31743);
  if (game_mode == 0 && momPlayer == 1)
  {
    spRotateY(-SP_PI/2);
    spQuad3D((1<<SP_ACCURACY-4)+0,+1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+0,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[1].energy/16,-1<<SP_ACCURACY-6,0,
             (1<<SP_ACCURACY-4)+level.ship[1].energy/16,+1<<SP_ACCURACY-6,0,63488);
  }
  spSetZSet(1);
  spSetZTest(1);
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
  
  
  //HUD
  spSetZSet(0);
  spSetZTest(0);  

  sprintf(buffer,"E: %.2f",(float)level.ship[0].allEnergy/SP_ACCURACY_FACTOR);
  spFontDraw(2,2,-1,buffer,getFont(1));
  int w = spFontWidth(buffer,getFont(1));
  sprintf(buffer,"-%.2f",(float)level.ship[0].energy/SP_ACCURACY_FACTOR);
  spFontDrawRight(2+w,2+getFont(1)->maxheight,-1,buffer,getFont(2));
  sprintf(buffer,"%.2f deg",(float)level.ship[0].direction*180.0/3.14159265f/SP_ACCURACY_FACTOR);
  spFontDraw(2,2+2*getFont(1)->maxheight,-1,buffer,getFont(3));
  
  sprintf(buffer,"E: %.2f",(float)level.ship[1].allEnergy/SP_ACCURACY_FACTOR);
  spFontDrawRight(screen->w-2,2,-1,buffer,getFont(1));  
  w = spFontWidth(buffer,getFont(1));
  sprintf(buffer,"-%.2f",(float)level.ship[1].energy/SP_ACCURACY_FACTOR);
  spFontDrawRight(screen->w-2,2+getFont(1)->maxheight,-1,buffer,getFont(2));
  sprintf(buffer,"%.2f deg",360.0f-(float)level.ship[1].direction*180.0/3.14159265f/SP_ACCURACY_FACTOR);
  spFontDrawRight(screen->w-2,2+2*getFont(1)->maxheight,-1,buffer,getFont(3));

  if (game_mode < 2)
  {
    if (momPlayer == 0)
    {
      sprintf(buffer,"< Green Player");
      spFontDrawMiddle(screen->w/2,2,-1,buffer,getFont(4));  
    }
    else
    {
      sprintf(buffer,"Blue Player >");
      spFontDrawMiddle(screen->w/2,2,-1,buffer,getFont(5));  
    }
    sprintf(buffer,"Distance: %.2f",(float)level.width/SP_ACCURACY_FACTOR);
    spFontDrawMiddle(screen->w/2,2+getFont(4)->maxheight,-1,buffer,getFont(3));  
  }
  else
  {
    if (winner == 0)
    {
      sprintf(buffer,"Green Player won");
      spFontDrawMiddle(screen->w/2,screen->h/2,-1,buffer,getFont(4));  
    }
    else
    {
      sprintf(buffer,"Blue Player won");
      spFontDrawMiddle(screen->w/2,screen->h/2,-1,buffer,getFont(5));  
    }    
    sprintf(buffer,"Press ("SP_BUTTON_B_NAME")");
    spFontDrawMiddle(screen->w/2,screen->h/2+getFont(4)->maxheight,-1,buffer,getFont(3));  
  }

  sprintf(buffer,"%is left",(countdown+999)/1000);
  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(0)->maxheight-getFont(3)->maxheight,-1,buffer,getFont(0));
  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(3)->maxheight,-1,"Press ("SP_BUTTON_START_NAME") to exit",getFont(3));
}

int calcLevel(Sint32 steps)
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
    if (ki == 0 || momPlayer == 0)
    {
      Sint32 slow_shift = 0;
      if (spGetInput()->button[SP_BUTTON_A])
        slow_shift = 2;
      if (!momPlayer)
      {
        if (spGetInput()->axis[1]<0)
        {
          level.ship[momPlayer].direction+=steps<<SP_ACCURACY-10-slow_shift;
          if (level.ship[momPlayer].direction >= SP_PI-(1<<SP_ACCURACY-10))
            level.ship[momPlayer].direction = SP_PI-(1<<SP_ACCURACY-10);
        }
        if (spGetInput()->axis[1]>0)
        {
          level.ship[momPlayer].direction-=steps<<SP_ACCURACY-10-slow_shift;
          if (level.ship[momPlayer].direction <= (1<<SP_ACCURACY-10))
            level.ship[momPlayer].direction = (1<<SP_ACCURACY-10);
        }
      }
      else
      {
        if (spGetInput()->axis[1]>0)
        {
          level.ship[momPlayer].direction+=steps<<SP_ACCURACY-10-slow_shift;
          if (level.ship[momPlayer].direction >= 2*SP_PI-(1<<SP_ACCURACY-10))
            level.ship[momPlayer].direction = 2*SP_PI-(1<<SP_ACCURACY-10);
        }
        if (spGetInput()->axis[1]<0)
        {
          level.ship[momPlayer].direction-=steps<<SP_ACCURACY-10-slow_shift;
          if (level.ship[momPlayer].direction <= SP_PI+(1<<SP_ACCURACY-10))
            level.ship[momPlayer].direction = SP_PI+(1<<SP_ACCURACY-10);
        }
      }
      if ((!momPlayer && spGetInput()->axis[0]<0) || (momPlayer && spGetInput()->axis[0]>0))
      {
        level.ship[momPlayer].energy-=steps<<SP_ACCURACY-10-slow_shift;
        if (level.ship[momPlayer].energy < (0<<SP_ACCURACY-1))
          level.ship[momPlayer].energy = 0<<SP_ACCURACY-1;
      }
      if ((!momPlayer && spGetInput()->axis[0]>0) || (momPlayer && spGetInput()->axis[0]<0))
      {
        level.ship[momPlayer].energy+=steps<<SP_ACCURACY-10-slow_shift;
        if (level.ship[momPlayer].energy > (2<<SP_ACCURACY))
          level.ship[momPlayer].energy = 2<<SP_ACCURACY;
        if (level.ship[momPlayer].energy >= level.ship[momPlayer].allEnergy)
          level.ship[momPlayer].energy = level.ship[momPlayer].allEnergy;
      }
    }
    else
    {
      ki_search_best();
      spGetInput()->button[SP_BUTTON_B] = 1;
    }
    if (spGetInput()->button[SP_BUTTON_B])
    {
      spGetInput()->button[SP_BUTTON_B] = 0;
      game_mode = 1;
      countdown = MAX_COUNTDOWN_FLYING;
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
      level.ship[momPlayer].allEnergy -= level.ship[momPlayer].energy;
      if (level.ship[momPlayer].energy >= level.ship[momPlayer].allEnergy)
        level.ship[momPlayer].energy = level.ship[momPlayer].allEnergy;      
    }
    if (countdown <= 0)
    {
      momPlayer = 1-momPlayer;
      countdown = MAX_COUNTDOWN_TARGETING;
    }
  }
  else
  if (game_mode == 1)
  {
    int i;
    int hit = 0;
    int out = 0;
    int selfHit = 0;
    int enemyHit = 0;
    for (i = 0; i < steps; i++)
    {
      if (hit || out || selfHit || enemyHit)
        break;
      Sint32 gx = 0;
      Sint32 gy = 0;
      
      pPlanet planet = level.firstPlanet;
      while (planet)
      {
        Sint32 vx = planet->x-(level.bullet.x >> BULLET_ACCURACY);
        Sint32 vy = planet->y-(level.bullet.y >> BULLET_ACCURACY);
        Sint32 vlength = spSqrt(spMulHigh(vx,vx)+spMulHigh(vy,vy));
        if (planet->kind == PLANET_NORMAL && vlength < (planet->radius+(1<<SP_ACCURACY-5)))
          hit = 1;
        Sint32 vlength_3 = spMulHigh(spMulHigh(vlength,vlength),vlength);
        if (vlength_3!=0)
        {
          gx += spDivHigh(spMulHigh(planet->mass,vx),vlength_3);
          gy += spDivHigh(spMulHigh(planet->mass,vy),vlength_3);
        }
        planet = planet->next;
      }
      
      level.bullet.dx += gx>>15;
      level.bullet.dy += gy>>15;

      level.bullet.x += level.bullet.dx;
      level.bullet.y += level.bullet.dy;
      addTracePoint(level.bullet.x>>BULLET_ACCURACY,level.bullet.y>>BULLET_ACCURACY);
      if ((level.bullet.x >> BULLET_ACCURACY) < -level.width *5/2 ||
          (level.bullet.x >> BULLET_ACCURACY) >  level.width *5/2  ||
          (level.bullet.y >> BULLET_ACCURACY) < -(5<<SP_ACCURACY-1) ||
          (level.bullet.y >> BULLET_ACCURACY) >  (5<<SP_ACCURACY-1))
        out = 1;
      if (spMul((level.bullet.x >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)),
                (level.bullet.x >> BULLET_ACCURACY)-(-level.width-(1<<SP_ACCURACY-2)))+
          spMul((level.bullet.y >> BULLET_ACCURACY)-(level.ship[0].y),
                (level.bullet.y >> BULLET_ACCURACY)-(level.ship[0].y)) <
          spMul(4<<SP_ACCURACY-5,4<<SP_ACCURACY-5))
      {
        if (momPlayer == 0)
          selfHit = 1;
        else
          enemyHit = 1;
      }
      if (spMul((level.bullet.x >> BULLET_ACCURACY)-(level.width+(1<<SP_ACCURACY-2)),
                (level.bullet.x >> BULLET_ACCURACY)-(level.width+(1<<SP_ACCURACY-2)))+
          spMul((level.bullet.y >> BULLET_ACCURACY)-(level.ship[1].y),
                (level.bullet.y >> BULLET_ACCURACY)-(level.ship[1].y)) <
          spMul(4<<SP_ACCURACY-5,4<<SP_ACCURACY-5))
      {
        if (momPlayer == 1)
          selfHit = 1;
        else
          enemyHit = 1;
      }
    }
    if (selfHit)
    {
      winner = 1-momPlayer;
      game_mode = 2;
      printf("Player %i won\n",winner);
      countdown = 20000;
      spGetInput()->button[SP_BUTTON_B] = 0;
    }
    else
    if (enemyHit)
    {
      winner = momPlayer;
      game_mode = 2;
      printf("Player %i won\n",winner);
      countdown = 20000;
      spGetInput()->button[SP_BUTTON_B] = 0;
    }
    else
    if (countdown <= 0 || hit || out)
    {
      momPlayer = 1-momPlayer;
      if (level.ship[momPlayer].allEnergy <= 0)
      {
        winner = 1-momPlayer;
        game_mode = 2;
        printf("Player %i won\n",winner);
        countdown = 20000;
        spGetInput()->button[SP_BUTTON_B] = 0;
      }
      else
      {
        game_mode = 0;
        countdown = MAX_COUNTDOWN_TARGETING;
      }
    }
  }
  else
  {
    if (spGetInput()->button[SP_BUTTON_B] || countdown <= 0)
    {
      spGetInput()->button[SP_BUTTON_B] = 0;
      return 1;
    }
  }
  return 0;
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
