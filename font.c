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

#include "font.h"

spFontPointer font[6] = {NULL,NULL,NULL,NULL,NULL,NULL}; //0 (biggest) to 3 (small), 4 green fat, 5 blue fat

void reloadFont()
{
  //Font Loading
  spFontSetShadeColor(65535);
  if (font[0])
    spFontDelete(font[0]);
  font[0] = spFontLoad(FONT_LOCATION,FONT_SIZE_0*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[0],SP_FONT_GROUP_ASCII,0);//whole ASCII
  spFontAddBorder(font[0],65535);
  spFontReplaceColor(font[0],0,SP_ALPHA_COLOR);

  if (font[1])
    spFontDelete(font[1]);
  font[1] = spFontLoad(FONT_LOCATION,FONT_SIZE_1*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[1],SP_FONT_GROUP_ASCII,0);//whole ASCII
  spFontAddBorder(font[1],65535);
  spFontReplaceColor(font[1],0,SP_ALPHA_COLOR);

  spFontSetShadeColor(48631);
  if (font[2])
    spFontDelete(font[2]);
  font[2] = spFontLoad(FONT_LOCATION,FONT_SIZE_2*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[2],SP_FONT_GROUP_ASCII,14823);//whole ASCII
  spFontAddBorder(font[2],48631);
  spFontReplaceColor(font[2],0,SP_ALPHA_COLOR);

  if (font[3])
    spFontDelete(font[3]);
  font[3] = spFontLoad(FONT_LOCATION,FONT_SIZE_3*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[3],SP_FONT_GROUP_ASCII,14823);//whole ASCII
  spFontAddBorder(font[3],48631);
  spFontReplaceColor(font[3],0,SP_ALPHA_COLOR);

  spFontAddButton( font[3], 'A', SP_BUTTON_A_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'B', SP_BUTTON_B_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'X', SP_BUTTON_X_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'Y', SP_BUTTON_Y_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'L', SP_BUTTON_L_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'R', SP_BUTTON_R_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
  spFontAddButton( font[3], 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

  spFontSetShadeColor(2016);
  if (font[4])
    spFontDelete(font[4]);
  font[4] = spFontLoad(FONT_LOCATION,FONT_SIZE_4*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[4],SP_FONT_GROUP_ASCII,0);//whole ASCII
  spFontAddBorder(font[4],2016);
  spFontReplaceColor(font[4],0,SP_ALPHA_COLOR);

  spFontSetShadeColor(31743);
  if (font[5])
    spFontDelete(font[5]);
  font[5] = spFontLoad(FONT_LOCATION,FONT_SIZE_5*spGetSizeFactor()>>SP_ACCURACY);
  spFontAdd(font[5],SP_FONT_GROUP_ASCII,0);//whole ASCII
  spFontAddBorder(font[5],31743);
  spFontReplaceColor(font[5],0,SP_ALPHA_COLOR);
  
  int i;
  for (i = 0; i < 6; i++)
    spFontMulWidth(font[i],spFloatToFixed(0.9f));
}

spFontPointer getFont(int number)
{
  return font[number];
}

void quitFont()
{
  int i;
  for (i=0;i<6;i++)
    spFontDelete(font[i]);
}
