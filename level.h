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
#ifndef _LEVEL_H
#define _LEVEL_H

#include <SDL.h>
#include "../sparrow3d/sparrow3d.h"
#include "planet.h"
#include "ship.h"

typedef struct sLevel *pLevel;
typedef struct sLevel {
  Sint32 width;
  pPlanet firstPlanet;
  tShip ship[2]; //0 left, 1 right
  struct {Sint32 x,y;} bullet;
} tLevel;

void initLevel();
void createRandomLevel();
void deleteLevel();
void drawLevel();
void calcLevel(Sint32 steps);
void quitLevel();

#endif
