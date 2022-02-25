#include "ISection.h"

namespace OC
{
	namespace Modeling
	{
		PipeSection::PipeSection():mType(UNKNOWM_SECTION),
			mWidth(0.0),
			mHeight(0.0),
			mRadius(0.0)
		{

		}

		PipeSection::PipeSection(double radius)
		{
			mType = CIRCULAR_SECTION;
			setRadius(radius);
		}

		PipeSection::PipeSection(double width,double height)
		{
			mType = RECT_SECTION;
			setWidth(width);
			setHeight(height);
		}

		PipeSection::~PipeSection()
		{

		}

		void PipeSection::setWidth(double w)
		{
			mWidth = w;
		}

		double PipeSection::getWidth()
		{
			return mWidth;
		}


		void PipeSection::setHeight(double h)
		{
			mHeight = h;
		}

		double PipeSection::getHeight()
		{
			return mHeight;
		}

		double PipeSection::getRadius()
		{
			if (mType == CIRCULAR_SECTION)
			{
				return mRadius;
			}
			else if (mType == RECT_SECTION)
			{
				return pow((mWidth*mWidth + mHeight*mHeight),0.5)/2.0;
			}
			else
			{
				return 0.0f;
			}
		}

		void PipeSection::setRadius(double r)
		{
			mRadius = r;
		}

		SectionType PipeSection::getType()
		{
			return mType;
		}

		void PipeSection::setType(SectionType type)
		{
			mType = type;
		}

		String PipeSection::toString()
		{
			StringStream sstream;
			sstream.precision(8);
			sstream <<mType<<" "<<mWidth<<" "<<mHeight<<" "<<mRadius;

			return sstream.str();
		}

		PipeSection PipeSection::parseSection(String sectStr)
		{
			PipeSection sect;
			StringVector sv = StringUtil::split(sectStr," ");
			if (sv.size() == 4)
			{
				sect.setType((SectionType)StringConverter::parseInt(sv[0]));
				sect.setWidth(StringConverter::parseReal(sv[1]));
				sect.setHeight(StringConverter::parseReal(sv[2]));
				sect.setRadius(StringConverter::parseReal(sv[3]));
			}
			return sect;
		}

		bool PipeSection::operator ==(const PipeSection& rhs)
		{
			return mType == rhs.mType && osg::equivalent(mWidth,rhs.mWidth)
				&& osg::equivalent(mHeight,rhs.mHeight)
				&& osg::equivalent(mRadius,rhs.mRadius);
		}

		bool PipeSection::operator !=(const PipeSection& rhs)
		{
			return !(*this == rhs);
		}

		inline bool PipeSection::operator > ( const PipeSection& rkSect )
		{
			return !(*this < rkSect);
		}

		inline bool PipeSection::operator < ( const PipeSection& rkSect )
		{
			return mRadius < rkSect.mRadius;
		}

		inline PipeSection& PipeSection::operator = ( const PipeSection& rkSect )
		{
			mType = rkSect.mType;
			mHeight = rkSect.mHeight;
			mWidth = rkSect.mWidth;
			mRadius = rkSect.mRadius;
			return *this;
		}

		inline PipeSection PipeSection::operator + ( const PipeSection& rkSect )
		{
			PipeSection sect = *this;

			if (CIRCULAR_SECTION == rkSect.mType)
			{
				sect.mRadius += rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				sect.mHeight += rkSect.mHeight;
				sect.mWidth += rkSect.mWidth;
			}

			return sect;
		}

		inline PipeSection PipeSection::operator - ( const PipeSection& rkSect )
		{
			PipeSection sect = *this;

			if (CIRCULAR_SECTION == rkSect.mType)
			{
				sect.mRadius -= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				sect.mHeight -= rkSect.mHeight;
				sect.mWidth -= rkSect.mWidth;
			}

			return sect;
		}

		inline PipeSection PipeSection::operator * ( const PipeSection& rkSect )
		{
			PipeSection sect = *this;

			if (CIRCULAR_SECTION == rkSect.mType)
			{
				sect.mRadius *= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				sect.mHeight *= rkSect.mHeight;
				sect.mWidth *= rkSect.mWidth;
			}

			return sect;
		}

		inline PipeSection PipeSection::operator / ( const PipeSection& rkSect )
		{
			PipeSection sect = *this;

			if (CIRCULAR_SECTION == rkSect.mType)
			{
				sect.mRadius /= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				sect.mHeight /= rkSect.mHeight;
				sect.mWidth /= rkSect.mWidth;
			}

			return sect;
		}

		inline PipeSection& PipeSection::operator += ( const PipeSection& rkSect )
		{
			if (CIRCULAR_SECTION == rkSect.mType)
			{
				mRadius += rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				mHeight += rkSect.mHeight;
				mWidth += rkSect.mWidth;
			}

			return *this;
		}

		inline PipeSection& PipeSection::operator -= ( const PipeSection& rkSect )
		{
			if (CIRCULAR_SECTION == rkSect.mType)
			{
				mRadius -= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				mHeight -= rkSect.mHeight;
				mWidth -= rkSect.mWidth;
			}

			return *this;
		}

		inline PipeSection& PipeSection::operator *= ( const PipeSection& rkSect )
		{
			if (CIRCULAR_SECTION == rkSect.mType)
			{
				mRadius *= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				mHeight *= rkSect.mHeight;
				mWidth *= rkSect.mWidth;
			}

			return *this;

		}
		inline PipeSection& PipeSection::operator /= ( const PipeSection& rkSect )
		{
			if (CIRCULAR_SECTION == rkSect.mType)
			{
				mRadius /= rkSect.mRadius;
			}
			if (RECT_SECTION == rkSect.mType)
			{
				mHeight /= rkSect.mHeight;
				mWidth /= rkSect.mWidth;
			}

			return *this;
		}


		inline PipeSection PipeSection::operator * ( const int Val)
		{
			PipeSection sect = *this;
			sect.mRadius *= (double)Val;
			sect.mHeight *= (double)Val;
			sect.mWidth *= (double)Val;
			return sect;
		}

		inline PipeSection PipeSection::operator * ( const double Val)
		{
			PipeSection sect = *this;
			sect.mRadius *= Val;
			sect.mHeight *= Val;
			sect.mWidth *= Val;
			return sect;
		}

		inline PipeSection PipeSection::operator / ( const double Val)
		{
			PipeSection sect = *this;
			sect.mRadius /= Val;
			sect.mHeight /= Val;
			sect.mWidth /= Val;
			return sect;

		}

		inline PipeSection& PipeSection::operator *= ( const double Val)
		{
			mRadius *= Val;
			mHeight *= Val;
			mWidth *= Val;
			return *this;
		}

		inline PipeSection& PipeSection::operator /= ( const double Val)
		{
			mRadius /= Val;
			mHeight /= Val;
			mWidth /= Val;
			return *this;
		}

		//////////////////////////////////////////////////////////////////////////
		ShelfSection::ShelfSection()
			:mType(H_SECTION)
		{

		}

		ShelfSection::~ShelfSection()
		{

		}

		ShelfSection ShelfSection::parse(String sectStr,bool isMillimeter)
		{
			int num = 1;
			if(isMillimeter)
			{
				num = 1000;
			}

			ShelfSection sect;

			StringVector sv = StringUtil::split(sectStr,"¡Á");
			if (sv.size() == 1)
			{
				sv = StringUtil::split(sectStr,"x");
			}
			if (sv.size() == 1)
			{
				sv = StringUtil::split(sectStr,"X");
			}

			if (sv.size() == 4)
			{				
				sect.params.push_back(StringConverter::parseReal(sv[0])/num);
				sect.params.push_back(StringConverter::parseReal(sv[1])/num);
				sect.params.push_back(StringConverter::parseReal(sv[2])/num);
				sect.params.push_back(StringConverter::parseReal(sv[3])/num);
				sect.setType(H_SECTION);
			}
			else
			{
				for(StringVector::iterator itr = sv.begin(); itr != sv.end() ;++itr)
				{
					sect.params.push_back(StringConverter::parseReal(*itr)/num);
				}
				sect.setType(UNKNOWM_SECTION);
			}

			return sect;
		}

		SectionType ShelfSection::getType()
		{
			return mType;
		}

		void ShelfSection::setType(SectionType type)
		{
			mType = type;
		}

		String ShelfSection::TagName = "ShelfSection";

		TiXmlElement* ShelfSection::toXmlElement()
		{
			TiXmlElement* element = new TiXmlElement(TagName);
			if (params.size() == 4)
			{
				element->SetAttribute("h",StringConverter::toString(params[0]));
				element->SetAttribute("w",StringConverter::toString(params[1]));
				element->SetAttribute("t1",StringConverter::toString(params[2]));
				element->SetAttribute("t2",StringConverter::toString(params[3]));
			}
			if(params.size() == 3)
			{
				element->SetAttribute("h",StringConverter::toString(params[0]));
				element->SetAttribute("w",StringConverter::toString(params[1]));
				element->SetAttribute("t1",StringConverter::toString(params[2]));
			}
			return element;
		}

		void ShelfSection::fromElement(TiXmlElement* element)
		{
			if (element == NULL)
			{
				return;
			}

			params.resize(4);
			params[0] = StringConverter::parseReal(TinyXml::read_attribute(element,"h"));
			params[1] = StringConverter::parseReal(TinyXml::read_attribute(element,"w"));
			params[2] = StringConverter::parseReal(TinyXml::read_attribute(element,"t1"));
			params[3] = StringConverter::parseReal(TinyXml::read_attribute(element,"t2"));
		}

	}
}


