#ifndef __Matrix3_H__
#define __Matrix3_H__

#include "UtilityDefine.h"

#include "Vector3.h"

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

namespace OC
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** A 3x3 matrix which can represent rotations around axes.
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.geometrictools.com/).</b>
        @par
            The coordinate system is assumed to be <b>right-handed</b>.
    */
    class _UtilityExport CMatrix3
    {
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
		inline CMatrix3 () {}
        inline explicit CMatrix3 (const Real arr[3][3])
		{
			memcpy(m,arr,9*sizeof(Real));
		}
        inline CMatrix3 (const CMatrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(Real));
		}
        CMatrix3 (Real fEntry00, Real fEntry01, Real fEntry02,
                    Real fEntry10, Real fEntry11, Real fEntry12,
                    Real fEntry20, Real fEntry21, Real fEntry22)
		{
			m[0][0] = fEntry00;
			m[0][1] = fEntry01;
			m[0][2] = fEntry02;
			m[1][0] = fEntry10;
			m[1][1] = fEntry11;
			m[1][2] = fEntry12;
			m[2][0] = fEntry20;
			m[2][1] = fEntry21;
			m[2][2] = fEntry22;
		}

		/** Exchange the contents of this matrix with another. 
		*/
		inline void swap(CMatrix3& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
		}

        // member access, allows use of construct mat[r][c]
        inline Real* operator[] (size_t iRow) const
		{
			return (Real*)m[iRow];
		}
        /*inline operator Real* ()
		{
			return (Real*)m[0];
		}*/
        CVector3 GetColumn (size_t iCol) const;
        void SetColumn(size_t iCol, const CVector3& vec);
        void FromAxes(const CVector3& xAxis, const CVector3& yAxis, const CVector3& zAxis);

        // assignment and comparison
        inline CMatrix3& operator= (const CMatrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(Real));
			return *this;
		}

        /** Tests 2 matrices for equality.
         */
        bool operator== (const CMatrix3& rkMatrix) const;

        /** Tests 2 matrices for inequality.
         */
        inline bool operator!= (const CMatrix3& rkMatrix) const
		{
			return !operator==(rkMatrix);
		}

        // arithmetic operations
        /** Matrix addition.
         */
        CMatrix3 operator+ (const CMatrix3& rkMatrix) const;

        /** Matrix subtraction.
         */
        CMatrix3 operator- (const CMatrix3& rkMatrix) const;

        /** Matrix concatenation using '*'.
         */
        CMatrix3 operator* (const CMatrix3& rkMatrix) const;
        CMatrix3 operator- () const;

        /// Matrix * vector [3x3 * 3x1 = 3x1]
        CVector3 operator* (const CVector3& rkVector) const;

        /// Vector * matrix [1x3 * 3x3 = 1x3]
        _UtilityExport friend CVector3 operator* (const CVector3& rkVector,
            const CMatrix3& rkMatrix);

        /// Matrix * scalar
        CMatrix3 operator* (Real fScalar) const;

        /// Scalar * matrix
        _UtilityExport friend CMatrix3 operator* (Real fScalar, const CMatrix3& rkMatrix);

        // utilities
        CMatrix3 Transpose () const;
        bool Inverse (CMatrix3& rkInverse, Real fTolerance = 1e-06) const;
        CMatrix3 Inverse (Real fTolerance = 1e-06) const;
        Real Determinant () const;

        // singular value decomposition
        void SingularValueDecomposition (CMatrix3& rkL, CVector3& rkS,
            CMatrix3& rkR) const;
        void SingularValueComposition (const CMatrix3& rkL,
            const CVector3& rkS, const CMatrix3& rkR);

        /// Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        void Orthonormalize ();

        /// Orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (CMatrix3& rkQ, CVector3& rkD,
            CVector3& rkU) const;

        Real SpectralNorm () const;

        // matrix must be orthonormal
        void ToAngleAxis (CVector3& rkAxis, CRadian& rfAngle) const;
		inline void ToAngleAxis (CVector3& rkAxis, CDegree& rfAngle) const {
			CRadian r;
			ToAngleAxis ( rkAxis, r );
			rfAngle = r;
		}
        void FromAngleAxis (const CVector3& rkAxis, const CRadian& fRadians);

        // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        // where yaw is rotation about the Up vector, pitch is rotation about the
        // Right axis, and roll is rotation about the Direction axis.
        bool ToEulerAnglesXYZ (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        bool ToEulerAnglesXZY (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        bool ToEulerAnglesYXZ (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        bool ToEulerAnglesYZX (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        bool ToEulerAnglesZXY (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        bool ToEulerAnglesZYX (CRadian& rfYAngle, CRadian& rfPAngle,
            CRadian& rfRAngle) const;
        void FromEulerAnglesXYZ (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        void FromEulerAnglesXZY (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        void FromEulerAnglesYXZ (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        void FromEulerAnglesYZX (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        void FromEulerAnglesZXY (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        void FromEulerAnglesZYX (const CRadian& fYAngle, const CRadian& fPAngle, const CRadian& fRAngle);
        /// Eigensolver, matrix must be symmetric
        void EigenSolveSymmetric (Real afEigenvalue[3],
            CVector3 akEigenvector[3]) const;

        static void TensorProduct (const CVector3& rkU, const CVector3& rkV,
            CMatrix3& rkProduct);

		/** Determines if this matrix involves a scaling. */
		inline bool hasScale() const
		{
			// check magnitude of column vectors (==local axes)
			Real t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
			if (!Math::RealEqual(t, 1.0, (Real)1e-04))
				return true;
			t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
			if (!Math::RealEqual(t, 1.0, (Real)1e-04))
				return true;
			t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
			if (!Math::RealEqual(t, 1.0, (Real)1e-04))
				return true;

			return false;
		}

		/** Function for writing to a stream.
		*/
		inline _UtilityExport friend std::ostream& operator <<
			( std::ostream& o, const CMatrix3& mat )
		{
			o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", " 
                            << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", " 
                            << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
			return o;
		}

        static const Real EPSILON;
        static const CMatrix3 ZERO;
        static const CMatrix3 IDENTITY;

    protected:
        // support for eigensolver
        void Tridiagonal (Real afDiag[3], Real afSubDiag[3]);
        bool QLAlgorithm (Real afDiag[3], Real afSubDiag[3]);

        // support for singular value decomposition
        static const Real msSvdEpsilon;
        static const unsigned int msSvdMaxIterations;
        static void Bidiagonalize (CMatrix3& kA, CMatrix3& kL,
            CMatrix3& kR);
        static void GolubKahanStep (CMatrix3& kA, CMatrix3& kL,
            CMatrix3& kR);

        // support for spectral norm
        static Real MaxCubicRoot (Real afCoeff[3]);

        Real m[3][3];

        // for faster access
        friend class CMatrix4;
    };
	/** @} */
	/** @} */
}
#endif
