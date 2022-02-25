#ifndef __PlaneBoundedVolume_H_
#define __PlaneBoundedVolume_H_

// Precompiler options
#include "UtilityDefine.h"
#include "AxisAlignedBox.h"
#include "Sphere.h"
#include "MathUtil.h"
#include "Plane.h"

namespace OC {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** Represents a convex volume bounded by planes.
    */
    class _UtilityExport CPlaneBoundedVolume
    {
    public:
        typedef vector<CPlane> PlaneList;
        /// Publicly accessible plane list, you can modify this direct
        PlaneList planes;
        CPlane::Side outside;

        CPlaneBoundedVolume() :outside(CPlane::NEGATIVE_SIDE) {}
        /** Constructor, determines which side is deemed to be 'outside' */
        CPlaneBoundedVolume(CPlane::Side theOutside) 
            : outside(theOutside) {}

        /** Intersection test with AABB
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const CAxisAlignedBox& box) const
        {
            if (box.isNull()) return false;
            if (box.isInfinite()) return true;

            // Get centre of the box
            CVector3 centre = box.getCenter();
            // Get the half-size of the box
            CVector3 halfSize = box.getHalfSize();
            
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const CPlane& plane = *i;

                CPlane::Side side = plane.getSide(centre, halfSize);
                if (side == outside)
                {
                    // Found a splitting plane therefore return not intersecting
                    return false;
                }
            }

            // couldn't find a splitting plane, assume intersecting
            return true;

        }
        /** Intersection test with Sphere
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const CSphere& sphere) const
        {
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const CPlane& plane = *i;

                // Test which side of the plane the sphere is
                Real d = plane.getDistance(sphere.getCenter());
                // Negate d if planes point inwards
                if (outside == CPlane::NEGATIVE_SIDE) d = -d;

                if ( (d - sphere.getRadius()) > 0)
                    return false;
            }

            return true;

        }

        /** Intersection test with a Ray
        @return std::pair of hit (bool) and distance
        @remarks May return false positives but will never miss an intersection.
        */
        inline std::pair<bool, Real> intersects(const CRay& ray)
        {
            return Math::intersects(ray, planes, outside == CPlane::POSITIVE_SIDE);
        }

    };

    typedef vector<CPlaneBoundedVolume> PlaneBoundedVolumeList;

	/** @} */
	/** @} */

}

#endif

