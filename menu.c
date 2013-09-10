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

#include "menu.h"
#include "font.h"

int menu_choice = 1;
int wait_for_release = 0;

SDL_Surface* banner;
SDL_Surface* logo;
SDL_Surface* help[5];

int help_page = 1;

void init_menu_stuff()
{
  banner = spLoadSurface("./data/rebirth-banner-entered-800.png"); 
  logo = spLoadSurface("./data/sparrow.png"); 
  int i;
  for (i = 0; i < 4; i++)
  {
    char buffer[256];
    sprintf(buffer,"./data/help_%i.png",i+1);
    help[i] = spLoadSurface(buffer); 
  }
}


void draw_menu()
{
  SDL_Surface *screen = spGetWindowSurface();

  spResetZBuffer();
  spClearTarget(3);
  spIdentity();
  spSetZSet(0);
  spSetZTest(0);
  spFontDraw(screen->w/2-spFontWidth("SchwerKraft",getFont(4))/2,1*screen->h/8,-1,"Schwer",getFont(4));  
  spFontDraw(screen->w/2-spFontWidth("SchwerKraft",getFont(4))/2+spFontWidth("Schwer",getFont(4)),1*screen->h/8,-1,"Kraft",getFont(5));  

  spFontDrawMiddle(screen->w/2,5*screen->h/16,-1,"Human VS Human",getFont((menu_choice!=1)*2));  
  spFontDrawMiddle(screen->w/2,7*screen->h/16,-1,"Human VS Computer",getFont((menu_choice!=2)*2));  
  spFontDrawMiddle(screen->w/2,9*screen->h/16,-1,"How to Play",getFont((menu_choice!=3)*2));  
  spFontDrawMiddle(screen->w/2,11*screen->h/16,-1,"About",getFont((menu_choice!=4)*2));  
  spFontDrawMiddle(screen->w/2,13*screen->h/16,-1,"Quit",getFont((menu_choice!=5)*2));

  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(3)->maxheight,-1,"Press [B] to select a point or [S] to exit",getFont(3));
  draw_music();
  
  spFlip();
}

int calc_menu(Uint32 steps)
{
  calc_music(steps);
  if (wait_for_release<0 && spGetInput()->axis[1]<=0)
   wait_for_release = 0;
  if (wait_for_release>0 && spGetInput()->axis[1]>=0)
   wait_for_release = 0;
  if (spGetInput()->axis[1]>0 && menu_choice!=5 && wait_for_release == 0)
  {
    wait_for_release = -1;
    menu_choice++;
  }
  if (spGetInput()->axis[1]<0 && menu_choice!=1 && wait_for_release == 0)
  {
    wait_for_release = 1;
    menu_choice--;
  }
  if (spGetInput()->button[SP_BUTTON_B_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_B_NOWASD] = 0;
    return menu_choice;  
  }
  if (spGetInput()->button[SP_BUTTON_START_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
    return 5;
  }
  return 0;
}

void draw_about()
{
  SDL_Surface *screen = spGetWindowSurface();

  spResetZBuffer();
  spClearTarget(3);
  spIdentity();
  spSetZSet(0);
  spSetZTest(0);
  spFontDraw(screen->w/2-spFontWidth("AbOut",getFont(4))/2,1*screen->h/8,-1,"Ab",getFont(4));  
  spFontDraw(screen->w/2-spFontWidth("AbOut",getFont(4))/2+spFontWidth("Ab",getFont(4)),1*screen->h/8,-1,"Out",getFont(5));  

  spRotozoomSurface(screen->w/6,2*screen->h/5,-1,logo,spGetSizeFactor()*2/5,spGetSizeFactor()*2/5,SDL_GetTicks()*32);
  
  spFontDraw(5*screen->w/16,4*screen->h/20,-1,"Made for the",getFont(3));
  spFontDraw(5*screen->w/16,5*screen->h/20,-1,"  Pandora Rebirth Competition",getFont(3));
  spFontDraw(5*screen->w/16,7*screen->h/20,-1,"Made by Ziz with sparrow3D",getFont(3));
  spFontDraw(5*screen->w/16,8*screen->h/20,-1,"  Licenced under MPL / LGPL",getFont(3));
  spFontDraw(5*screen->w/16,10*screen->h/20,-1,"Music made by Kevin MaCLeod",getFont(3));
  spFontDraw(5*screen->w/16,11*screen->h/20,-1,"  Licenced under CC3.0",getFont(3));
  spFontDraw(5*screen->w/16,12*screen->h/20,-1,"  www.incompetech.com",getFont(3));
  
  spRotozoomSurface(screen->w/2,4*screen->h/5,-1,banner,spGetSizeFactor()*2/5,spGetSizeFactor()*2/5,0);
  
  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(3)->maxheight,-1,"Press [B] to select a point or [S] to exit",getFont(3));
 
  draw_music();
  spFlip();
}

int calc_about(Uint32 steps)
{
  calc_music(steps);
  if (spGetInput()->button[SP_BUTTON_B_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_B_NOWASD] = 0;
    return 1;
  }
  if (spGetInput()->button[SP_BUTTON_START_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
    return 1;
  }
  return 0;
}

void quit_menu_stuff()
{
  SDL_FreeSurface(logo); 
  SDL_FreeSurface(banner); 
  int i;
  for (i = 0;i < 4; i++)
    SDL_FreeSurface(help[i]); 
}

void draw_help()
{
  SDL_Surface *screen = spGetWindowSurface();

  spResetZBuffer();
  spClearTarget(3);
  spIdentity();
  spSetZSet(0);
  spSetZTest(0);
  spFontDraw(screen->w/2-spFontWidth("How to Play",getFont(4))/2,1*screen->h/8,-1,"How to",getFont(4));  
  spFontDraw(screen->w/2-spFontWidth("How to Play",getFont(4))/2+spFontWidth("How to",getFont(4)),1*screen->h/8,-1," Play",getFont(5));  
  
  switch (help_page)
  {
    case 1:
      spFontDrawMiddle(screen->w/2, 4*screen->h/16,-1,"We have the year 2200. The earth exploded",getFont(3));
      spFontDrawMiddle(screen->w/2, 5*screen->h/16,-1,"(don't ask why) and generation ships made",getFont(3));
      spFontDrawMiddle(screen->w/2, 6*screen->h/16,-1,"their ways to new systems.",getFont(3));
      spFontDrawMiddle(screen->w/2, 7*screen->h/16,-1,"But unlikely you meet another generation",getFont(3));
      spFontDrawMiddle(screen->w/2, 8*screen->h/16,-1,"ship in the nowhere. Nearly out of",getFont(3));
      spFontDrawMiddle(screen->w/2, 9*screen->h/16,-1,"energy you have no choice but to fight",getFont(3));
      spFontDrawMiddle(screen->w/2,10*screen->h/16,-1,"for the planets.",getFont(3));
      spFontDrawMiddle(screen->w/3,12*screen->h/16,-1,"Press [L] or [R] to select",getFont(3));
      spFontDrawMiddle(screen->w/3,13*screen->h/16,-1,"the previous or next page.",getFont(3));
      spRotozoomSurface(5*screen->w/6,12*screen->h/16,-1,help[0],spGetSizeFactor()/2,spGetSizeFactor()/2,0);
      spFontDrawMiddle(5*screen->w/6,13*screen->h/16,-1,"a ship",getFont(3));
      break;
    case 2:
      spFontDrawMiddle(screen->w/2, 4*screen->h/16,-1,"So you have to hit the other ship before",getFont(3));
      spFontDrawMiddle(screen->w/2, 5*screen->h/16,-1,"it hits you. But you have only limited",getFont(3));
      spFontDrawMiddle(screen->w/2, 6*screen->h/16,-1,"energy and the gravitation of the planets",getFont(3));
      spFontDrawMiddle(screen->w/2, 7*screen->h/16,-1,"will redirect your projectile. If you get",getFont(3));
      spFontDrawMiddle(screen->w/2, 8*screen->h/16,-1,"hit or out of energy, you loose.",getFont(3));
      spRotozoomSurface(screen->w/2,11*screen->h/16,-1,help[1],spGetSizeFactor()*2/5,spGetSizeFactor()*2/5,0);
      spFontDrawMiddle(screen->w/2,14*screen->h/16,-1,"gravity is an asshole...",getFont(3));
      break;
    case 3:
      spFontDrawMiddle(screen->w/2, 4*screen->h/16,-1,"With left and right on your "SP_PAD_NAME,getFont(3));
      spFontDrawMiddle(screen->w/2, 5*screen->h/16,-1,"you can change the amount of energy used",getFont(3));
      spFontDrawMiddle(screen->w/2, 6*screen->h/16,-1,"for the shoot. More energy means fewer",getFont(3));
      spFontDrawMiddle(screen->w/2, 7*screen->h/16,-1,"redirection but also a fewer amount of total",getFont(3));
      spFontDrawMiddle(screen->w/2, 8*screen->h/16,-1,"shoots. With up and down you can change the",getFont(3));
      spFontDrawMiddle(screen->w/2, 9*screen->h/16,-1,"angle of shooting. Finally you shoot with [B].",getFont(3));
      spFontDrawMiddle(screen->w/2,10*screen->h/16,-1,"Hold [A] pressed for more accurate control.",getFont(3));
      spFontDrawMiddle(3*screen->w/5,23*screen->h/32,-1,"In the upper left and right",getFont(3));
      spFontDrawMiddle(3*screen->w/5,25*screen->h/32,-1,"corner you see your total",getFont(3));
      spFontDrawMiddle(3*screen->w/5,27*screen->h/32,-1,"energy, shoot energy and angle.",getFont(3));
      spRotozoomSurface(4*screen->w/30,13*screen->h/16,-1,help[2],spGetSizeFactor()*2/4,spGetSizeFactor()*2/4,0);
      break;
    case 4:
      spFontDrawMiddle(screen->w/2, 4*screen->h/16,-1,"Last thing to know is there are two kinds",getFont(3));
      spFontDrawMiddle(screen->w/2, 5*screen->h/16,-1,"of planets: solid planets and gas planets.",getFont(3));
      spFontDrawMiddle(screen->w/2, 6*screen->h/16,-1,"gas planets are permeable for projectiles",getFont(3));
      spFontDrawMiddle(screen->w/2, 7*screen->h/16,-1,"and the gravity is much lower (1/4).",getFont(3));
      spFontDrawMiddle(screen->w/3, 9*screen->h/16,-1,"Some words at the end: If you",getFont(3));
      spFontDrawMiddle(screen->w/3,10*screen->h/16,-1,"wonder about the quiteness:",getFont(3));
      spFontDrawMiddle(screen->w/3,11*screen->h/16,-1,"There is no sound in space!",getFont(3));
      spFontDrawMiddle(screen->w/3,12*screen->h/16,-1,"Oh and don't use this for",getFont(3));
      spFontDrawMiddle(screen->w/3,13*screen->h/16,-1,"real rocket sciene.",getFont(3));
      spRotozoomSurface(5*screen->w/6,11*screen->h/16,-1,help[3],spGetSizeFactor()/3,spGetSizeFactor()/3,0);
      spFontDrawMiddle(5*screen->w/6,13*screen->h/16,-1,"gas planets:",getFont(3));
      spFontDrawMiddle(5*screen->w/6,14*screen->h/16,-1,"made of farts",getFont(3));
      break;    
  }

  spFontDraw(2,2,-1,"(L) previous",getFont(3));
  spFontDrawRight(screen->w-2,2,-1,"next (R)",getFont(3));
  char buffer[256];
  sprintf(buffer,"Page %i of 4",help_page);
  spFontDrawMiddle(screen->w/2,2,-1,buffer,getFont(3));
  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(3)->maxheight,-1,"Press [B] to select a point or [S] to exit",getFont(3));
 
  draw_music();
  spFlip();
}

int calc_help(Uint32 steps)
{
  calc_music(steps);
  if (spGetInput()->button[SP_BUTTON_L_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_L_NOWASD] = 0;
    help_page--;
    if (help_page<1)
      help_page = 4;
  }
  if (spGetInput()->button[SP_BUTTON_R_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_R_NOWASD] = 0;
    help_page++;
    if (help_page>4)
      help_page = 1;
  }
  if (spGetInput()->button[SP_BUTTON_B_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_B_NOWASD] = 0;
    return 1;
  }
  if (spGetInput()->button[SP_BUTTON_START_NOWASD])
  {
    spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
    return 1;
  }
  return 0;
}
