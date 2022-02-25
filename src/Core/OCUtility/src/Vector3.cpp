#include "Vector3.h"
#include "Vector3i.h"
#include "MathUtil.h"

namespace OC
{
    const CVector3 CVector3::ZERO( 0, 0, 0 );

    const CVector3 CVector3::UNIT_X( 1, 0, 0 );
    const CVector3 CVector3::UNIT_Y( 0, 1, 0 );
    const CVector3 CVector3::UNIT_Z( 0, 0, 1 );
    const CVector3 CVector3::NEGATIVE_UNIT_X( -1,  0,  0 );
    const CVector3 CVector3::NEGATIVE_UNIT_Y(  0, -1,  0 );
    const CVector3 CVector3::NEGATIVE_UNIT_Z(  0,  0, -1 );
    const CVector3 CVector3::UNIT_SCALE(1, 1, 1);

	const CVector3i CVector3i::ZERO( 0, 0, 0 );
	const CVector3i CVector3i::UNIT_X( 1, 0, 0 );
	const CVector3i CVector3i::UNIT_Y( 0, 1, 0 );
	const CVector3i CVector3i::UNIT_Z( 0, 0, 1 );
	const CVector3i CVector3i::NEGATIVE_UNIT_X( -1,  0,  0 );
	const CVector3i CVector3i::NEGATIVE_UNIT_Y(  0, -1,  0 );
	const CVector3i CVector3i::NEGATIVE_UNIT_Z(  0,  0, -1 );
	const CVector3i CVector3i::UNIT_SCALE(1, 1, 1);

	inline CVector3i& CVector3i::operator = ( const CVector3& rkVector )
	{
		x = (int)rkVector.x;
		y = (int)rkVector.y;
		z = (int)rkVector.z;

		return *this;
	}

	inline CVector3& CVector3::operator = ( const CVector3i& rkVector )
	{
		x = (Real)rkVector.x;
		y = (Real)rkVector.y;
		z = (Real)rkVector.z;

		return *this;
	}

	inline CVector3 CVector3::operator + ( const CVector3i& rkVector ) const
	{
		return CVector3(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z);
	}

	inline CVector3 CVector3::operator - ( const CVector3i& rkVector ) const
	{
		return CVector3(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z);
	}

	inline CVector3& CVector3::operator += ( const CVector3i& rkVector )
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;

		return *this;
	}

	inline CVector3& CVector3::operator -= ( const CVector3i& rkVector )
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;

		return *this;
	}
}
