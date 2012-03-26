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

#include "trace.h"

pTracePoint trace[16] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
Sint32 traceColor[16];

int traceNr = 0;

void addTracePoint(Sint32 x,Sint32 y)
{
  if (trace[traceNr] && spMul(trace[traceNr]->x-x,trace[traceNr]->x-x)+spMul(trace[traceNr]->y-y,trace[traceNr]->y-y) < spMul(TRACE_LINE_LENGTH,TRACE_LINE_LENGTH))
    return;
  pTracePoint point = (pTracePoint)malloc(sizeof(tTracePoint));
  point->x = x;
  point->y = y;
  point->next = trace[traceNr];
  trace[traceNr] = point;
}

void deleteTrace(int traceNr)
{
  while (trace[traceNr])
  {
    pTracePoint next = trace[traceNr]->next;
    free(trace[traceNr]);
    trace[traceNr] = next;
  }
}

void nextTrace()
{
  traceNr = (traceNr + 1) & 15;
  deleteTrace(traceNr);
  Sint32 h = rand() % (2*SP_PI);
  Sint32 s = rand() & 255;
  Sint32 v = (rand() & 127)+128;
  traceColor[traceNr] = spGetHSV(h,s,v);
}

void drawTracePoints()
{
  int traceNr;
  for (traceNr = 0; traceNr < 16; traceNr ++)
  {
    if (trace[traceNr] == NULL)
      continue;
    Sint32 ox = trace[traceNr]->x;
    Sint32 oy = trace[traceNr]->y;
    pTracePoint point = trace[traceNr]->next;
    while (point)
    {
      spLine3D(point->x,point->y,0,ox,oy,0,traceColor[traceNr]);
      ox = point->x;
      oy = point->y;
      point = point->next;
    }
  }
}

void deleteAllTracePoints()
{
  int i;
  for (i = 0;i<16;i++)
    deleteTrace(i);
  traceNr = 15;
  nextTrace();
}
