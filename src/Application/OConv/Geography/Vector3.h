// Original File: Vector3.h
// Author: Rodolfo Mora Z.
// Date: May 19, 2015
// Purpose: Simple TriDimentional Vector Class
// https://github.com/CNCA-CeNAT/kriging/blob/master/src/Vector3.h

#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using namespace std;

class vector3
{
public:
	double x;
	double y;
	double z;

	// Constructors
	//Default
	vector3();

	vector3(double x, double y, double z);
	// Copies another
	vector3(vector3 * v);

	// Default values
	static vector3 zero(){return vector3(0,0,0);};
	static vector3 forward(){return vector3(0,0,1);};
	static vector3 up(){return vector3(0,1,0);};
	static vector3 right(){return vector3(1,0,0);};
	static vector3 one(){return vector3(1,1,1);};

	// Printing
	string toString();
	void print();

	vector3 operator+(vector3 other);
	vector3 operator-();
	vector3 operator-(vector3 other);
	vector3 operator*(double scalar);
	double operator*(vector3 other);

	double magnitude();
	static double distance(vector3 a, vector3 b){return (a-b).magnitude();};
};

vector3::vector3()
{
	x = 0;
	y =0;
	z = 0;
}

vector3::vector3(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vector3::vector3(vector3 * v)
{
	this->x = v->x;
	this->y = v->y;
	this->z = v->z;
}

vector3 vector3::operator+(vector3 other)
{
	return vector3(x+other.x,y+other.y,z+other.z);
}

vector3 vector3::operator-()
{
	return vector3(-x,-y,-z);
}

vector3 vector3::operator-(vector3 other)
{
	return vector3(x-other.x,y-other.y,z-other.z);
}

vector3 vector3::operator*(double scalar)
{
	return vector3(x*scalar,y*scalar,z*scalar);
}

double vector3::operator*(vector3 other)
{
	return x*other.x + y*other.y + z*other.z;
}

string vector3::toString()
{
	std::stringstream s;
	s.precision(2);
	s << "(" << x << "," << y << "," << z << ")" << endl;
	return s.str();
}

void vector3::print()
{
	std::cout << toString() << endl;
}

double vector3::magnitude()
{
	double x2 = x*x;
	double y2 = y*y;
	double z2 = z*z;
	double p = x2+y2+z2;
	return sqrt(p);
}

#endif /* VECTOR3_H_ */
