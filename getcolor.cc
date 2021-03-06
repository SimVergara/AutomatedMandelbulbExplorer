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
#include "color.h"
#include "renderer.h"
#include "vector3d.h"
#include <cmath>
#include <algorithm>

using namespace std;


inline void lighting(const vec3 &n, const vec3 &color, const vec3 &pos, const vec3 &toMatrix[i][j],  vec3 &outV)
{
  //---lightning and colouring---------
  vec3 CamLight = {1.0, 1.0, 1.0};
  double CamLightW = 1.8;// 1.27536;
  double CamLightMin = 0.3;// 0.48193;
 
  vec3 nn = {n.x-1.0, 
             n.y-1.0,
             n.z-1.0};

  double ambient = MAX( CamLightMin, DOT(nn,toMatrix[i][j]) )*CamLightW;
  
  VMUL(outV, CamLight, color);
  SMUL(outV, ambient);
}

vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
	       const vec3 &from, const vec3  &toMatrix[i][j])
{
  vec3 baseColor = {0.05, 0.7, 0.85};
  vec3 backColor = {0.4, 0.4, 0.4};
  //colouring and lightning
  vec3 hitColor = baseColor;
 
  if (pixData.escaped == false) 
  {
    //apply lighting
    lighting(pixData.normal, hitColor, pixData.hit, toMatrix[i][j], hitColor);

    //add normal based colouring
    if(render_params.colourType == 0 || render_params.colourType == 1)
    {
      
      VMUL(hitColor, hitColor, pixData.normal);
      
      ADD_POINT(hitColor, hitColor, 1.0);
      DIV_PT(hitColor, hitColor, 2.0);
      
      SMUL(hitColor, render_params.brightness);

      //gamma correction
      CLAMP(hitColor.x, 0.0, 1.0, hitColor.x); 
      CLAMP(hitColor.y, 0.0, 1.0, hitColor.y);
      CLAMP(hitColor.z, 0.0, 1.0, hitColor.z);
      
      VMUL(hitColor, hitColor, hitColor);
    }
    if(render_params.colourType == 1)
    {
      //"swap" colors
      double t = hitColor.x;
      hitColor.x = hitColor.z;
      hitColor.z = t;
    }
  }
  else 
    //we have the background colour
    hitColor = backColor;
  
  return hitColor;
}

getColour(pix_data, renderer_params, from, toMatrix[j][i]);
