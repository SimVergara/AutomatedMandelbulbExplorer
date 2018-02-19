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
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"

extern double getTime();

extern void   printProgress( double perc, double time );

//# pragma acc routine seq
//extern void rayMarch (const RenderParams &renderer_params, const vec3 &from, const vec3  &to, double eps, pixelData &pix_data, const MandelBoxParams &mandelBox_params);

#pragma acc routine seq
extern void rayMarch(float maxDistance, int maxRaySteps, const vec3 &from, const vec3  &direction, double eps, pixelData& pix_data, const double mBoxParams[]);

/*extern vec3 getColour(const pixelData &pix_data, const RenderParams &renderer_params,
  const vec3 &from, const vec3  &direction);
  */


inline void lighting(const vec3 &n, const vec3 &color, const vec3 &pos, const vec3 &direction,  vec3 &outV)
{ 
	//---lightning and colouring---------
	vec3 CamLight = {0.80, 0.850, 0.50};
	double CamLightW = 1.27536;// 1.27536;
	double CamLightMin = 0.58193;// 0.48193;

	vec3 nn = {n.x-1.0,
		n.y-1.0,
		n.z-1.0};

	double ambient = MAX( CamLightMin, DOT(nn,direction) )*CamLightW;

	VMUL(outV, CamLight, color);
	SMUL(outV, ambient);
}




vec3 renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, 
		unsigned char *image, const MandelBoxParams &mandelBox_params)
{
	const double eps = pow(10.0, renderer_params.detail); 
	double farPoint[3];
	vec3 to, from;

	SET_POINT(from, camera_params.camPos);

	const int height = renderer_params.height;
	const int width  = renderer_params.width;


	double time = getTime();
	int l, m;
	vec3 *to_matrix = (vec3*)malloc(sizeof(vec3)*height*width);
	pixelData **pix_data_matrix = (pixelData**)malloc(sizeof(pixelData*)*height);
	for (m = 0; m < height; m++){
		pix_data_matrix[m] = (pixelData*)malloc(sizeof(pixelData)*width);
		for(l = 0;  l < width; l++){
			UnProject(l, m, camera_params, farPoint);
			SUB_PTS(to_matrix[m*(width)+l], farPoint, camera_params.camPos);
			NORMALIZE(to_matrix[m*(width)+l]);
		}
	}

	int i, j;
  
  float maxDistance  = renderer_params.maxDistance;
  float maxRaySteps  = renderer_params.maxRaySteps;
  int colourType     = renderer_params.colourType;
  float brightness   = renderer_params.brightness;
  double rMin        = mandelBox_params.rMin;
  double rFixed      = mandelBox_params.rFixed;
  double escape_time = mandelBox_params.escape_time;
  double scale       = mandelBox_params.scale;
  double num_iter    = mandelBox_params.num_iter;
  vec3 camparam      = {camera_params.camPos[0],
                        camera_params.camPos[1],
                        camera_params.camPos[2]};

  vec3 camTarget     = {camera_params.camTarget[0],
                        camera_params.camTarget[1],
                        camera_params.camTarget[2]};

  vec3 newTarget     = {0.0,0.0,0.0};
  double targetDist  = 0.0;
  int location[2];


#pragma acc kernels pcopyin(camTarget, camparam, pix_data_matrix[0:height][0:width],\
                            to_matrix[0:height*width], colourType, brightness, maxRaySteps, \
                            maxDistance, from, eps, width, height, rMin, rFixed, num_iter, scale, \
                            escape_time) pcopy(location,image[0:width*height*3], newTarget, targetDist)
	{
    vec3 color;
    vec3 hitColor     = {0.0,0.0,0.0};

		const double mBoxParams[] = {rMin, rFixed, scale, escape_time, num_iter};
		//#pragma acc kernels copyin(to_matrix[0:height][0:width], image[0:width*height])
#pragma acc for independent collapse(2) private(hitColor, i, j)
		for(j = 0; j < height; j++)
		{
//#pragma acc for independent
			for(i = 0; i <width; i++)
			{

		//		const double mBoxParams[] = {rMin, rFixed, scale, escape_time, num_iter};

				// get point on the 'far' plane
				// since we render one frame only, we can use the more specialized method

				//render the pixel
				rayMarch(maxDistance, maxRaySteps, from, to_matrix[j*(width)+i], eps, pix_data_matrix[j][i], mBoxParams);

				//get the colour at this pixel
				vec3 baseColor = {0.4, 0.78, 0.86};
				vec3 backColor = {0.1, 0.09, 0.11};
				//colouring and lightning
				hitColor = baseColor;

				if (pix_data_matrix[j][i].escaped == false)
				{
					//apply lighting
					lighting(pix_data_matrix[j][i].normal, hitColor, pix_data_matrix[j][i].hit, to_matrix[j*(width)+i], hitColor);

					//add normal based colouring
					if(colourType == 0 || colourType == 1)
					{

						VMUL(hitColor, hitColor, pix_data_matrix[j][i].normal);

						ADD_POINT(hitColor, hitColor, 1.0);
						DIV_PT(hitColor, hitColor, 2.0);

						SMUL(hitColor, brightness);

						//gamma correction
						CLAMP(hitColor.x, 0.0, 1.0, hitColor.x);
						CLAMP(hitColor.y, 0.0, 1.0, hitColor.y);
						CLAMP(hitColor.z, 0.0, 1.0, hitColor.z);

						VMUL(hitColor, hitColor, hitColor);
					}
					if(colourType == 1)
					{
						//"swap" colors
						double t = hitColor.x;
						hitColor.x = hitColor.z;
						hitColor.z = t;
					}
          vec3 tmp;
          //SUBTRACT_VEC(tmp, camparam, camTarget);
          SUBTRACT_VEC(tmp, camparam,pix_data_matrix[j][i].hit);
          double m;
          MAGNITUDE(m, tmp);
          pix_data_matrix[j][i].dist = m;
          /*if (m>targetDist){
            targetDist = m;
            newTarget = pix_data_matrix[j][i].hit;
          }*/
				}
				else{
					//we have the background colour
					hitColor = backColor;
          			pix_data_matrix[j][i].dist = 0;
// if (pix_data_matrix[j][i+1].escaped || \
// 	pix_data_matrix[j][i+2].escaped || \
// 	pix_data_matrix[j][i+3].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped || \
// 	pix_data_matrix[j][i].escaped)

        		}

				//return hitColor;
				//color = hitColor;
				//color = getColour(pix_data_matrix[j][i], renderer_params, from, to_matrix[j][i]);

				//save colour into texture
				int k = (j*width+ i)*3;
				image[k+2] = (unsigned char)(hitColor.x * 255);
				image[k+1] = (unsigned char)(hitColor.y * 255);
				image[k]   = (unsigned char)(hitColor.z * 255);
			}
			//printProgress((j+1)/(double)height,getTime()-time);
		}

  //#pragma acc for independent collapse(2)
    for(j = height/5; j < 4*height/5; j++)
    {
    //#pragma acc for independent
      for(i = width/5; i <4*width/5; i++)
      {
        if (pix_data_matrix[j][i].dist>targetDist)
        {
          targetDist = pix_data_matrix[j][j].dist;
          newTarget = pix_data_matrix[j][i].hit;
          location[0]=i;
          location[1]=j;
        }
      }
    }

	}

	printf("\n rendering done: target at i%d j%d\n", location[0],location[1]);

  return newTarget;
}

