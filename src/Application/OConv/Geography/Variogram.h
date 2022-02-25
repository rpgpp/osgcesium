// Original File: Variogram.h
// Author: Rodolfo Mora Z.
// Date: Jun 8, 2016
// Purpose: Multiple semivariogram models for Kriging Implementation
// https://github.com/CNCA-CeNAT/kriging/blob/master/src/Variogram.h

#ifndef VARIOGRAM_H
#define VARIOGRAM_H

//Standard and Math
#include <stdlib.h>
#include <math.h>

#define SPHERICAL 0
#define EXPONENTIAL 1
#define GAUSSIAN 2
#define WAVE 3
#define RATIONAL_Q 4
#define CIRCULAR 5
#define M_PI 3.1415926

struct VariogramModel {
	double C0; //NUGGET
	double CX; //SILL (C0+C)
	double A;  //RANGE (Max distance to consider v(a)=SILL)
	int VAR;   //Type of variogram to use
};

/**
 * @brief Spherical Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double sphericalVariogram(double H, double C0, double CX, double A)
{
	if(H > A)
	{
		return CX;
	}
	else
	{
		return C0 + ((CX-C0) * ((3*H)/(2*A))-(H*H*H)/(2*A*A*A));
	}
}

/**
 * @brief Exponential Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double exponentialVariogram(double H, double C0, double CX, double A)
{
	return C0 + ((CX-C0) * (1-exp((-H/A))));
}

/**
 * @brief Gaussian Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double gaussianVariogram(double H, double C0, double CX, double A)
{
	double e = (H/A);
	e = e*e;
	return C0 + ((CX-C0) * (1-exp(-e)));
}

/**
 * @brief Wave Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double waveVariogram(double H, double C0, double CX, double A)
{
	double e = H/A;
	return C0 + ((CX-C0) * (1-(sin(e)/e)));
}

/**
 * @brief Rational Quadratic Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double rationalQuadraticVariogram(double H, double C0, double CX, double A)
{
	double e = (H*H)/(A*A);
	return C0 + ((CX-C0) * (e/(1+e)));
}

/**
 * @brief Circular Semivariogram Model based on http://spatial-analyst.net/ILWIS/htm/ilwisapp/sec/semivar_models_sec.htm
 * @param H distance between points to covariate
 * @param C0 nugget
 * @param CX sill: C0+C
 * @param A range
 * @return The output of the semivariogram determines how much impact will have the sample in the stimation of the predictants value
 */
double circularVariogram(double H, double C0, double CX, double A)
{
	if(H > A)
	{
		return CX;
	}
	else
	{
		double e = H/A;
		double p = 2/M_PI;
		double r = sqrt(1-e*e);
		return C0 + ((CX-C0) * (1-p*acos(e)+p*e*r));
	}
}

/** 
  * A, B arrays of points
  *	A/B[i] is the form of {x,y,z}
  *	z might be unknown
  * startA/startB is the first row to calculate, it is assumed that start >= 0
  * endA/endB is the last row to calculate, it is assumed that endA/endB < sizeA/sizeB,
  */
  
/**
 * @brief Calculate the mutual impact between the points of B and the points of A based on their planar xy coordinates
 * @param A sample points in the form of {x,y,z}
 * @param B predictant points in the form of {x,y,z}
 * @param M size of A
 * @param N size of B
 * @param startA first row to calculate, it is assumed that startA >= 0
 * @param endA last row to calculate, it is assumed that endA < M
 * @param startB first row to calculate, it is assumed that startB >= 0
 * @param endB last row to calculate, it is assumed that endB < N
 * @param v semivariogram model to apply
 * @return matrix D with the semivariogram outputs for each pair of points (A,B)
 */
matrix calculateVariogram(vector3 A[], vector3 B[], int M, int N, int startA, int endA, int startB, int endB, VariogramModel v)
{
	matrix D = matrix(M,N);
	for(int a = startA; a < endA; ++a)
	{
		for(int b = startB; b < endB; ++b)
		{
			double d = vector3::distance(A[a],B[b]);
			if(v.VAR == SPHERICAL)
			{
				D.set(a,b,sphericalVariogram(d, v.C0, v.CX, v.A));
			}
			else if(v.VAR == EXPONENTIAL)
			{
				D.set(a,b,exponentialVariogram(d, v.C0, v.CX, v.A));
			}
			else if(v.VAR == GAUSSIAN)
			{
				D.set(a,b,gaussianVariogram(d, v.C0, v.CX, v.A));
			}
			else if(v.VAR == WAVE)
			{
				D.set(a,b,waveVariogram(d, v.C0, v.CX, v.A));
			}
			else if(v.VAR == RATIONAL_Q)
			{
				D.set(a,b,rationalQuadraticVariogram(d, v.C0, v.CX, v.A));
			}
			else if(v.VAR == CIRCULAR)
			{
				D.set(a,b,circularVariogram(d, v.C0, v.CX, v.A));
			}
		}
	}
	return D;
}

/**
 * @brief Calculate the impact impact between a single point A and a block of points B based on their planar xy coordinates  
 * @param A the predictant point
 * @param B the sample points
 * @param M size of B
 * @param D output matrix
 * @param v variogram model
 * @return a matrix of size 1xM with the variogram outputs of the model between A and every point in B.
 */
matrix calculateVariogram(vector3 A, vector3 B[], int M, matrix & D, VariogramModel v)
{
	for(int b = 0; b < M; ++b)
	{
		double d = vector3::distance(A,B[b]);
		if(v.VAR == SPHERICAL)
		{
			D.set(b,0,sphericalVariogram(d, v.C0, v.CX, v.A));
		}
		else if(v.VAR == EXPONENTIAL)
		{
			D.set(b,0,exponentialVariogram(d, v.C0, v.CX, v.A));
		}
		else if(v.VAR == GAUSSIAN)
		{
			D.set(b,0,gaussianVariogram(d, v.C0, v.CX, v.A));
		}
		else if(v.VAR == WAVE)
		{
			D.set(b,0,waveVariogram(d, v.C0, v.CX, v.A));
		}
		else if(v.VAR == RATIONAL_Q)
		{
			D.set(b,0,rationalQuadraticVariogram(d, v.C0, v.CX, v.A));
		}
		else if(v.VAR == CIRCULAR)
		{
			D.set(b,0,circularVariogram(d, v.C0, v.CX, v.A));
		}
	}
	return D;
}


#endif