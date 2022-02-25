#ifndef __Quaternion_H__
#define __Quaternion_H__

#include "UtilityDefine.h"
#include "MathUtil.h"

namespace OC {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** Implementation of a Quaternion, i.e. a rotation around an axis.
		For more information about Quaternions and the theory behind it, we recommend reading:
		http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer
		http://www.cprogramming.com/tutorial/3d/quaternions.html
		http://www.gamedev.net/page/resources/_/reference/programming/math-and-physics/
		quaternions/quaternion-powers-r1095
    */
    class _UtilityExport CQuaternion
    {
    public:
		/// Default constructor, initializes to identity rotation (aka 0°)
		inline CQuaternion ()
			: w(1), x(0), y(0), z(0)
		{
		}
		/// Construct from an explicit list of values
		inline CQuaternion (
			Real fW,
			Real fX, Real fY, Real fZ)
			: w(fW), x(fX), y(fY), z(fZ)
		{
		}
        /// Construct a quaternion from a rotation matrix
        inline CQuaternion(const CMatrix3& rot)
        {
            this->FromRotationMatrix(rot);
        }
        /// Construct a quaternion from an angle/axis
        inline CQuaternion(const CRadian& rfAngle, const CVector3& rkAxis)
        {
            this->FromAngleAxis(rfAngle, rkAxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline CQuaternion(const CVector3& xaxis, const CVector3& yaxis, const CVector3& zaxis)
        {
            this->FromAxes(xaxis, yaxis, zaxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline CQuaternion(const CVector3* akAxis)
        {
            this->FromAxes(akAxis);
        }
		/// Construct a quaternion from 4 manual w/x/y/z values
		inline CQuaternion(Real* valptr)
		{
			memcpy(&w, valptr, sizeof(Real)*4);
		}

		/** Exchange the contents of this quaternion with another. 
		*/
		inline void swap(CQuaternion& other)
		{
			std::swap(w, other.w);
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		/// Array accessor operator
		inline Real operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Array accessor operator
		inline Real& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Pointer accessor for direct copying
		inline Real* ptr()
		{
			return &w;
		}

		/// Pointer accessor for direct copying
		inline const Real* ptr() const
		{
			return &w;
		}

		void FromRotationMatrix (const CMatrix3& kRot);
        void ToRotationMatrix (CMatrix3& kRot) const;
		/** Setups the quaternion using the supplied vector, and "roll" around
			that vector by the specified radians.
		*/
        void FromAngleAxis (const CRadian& rfAngle, const CVector3& rkAxis);
        void ToAngleAxis (CRadian& rfAngle, CVector3& rkAxis) const;
        inline void ToAngleAxis (CDegree& dAngle, CVector3& rkAxis) const {
            CRadian rAngle;
            ToAngleAxis ( rAngle, rkAxis );
            dAngle = rAngle;
        }
		/** Constructs the quaternion using 3 axes, the axes are assumed to be orthonormal
			@see FromAxes
		*/
        void FromAxes (const CVector3* akAxis);
        void FromAxes (const CVector3& xAxis, const CVector3& yAxis, const CVector3& zAxis);
		/** Gets the 3 orthonormal axes defining the quaternion. @see FromAxes */
        void ToAxes (CVector3* akAxis) const;
        void ToAxes (CVector3& xAxis, CVector3& yAxis, CVector3& zAxis) const;

		/** Returns the X orthonormal axis defining the quaternion. Same as doing
			xAxis = Vector3::UNIT_X * this. Also called the local X-axis
		*/
        CVector3 xAxis(void) const;

        /** Returns the Y orthonormal axis defining the quaternion. Same as doing
			yAxis = Vector3::UNIT_Y * this. Also called the local Y-axis
		*/
        CVector3 yAxis(void) const;

		/** Returns the Z orthonormal axis defining the quaternion. Same as doing
			zAxis = Vector3::UNIT_Z * this. Also called the local Z-axis
		*/
        CVector3 zAxis(void) const;

        inline CQuaternion& operator= (const CQuaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        CQuaternion operator+ (const CQuaternion& rkQ) const;
        CQuaternion operator- (const CQuaternion& rkQ) const;
        CQuaternion operator* (const CQuaternion& rkQ) const;
        CQuaternion operator* (Real fScalar) const;
        _UtilityExport friend CQuaternion operator* (Real fScalar,
            const CQuaternion& rkQ);
        CQuaternion operator- () const;
        inline bool operator== (const CQuaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
        inline bool operator!= (const CQuaternion& rhs) const
		{
			return !operator==(rhs);
		}
        // functions of a quaternion
        /// Returns the dot product of the quaternion
        Real Dot (const CQuaternion& rkQ) const;
        /* Returns the normal length of this quaternion.
            @note This does <b>not</b> alter any values.
        */
        Real Norm () const;
        /// Normalises this quaternion, and returns the previous length
        Real normalise(void); 
        CQuaternion Inverse () const;  // apply to non-zero quaternion
        CQuaternion UnitInverse () const;  // apply to unit-length quaternion
        CQuaternion Exp () const;
        CQuaternion Log () const;

        /// Rotation of a vector by a quaternion
        CVector3 operator* (const CVector3& rkVector) const;

   		/** Calculate the local roll element of this quaternion.
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to false though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
             may involve less axial rotation.  The co-domain of the returned value is 
             from -180 to 180 degrees.
		*/
		CRadian getRoll(bool reprojectAxis = true) const;
   		/** Calculate the local pitch element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
            may involve less axial rotation.  The co-domain of the returned value is 
            from -180 to 180 degrees.
		*/
		CRadian getPitch(bool reprojectAxis = true) const;
   		/** Calculate the local yaw element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaternion onto the X and
			Z axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. The co-domain of the returned value is 
            from -180 to 180 degrees.
		*/
		CRadian getYaw(bool reprojectAxis = true) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool equals(const CQuaternion& rhs, const CRadian& tolerance) const;
		
	    /** Performs Spherical linear interpolation between two quaternions, and returns the result.
			Slerp ( 0.0f, A, B ) = A
			Slerp ( 1.0f, A, B ) = B
			@return Interpolated quaternion
			@remarks
			Slerp has the proprieties of performing the interpolation at constant
			velocity, and being torque-minimal (unless shortestPath=false).
			However, it's NOT commutative, which means
			Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
			therefore be careful if your code relies in the order of the operands.
			This is specially important in IK animation.
		*/
        static CQuaternion Slerp (Real fT, const CQuaternion& rkP,
            const CQuaternion& rkQ, bool shortestPath = false);

		/** @see Slerp. It adds extra "spins" (i.e. rotates several times) specified
			by parameter 'iExtraSpins' while interpolating before arriving to the
			final values
		*/
        static CQuaternion SlerpExtraSpins (Real fT,
            const CQuaternion& rkP, const CQuaternion& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void Intermediate (const CQuaternion& rkQ0,
            const CQuaternion& rkQ1, const CQuaternion& rkQ2,
            CQuaternion& rka, CQuaternion& rkB);

        // spherical quadratic interpolation
        static CQuaternion Squad (Real fT, const CQuaternion& rkP,
            const CQuaternion& rkA, const CQuaternion& rkB,
            const CQuaternion& rkQ, bool shortestPath = false);

        /** Performs Normalised linear interpolation between two quaternions, and returns the result.
			nlerp ( 0.0f, A, B ) = A
			nlerp ( 1.0f, A, B ) = B
			@remarks
			Nlerp is faster than Slerp.
			Nlerp has the proprieties of being commutative (@see Slerp;
			commutativity is desired in certain places, like IK animation), and
			being torque-minimal (unless shortestPath=false). However, it's performing
			the interpolation at non-constant velocity; sometimes this is desired,
			sometimes it is not. Having a non-constant velocity can produce a more
			natural rotation feeling without the need of tweaking the weights; however
			if your scene relies on the timing of the rotation or assumes it will point
			at a specific angle at a specific weight value, Slerp is a better choice.
		*/
        static CQuaternion nlerp(Real fT, const CQuaternion& rkP, 
            const CQuaternion& rkQ, bool shortestPath = false);

        /// Cutoff for sine near zero
        static const Real msEpsilon;

        // special values
        static const CQuaternion ZERO;
        static const CQuaternion IDENTITY;

		Real w, x, y, z;

		/// Check whether this quaternion contains valid values
		inline bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}

        /** Function for writing to a stream. Outputs "Quaternion(w, x, y, z)" with w,x,y,z
            being the member values of the quaternion.
        */
        inline _UtilityExport friend std::ostream& operator <<
            ( std::ostream& o, const CQuaternion& q )
        {
            o << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
            return o;
        }

    };
	/** @} */
	/** @} */

}




#endif 
