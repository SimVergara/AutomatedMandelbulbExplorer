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
#include "vector3d.h"
#include "mandelbox.h"
#include "distance_est.h"


extern MandelBoxParams mandelBox_params;
extern double MandelBoxDE(const vec3 &pos, const MandelBoxParams &mPar, double c1, double c2);
extern double MandelBulbDistanceEstimator(const vec3 &p0, const MandelBoxParams &params);

double DE(const vec3 &p)
{
 double d = MandelBulbDistanceEstimator(p, mandelBox_params);
  return d;
}
