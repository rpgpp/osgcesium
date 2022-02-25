#ifndef _CORE_Rectangle_H__
#define _CORE_Rectangle_H__

#include "Vector2.h"

namespace OC {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/

    class _UtilityExport CRectangle
    {
	public:
		enum Extent
		{
			EXTENT_NULL,
			EXTENT_FINITE,
			EXTENT_INFINITE
		};
	protected:
		CVector2 mMinimum;
		CVector2 mMaximum;
		Extent mExtent;
	public:
		inline CRectangle()
		{
			setMinimum(0.0f,0.0f);
			setMaximum(0.0f,0.0f);
			mExtent = EXTENT_NULL;
		}

		inline CRectangle( const Real xmin, const Real ymin, const Real xmax, const Real ymax )
		{
			setMinimum(xmin,ymin);
			setMaximum(xmax,ymax);
		}

		void setMinimum(Real x, Real y)
		{
			mExtent = EXTENT_FINITE;
			mMinimum.x = x;
			mMinimum.y = y;
		}

		void setMaximum(Real x, Real y)
		{
			mExtent = EXTENT_FINITE;
			mMaximum.x = x;
			mMaximum.y = y;
		}

        inline bool inside(Real x, Real y) const { return x >= mMinimum.x && x <= mMaximum.x && y >= mMinimum.y && y <= mMaximum.y; }

		inline void setExtents( const CVector2& min, const CVector2& max )
		{
			assert( (min.x <= max.x && min.y <= max.y) &&
				"The minimum corner of the box must be less than or equal to maximum corner" );

			mExtent = EXTENT_FINITE;
			mMinimum = min;
			mMaximum = max;
		}

		inline bool operator == ( const CRectangle& rkVector ) const
		{
			return ( mMinimum == rkVector.mMinimum &&
				mMaximum == rkVector.mMaximum );
		}

		inline bool operator != ( const CRectangle& rkVector ) const
		{
			return ( mMinimum != rkVector.mMinimum ||
				mMaximum != rkVector.mMaximum );
		}

		inline CRectangle& operator = (const CRectangle& rhs)
		{
			mMinimum = rhs.mMinimum;
			mMaximum = rhs.mMaximum;
			return *this;
		}

		inline void merge( const CVector2& point )
		{
			switch (mExtent)
			{
			case EXTENT_NULL: // if null, use this point
				setExtents(point, point);
				return;

			case EXTENT_FINITE:
				mMaximum.makeCeil(point);
				mMinimum.makeFloor(point);
				return;

			case EXTENT_INFINITE: // if infinite, makes no difference
				return;
			}

			assert( false && "Never reached" );
		}

		inline CVector2 getCenter()
		{
			return (mMinimum + mMaximum) * 0.5f;
		}

		inline Real getWidth()
		{
			return mMaximum.x - mMinimum.x;
		}

		inline Real getHeight()
		{
			return mMaximum.y - mMinimum.y;
		}

		/** Gets the minimum corner of the box.
		*/
		inline const CVector2& getMinimum(void) const
		{ 
			return mMinimum; 
		}

		/** Gets a modifiable version of the minimum
		corner of the box.
		*/
		inline CVector2& getMinimum(void)
		{ 
			return mMinimum; 
		}

		/** Gets the maximum corner of the box.
		*/
		inline const CVector2& getMaximum(void) const
		{ 
			return mMaximum;
		}

		/** Gets a modifiable version of the maximum
		corner of the box.
		*/
		inline CVector2& getMaximum(void)
		{ 
			return mMaximum;
		}

		inline CRectangle intersect(const CRectangle& rhs)
		{
			CRectangle r = *this;

			r.getMinimum().makeCeil(rhs.getMinimum());
			r.getMaximum().makeFloor(rhs.getMaximum());

			return r;
		}

		inline CRectangle unionRect(const CRectangle& rhs)
		{
			CRectangle r = *this;

			r.getMinimum().makeFloor(rhs.getMinimum());
			r.getMaximum().makeCeil(rhs.getMaximum());

			return r;
		}

		static const CRectangle ZERO;
    };

	

	/** @} */
	/** @} */
}

#endif // _Rectangle_H__
