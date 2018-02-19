#ifndef vec3_h
#define vec3_h

#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif


typedef struct 
{
  double x, y, z;
}  vec3;

#define SET_VEC(p,v) { p.x=v.x; p.y=v.y; p.z=v.z; }

#define SET_POINT(p,v) { p.x=v[0]; p.y=v[1]; p.z=v[2];}

#define SUBTRACT_VEC(p,v,u)		\
  {								\
  p.x=(v.x)-(u.x);				\
  p.y=(v.y)-(u.y);				\
  p.z=(v.z)-(u.z);				\
}

#define ADD_VEC(p,v,u)			\
{								\
	p.x=(v.x)+(u.x);			\
	p.y=(v.y)+(u.y);			\
	p.z=(v.z)+(u.z);			\
}

#define ADD_POINT(p,v,d)	\
{							\
	p.x=v.x+d; 				\
	p.y=v.y+d; 				\
	p.z=v.z+d;				\
}

#define DIV_PT(p,v,d) 		\
{ 							\
	double invd = 1.0/d; 	\
	p.x=v.x*invd; 				\
	p.y=v.y*invd; 				\
	p.z=v.z*invd;				\
}

#define SUB_PTS(p,v,u)				\
  {									\
  p.x=(v[0])-(u[0]);				\
  p.y=(v[1])-(u[1]);				\
  p.z=(v[2])-(u[2]);				\
}

#define NORMALIZE(p) {								\
    double fMag = ( p.x*p.x + p.y*p.y + p.z*p.z );	\
    if (fMag != 0)									\
      {												\
	double fMult = 1.0/sqrt(fMag);					\
	p.x *= fMult;									\
	p.y *= fMult;									\
	p.z *= fMult;									\
      }												\
  }


#define MAGNITUDE(m,p) 	({ m=sqrt( p.x*p.x + p.y*p.y + p.z*p.z ); })

#define DOT(d,p)   ( p.x*d.x + p.y*d.y + p.z*d.z )

#define MAX(a,b) ( ((a)>(b))? (a):(b))

#define VEC(v,a,b,c) { v.x = a; v.y = b; v.z = c; }

#define SMUL(v,s) {v.x=v.x*s; v.y=v.y*s; v.z=v.z*s;}

#define VMUL(p,v,u) \
{					\
	p.x=v.x*u.x; 	\
	p.y=v.y*u.y; 	\
	p.z=v.z*u.z;	\
}


#define CLAMP(d, min, max, out) \
{                                       \
        double t = d < min ? min : d;;    \
        out = t > max ? max : t;    \
}


inline void clamp(vec3 &v, double min, double max) 
{
  CLAMP(v.x,min,max,v.x);
  CLAMP(v.y,min,max,v.y);
  CLAMP(v.z,min,max,v.z);
}

#endif
