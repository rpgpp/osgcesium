// Original File: Matrix.cxx
// Author: Jharrod LaFon
// Date: Spring 2011
// Purpose: Simple Matrix Class
// https://github.com/hpc/MPI-Examples/blob/master/newton-type-optimization/matrix.cxx

// Modified by: Rodolfo Mora
// Date: February 2015
// Purpose: Parallel implementation of Matrix Multiplication and Matrix Invertion algorithms
// https://github.com/CNCA-CeNAT/kriging/blob/master/src/Matrix.h

#ifndef MATRIX_CXX
#define MATRIX_CXX

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>

using std::cout;
using std::endl;

#define EPSILON 0.00001

// Matrix class definition
class matrix
{
public:
	// Constructors
	matrix(int Row = 10,  int Col = 10);
	// Copies another
	matrix(matrix * m);
	// Constructs a square matrix 
	matrix(int Size = 10, bool One = false);

	// Destructor
	~matrix();

	// Accessors
	int rowCount();
	int colCount();

	// Print
	void print();

	// Print A:I
	void printAugmented();

	// Returns the identity value for this pivoted matrix on the required row and column
	float i(int r, int c);

	// Swaps two rows of the matrix
	void swap(int r1, int r2);

	// Clears permutation of a matrix, returning it to it's original row order
	void clearPermutation();

	// Applies LU Factorization to matrix
	void luFactorize();

	// Solves
	void solveLU(int c);

	// Replaces row r1 with r1-r2*s
	void setRow(int r1, int r2, float s);

	// Replaces row r with r*s
	void setRow(int r, float s);

	// Inverts a matrix using the gauss jordan algorithm
	void gaussInversion();

	// Returns the inverse of a matrix
	void invert();

	// Returns true if this is an identity matrix
	bool testIdentity();

	// Returns this matrix multiplied by another matrix
	matrix multiply(matrix m);

	// Swaps two matrix rows
	void swap_rows(const  int r1, const  int r2);

	// Multiplies the matrix by a vector
	matrix vector_multiply(const std::vector<float> v);

	// Reference Operator
	float  get(const  int row, const  int col);
	float & operator() (const  int row, const  int col);
	// Assignation Operator
	float & set(const  int row, const  int col, float value);

	matrix * L;
	matrix * U;
	matrix * I;
private:
	int row, col;
	int * r;
	float * array;
};

// Constructors
matrix::matrix(int Row,  int Col)
{
	this->row = Row;
	this->col = Col;
	array = new float [row*col];

	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; ++j)
		{
			array[i*col+j] = 0;
		}
	}

	r = new int[row];
	for(int i = 0; i < row; ++i)
	{
		r[i] = i;
	}
	L = NULL;
	U = NULL;
	I = NULL;
}

matrix::matrix(matrix * m)
{
	cout << "COPY CONSTRUCTOR" << endl;
	row = m->row;
	col = m->col;
	array = new float [row*col];
	for(int i = 0; i < row*col; ++i)
	{
		array[i] = m->array[i];
	}

	r = new int[row];
	for(int i = 0; i < row; ++i)
	{
		r[i] = m->r[i];
	}
	if(m->L != NULL) L = new matrix(m->L); else L = NULL;
	if(m->U != NULL) U = new matrix(m->U); else U = NULL;
	if(m->I != NULL) I = new matrix(m->I); else I = NULL;
}

matrix::matrix(int Size, bool One)
{
	this->row = Size;
	this->col = Size;

	array = new float[row*col];
	for(  int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; ++j)
		{
			array[i*row+j] = 0;
		}
		if(One)
			array[i*row + i] = 1;
	}

	r = new int[row];
	for(int i = 0; i < row; ++i)
	{
		r[i] = i;
	}
	L = NULL;
	U = NULL;
	I = NULL;
}

// Destructor
matrix::~matrix()
{
	//THIS LINE YIELDS A DOUBLE FREE OR CORRUPTION ERROR
	//delete [] array;
	//delete r;
	if(L != NULL) delete(L);
	if(U != NULL) delete(U);
	if(I != NULL) delete(I);
}

// Accessors
int matrix::rowCount(){ return this->row; }
int matrix::colCount(){ return this->col; }

float matrix::get(const  int row, const  int col)
{
	assert(row >= 0 && row < this->row && col >= 0 && col < this->col);
	return array[r[row]*this->col + col];
}

float & matrix::set(const  int row, const  int col, float value)
{
	assert(row >= 0 && row < this->row && col >= 0 && col < this->col);
	float rnd = round(value);
	float res = value-rnd;
	if(res < EPSILON && res > -EPSILON)
	{
		value = rnd;
	}
	array[r[row]*this->col + col] = value;
	return array[r[row]*this->col + col];
}

// Reference Operator
float & matrix::operator () ( int row,  int col)
{
	assert(row >= 0 && row < this->row && col >= 0 && col < this->col);
	return array[r[row]*this->col + col];
}

//
void matrix::swap(int r1, int r2)
{
	int t = r[r1];
	r[r1] = r[r2];
	r[r2] = t;
}

//
void matrix::clearPermutation()
{
	for(int i = 0; i < row; ++i)
	{
		this->r[i] = i;
	}
}

//
float matrix::i(int r, int c)
{
	assert(r >= 0 && r < this->row && c >= 0 && c < this->col);
	if(c == this->r[r])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Multiplies the matrix by a vector
matrix matrix::vector_multiply(const std::vector<float> v)
{
	assert(this->col == ( int) v.size());
	matrix result(this->row,1);
	int i,j;
	float temp;
	for(i = 0; i < this->row; i++)
	{
		temp = 0.0;
		for(j = 0; j < this->col; j++)
			temp += get(i,j)*v[j];
		result(i,0) = temp;
	}
	return result;
}

// Multiply this matrix by another matrix m
matrix matrix::multiply(matrix m)
{
	assert(col == m.row);

	/*
	int n = row;
	int q = n/100;
	int p = q;*/

	matrix K = matrix(row, m.col);
	for(int i = 0; i < row; ++i)
	{
		/*
		if(q != 0 && i > p)
		{
			p += q;
			int prog = p/q;
			std::cout << "\r" << prog - 1 << "% completed: ";
			std::cout.flush();
		}*/

		for(int j = 0; j < m.col; ++j)
		{
			float v = 0;
			for(int k = 0; k < col; ++k)
			{
				v += get(i,k)*m.get(k,j);
			}
			K.set(i,j,v);
		}
	}
	return K;
}

//
void matrix::luFactorize()
{
	int n = row;
	float val;

	//Pivoting
	for(int i = 1; i < n; ++i)
	{
		if(get(i,i) == 0)
		{
			swap(i-1,i);
		}
	}

	L = new matrix(row, true);  //Unit Matrix
	U = new matrix(row, false); //Zero Matrix

	for(int p = 0; p < n-1; ++p)
	{
		for(int k = 0; k < n; ++k)
		{
			for(int j = k; j < n; ++j)
			{
				val = 0;
				for(int m = 0; m < k; ++m)
				{
					val += L->get(k,m) * U->get(m,j);
				}
				val = get(k,j)-val;
				U->set(k,j,val);
			}
			for(int i = k+1; i < n; ++i)
			{
				val = 0;
				for(int m = 0; m < k; ++m)
				{
					val += L->get(i,m) * U->get(m,k);
				}
				val = (get(i,k) - val) / U->get(k,k);
				L->set(i,k,val);
			}
		}
	}
}

// replaces row r1 with r1-(r2*s)
void matrix::setRow(int r1, int r2, float s)
{
	//cout << "O: [" << r1 << "]" << " = [" << r1 << "] - [" << r2 << "] * " << s << endl;
	for(int c = 0; c < col; ++c)
	{
		set(r1,c,get(r1,c) - (get(r2,c) * s));
		I->set(r1,c,I->get(r1,c) - (I->get(r2,c) * s));
	}
	//printAugmented();
}

// replaces row r1 with r1*s
void matrix::setRow(int r, float s)
{
	//cout << "O: [" << r << "] = [" << r << "] * " << s << endl;
	for(int c = 0; c < col; ++c)
	{
		set(r,c,get(r,c)*s);
		I->set(r,c,I->get(r,c)*s);
	}
	//printAugmented();
}

// Inverts a matrix using the gauss-jordan reduction method
void matrix::gaussInversion()
{
	I = new matrix(row,true);

	int n = row;
	float q = float(n)/100.0;
	float p = q;

	//Pivoting
	for(int i = 1; i < n; ++i)
	{
		if(get(i,i) == 0)
		{
			swap(i-1,i);
			I->swap(i-1,i);
		}
	}

	for(int i = 0; i < row; ++i)
	{

		if(q != 0 && i > p)
		{
			p += q;
			int prog = p/q;
			std::cout << "\r" << (prog - 1) << "% completed: ";
			std::cout.flush();
		}


		try
		{
			setRow(i,1.0/get(i,i));
		}
		catch(std::runtime_error& e)
		{
			cout << "AN ERROR HAS OCURRED: " << e.what();
			exit(0);
		}
		catch(int e)
		{
			std::cout << "AN UNIDENTIFIED ERROR OCURRED: " << e << endl;
			exit(0);
		}

		for(int r = 0; r < row; ++r)
		{
			if(r != i)
			{
				setRow(r,i,get(r,i));
			}
		}
	}

	cout << endl;
}

// Inverts a matrix. The original matrix is destroyed
// The matrix must be invertible.
void matrix::invert()
{
	gaussInversion();
	I->clearPermutation();
}

bool matrix::testIdentity()
{
	bool test = true;
	int r = 0;
	int c = 0;
	while((r < this->row) && test)
	{
		c = 0;
		while ((c < this->col) && test)
		{
			if(c == r)
				test = get(r,c) == 1;
			else
				test = get(r,c) == 0;
			++c;
		}
		++r;
	}
	if(!test)
	{
		cout << "TEST FAILED ON ROW: " << r-1 << ", COL: " << c-1 << " WITH VALUE: " << get(r-1, c-1);
	}
	return test;
}

// Prints a matrix
void matrix::print()
{
	for(int i = 0; i < this->row; i++)
	{
		std::cout << "[" << r[i] << "] ";
		for(  int j = 0; j < this->col; j++)
		{
			if(get(i,j) >= 0)
				cout << " ";
			if(get(i,j) == -0.0)
				set(i,j,0);
			std::cout << get(i,j) << " ";
		}
		std::cout << std::endl;
	}
}

void matrix::printAugmented()
{
	std::cout << std::fixed;
	std::cout << std::setprecision(2);

	for(int i = 0; i < this->row; i++)
	{
		std::cout << "[" << r[i] << "] ";
		for(  int j = 0; j < this->col; j++)
		{
			if(get(i,j) >= 0)
				cout << " ";
			if(get(i,j) == -0.0)
				set(i,j,0);
			std::cout << get(i,j) << " ";
		}

		cout << "\t : ";

		for(  int j = 0; j < this->col; j++)
		{
			if(I->get(i,j) >= 0)
				cout << " ";
			if(I->get(i,j) == -0.0)
				I->set(i,j,0);
			std::cout << I->get(i,j) << " ";
		}

		std::cout << std::endl;
	}
}

#endif
