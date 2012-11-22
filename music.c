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

#include "music.h"
#include "font.h"
Mix_Music *music;
char audio_failed;
int volume;
int show_music;

void init_music()
{
  music = NULL; 
  volume = 50;
  show_music = 0;
  printf("Try to open Audio...\n");
  if (!
  #ifdef F100
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,256)
  #else
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048)
  #endif
  )
  {
    printf("Success\n");
    audio_failed = 0;
    music = Mix_LoadMUS("./data/space explorers.ogg");
    Mix_PlayMusic(music,-1);
    set_volume(volume);
  }
  else
  {
    printf("Failed\n");
    audio_failed = 1;
    return;
  }
}


void set_volume(int volume)
{
  volume = volume * 128 / 100;
  if (audio_failed)
    return;
  Mix_VolumeMusic(volume);
}

int volume_wait = 0;

void draw_music()
{
  if (show_music > 0)
  {
    SDL_Surface *screen = spGetWindowSurface();
    char buffer[256];
    sprintf(buffer,"Vol: %i%%",volume);
    spFontDrawMiddle(screen->w/2,screen->h/2,-1,buffer,getFont(0));  
  }
}

void calc_music(int steps)
{ 
  int i;
  for (i = 0;i<steps;i++)
  {
    show_music--;
    if (show_music < 0)
      show_music = 0;
    volume_wait--;
    if (volume_wait < 0)
      volume_wait = 0;
    /*if (spGetInput()->button[SP_BUTTON_VOLPLUS] && volume_wait<=0)
    {
      if (volume<100)
        volume++;
      set_volume(volume);
      volume_wait = 25;
      show_music = 1000;
    }
    if (spGetInput()->button[SP_BUTTON_VOLMINUS] && volume_wait<=0)
    {
      if (volume>0)
        volume--;
      set_volume(volume);
      volume_wait = 25;
      show_music = 1000;
    }*/
  }
}

void quit_music()
{
  if (audio_failed)
    return;
  Mix_FreeMusic(music);
  Mix_CloseAudio();
}

