/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015-2016 at McMaster University.

    Copyright (C) 2015-2016 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>
#include "camera.h"
#include "renderer.h"
#include "mandelbox.h"

#include "vector3d.h"
void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
       MandelBoxParams *mandelBox_paramsP);
void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
vec3 renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, unsigned char* image, const MandelBoxParams &params);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);


void changeparams(int i, MandelBoxParams *mandelBox_paramsP, RenderParams *renderer_params, CameraParams *camera_params, vec3 newTarget, int startFrame);

MandelBoxParams mandelBox_params;

int main(int argc, char** argv)
{
  CameraParams    camera_params;
  RenderParams    renderer_params;

  vec3 newTarget = {0.0,0.0,0.0};
  int numFrames;
  numFrames = argc >= 3? atoi(argv[2]): 1;

  int startFrame;
  startFrame = argc >= 4? atoi(argv[3]): 0;

  int i;
  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);
  int image_size = renderer_params.width * renderer_params.height;
  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
  
  for (i=0;i<numFrames;i++){
  

  init3D(&camera_params, &renderer_params);

  newTarget = renderFractal(camera_params, renderer_params, image, mandelBox_params);
  //printf("x=%f, y=%f, z=%f\n", newTarget.x, newTarget.y, newTarget.z);
  changeparams(i, &mandelBox_params, &renderer_params, &camera_params, newTarget, startFrame);

  saveBMP(renderer_params.file_name, image, renderer_params.width, renderer_params.height);

  //system("./convert.sc");
  printf("image %s\n", renderer_params.file_name);
  printf("CamPos: <%01.4f,%01.4f,%01.4f>\n", camera_params.camPos[0],
                                             camera_params.camPos[1],
                                             camera_params.camPos[2]);

  printf("LookAt: <%01.4f,%01.4f,%01.4f>\n", camera_params.camTarget[0],
                                             camera_params.camTarget[1],
                                             camera_params.camTarget[2]);

  printf("NewTarget: <%01.4f,%01.4f,%01.4f>\n\n------------\n", newTarget.x,
                                                                newTarget.y,
                                                                newTarget.z);

  }
  free(image);

  //system("./video.sc");
  return 0;
}




void changeparams(int i, MandelBoxParams *mandelBox_paramsP, RenderParams *renderer_params, CameraParams *camera_params, vec3 newTarget, int startFrame)
{
  //change power
  mandelBox_paramsP->rFixed = mandelBox_paramsP->rFixed + 1.0/10000.00;//10.0;

//if (i%2==0){
  vec3 tempTarg;
  tempTarg.x=camera_params->camTarget[0]-(newTarget.x);
  tempTarg.y=camera_params->camTarget[1]-(newTarget.y);
  tempTarg.z=camera_params->camTarget[2]-(newTarget.z);

//change camera angle
  camera_params->camTarget[0] -= (1.8/100.0)*tempTarg.x;
  camera_params->camTarget[1] -= (1.8/100.0)*tempTarg.y;
  camera_params->camTarget[2] -= (1.8/100.0)*tempTarg.z;
//}

  vec3 tempPos;
  tempPos.x=camera_params->camPos[0]-(newTarget.x);
  tempPos.y=camera_params->camPos[1]-(newTarget.y);
  tempPos.z=camera_params->camPos[2]-(newTarget.z);


  //change position
  camera_params->camPos[0] -= (1.030/100.0)*tempPos.x;
  camera_params->camPos[1] -= (1.030/100.0)*tempPos.y;
  camera_params->camPos[2] -= (1.030/100.0)*tempPos.z;

  //starting frame
  i+=startFrame;
  sprintf(renderer_params->file_name, "img%05.0f.bmp", double(i));

}
