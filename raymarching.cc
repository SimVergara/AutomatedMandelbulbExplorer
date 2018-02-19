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
#include <assert.h>
#include <algorithm>
#include <stdio.h>
#include "mandelbox.h"
#include "color.h"
#include "renderer.h"
//#include "distance_est.h"
#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif

//extern double DE(const vec3 &p);

inline double MandelBulbDistanceEstimator(const vec3 &p0, const MandelBoxParams &mandelBox_params)
{
	vec3 z;
	z = p0;

	double dr = 1.0;
	double r = 0.0;

	double Bailout = mandelBox_params.rMin;
	double Power = mandelBox_params.rFixed;

	for (int i=0; i < mandelBox_params.num_iter; i++)
	{
		MAGNITUDE(r,z);
		if(r > Bailout) break;

		double theta = acos(z.z/r);
		double phi   = atan2(z.y, z.x);
		dr = pow(r, Power - 1.0) * Power * dr + 1.0;

		double zr = pow(r, Power);
		theta     = theta * Power;
		phi       = phi * Power;

		z.x = zr*sin(theta)*cos(phi);
		z.y = zr*sin(phi)*sin(theta);
		z.z = zr*cos(theta);

		z.x = z.x + p0.x;
		z.y = z.y + p0.y;
		z.z = z.z + p0.z;
	}

	return 0.5*log(r)*r/dr;

}
/*
inline void pix_data.normal(const vec3 & normPos, vec3 & normal, double rMin, double rFixed, double num_iter)
{
	// compute the pix_data.normal at p
	const double sqrt_mach_eps = 1.4901e-08;

	double m;
	MAGNITUDE(m,p);
	double eps = MAX( m, 1.0 )*sqrt_mach_eps;

	vec3 e1 = {eps, 0,   0};
	vec3 e2 = {0  , eps, 0};
	vec3 e3 = {0  , 0, eps};

	vec3 ppe1, ppe2, ppe3, pme1, pme2, pme3;
	ADD_VEC(ppe1, normPos, e1);
	ADD_VEC(ppe2, normPos, e2);
	ADD_VEC(ppe3, normPos, e3);

	SUBTRACT_VEC(pme1, normPos, e1);
	SUBTRACT_VEC(pme2, normPos, e2);
	SUBTRACT_VEC(pme3, normPos, e3);


	double DEst[3];
	DEst[0] = MandelBulbDistanceEstimator(ppe1, mBox_params) - MandelBulbDistanceEstimator(pme1, mBox_params);//DE(ppe1) - DE(pme1);
	DEst[1] = MandelBulbDistanceEstimator(ppe2, mBox_params) - MandelBulbDistanceEstimator(pme2, mBox_params);//DE(ppe2) - DE(pme2);
	DEst[2] = MandelBulbDistanceEstimator(ppe3, mBox_params) - MandelBulbDistanceEstimator(pme3, mBox_params);//DE(ppe3) - DE(pme3);

	SET_POINT(pix_data.normal, DEst);

	NORMALIZE(pix_data.normal);
}*/


#pragma acc routine seq
void rayMarch(float maxDistance, int maxRaySteps, const vec3 &from, const vec3  &direction, double eps,
		pixelData& pix_data, const double mBox_params[])
{

	double dist = 0.0;
	double totalDist = 0.0;

	const MandelBoxParams mandelBox_params = {mBox_params[0], mBox_params[1], mBox_params[2], mBox_params[3], mBox_params[4]};

	// We will adjust the minimum distance based on the current zoom

	double epsModified = 0.0;

	int steps=0;
	vec3 p;

	do 
	{      
		SET_VEC(p,direction);
		SMUL(p,totalDist);
		ADD_VEC(p,p,from);

		dist = MandelBulbDistanceEstimator(p, mandelBox_params);

		totalDist += .95*dist;

		epsModified = totalDist;
		epsModified*=eps;
		steps++;
	}
	while (dist > epsModified && totalDist <= maxDistance && steps < maxRaySteps);

	//  vec3 hitNormal;
	if (dist < epsModified) 
	{
		//we didnt escape
		pix_data.escaped = false;

		// We hit something, or reached MaxRaySteps
		pix_data.hit = p;

		//figure out the normal of the surface at this point
		vec3 normPos;
		SET_VEC(normPos, direction);
		SMUL(normPos, epsModified);
		SUBTRACT_VEC(normPos, p, normPos);

		//normal(normPos, pix_data.normal, mBox_params);
		const double sqrt_mach_eps = 1.4901e-08;

		double m;
		MAGNITUDE(m,p);
		double eps = MAX( m, 1.0 )*sqrt_mach_eps;

		vec3 e1 = {eps, 0,   0}; 
		vec3 e2 = {0  , eps, 0}; 
		vec3 e3 = {0  , 0, eps};

		vec3 ppe1, ppe2, ppe3, pme1, pme2, pme3;
		ADD_VEC(ppe1, normPos, e1);
		ADD_VEC(ppe2, normPos, e2);
		ADD_VEC(ppe3, normPos, e3);

		SUBTRACT_VEC(pme1, normPos, e1);
		SUBTRACT_VEC(pme2, normPos, e2);
		SUBTRACT_VEC(pme3, normPos, e3);


		double DEst[3];
		DEst[0] = MandelBulbDistanceEstimator(ppe1, mandelBox_params) - MandelBulbDistanceEstimator(pme1, mandelBox_params);//DE(ppe1) - DE(pme1);
		DEst[1] = MandelBulbDistanceEstimator(ppe2, mandelBox_params) - MandelBulbDistanceEstimator(pme2, mandelBox_params);//DE(ppe2) - DE(pme2);
		DEst[2] = MandelBulbDistanceEstimator(ppe3, mandelBox_params) - MandelBulbDistanceEstimator(pme3, mandelBox_params);//DE(ppe3) - DE(pme3);

		SET_POINT(pix_data.normal, DEst);

		NORMALIZE(pix_data.normal);    
	}
	else 
		//we have the background colour
		pix_data.escaped = true;

}


