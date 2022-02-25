#ifndef __Math_H__
#define __Math_H__

#include "UtilityDefine.h"

using namespace std;
namespace OC
{
	class _UtilityExport CRadian
	{
		Real mRad;

	public:
		explicit CRadian ( Real r=0 ) : mRad(r) {}
		CRadian ( const CDegree& d );
		CRadian& operator = ( const Real& f ) { mRad = f; return *this; }
		CRadian& operator = ( const CRadian& r ) { mRad = r.mRad; return *this; }
		CRadian& operator = ( const CDegree& d );

		Real valueDegrees() const; // see bottom of this file
		Real valueRadians() const { return mRad; }
		Real valueAngleUnits() const;

        const CRadian& operator + () const { return *this; }
		CRadian operator + ( const CRadian& r ) const { return CRadian ( mRad + r.mRad ); }
		CRadian operator + ( const CDegree& d ) const;
		CRadian& operator += ( const CRadian& r ) { mRad += r.mRad; return *this; }
		CRadian& operator += ( const CDegree& d );
		CRadian operator - () const { return CRadian(-mRad); }
		CRadian operator - ( const CRadian& r ) const { return CRadian ( mRad - r.mRad ); }
		CRadian operator - ( const CDegree& d ) const;
		CRadian& operator -= ( const CRadian& r ) { mRad -= r.mRad; return *this; }
		CRadian& operator -= ( const CDegree& d );
		CRadian operator * ( Real f ) const { return CRadian ( mRad * f ); }
        CRadian operator * ( const CRadian& f ) const { return CRadian ( mRad * f.mRad ); }
		CRadian& operator *= ( Real f ) { mRad *= f; return *this; }
		CRadian operator / ( Real f ) const { return CRadian ( mRad / f ); }
		CRadian& operator /= ( Real f ) { mRad /= f; return *this; }

		bool operator <  ( const CRadian& r ) const { return mRad <  r.mRad; }
		bool operator <= ( const CRadian& r ) const { return mRad <= r.mRad; }
		bool operator == ( const CRadian& r ) const { return mRad == r.mRad; }
		bool operator != ( const CRadian& r ) const { return mRad != r.mRad; }
		bool operator >= ( const CRadian& r ) const { return mRad >= r.mRad; }
		bool operator >  ( const CRadian& r ) const { return mRad >  r.mRad; }

		inline _UtilityExport friend std::ostream& operator <<
			( std::ostream& o, const CRadian& v )
		{
			o << "Radian(" << v.valueRadians() << ")";
			return o;
		}
	};

    /** Wrapper class which indicates a given angle value is in Degrees.
    @remarks
        Degree values are interchangeable with Radian values, and conversions
        will be done automatically between them.
    */
	class _UtilityExport CDegree
	{
		Real mDeg; // if you get an error here - make sure to define/typedef 'Real' first

	public:
		explicit CDegree ( Real d=0 ) : mDeg(d) {}
		CDegree ( const CRadian& r ) : mDeg(r.valueDegrees()) {}
		CDegree& operator = ( const Real& f ) { mDeg = f; return *this; }
		CDegree& operator = ( const CDegree& d ) { mDeg = d.mDeg; return *this; }
		CDegree& operator = ( const CRadian& r ) { mDeg = r.valueDegrees(); return *this; }

		Real valueDegrees() const { return mDeg; }
		Real valueRadians() const; // see bottom of this file
		Real valueAngleUnits() const;

		const CDegree& operator + () const { return *this; }
		CDegree operator + ( const CDegree& d ) const { return CDegree ( mDeg + d.mDeg ); }
		CDegree operator + ( const CRadian& r ) const { return CDegree ( mDeg + r.valueDegrees() ); }
		CDegree& operator += ( const CDegree& d ) { mDeg += d.mDeg; return *this; }
		CDegree& operator += ( const CRadian& r ) { mDeg += r.valueDegrees(); return *this; }
		CDegree operator - () const { return CDegree(-mDeg); }
		CDegree operator - ( const CDegree& d ) const { return CDegree ( mDeg - d.mDeg ); }
		CDegree operator - ( const CRadian& r ) const { return CDegree ( mDeg - r.valueDegrees() ); }
		CDegree& operator -= ( const CDegree& d ) { mDeg -= d.mDeg; return *this; }
		CDegree& operator -= ( const CRadian& r ) { mDeg -= r.valueDegrees(); return *this; }
		CDegree operator * ( Real f ) const { return CDegree ( mDeg * f ); }
        CDegree operator * ( const CDegree& f ) const { return CDegree ( mDeg * f.mDeg ); }
		CDegree& operator *= ( Real f ) { mDeg *= f; return *this; }
		CDegree operator / ( Real f ) const { return CDegree ( mDeg / f ); }
		CDegree& operator /= ( Real f ) { mDeg /= f; return *this; }

		bool operator <  ( const CDegree& d ) const { return mDeg <  d.mDeg; }
		bool operator <= ( const CDegree& d ) const { return mDeg <= d.mDeg; }
		bool operator == ( const CDegree& d ) const { return mDeg == d.mDeg; }
		bool operator != ( const CDegree& d ) const { return mDeg != d.mDeg; }
		bool operator >= ( const CDegree& d ) const { return mDeg >= d.mDeg; }
		bool operator >  ( const CDegree& d ) const { return mDeg >  d.mDeg; }

		inline _UtilityExport friend std::ostream& operator <<
			( std::ostream& o, const CDegree& v )
		{
			o << "Degree(" << v.valueDegrees() << ")";
			return o;
		}
	};

    /** Wrapper class which identifies a value as the currently default angle 
        type, as defined by Math::setAngleUnit.
    @remarks
        Angle values will be automatically converted between radians and degrees,
        as appropriate.
    */
	class _UtilityExport Angle
	{
		Real mAngle;
	public:
		explicit Angle ( Real angle ) : mAngle(angle) {}
		operator CRadian() const;
		operator CDegree() const;
	};

	// these functions could not be defined within the class definition of class
	// Radian because they required class Degree to be defined
	inline CRadian::CRadian ( const CDegree& d ) : mRad(d.valueRadians()) {
	}
	inline CRadian& CRadian::operator = ( const CDegree& d ) {
		mRad = d.valueRadians(); return *this;
	}
	inline CRadian CRadian::operator + ( const CDegree& d ) const {
		return CRadian ( mRad + d.valueRadians() );
	}
	inline CRadian& CRadian::operator += ( const CDegree& d ) {
		mRad += d.valueRadians();
		return *this;
	}
	inline CRadian CRadian::operator - ( const CDegree& d ) const {
		return CRadian ( mRad - d.valueRadians() );
	}
	inline CRadian& CRadian::operator -= ( const CDegree& d ) {
		mRad -= d.valueRadians();
		return *this;
	}

    /** Class to provide access to common mathematical functions.
        @remarks
            Most of the maths functions are aliased versions of the C runtime
            library functions. They are aliased here to provide future
            optimisation opportunities, either from faster RTLs or custom
            math approximations.
        @note
            <br>This is based on MgcMath.h from
            <a href="http://www.geometrictools.com/">Wild Magic</a>.
    */
    class _UtilityExport Math 
    {
   public:
       /** The angular units used by the API. This functionality is now deprecated in favor
	       of discreet angular unit types ( see Degree and Radian above ). The only place
		   this functionality is actually still used is when parsing files. Search for
		   usage of the Angle class for those instances
       */
       enum AngleUnit
       {
           AU_DEGREE,
           AU_RADIAN
       };

    protected:
        // angle units used by the api
        static AngleUnit msAngleUnit;

        /// Size of the trig tables as determined by constructor.
        static int mTrigTableSize;

        /// Radian -> index factor value ( mTrigTableSize / 2 * PI )
        static Real mTrigTableFactor;
        static Real* mSinTable;
        static Real* mTanTable;

        /** Private function to build trig tables.
        */
        void buildTrigTables();

		static Real SinTable (Real fValue);
		static Real TanTable (Real fValue);
    public:
        /** Default constructor.
            @param
                trigTableSize Optional parameter to set the size of the
                tables used to implement Sin, Cos, Tan
        */
        Math(unsigned int trigTableSize = 4096);

        /** Default destructor.
        */
        ~Math();

		static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
		static inline int ICeil (float fValue) { return int(ceil(fValue)); }
		static inline int IFloor (float fValue) { return int(floor(fValue)); }
        static int ISign (int iValue);

        /** Absolute value function
            @param
                fValue The value whose absolute value will be returned.
        */
		static inline Real Abs (Real fValue) { return Real(fabs(fValue)); }

        /** Absolute value function
            @param
                fValue The value, in degrees, whose absolute value will be returned.
         */
		static inline CDegree Abs (const CDegree& dValue) { return CDegree(fabs(dValue.valueDegrees())); }

        /** Absolute value function
            @param
                fValue The value, in radians, whose absolute value will be returned.
         */
        static inline CRadian Abs (const CRadian& rValue) { return CRadian(fabs(rValue.valueRadians())); }

        /** Arc cosine function
            @param
                fValue The value whose arc cosine will be returned.
         */
		static CRadian ACos (Real fValue);

        /** Arc sine function
            @param
                fValue The value whose arc sine will be returned.
         */
		static CRadian ASin (Real fValue);

        /** Arc tangent function
            @param
                fValue The value whose arc tangent will be returned.
         */
		static inline CRadian ATan (Real fValue) { return CRadian(atan(fValue)); }

        /** Arc tangent between two values function
            @param
                fY The first value to calculate the arc tangent with.
            @param
                fX The second value to calculate the arc tangent with.
         */
		static inline CRadian ATan2 (Real fY, Real fX) { return CRadian(atan2(fY,fX)); }

        /** Ceiling function
            Returns the smallest following integer. (example: Ceil(1.1) = 2)

            @param
                fValue The value to round up to the nearest integer.
         */
		static inline Real Ceil (Real fValue) { return Real(ceil(fValue)); }
		static inline bool isNaN(Real f)
		{
			// std::isnan() is C99, not supported by all compilers
			// However NaN always fails this next test, no other number does.
			return f != f;
		}

        /** Cosine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline Real Cos (const CRadian& fValue, bool useTables = false) {
			return (!useTables) ? Real(cos(fValue.valueRadians())) : SinTable(fValue.valueRadians() + HALF_PI);
		}
        /** Cosine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline Real Cos (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(cos(fValue)) : SinTable(fValue + HALF_PI);
		}

		static inline Real Exp (Real fValue) { return Real(exp(fValue)); }

        /** Floor function
            Returns the largest previous integer. (example: Floor(1.9) = 1)
         
            @param
                fValue The value to round down to the nearest integer.
         */
		static inline Real Floor (Real fValue) { return Real(floor(fValue)); }

		static inline Real Log (Real fValue) { return Real(log(fValue)); }

		/// Stored value of log(2) for frequent use
		static const Real LOG2;

		static inline Real Log2 (Real fValue) { return Real(log(fValue)/LOG2); }

		static inline Real LogN (Real base, Real fValue) { return Real(log(fValue)/log(base)); }

		static inline Real Pow (Real fBase, Real fExponent) { return Real(pow(fBase,fExponent)); }

        static Real Sign (Real fValue);
		static inline CRadian Sign ( const CRadian& rValue )
		{
			return CRadian(Sign(rValue.valueRadians()));
		}
		static inline CDegree Sign ( const CDegree& dValue )
		{
			return CDegree(Sign(dValue.valueDegrees()));
		}

        /** Sine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline Real Sin (const CRadian& fValue, bool useTables = false) {
			return (!useTables) ? Real(sin(fValue.valueRadians())) : SinTable(fValue.valueRadians());
		}
        /** Sine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline Real Sin (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(sin(fValue)) : SinTable(fValue);
		}

        /** Squared function.
            @param
                fValue The value to be squared (fValue^2)
        */
		static inline Real Sqr (Real fValue) { return fValue*fValue; }

        /** Square root function.
            @param
                fValue The value whose square root will be calculated.
         */
		static inline Real Sqrt (Real fValue) { return Real(sqrt(fValue)); }

        /** Square root function.
            @param
                fValue The value, in radians, whose square root will be calculated.
            @return
                The square root of the angle in radians.
         */
        static inline CRadian Sqrt (const CRadian& fValue) { return CRadian(sqrt(fValue.valueRadians())); }

        /** Square root function.
            @param
                fValue The value, in degrees, whose square root will be calculated.
            @return
                The square root of the angle in degrees.
         */
        static inline CDegree Sqrt (const CDegree& fValue) { return CDegree(sqrt(fValue.valueDegrees())); }

        /** Inverse square root i.e. 1 / Sqrt(x), good for vector
            normalisation.
            @param
                fValue The value whose inverse square root will be calculated.
        */
		static Real InvSqrt (Real fValue);

        /** Generate a random number of unit length.
            @return
                A random number in the range from [0,1].
        */
        static Real UnitRandom ();

        /** Generate a random number within the range provided.
            @param
                fLow The lower bound of the range.
            @param
                fHigh The upper bound of the range.
            @return
                A random number in the range from [fLow,fHigh].
         */
        static Real RangeRandom (Real fLow, Real fHigh);

        /** Generate a random number in the range [-1,1].
            @return
                A random number in the range from [-1,1].
         */
        static Real SymmetricRandom ();

        /** Tangent function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
		static inline Real Tan (const CRadian& fValue, bool useTables = false) {
			return (!useTables) ? Real(tan(fValue.valueRadians())) : TanTable(fValue.valueRadians());
		}
        /** Tangent function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
		static inline Real Tan (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(tan(fValue)) : TanTable(fValue);
		}

		static inline Real DegreesToRadians(Real degrees) { return degrees * fDeg2Rad; }
        static inline Real RadiansToDegrees(Real radians) { return radians * fRad2Deg; }

       /** These functions used to set the assumed angle units (radians or degrees) 
            expected when using the Angle type.
       @par
            You can set this directly after creating a new Root, and also before/after resource creation,
            depending on whether you want the change to affect resource files.
       */
       static void setAngleUnit(AngleUnit unit);
       /** Get the unit being used for angles. */
       static AngleUnit getAngleUnit(void);

       /** Convert from the current AngleUnit to radians. */
       static Real AngleUnitsToRadians(Real units);
       /** Convert from radians to the current AngleUnit . */
       static Real RadiansToAngleUnits(Real radians);
       /** Convert from the current AngleUnit to degrees. */
       static Real AngleUnitsToDegrees(Real units);
       /** Convert from degrees to the current AngleUnit. */
       static Real DegreesToAngleUnits(Real degrees);

       /** Checks whether a given point is inside a triangle, in a
            2-dimensional (Cartesian) space.
            @remarks
                The vertices of the triangle must be given in either
                trigonometrical (anticlockwise) or inverse trigonometrical
                (clockwise) order.
            @param
                p The point.
            @param
                a The triangle's first vertex.
            @param
                b The triangle's second vertex.
            @param
                c The triangle's third vertex.
            @return
                If the point resides in the triangle, <b>true</b> is
                returned.
            @par
                If the point is outside the triangle, <b>false</b> is
                returned.
        */
        static bool pointInTri2D(const CVector2& p, const CVector2& a, 
			const CVector2& b, const CVector2& c);

       /** Checks whether a given 3D point is inside a triangle.
       @remarks
            The vertices of the triangle must be given in either
            trigonometrical (anticlockwise) or inverse trigonometrical
            (clockwise) order, and the point must be guaranteed to be in the
			same plane as the triangle
        @param
            p The point.
        @param
            a The triangle's first vertex.
        @param
            b The triangle's second vertex.
        @param
            c The triangle's third vertex.
		@param 
			normal The triangle plane's normal (passed in rather than calculated
				on demand since the caller may already have it)
        @return
            If the point resides in the triangle, <b>true</b> is
            returned.
        @par
            If the point is outside the triangle, <b>false</b> is
            returned.
        */
        static bool pointInTri3D(const CVector3& p, const CVector3& a, 
			const CVector3& b, const CVector3& c, const CVector3& normal);
        /** Ray / plane intersection, returns boolean result and distance. */
        static std::pair<bool, Real> intersects(const CRay& ray, const CPlane& plane);

        /** Ray / sphere intersection, returns boolean result and distance. */
        static std::pair<bool, Real> intersects(const CRay& ray, const CSphere& sphere, 
            bool discardInside = true);
        
        /** Ray / box intersection, returns boolean result and distance. */
        static std::pair<bool, Real> intersects(const CRay& ray, const CAxisAlignedBox& box);

        /** Ray / box intersection, returns boolean result and two intersection distance.
        @param
            ray The ray.
        @param
            box The box.
        @param
            d1 A real pointer to retrieve the near intersection distance
                from the ray origin, maybe <b>null</b> which means don't care
                about the near intersection distance.
        @param
            d2 A real pointer to retrieve the far intersection distance
                from the ray origin, maybe <b>null</b> which means don't care
                about the far intersection distance.
        @return
            If the ray is intersects the box, <b>true</b> is returned, and
            the near intersection distance is return by <i>d1</i>, the
            far intersection distance is return by <i>d2</i>. Guarantee
            <b>0</b> <= <i>d1</i> <= <i>d2</i>.
        @par
            If the ray isn't intersects the box, <b>false</b> is returned, and
            <i>d1</i> and <i>d2</i> is unmodified.
        */
        static bool intersects(const CRay& ray, const CAxisAlignedBox& box,
            Real* d1, Real* d2);

        /** Ray / triangle intersection, returns boolean result and distance.
        @param
            ray The ray.
        @param
            a The triangle's first vertex.
        @param
            b The triangle's second vertex.
        @param
            c The triangle's third vertex.
		@param 
			normal The triangle plane's normal (passed in rather than calculated
				on demand since the caller may already have it), doesn't need
                normalised since we don't care.
        @param
            positiveSide Intersect with "positive side" of the triangle
        @param
            negativeSide Intersect with "negative side" of the triangle
        @return
            If the ray is intersects the triangle, a pair of <b>true</b> and the
            distance between intersection point and ray origin returned.
        @par
            If the ray isn't intersects the triangle, a pair of <b>false</b> and
            <b>0</b> returned.
        */
        static std::pair<bool, Real> intersects(const CRay& ray, const CVector3& a,
            const CVector3& b, const CVector3& c, const CVector3& normal,
            bool positiveSide = true, bool negativeSide = true);

        /** Ray / triangle intersection, returns boolean result and distance.
        @param
            ray The ray.
        @param
            a The triangle's first vertex.
        @param
            b The triangle's second vertex.
        @param
            c The triangle's third vertex.
        @param
            positiveSide Intersect with "positive side" of the triangle
        @param
            negativeSide Intersect with "negative side" of the triangle
        @return
            If the ray is intersects the triangle, a pair of <b>true</b> and the
            distance between intersection point and ray origin returned.
        @par
            If the ray isn't intersects the triangle, a pair of <b>false</b> and
            <b>0</b> returned.
        */
        static std::pair<bool, Real> intersects(const CRay& ray, const CVector3& a,
            const CVector3& b, const CVector3& c,
            bool positiveSide = true, bool negativeSide = true);

        /** Sphere / box intersection test. */
        static bool intersects(const CSphere& sphere, const CAxisAlignedBox& box);

        /** Plane / box intersection test. */
        static bool intersects(const CPlane& plane, const CAxisAlignedBox& box);

        /** Ray / convex plane list intersection test. 
        @param ray The ray to test with
        @param plaeList List of planes which form a convex volume
        @param normalIsOutside Does the normal point outside the volume
        */
        static std::pair<bool, Real> intersects(
            const CRay& ray, const vector<CPlane>& planeList, 
            bool normalIsOutside);
        /** Ray / convex plane list intersection test. 
        @param ray The ray to test with
        @param plaeList List of planes which form a convex volume
        @param normalIsOutside Does the normal point outside the volume
        */
        static std::pair<bool, Real> intersects(
            const CRay& ray, const list<CPlane>& planeList, 
            bool normalIsOutside);

        /** Sphere / plane intersection test. 
        @remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
        */
        static bool intersects(const CSphere& sphere, const CPlane& plane);

        /** Compare 2 reals, using tolerance for inaccuracies.
        */
        static bool RealEqual(Real a, Real b,
            Real tolerance = std::numeric_limits<Real>::epsilon());

        /** Calculates the tangent space vector for a given set of positions / texture coords. */
        static CVector3 calculateTangentSpaceVector(
            const CVector3& position1, const CVector3& position2, const CVector3& position3,
            Real u1, Real v1, Real u2, Real v2, Real u3, Real v3);

        /** Build a reflection matrix for the passed in plane. */
        static CMatrix4 buildReflectionMatrix(const CPlane& p);
        /** Calculate a face normal, including the w component which is the offset from the origin. */
        static CVector4 calculateFaceNormal(const CVector3& v1, const CVector3& v2, const CVector3& v3);
        /** Calculate a face normal, no w-information. */
        static CVector3 calculateBasicFaceNormal(const CVector3& v1, const CVector3& v2, const CVector3& v3);
        /** Calculate a face normal without normalize, including the w component which is the offset from the origin. */
        static CVector4 calculateFaceNormalWithoutNormalize(const CVector3& v1, const CVector3& v2, const CVector3& v3);
        /** Calculate a face normal without normalize, no w-information. */
        static CVector3 calculateBasicFaceNormalWithoutNormalize(const CVector3& v1, const CVector3& v2, const CVector3& v3);

		/** Generates a value based on the Gaussian (normal) distribution function
			with the given offset and scale parameters.
		*/
		static Real gaussianDistribution(Real x, Real offset = 0.0f, Real scale = 1.0f);

		/** Clamp a value within an inclusive range. */
		template <typename T>
		static T Clamp(T val, T minval, T maxval)
		{
			assert (minval <= maxval && "Invalid clamp range");
			return max(min(val, maxval), minval);
		}

		static CMatrix4 makeViewMatrix(const CVector3& position, const CQuaternion& orientation, 
			const CMatrix4* reflectMatrix = 0);

		/** Get a bounding radius value from a bounding box. */
		static Real boundingRadiusFromAABB(const CAxisAlignedBox& aabb);



        static const Real POS_INFINITY;
        static const Real NEG_INFINITY;
        static const Real PI;
        static const Real TWO_PI;
        static const Real HALF_PI;
		static const Real fDeg2Rad;
		static const Real fRad2Deg;

    };

	// these functions must be defined down here, because they rely on the
	// angle unit conversion functions in class Math:

	inline Real CRadian::valueDegrees() const
	{
		return Math::RadiansToDegrees ( mRad );
	}

	inline Real CRadian::valueAngleUnits() const
	{
		return Math::RadiansToAngleUnits ( mRad );
	}

	inline Real CDegree::valueRadians() const
	{
		return Math::DegreesToRadians ( mDeg );
	}

	inline Real CDegree::valueAngleUnits() const
	{
		return Math::DegreesToAngleUnits ( mDeg );
	}

	inline Angle::operator CRadian() const
	{
		return CRadian(Math::AngleUnitsToRadians(mAngle));
	}

	inline Angle::operator CDegree() const
	{
		return CDegree(Math::AngleUnitsToDegrees(mAngle));
	}

	inline CRadian operator * ( Real a, const CRadian& b )
	{
		return CRadian ( a * b.valueRadians() );
	}

	inline CRadian operator / ( Real a, const CRadian& b )
	{
		return CRadian ( a / b.valueRadians() );
	}

	inline CDegree operator * ( Real a, const CDegree& b )
	{
		return CDegree ( a * b.valueDegrees() );
	}

	inline CDegree operator / ( Real a, const CDegree& b )
	{
		return CDegree ( a / b.valueDegrees() );
	}
	/** @} */
	/** @} */

}
#endif
