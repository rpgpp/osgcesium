#ifndef __Vector3i_H__
#define __Vector3i_H__

#include "UtilityDefine.h"
#include "MathUtil.h"
#include "Quaternion.h"

namespace OC
{

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** Standard 3-dimensional vector.
        @remarks
            A direction in 3D space represented as distances along the 3
            orthogonal axes (x, y, z). Note that positions, directions and
            scaling factors can be represented by a vector, depending on how
            you interpret the values.
    */
    class _UtilityExport CVector3i
    {
    public:
		int x, y, z;

    public:
        inline CVector3i()
        {
        }

        inline CVector3i( const int fX, const int fY, const int fZ )
            : x( fX ), y( fY ), z( fZ )
        {
        }

        inline explicit CVector3i( const int afCoordinate[3] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] ),
              z( afCoordinate[2] )
        {
        }

        inline explicit CVector3i( const Real afCoordinate[3] )
        {
            x = (int)afCoordinate[0];
            y = (int)afCoordinate[1];
            z = (int)afCoordinate[2];
        }

        inline explicit CVector3i( int* const r )
            : x( r[0] ), y( r[1] ), z( r[2] )
        {
        }

        inline explicit CVector3i( const int scaler )
            : x( scaler )
            , y( scaler )
            , z( scaler )
        {
        }


		/** Exchange the contents of this vector with another. 
		*/
		inline void swap(CVector3i& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		inline int operator [] ( const size_t i ) const
        {
            assert( i < 3 );

            return *(&x+i);
        }

		inline int& operator [] ( const size_t i )
        {
            assert( i < 3 );

            return *(&x+i);
        }
		/// Pointer accessor for direct copying
		inline int* ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const int* ptr() const
		{
			return &x;
		}

        /** Assigns the value of the other vector.
            @param
                rkVector The other vector
				*/
		inline CVector3i& operator = ( const CVector3i& rkVector )
		{
			x = rkVector.x;
			y = rkVector.y;
			z = rkVector.z;

			return *this;
		}

		inline CVector3i& operator = (const CVector3& rkVector);

        inline CVector3i& operator = ( const int fScaler )
        {
            x = fScaler;
            y = fScaler;
            z = fScaler;

            return *this;
        }

        inline bool operator == ( const CVector3i& rkVector ) const
        {
            return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
        }

        inline bool operator != ( const CVector3i& rkVector ) const
        {
            return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
        }

        // arithmetic operations
        inline CVector3i operator + ( const CVector3i& rkVector ) const
        {
            return CVector3i(
                x + rkVector.x,
                y + rkVector.y,
                z + rkVector.z);
        }

        inline CVector3i operator - ( const CVector3i& rkVector ) const
        {
            return CVector3i(
                x - rkVector.x,
                y - rkVector.y,
                z - rkVector.z);
        }

        inline CVector3i operator * ( const int fScalar ) const
        {
            return CVector3i(
                x * fScalar,
                y * fScalar,
                z * fScalar);
        }

        inline CVector3i operator * ( const CVector3i& rhs) const
        {
            return CVector3i(
                x * rhs.x,
                y * rhs.y,
                z * rhs.z);
        }

        inline CVector3i operator / ( const int fScalar ) const
        {
            assert( fScalar != 0.0 );

            int fInv = 1.0f / fScalar;

            return CVector3i(
                x * fInv,
                y * fInv,
                z * fInv);
        }

        inline CVector3i operator / ( const CVector3i& rhs) const
        {
            return CVector3i(
                x / rhs.x,
                y / rhs.y,
                z / rhs.z);
        }

        inline const CVector3i& operator + () const
        {
            return *this;
        }

        inline CVector3i operator - () const
        {
            return CVector3i(-x, -y, -z);
        }

        // overloaded operators to help Vector3
        inline friend CVector3i operator * ( const int fScalar, const CVector3i& rkVector )
        {
            return CVector3i(
                fScalar * rkVector.x,
                fScalar * rkVector.y,
                fScalar * rkVector.z);
        }

        inline friend CVector3i operator / ( const int fScalar, const CVector3i& rkVector )
        {
            return CVector3i(
                fScalar / rkVector.x,
                fScalar / rkVector.y,
                fScalar / rkVector.z);
        }

        inline friend CVector3i operator + (const CVector3i& lhs, const int rhs)
        {
            return CVector3i(
                lhs.x + rhs,
                lhs.y + rhs,
                lhs.z + rhs);
        }

        inline friend CVector3i operator + (const int lhs, const CVector3i& rhs)
        {
            return CVector3i(
                lhs + rhs.x,
                lhs + rhs.y,
                lhs + rhs.z);
        }

        inline friend CVector3i operator - (const CVector3i& lhs, const int rhs)
        {
            return CVector3i(
                lhs.x - rhs,
                lhs.y - rhs,
                lhs.z - rhs);
        }

        inline friend CVector3i operator - (const int lhs, const CVector3i& rhs)
        {
            return CVector3i(
                lhs - rhs.x,
                lhs - rhs.y,
                lhs - rhs.z);
        }

        // arithmetic updates
        inline CVector3i& operator += ( const CVector3i& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;

            return *this;
        }

        inline CVector3i& operator += ( const int fScalar )
        {
            x += fScalar;
            y += fScalar;
            z += fScalar;
            return *this;
        }

        inline CVector3i& operator -= ( const CVector3i& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;

            return *this;
        }

        inline CVector3i& operator -= ( const int fScalar )
        {
            x -= fScalar;
            y -= fScalar;
            z -= fScalar;
            return *this;
        }

        inline CVector3i& operator *= ( const int fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            return *this;
        }

        inline CVector3i& operator *= ( const CVector3i& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;
            z *= rkVector.z;

            return *this;
        }

        inline CVector3i& operator /= ( const int fScalar )
        {
            assert( fScalar != 0.0 );

            int fInv = 1.0f / fScalar;

            x *= fInv;
            y *= fInv;
            z *= fInv;

            return *this;
        }

        inline CVector3i& operator /= ( const CVector3i& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;
            z /= rkVector.z;

            return *this;
        }


        /** Returns the length (magnitude) of the vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                length (e.g. for just comparing lengths) use squaredLength()
                instead.
        */
        inline int length () const
        {
            return Math::Sqrt( x * x + y * y + z * z );
        }

        /** Returns the square of the length(magnitude) of the vector.
            @remarks
                This  method is for efficiency - calculating the actual
                length of a vector requires a square root, which is expensive
                in terms of the operations required. This method returns the
                square of the length of the vector, i.e. the same as the
                length but before the square root is taken. Use this if you
                want to find the longest / shortest vector without incurring
                the square root.
        */
        inline int squaredLength () const
        {
            return x * x + y * y + z * z;
        }

        /** Returns the distance to another vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                distance (e.g. for just comparing distances) use squaredDistance()
                instead.
        */
        inline int distance(const CVector3i& rhs) const
        {
            return (*this - rhs).length();
        }

        /** Returns the square of the distance to another vector.
            @remarks
                This method is for efficiency - calculating the actual
                distance to another vector requires a square root, which is
                expensive in terms of the operations required. This method
                returns the square of the distance to another vector, i.e.
                the same as the distance but before the square root is taken.
                Use this if you want to find the longest / shortest distance
                without incurring the square root.
        */
        inline int squaredDistance(const CVector3i& rhs) const
        {
            return (*this - rhs).squaredLength();
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @remarks
                The dot product can be used to calculate the angle between 2
                vectors. If both are unit vectors, the dot product is the
                cosine of the angle; otherwise the dot product must be
                divided by the product of the lengths of both vectors to get
                the cosine of the angle. This result can further be used to
                calculate the distance of a point from a plane.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @return
                A float representing the dot product value.
        */
        inline int dotProduct(const CVector3i& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        /** Calculates the absolute dot (scalar) product of this vector with another.
            @remarks
                This function work similar dotProduct, except it use absolute value
                of each component of the vector to computing.
            @param
                vec Vector with which to calculate the absolute dot product (together
                with this one).
            @return
                A int representing the absolute dot product value.
        */
        inline int absDotProduct(const CVector3i& vec) const
        {
            return Math::Abs(x * vec.x) + Math::Abs(y * vec.y) + Math::Abs(z * vec.z);
        }

        /** Normalises the vector.
            @remarks
                This method normalises the vector such that it's
                length / magnitude is 1. The result is called a unit vector.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @return The previous length of the vector.
        */
        inline int normalise()
        {
            int fLength = Math::Sqrt( x * x + y * y + z * z );

            // Will also work for zero-sized vectors, but will change nothing
			// We're not using epsilons because we don't need to.
            // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
            if ( fLength > int(0.0f) )
            {
                int fInvLength = 1.0f / fLength;
                x *= fInvLength;
                y *= fInvLength;
                z *= fInvLength;
            }

            return fLength;
        }

        /** Calculates the cross-product of 2 vectors, i.e. the vector that
            lies perpendicular to them both.
            @remarks
                The cross-product is normally used to calculate the normal
                vector of a plane, by calculating the cross-product of 2
                non-equivalent vectors which lie on the plane (e.g. 2 edges
                of a triangle).
            @param
                vec Vector which, together with this one, will be used to
                calculate the cross-product.
            @return
                A vector which is the result of the cross-product. This
                vector will <b>NOT</b> be normalised, to maximise efficiency
                - call Vector3::normalise on the result if you wish this to
                be done. As for which side the resultant vector will be on, the
                returned vector will be on the side from which the arc from 'this'
                to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
                = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
				This is because OGRE uses a right-handed coordinate system.
            @par
                For a clearer explanation, look a the left and the bottom edges
                of your monitor's screen. Assume that the first vector is the
                left edge and the second vector is the bottom edge, both of
                them starting from the lower-left corner of the screen. The
                resulting vector is going to be perpendicular to both of them
                and will go <i>inside</i> the screen, towards the cathode tube
                (assuming you're using a CRT monitor, of course).
        */
        inline CVector3i crossProduct( const CVector3i& rkVector ) const
        {
            return CVector3i(
                y * rkVector.z - z * rkVector.y,
                z * rkVector.x - x * rkVector.z,
                x * rkVector.y - y * rkVector.x);
        }

        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        inline CVector3i midPoint( const CVector3i& vec ) const
        {
            return CVector3i(
                ( x + vec.x ) * 0.5f,
                ( y + vec.y ) * 0.5f,
                ( z + vec.z ) * 0.5f );
        }

        /** Returns true if the vector's scalar components are all greater
            that the ones of the vector it is compared against.
        */
        inline bool operator < ( const CVector3i& rhs ) const
        {
            if( x < rhs.x && y < rhs.y && z < rhs.z )
                return true;
            return false;
        }

        /** Returns true if the vector's scalar components are all smaller
            that the ones of the vector it is compared against.
        */
        inline bool operator > ( const CVector3i& rhs ) const
        {
            if( x > rhs.x && y > rhs.y && z > rhs.z )
                return true;
            return false;
        }

        /** Sets this vector's components to the minimum of its own and the
            ones of the passed in vector.
            @remarks
                'Minimum' in this case means the combination of the lowest
                value of x, y and z from both vectors. Lowest is taken just
                numerically, not magnitude, so -1 < 0.
        */
        inline void makeFloor( const CVector3i& cmp )
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
            if( cmp.z < z ) z = cmp.z;
        }

        /** Sets this vector's components to the maximum of its own and the
            ones of the passed in vector.
            @remarks
                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
        inline void makeCeil( const CVector3i& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
            if( cmp.z > z ) z = cmp.z;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).
            @remarks
                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this
                method will guarantee to generate one of them. If you need more
                control you should use the Quaternion class.
        */
        inline CVector3i perpendicular(void) const
        {
            static const int fSquareZero = (int)(1e-06 * 1e-06);

            CVector3i perp = this->crossProduct( CVector3i::UNIT_X );

            // Check length
            if( perp.squaredLength() < fSquareZero )
            {
                /* This vector is the Y axis multiplied by a scalar, so we have
                   to use another axis.
                */
                perp = this->crossProduct( CVector3i::UNIT_Y );
            }
			perp.normalise();

            return perp;
        }

		/** Gets the angle between 2 vectors.
		@remarks
			Vectors do not have to be unit-length but must represent directions.
		*/
		inline CRadian angleBetween(const CVector3i& dest) const
		{
			int lenProduct = length() * dest.length();

			// Divide by zero check
			if(lenProduct < 1e-6f)
				lenProduct = 1e-6f;

			int f = dotProduct(dest) / lenProduct;

			f = Math::Clamp(f, (int)-1.0, (int)1.0);
			return Math::ACos(f);

		}

        /** Returns true if this vector is zero length. */
        inline bool isZeroLength(void) const
        {
            int sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));

        }

        /** As normalise, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        inline CVector3i normalisedCopy(void) const
        {
            CVector3i ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline CVector3i reflect(const CVector3i& normal) const
        {
            return CVector3i( *this - ( 2 * this->dotProduct(normal) * normal ) );
        }

		/** Returns whether this vector is within a positional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The amount that each element of the vector may vary by
			and still be considered equal
		*/
		inline bool positionEquals(const CVector3i& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}

		/** Returns whether this vector is within a positional tolerance
			of another vector, also take scale of the vectors into account.
		@param rhs The vector to compare with
		@param tolerance The amount (related to the scale of vectors) that distance
            of the vector may vary by and still be considered close
		*/
		inline bool positionCloses(const CVector3i& rhs, int tolerance = 1e-03f) const
		{
			return squaredDistance(rhs) <=
                (squaredLength() + rhs.squaredLength()) * tolerance;
		}

		/** Returns whether this vector is within a directional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The maximum angle by which the vectors may vary and
			still be considered equal
		@note Both vectors should be normalised.
		*/
		inline bool directionEquals(const CVector3i& rhs,
			const CRadian& tolerance) const
		{
			int dot = dotProduct(rhs);
			CRadian angle = Math::ACos(dot);

			return Math::Abs(angle.valueRadians()) <= tolerance.valueRadians();

		}

		/// Check whether this vector contains valid values
		inline bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z);
		}

		/// Extract the primary (dominant) axis from this direction vector
		inline CVector3i primaryAxis() const
		{
			int absx = Math::Abs(x);
			int absy = Math::Abs(y);
			int absz = Math::Abs(z);
			if (absx > absy)
				if (absx > absz)
					return x > 0 ? CVector3i::UNIT_X : CVector3i::NEGATIVE_UNIT_X;
				else
					return z > 0 ? CVector3i::UNIT_Z : CVector3i::NEGATIVE_UNIT_Z;
			else // absx <= absy
				if (absy > absz)
					return y > 0 ? CVector3i::UNIT_Y : CVector3i::NEGATIVE_UNIT_Y;
				else
					return z > 0 ? CVector3i::UNIT_Z : CVector3i::NEGATIVE_UNIT_Z;


		}

		// special points
        static const CVector3i ZERO;
        static const CVector3i UNIT_X;
        static const CVector3i UNIT_Y;
        static const CVector3i UNIT_Z;
        static const CVector3i NEGATIVE_UNIT_X;
        static const CVector3i NEGATIVE_UNIT_Y;
        static const CVector3i NEGATIVE_UNIT_Z;
        static const CVector3i UNIT_SCALE;

        /** Function for writing to a stream.
        */
        inline _UtilityExport friend std::ostream& operator <<
            ( std::ostream& o, const CVector3i& v )
        {
            o << "Vector3i(" << v.x << ", " << v.y << ", " << v.z << ")";
            return o;
        }
    };
	/** @} */
	/** @} */

}
#endif
