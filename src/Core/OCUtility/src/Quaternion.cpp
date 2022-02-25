#include "Quaternion.h"
#include "MathUtil.h"
#include "Matrix3.h"
#include "Vector3.h"

namespace OC {

    const Real CQuaternion::msEpsilon = 1e-03;
    const CQuaternion CQuaternion::ZERO(0,0,0,0);
    const CQuaternion CQuaternion::IDENTITY(1,0,0,0);

    //-----------------------------------------------------------------------
    void CQuaternion::FromRotationMatrix (const CMatrix3& kRot)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        Real fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
        Real fRoot;

        if ( fTrace > 0.0 )
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Math::Sqrt(fTrace + 1.0f);  // 2w
            w = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;  // 1/(4w)
            x = (kRot[2][1]-kRot[1][2])*fRoot;
            y = (kRot[0][2]-kRot[2][0])*fRoot;
            z = (kRot[1][0]-kRot[0][1])*fRoot;
        }
        else
        {
            // |w| <= 1/2
            static size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if ( kRot[1][1] > kRot[0][0] )
                i = 1;
            if ( kRot[2][2] > kRot[i][i] )
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = Math::Sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
            Real* apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            w = (kRot[k][j]-kRot[j][k])*fRoot;
            *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
            *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
        }
    }
    //-----------------------------------------------------------------------
    void CQuaternion::ToRotationMatrix (CMatrix3& kRot) const
    {
        Real fTx  = x+x;
        Real fTy  = y+y;
        Real fTz  = z+z;
        Real fTwx = fTx*w;
        Real fTwy = fTy*w;
        Real fTwz = fTz*w;
        Real fTxx = fTx*x;
        Real fTxy = fTy*x;
        Real fTxz = fTz*x;
        Real fTyy = fTy*y;
        Real fTyz = fTz*y;
        Real fTzz = fTz*z;

        kRot[0][0] = 1.0f-(fTyy+fTzz);
        kRot[0][1] = fTxy-fTwz;
        kRot[0][2] = fTxz+fTwy;
        kRot[1][0] = fTxy+fTwz;
        kRot[1][1] = 1.0f-(fTxx+fTzz);
        kRot[1][2] = fTyz-fTwx;
        kRot[2][0] = fTxz-fTwy;
        kRot[2][1] = fTyz+fTwx;
        kRot[2][2] = 1.0f-(fTxx+fTyy);
    }
    //-----------------------------------------------------------------------
    void CQuaternion::FromAngleAxis (const CRadian& rfAngle,
        const CVector3& rkAxis)
    {
        // assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        CRadian fHalfAngle ( 0.5*rfAngle );
        Real fSin = Math::Sin(fHalfAngle);
        w = Math::Cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;
    }
    //-----------------------------------------------------------------------
    void CQuaternion::ToAngleAxis (CRadian& rfAngle, CVector3& rkAxis) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        Real fSqrLength = x*x+y*y+z*z;
        if ( fSqrLength > 0.0 )
        {
            rfAngle = 2.0*Math::ACos(w);
            Real fInvLength = Math::InvSqrt(fSqrLength);
            rkAxis.x = x*fInvLength;
            rkAxis.y = y*fInvLength;
            rkAxis.z = z*fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rfAngle = CRadian(0.0);
            rkAxis.x = 1.0;
            rkAxis.y = 0.0;
            rkAxis.z = 0.0;
        }
    }
    //-----------------------------------------------------------------------
    void CQuaternion::FromAxes (const CVector3* akAxis)
    {
        CMatrix3 kRot;

        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            kRot[0][iCol] = akAxis[iCol].x;
            kRot[1][iCol] = akAxis[iCol].y;
            kRot[2][iCol] = akAxis[iCol].z;
        }

        FromRotationMatrix(kRot);
    }
    //-----------------------------------------------------------------------
    void CQuaternion::FromAxes (const CVector3& xaxis, const CVector3& yaxis, const CVector3& zaxis)
    {
        CMatrix3 kRot;

        kRot[0][0] = xaxis.x;
        kRot[1][0] = xaxis.y;
        kRot[2][0] = xaxis.z;

        kRot[0][1] = yaxis.x;
        kRot[1][1] = yaxis.y;
        kRot[2][1] = yaxis.z;

        kRot[0][2] = zaxis.x;
        kRot[1][2] = zaxis.y;
        kRot[2][2] = zaxis.z;

        FromRotationMatrix(kRot);

    }
    //-----------------------------------------------------------------------
    void CQuaternion::ToAxes (CVector3* akAxis) const
    {
        CMatrix3 kRot;

        ToRotationMatrix(kRot);

        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            akAxis[iCol].x = kRot[0][iCol];
            akAxis[iCol].y = kRot[1][iCol];
            akAxis[iCol].z = kRot[2][iCol];
        }
    }
    //-----------------------------------------------------------------------
    CVector3 CQuaternion::xAxis(void) const
    {
        //Real fTx  = 2.0*x;
        Real fTy  = 2.0f*y;
        Real fTz  = 2.0f*z;
        Real fTwy = fTy*w;
        Real fTwz = fTz*w;
        Real fTxy = fTy*x;
        Real fTxz = fTz*x;
        Real fTyy = fTy*y;
        Real fTzz = fTz*z;

        return CVector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
    }
    //-----------------------------------------------------------------------
    CVector3 CQuaternion::yAxis(void) const
    {
        Real fTx  = 2.0f*x;
        Real fTy  = 2.0f*y;
        Real fTz  = 2.0f*z;
        Real fTwx = fTx*w;
        Real fTwz = fTz*w;
        Real fTxx = fTx*x;
        Real fTxy = fTy*x;
        Real fTyz = fTz*y;
        Real fTzz = fTz*z;

        return CVector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
    }
    //-----------------------------------------------------------------------
    CVector3 CQuaternion::zAxis(void) const
    {
        Real fTx  = 2.0f*x;
        Real fTy  = 2.0f*y;
        Real fTz  = 2.0f*z;
        Real fTwx = fTx*w;
        Real fTwy = fTy*w;
        Real fTxx = fTx*x;
        Real fTxz = fTz*x;
        Real fTyy = fTy*y;
        Real fTyz = fTz*y;

        return CVector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
    }
    //-----------------------------------------------------------------------
    void CQuaternion::ToAxes (CVector3& xaxis, CVector3& yaxis, CVector3& zaxis) const
    {
        CMatrix3 kRot;

        ToRotationMatrix(kRot);

        xaxis.x = kRot[0][0];
        xaxis.y = kRot[1][0];
        xaxis.z = kRot[2][0];

        yaxis.x = kRot[0][1];
        yaxis.y = kRot[1][1];
        yaxis.z = kRot[2][1];

        zaxis.x = kRot[0][2];
        zaxis.y = kRot[1][2];
        zaxis.z = kRot[2][2];
    }

    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::operator+ (const CQuaternion& rkQ) const
    {
        return CQuaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::operator- (const CQuaternion& rkQ) const
    {
        return CQuaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::operator* (const CQuaternion& rkQ) const
    {
        // NOTE:  Multiplication is not generally commutative, so in most
        // cases p*q != q*p.

        return CQuaternion
        (
            w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
        );
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::operator* (Real fScalar) const
    {
        return CQuaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
    }
    //-----------------------------------------------------------------------
    CQuaternion operator* (Real fScalar, const CQuaternion& rkQ)
    {
        return CQuaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
            fScalar*rkQ.z);
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::operator- () const
    {
        return CQuaternion(-w,-x,-y,-z);
    }
    //-----------------------------------------------------------------------
    Real CQuaternion::Dot (const CQuaternion& rkQ) const
    {
        return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
    }
    //-----------------------------------------------------------------------
    Real CQuaternion::Norm () const
    {
        return w*w+x*x+y*y+z*z;
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::Inverse () const
    {
        Real fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            Real fInvNorm = 1.0f/fNorm;
            return CQuaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return ZERO;
        }
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::UnitInverse () const
    {
        // assert:  'this' is unit length
        return CQuaternion(w,-x,-y,-z);
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::Exp () const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
        // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

        CRadian fAngle ( Math::Sqrt(x*x+y*y+z*z) );
        Real fSin = Math::Sin(fAngle);

        CQuaternion kResult;
        kResult.w = Math::Cos(fAngle);

        if ( Math::Abs(fSin) >= msEpsilon )
        {
            Real fCoeff = fSin/(fAngle.valueRadians());
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
        }
        else
        {
            kResult.x = x;
            kResult.y = y;
            kResult.z = z;
        }

        return kResult;
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::Log () const
    {
        // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
        // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

        CQuaternion kResult;
        kResult.w = 0.0;

        if ( Math::Abs(w) < 1.0 )
        {
            CRadian fAngle ( Math::ACos(w) );
            Real fSin = Math::Sin(fAngle);
            if ( Math::Abs(fSin) >= msEpsilon )
            {
                Real fCoeff = fAngle.valueRadians()/fSin;
                kResult.x = fCoeff*x;
                kResult.y = fCoeff*y;
                kResult.z = fCoeff*z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
    }
    //-----------------------------------------------------------------------
    CVector3 CQuaternion::operator* (const CVector3& v) const
    {
		// nVidia SDK implementation
		CVector3 uv, uuv;
		CVector3 qvec(x, y, z);
		uv = qvec.crossProduct(v);
		uuv = qvec.crossProduct(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;

    }
    //-----------------------------------------------------------------------
	bool CQuaternion::equals(const CQuaternion& rhs, const CRadian& tolerance) const
	{
        Real fCos = Dot(rhs);
        CRadian angle = Math::ACos(fCos);

		return (Math::Abs(angle.valueRadians()) <= tolerance.valueRadians())
            || Math::RealEqual(angle.valueRadians(), Math::PI, tolerance.valueRadians());


	}
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::Slerp (Real fT, const CQuaternion& rkP,
        const CQuaternion& rkQ, bool shortestPath)
    {
        Real fCos = rkP.Dot(rkQ);
        CQuaternion rkT;

        // Do we need to invert rotation?
        if (fCos < 0.0f && shortestPath)
        {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else
        {
            rkT = rkQ;
        }

        if (Math::Abs(fCos) < 1 - msEpsilon)
        {
            // Standard case (slerp)
            Real fSin = Math::Sqrt(1 - Math::Sqr(fCos));
            CRadian fAngle = Math::ATan2(fSin, fCos);
            Real fInvSin = 1.0f / fSin;
            Real fCoeff0 = Math::Sin((1.0f - fT) * fAngle) * fInvSin;
            Real fCoeff1 = Math::Sin(fT * fAngle) * fInvSin;
            return fCoeff0 * rkP + fCoeff1 * rkT;
        }
        else
        {
            // There are two situations:
            // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
            //    interpolation safely.
            // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
            //    are an infinite number of possibilities interpolation. but we haven't
            //    have method to fix this case, so just use linear interpolation here.
            CQuaternion t = (1.0f - fT) * rkP + fT * rkT;
            // taking the complement requires renormalisation
            t.normalise();
            return t;
        }
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::SlerpExtraSpins (Real fT,
        const CQuaternion& rkP, const CQuaternion& rkQ, int iExtraSpins)
    {
        Real fCos = rkP.Dot(rkQ);
        CRadian fAngle ( Math::ACos(fCos) );

        if ( Math::Abs(fAngle.valueRadians()) < msEpsilon )
            return rkP;

        Real fSin = Math::Sin(fAngle);
        CRadian fPhase ( Math::PI*iExtraSpins*fT );
        Real fInvSin = 1.0f/fSin;
        Real fCoeff0 = Math::Sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
        Real fCoeff1 = Math::Sin(fT*fAngle + fPhase)*fInvSin;
        return fCoeff0*rkP + fCoeff1*rkQ;
    }
    //-----------------------------------------------------------------------
    void CQuaternion::Intermediate (const CQuaternion& rkQ0,
        const CQuaternion& rkQ1, const CQuaternion& rkQ2,
        CQuaternion& rkA, CQuaternion& rkB)
    {
        // assert:  q0, q1, q2 are unit quaternions

        CQuaternion kQ0inv = rkQ0.UnitInverse();
        CQuaternion kQ1inv = rkQ1.UnitInverse();
        CQuaternion rkP0 = kQ0inv*rkQ1;
        CQuaternion rkP1 = kQ1inv*rkQ2;
        CQuaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
        CQuaternion kMinusArg = -kArg;

        rkA = rkQ1*kArg.Exp();
        rkB = rkQ1*kMinusArg.Exp();
    }
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::Squad (Real fT,
        const CQuaternion& rkP, const CQuaternion& rkA,
        const CQuaternion& rkB, const CQuaternion& rkQ, bool shortestPath)
    {
        Real fSlerpT = 2.0f*fT*(1.0f-fT);
        CQuaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
        CQuaternion kSlerpQ = Slerp(fT, rkA, rkB);
        return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
    }
    //-----------------------------------------------------------------------
    Real CQuaternion::normalise(void)
    {
        Real len = Norm();
        Real factor = 1.0f / Math::Sqrt(len);
        *this = *this * factor;
        return len;
    }
    //-----------------------------------------------------------------------
	CRadian CQuaternion::getRoll(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// roll = atan2(localx.y, localx.x)
			// pick parts of xAxis() implementation that we need
//			Real fTx  = 2.0*x;
			Real fTy  = 2.0f*y;
			Real fTz  = 2.0f*z;
			Real fTwz = fTz*w;
			Real fTxy = fTy*x;
			Real fTyy = fTy*y;
			Real fTzz = fTz*z;

			// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

			return CRadian(Math::ATan2(fTxy+fTwz, 1.0f-(fTyy+fTzz)));

		}
		else
		{
			return CRadian(Math::ATan2(2*(x*y + w*z), w*w + x*x - y*y - z*z));
		}
	}
    //-----------------------------------------------------------------------
	CRadian CQuaternion::getPitch(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// pitch = atan2(localy.z, localy.y)
			// pick parts of yAxis() implementation that we need
			Real fTx  = 2.0f*x;
//			Real fTy  = 2.0f*y;
			Real fTz  = 2.0f*z;
			Real fTwx = fTx*w;
			Real fTxx = fTx*x;
			Real fTyz = fTz*y;
			Real fTzz = fTz*z;

			// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
			return CRadian(Math::ATan2(fTyz+fTwx, 1.0f-(fTxx+fTzz)));
		}
		else
		{
			// internal version
			return CRadian(Math::ATan2(2*(y*z + w*x), w*w - x*x - y*y + z*z));
		}
	}
    //-----------------------------------------------------------------------
	CRadian CQuaternion::getYaw(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// yaw = atan2(localz.x, localz.z)
			// pick parts of zAxis() implementation that we need
			Real fTx  = 2.0f*x;
			Real fTy  = 2.0f*y;
			Real fTz  = 2.0f*z;
			Real fTwy = fTy*w;
			Real fTxx = fTx*x;
			Real fTxz = fTz*x;
			Real fTyy = fTy*y;

			// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

			return CRadian(Math::ATan2(fTxz+fTwy, 1.0f-(fTxx+fTyy)));

		}
		else
		{
			// internal version
			return CRadian(Math::ASin(-2*(x*z - w*y)));
		}
	}
    //-----------------------------------------------------------------------
    CQuaternion CQuaternion::nlerp(Real fT, const CQuaternion& rkP,
        const CQuaternion& rkQ, bool shortestPath)
    {
		CQuaternion result;
        Real fCos = rkP.Dot(rkQ);
		if (fCos < 0.0f && shortestPath)
		{
			result = rkP + fT * ((-rkQ) - rkP);
		}
		else
		{
			result = rkP + fT * (rkQ - rkP);
		}
        result.normalise();
        return result;
    }
}
