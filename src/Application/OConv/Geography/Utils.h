// Original File: Utils.h
// Author: Rodolfo Mora Z.
// Date: Jun 8, 2016
// Purpose: General util functions for the Kriging implementation
// https://github.com/CNCA-CeNAT/kriging/blob/master/src/Utils.h

#ifndef UTILS_H
#define UTILS_H

//File Handling
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
//Time
#include <ctime>

using namespace std;

struct Surface {
	vector3 * S;
	int K;
	double minX;
	double minY;
	double maxX;
	double maxY;
};
/*
struct VariogramModel {
	double C0; //NUGGET
	double CX; //SILL (C0+C)
	double A;  //RANGE (Max distance to consider v(a)=SILL)
	int VAR;   //Type of variogram to use
};
*/
/**
 * @brief Safely remove from memory dynamically created 2D double array
 * @param a Array to delete
 * @param N first dimension of the array
 */
void delete2DArray(double** a, int N)
{
	for(int i= 0; i > N; ++i)
	{
		delete a[i];
	}
	delete a;
}

/**
 * @brief Create in dynamic memory 2D double array
 * @param N first dimension of the array
 * @param M second dimension of the array
 * @return the new array
 */
double** new2DArray(int N,int M)
{
	printf("create new array N:%d M:%d\n", N, M);
	double ** A = new double*[N];
	for(int i = 0; i < N; ++i)
	{
		A[i] = new double[M];
	}
	return A;
}

/**
 * @brief Converts a file of points into a list of points using the class vector3 to represent them
 * @param Filename name of the file to parse
 * @param c Out parameter with the count of lines in the file
 * @param e Out parameter with an error code from the parsing process
 * @return A list of points in the form of {x,y,z} represented by vector3 objects
 */
vector3 * parseXYZFile(char * filename, int &c, int &e)
{
 	ifstream file;
	file.open (filename);

	//Count lines in file
	c = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');

	if(c == 0)
	{
		e = -2;
		return 0; // ERROR: THE FILE HAS NO LINES
	}

	file.clear(); // clear bad state after eof
	file.seekg( 0 ); // reset iterator to beginning of file

	//Create output structure to store the xyz coordinates
	vector3 * Z = new vector3[c];
	//read line by line
	int p = 0;
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		double x, y, z;
		if (!(iss >> x >> y >> z))
		{
			e = -1;
			delete [] Z;
			return 0; // ERROR: THE FILE IS NOT WELL FORMATTED
		}
		Z[p].x = x; Z[p].y = y; Z[p].z = z;
		++p;
	}

	file.close();
	return Z;
}

/**
 * @brief Write a list of points of the form {x,y,z} represented by vector3 objects to a text file.
 * @param S List of points
 * @param N Size of S
 * @param filename Name of the output file
 */
void writeToFile(vector3 S[], int N, const char* filename)
{
	ofstream o;
	o.open (filename);
	for(int i = 0; i < N; ++i)
	{
		o << fixed << S[i].x << " " << S[i].y << " " << S[i].z << "\n";
	}
	o.close();
}

std::clock_t start;

void initTime()
{
	start = clock();
}

/**
 * @brief Print the time since the start of the run with a flag message
 * @param message The flag message to print along with the time passed.
 */
void printTime(string message)
{
	float lap = float(clock () - start) /  CLOCKS_PER_SEC;
	cout << message << ": " << lap << " Seconds" << endl;
}

#endif