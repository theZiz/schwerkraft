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

  spFontDrawMiddle(screen->w/2,screen->h-2-getFont(3)->maxheight,-1,"Press (A) to select a point",getFont(3));
  
  spFlip();
}

int calc_menu(Sint32 steps)
{
  if (wait_for_release<0 && spGetInput()->axis[1]>=0)
   wait_for_release = 0;
  if (wait_for_release>0 && spGetInput()->axis[1]<=0)
   wait_for_release = 0;
  if (spGetInput()->axis[1]<0 && menu_choice!=5 && wait_for_release == 0)
  {
    wait_for_release = -1;
    menu_choice++;
  }
  if (spGetInput()->axis[1]>0 && menu_choice!=1 && wait_for_release == 0)
  {
    wait_for_release = 1;
    menu_choice--;
  }
  if (spGetInput()->button[SP_BUTTON_A])
  {
    spGetInput()->button[SP_BUTTON_A] = 0;
    return menu_choice;  
  }
  if (spGetInput()->button[SP_BUTTON_START])
  {
    spGetInput()->button[SP_BUTTON_START] = 0;
    return 5;
  }
  return 0;
}
