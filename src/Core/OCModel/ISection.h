#ifndef _PIPE_SECTION_H_
#define _PIPE_SECTION_H_

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		enum SectionType{UNKNOWM_SECTION,CIRCULAR_SECTION,RECT_SECTION,H_SECTION};

		class _ModelExport PipeSection
 		{
		public:
			PipeSection();
			PipeSection(double radius);
			PipeSection(double width,double height);
			~PipeSection();

			void setWidth(double w);
			double getWidth();

			void setHeight(double h);
			double getHeight();

			double getRadius();
			void setRadius(double r);

			SectionType getType();
			void setType(SectionType type);

			bool operator ==(const PipeSection& rhs);
			bool operator !=(const PipeSection& rhs);

			inline bool operator > ( const PipeSection& rkSect );
			inline bool operator < ( const PipeSection& rkSect );
			inline PipeSection& operator = ( const PipeSection& rkSect );
			inline PipeSection operator + ( const PipeSection& rkSect );
			inline PipeSection operator - ( const PipeSection& rkSect );
			inline PipeSection operator * ( const PipeSection& rkSect );
			inline PipeSection operator / ( const PipeSection& rkSect );
			inline PipeSection& operator += ( const PipeSection& rkSect );
			inline PipeSection& operator -= ( const PipeSection& rkSect );
			inline PipeSection& operator *= ( const PipeSection& rkSect );
			inline PipeSection& operator /= ( const PipeSection& rkSect );
			inline PipeSection operator * ( const int Val);
			inline PipeSection operator * ( const double Val);
			inline PipeSection operator / ( const double Val);
			inline PipeSection& operator *= ( const double Val);
			inline PipeSection& operator /= ( const double Val);

			String toString();
			static PipeSection parseSection(String sectStr);

		protected:
			SectionType mType;
			double mWidth,mHeight,mRadius;
		};

		class _ModelExport TriangleSection
		{
		public:
			TriangleSection()
				:mLeftOffset(CVector2::ZERO)
				,mRightOffset(CVector2::ZERO)
			{}
			~TriangleSection(){}

			CVector2 mLeftOffset;
			CVector2 mRightOffset;
		};

		class _ModelExport ShelfSection
		{
		public:
			ShelfSection();
			~ShelfSection();
			static String TagName;
			doubleVector params;

			static ShelfSection parse(String sectStr,bool isMillimeter = true);

			SectionType getType();
			void setType(SectionType type);

			TiXmlElement* toXmlElement();
			void fromElement(TiXmlElement* element);
		protected:
			SectionType mType;
		};

	}
}


#endif