//Standard Output
//#include "ShpDrawer.h"
#include <stdio.h>
//Matrix Handling
#include "Matrix.h"
#include "Vector3.h"
//Local Files
#include "Utils.h"
#include "Variogram.h"

using namespace std;

/**
 * @brief Calculate the kriging interpolation for a given block of known points given a target resolution
 * @param Z List of known points, the samples, each with the form {x,y,z}
 * @param N size of Z
 * @param RX target horizontal resolution of the interpolated surface
 * @param RY target vertical resolution of the interpolated surface
 * @param VAR semivariogram model to apply
 * @return a Surface with the interpolated points in the target resolution
 */
Surface kriging(vector3 Z[], int N, double RX, double RY, VariogramModel VAR)
{
	//start = clock();
	initTime();
	Surface S;

	//Get BOUNDS of data minX, maxX, minY, maxY
	double minX = 1000000000, maxX = -1000000000, minY = 1000000000, maxY = -1000000000;
	for(int i = 0; i < N; ++i)
	{
		if(Z[i].x < minX) minX = Z[i].x;
		if(Z[i].x > maxX) maxX = Z[i].x;
		if(Z[i].y < minY) minY = Z[i].y;
		if(Z[i].y > maxY) maxY = Z[i].y;
	}
	S.minX = minX;
	S.minY = minY;
	S.maxX = maxX;
	S.maxY = maxY;

	//Calculate target size in coordinates
	double width = maxX - minX;
	double height = maxY - minY;
	//Calculate target size in points given the spatial resolutions RX, RY, width and height of the target set
	//The size of the target set must include the boundaries of the set given by the known points, that's why an extra row and column are added
	int sizeX = (int)(floor(width / RX)) + 1;
	int sizeY = (int)(floor(height / RY)) + 1;

	cout << "SURFACE DIMENSIONS IN UNITS: " << sizeX << " X " << sizeY << endl;

	S.K = sizeX * sizeY;

	//Create target array and calculate its coordinates
	S.S = new vector3[S.K];

	for(int x = 0; x < sizeX; ++x)
	{
		double coordX = minX + x*RX;
		for(int y = 0; y < sizeY; ++y)
		{
			double coordY = minY + y*RY;
			S.S[x*sizeY + y].x = coordX;
			S.S[x*sizeY + y].y = coordY;
		}
	}

	printTime("PREPARATION");

	//Calculate covariance from each known point to each other
	matrix V = calculateVariogram(Z, Z, N, N, 0, N, 0, N, VAR);

	printTime("Z VARIOGRAM");

	V.invert();
	printTime("V INVERTED");

	//Calculate S
	int n = S.K;
	int q = n/100;
	int p = q;
	for(int i = 0; i < S.K; ++i)
	{
		matrix D = matrix(N,1);
		if(q != 0 && i > p)
		{
			p += q;
			int prog = p/q;
			std::cout << "\r" << prog - 1 << "% completed: ";
			std::cout.flush();
		}
		//Calculate Variogram Model for point i
		calculateVariogram(S.S[i], Z, N, D, VAR);
		matrix W = V.I->multiply(D);
		S.S[i].z = 0;
		for(int n = 0; n < N; ++n)
		{
			S.S[i].z += W(n,0) * Z[n].z;
			//printf("W: %f Z: %f Zo: %f S: %f\n", W[n*N+k], Z[n][2], W[n*N+k] * Z[n][2], S.S[k][2]);
		}
	}

	printTime("KRIGING");
	return S;
}

////////////////////////////////////////////////////

////////////////////////////////////////////////////

/**
 * @brief Main method of the Kriging program, parses a file and calculates the missing points in the surface described by the target resolution
 * @param argc argument count
 * @param argv argument values, in order: filename [nugget sill range variogram resolutionX resolutionY]
 * @return error code, 0 if no errors ocurred
 */
int main1(int argc, char **argv)
{
	VariogramModel v;
	v.C0 = 0.1;
	v.CX = 8.5;
	v.A = 25;
	v.VAR = GAUSSIAN;

	// Test if any arguments were sent, print usage otherwise
	if(argc < 2)
	{
		printf("USAGE: Kriging xyz_file [nugget sill range variogram]\n"); //TODO add to parameters the target resolution values Rx and Ry
		return 0;
	}
	// Check if nugget is sane and store, exit with error otherwise
	if(argc > 2)
	{
		v.C0 = atof(argv[2]);
		if(v.C0 == 0)
		{
			printf("\nNUGGET CANNOT BE 0\n\n");
			return 1;
		}
	}
	// Save Sill
	if(argc > 3)
	{
		v.CX = atof(argv[3]);
	}
	// Save Range
	if(argc > 4)
	{
		v.A = atof(argv[4]);
	}
	// Save Var
	if(argc > 5)
	{
		v.VAR = atoi(argv[5]);
	}
	//TODO capture the target resolution for the final surface Rx and Ry

	printf("FILENAME: %s\n", argv[1]);

	//Load file from disk given the route of the file in ARGV[1]
	int c = 0;
	int e = 0;
	vector3 * Z = parseXYZFile(argv[1], c, e);
	if(e == -1)
	{
		printf("\nINPUT FILE IS INCORRECT\n\n");
		return 1;
	}
	else if(e == -2)
	{
		printf("\nINPUT FILE HAS NO LINES\n\n");
		return 1;
	}

	cout << c << endl;

	//TODO send as parameter the target resolution Rx and Ry instead of 1 1
	Surface S = kriging(Z, c, 1, 1, v);

	stringstream ss;
	ss << "Kriging-" << argv[1] << "-" << v.C0 << "-" << v.CX << "-" << v.A << ".xyz";
	writeToFile(S.S, S.K, ss.str().c_str());
	printTime("FILE: - " + ss.str());

//	printf("\n");
//
//	for(int s = 0; s < S.K; ++s)
//	{
//		printf("X: %f Y: %f Z: %f\n", S.S[s][0], S.S[s][1], S.S[s][2]);
//	}

	//delete2DArray(S.S,S.K);
	//delete2DArray(Z,c);

	return 0;
}

void TestKriging()
{
	std::vector<vector3> data;
	data.push_back(vector3(192.280, 369.342, 68.939));
	data.push_back(vector3(187.156, 314.564, 68.913));
	data.push_back(vector3(185.217, 323.621, 68.613));
	data.push_back(vector3(182.537, 333.906, 68.346));
	data.push_back(vector3(180.822, 342.942, 68.121));
	data.push_back(vector3(179.144, 352.310, 67.939));
	data.push_back(vector3(176.355, 362.808, 67.806));
	data.push_back(vector3(174.503, 371.384, 67.724));
	data.push_back(vector3(197.378, 345.207, 66.320));
	data.push_back(vector3(193.004, 354.025, 66.820));
	data.push_back(vector3(192.280, 369.342, 68.939));
	data.push_back(vector3(187.156, 314.564, 68.913));
	data.push_back(vector3(185.217, 323.621, 68.613));
	data.push_back(vector3(182.537, 333.906, 68.346));
	data.push_back(vector3(180.822, 342.942, 68.121));

	VariogramModel v;
	v.C0 = 0.1;
	v.CX = 8.5;
	v.A = 25;
	v.VAR = SPHERICAL;

	int c = (int)data.size();
	Surface S = kriging(&data[0], c, 1,1, v);
	std::string outfilename = "I:/1.xyz";
	writeToFile(S.S, S.K, outfilename.c_str());

	//OC::Vector3List list;

	//for (int i = 0; i < S.K; ++i)
	//{
	//	OC::CVector3 v;
	//	v.x = S.S[i].x;
	//	v.y = S.S[i].y;
	//	v.z = S.S[i].z;
	//	list.push_back(v);
	//}

	//OC::CShpDrawer drawer;
	//drawer.test(list);
}