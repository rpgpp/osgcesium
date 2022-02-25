#ifndef __Ray_H_
#define __Ray_H_

// Precompiler options
#include "UtilityDefine.h"
#include "Vector3.h"
#include "PlaneBoundedVolume.h"

namespace OC {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** Representation of a ray in space, i.e. a line with an origin and direction. */
    class _UtilityExport CRay
    {
    protected:
        CVector3 mOrigin;
        CVector3 mDirection;
    public:
        CRay():mOrigin(CVector3::ZERO), mDirection(CVector3::UNIT_Z) {}
        CRay(const CVector3& origin, const CVector3& direction)
            :mOrigin(origin), mDirection(direction) {}

        /** Sets the origin of the ray. */
        void setOrigin(const CVector3& origin) {mOrigin = origin;} 
        /** Gets the origin of the ray. */
        const CVector3& getOrigin(void) const {return mOrigin;} 

        /** Sets the direction of the ray. */
        void setDirection(const CVector3& dir) {mDirection = dir;} 
        /** Gets the direction of the ray. */
        const CVector3& getDirection(void) const {return mDirection;} 

		/** Gets the position of a point t units along the ray. */
		CVector3 getPoint(Real t) const { 
			return CVector3(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		CVector3 operator*(Real t) const { 
			return getPoint(t);
		}

		/** Tests whether this ray intersects the given plane. 
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, Real> intersects(const CPlane& p) const
		{
			return Math::intersects(*this, p);
		}
        /** Tests whether this ray intersects the given plane bounded volume. 
        @return A pair structure where the first element indicates whether
        an intersection occurs, and if true, the second element will
        indicate the distance along the ray at which it intersects. 
        This can be converted to a point in space by calling getPoint().
        */
        std::pair<bool, Real> intersects(const CPlaneBoundedVolume& p) const
        {
            return Math::intersects(*this, p.planes, p.outside == CPlane::POSITIVE_SIDE);
        }
		/** Tests whether this ray intersects the given sphere. 
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, Real> intersects(const CSphere& s) const
		{
			return Math::intersects(*this, s);
		}
		/** Tests whether this ray intersects the given box. 
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, Real> intersects(const CAxisAlignedBox& box) const
		{
			return Math::intersects(*this, box);
		}

    };
	/** @} */
	/** @} */

}
#endif
