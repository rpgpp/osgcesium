#include "Plane.h"
#include "Matrix3.h"
#include "AxisAlignedBox.h" 

namespace OC {
	//-----------------------------------------------------------------------
	CPlane::CPlane ()
	{
		normal = CVector3::ZERO;
		d = 0.0;
	}
	//-----------------------------------------------------------------------
	CPlane::CPlane (const CPlane& rhs)
	{
		normal = rhs.normal;
		d = rhs.d;
	}
	//-----------------------------------------------------------------------
	CPlane::CPlane (const CVector3& rkNormal, Real fConstant)
	{
		normal = rkNormal;
		d = -fConstant;
	}
	//---------------------------------------------------------------------
	CPlane::CPlane (Real a, Real b, Real c, Real _d)
		: normal(a, b, c), d(_d)
	{
	}
	//-----------------------------------------------------------------------
	CPlane::CPlane (const CVector3& rkNormal, const CVector3& rkPoint)
	{
		redefine(rkNormal, rkPoint);
	}
	//-----------------------------------------------------------------------
	CPlane::CPlane (const CVector3& rkPoint0, const CVector3& rkPoint1,
		const CVector3& rkPoint2)
	{
		redefine(rkPoint0, rkPoint1, rkPoint2);
	}
	//-----------------------------------------------------------------------
	Real CPlane::getDistance (const CVector3& rkPoint) const
	{
		return normal.dotProduct(rkPoint) + d;
	}
	//-----------------------------------------------------------------------
	CPlane::Side CPlane::getSide (const CVector3& rkPoint) const
	{
		Real fDistance = getDistance(rkPoint);

		if ( fDistance < 0.0 )
			return CPlane::NEGATIVE_SIDE;

		if ( fDistance > 0.0 )
			return CPlane::POSITIVE_SIDE;

		return CPlane::NO_SIDE;
	}


	//-----------------------------------------------------------------------
	CPlane::Side CPlane::getSide (const CAxisAlignedBox& box) const
	{
		if (box.isNull()) 
			return NO_SIDE;
		if (box.isInfinite())
			return BOTH_SIDE;

        return getSide(box.getCenter(), box.getHalfSize());
	}
    //-----------------------------------------------------------------------
    CPlane::Side CPlane::getSide (const CVector3& centre, const CVector3& halfSize) const
    {
        // Calculate the distance between box centre and the plane
        Real dist = getDistance(centre);

        // Calculate the maximise allows absolute distance for
        // the distance between box centre and plane
        Real maxAbsDist = normal.absDotProduct(halfSize);

        if (dist < -maxAbsDist)
            return CPlane::NEGATIVE_SIDE;

        if (dist > +maxAbsDist)
            return CPlane::POSITIVE_SIDE;

        return CPlane::BOTH_SIDE;
    }
	//-----------------------------------------------------------------------
	void CPlane::redefine(const CVector3& rkPoint0, const CVector3& rkPoint1,
		const CVector3& rkPoint2)
	{
		CVector3 kEdge1 = rkPoint1 - rkPoint0;
		CVector3 kEdge2 = rkPoint2 - rkPoint0;
		normal = kEdge1.crossProduct(kEdge2);
		normal.normalise();
		d = -normal.dotProduct(rkPoint0);
	}
	//-----------------------------------------------------------------------
	void CPlane::redefine(const CVector3& rkNormal, const CVector3& rkPoint)
	{
		normal = rkNormal;
		d = -rkNormal.dotProduct(rkPoint);
	}
	//-----------------------------------------------------------------------
	CVector3 CPlane::projectVector(const CVector3& p) const
	{
		// We know plane normal is unit length, so use simple method
		CMatrix3 xform;
		xform[0][0] = 1.0f - normal.x * normal.x;
		xform[0][1] = -normal.x * normal.y;
		xform[0][2] = -normal.x * normal.z;
		xform[1][0] = -normal.y * normal.x;
		xform[1][1] = 1.0f - normal.y * normal.y;
		xform[1][2] = -normal.y * normal.z;
		xform[2][0] = -normal.z * normal.x;
		xform[2][1] = -normal.z * normal.y;
		xform[2][2] = 1.0f - normal.z * normal.z;
		return xform * p;

	}
	//-----------------------------------------------------------------------
    Real CPlane::normalise(void)
    {
        Real fLength = normal.length();

        // Will also work for zero-sized vectors, but will change nothing
		// We're not using epsilons because we don't need to.
        // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
        if ( fLength > Real(0.0f) )
        {
            Real fInvLength = 1.0f / fLength;
            normal *= fInvLength;
            d *= fInvLength;
        }

        return fLength;
    }
	//-----------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& o, const CPlane& p)
	{
		o << "Plane(normal=" << p.normal << ", d=" << p.d << ")";
		return o;
	}
} // namespace Ogre
