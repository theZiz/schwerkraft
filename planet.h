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
#ifndef _PLANET_H
#define _PLANET_H

#include <SDL.h>
#include "../sparrow3d/sparrow3d.h"

#define PLANET_NORMAL 0
#define PLANET_GAS 1
//#define PLANET_STAR 2

typedef struct sPlanet *pPlanet;
typedef struct sPlanet {
  Sint32 x,y;
  Sint32 radius;
  Sint32 mass; //mass and radius determine the gravity
  Sint32 kind; //PLANET_NORMAL, PLANET_GAS (or PLANET_STAR)
  Sint32 satellite; //is 1, if the planet orbits a point in space (e.g. another planet)
  spModelPointer mesh;
  Sint32 rx,ry,rz;
  Sint32 dx,dy,dz;
  //TODO orbiting of planets :P
  
  pPlanet next;
} tPlanet;

#endif
