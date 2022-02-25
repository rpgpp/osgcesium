#include "PipeData.h"
#include "GeometryBuilder.h"
#include "ModelUtility.h"
#include "IModelTable.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	template<>Core* CSingleton<PipeGeometryFactory>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int,Modeling::PipeGeometryFactory*> CSingleton<Modeling::PipeGeometryFactory>::msPidton;
#endif


	namespace Modeling
	{
		unsigned short IPipeDataBuilder::LOOP_R = 20;
		unsigned short IPipeDataBuilder::LOOP_C = 10;

		void IPipeDataBuilder::setLowQuality()
		{
			LOOP_R = 20;
			LOOP_C = 4;
		}

		IPipeData* PipeGeometryFactory::createPipeData(String name)
		{
			if (name == CCirclePipeLine::TagName)
			{
				return new CCirclePipeLine;
			}
			else if (name == CCirclePipeNode1::TagName)
			{
				return new CCirclePipeNode1;
			}
			else if (name == CCirclePipeNode2::TagName)
			{
				return new CCirclePipeNode2;
			}
			else if (name == CCirclePipeNode3::TagName)
			{
				return new CCirclePipeNode3;
			}
			else if (name == CRectPipeLine::TagName)
			{
				return new CRectPipeLine;
			}
			else if (name == CRectPipeNode1::TagName)
			{
				return new CRectPipeNode1;
			}
			else if (name == CRectPipeNode2::TagName)
			{
				return new CRectPipeNode2;
			}
			else if (name == CPipeWell::TagName)
			{
				return new CPipeWell;
			}
			else if (name == CPipeCylinder::TagName)
			{
				return new CPipeCylinder;
			}
			else if (name == CGlobalNode::TagName)
			{
				return new CGlobalNode;
			}
			else if(name == CPipeWell1::TagName)
			{
				return new CPipeWell1;
			}		

			return NULL;
		}

		String PipePoint::TagName  = "PipePoint";
		TiXmlElement* PipePoint::toXmlElement()
		{
			TiXmlElement* element = new TiXmlElement(TagName);
			element->SetAttribute("Position",StringConverter::toString(Position));
			element->SetAttribute("PipeSection",Section.toString());
			return element;
		}

		PipePoint PipePoint::parse(TiXmlElement* element)
		{
			PipePoint p;

			if (element != NULL)
			{
				p.Position = StringConverter::parseVector3(TinyXml::read_attribute(element,"Position"));
				p.Section = PipeSection::parseSection(TinyXml::read_attribute(element,"PipeSection"));
			}

			return p;
		}

		//////////////////////////////////////////////////////////////////////////
		IPipeData::IPipeData()
			:mTurber(false)
			,mClose(false)
			,mImageInternal(true)
			,mOffset(CVector3::ZERO)
			,mQuat(CQuaternion::IDENTITY)
			,mScale(CVector3::UNIT_SCALE)
			,mPreferInstance(false)
			,mLoopc(IPipeDataBuilder::LOOP_C)
			,mLoopr(IPipeDataBuilder::LOOP_R)
		{
			
		}

		IPipeData::~IPipeData()
		{
			
		}

		TiXmlElement* IPipeData::toXmlElement()
		{
			TiXmlElement* element = new TiXmlElement("IPipeData");
			element->SetAttribute("close",StringConverter::toString(mClose));
			element->SetAttribute("tuber",StringConverter::toString(mTurber));
			element->SetAttribute("texture",mTextureImage);
			if (mOffset != CVector3::ZERO)
			{
				element->SetAttribute("offset",StringConverter::toString(mOffset));
			}
			if (mScale != CVector3::UNIT_SCALE)
			{
				element->SetAttribute("scale",StringConverter::toString(mScale));
			}
			if (mQuat != CQuaternion::IDENTITY)
			{
				element->SetAttribute("rotation",StringConverter::toString(mQuat));
			}
			return element;
		}

		void IPipeData::readXmlElement(TiXmlElement* elemennt)
		{
			mClose = StringConverter::parseBool(TinyXml::read_attribute(elemennt,"close"));
			mTurber = StringConverter::parseBool(TinyXml::read_attribute(elemennt,"tuber"));
			mTextureImage = TinyXml::read_attribute(elemennt,"texture");
			mOffset = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"offset"));
			mScale = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"scale"),CVector3::UNIT_SCALE);
			mQuat = StringConverter::parseQuaternion(TinyXml::read_attribute(elemennt,"rotation"));
		}

		osg::Geode* IPipeData::buildGeode()
		{
			ModelUtility::setTexture(mGeode.get(),mTextureImage,mImageInternal);
			
			return mGeode.release();
		}

		void IPipeData::setTurber(bool turber)
		{
			mTurber = turber;
		}

		bool IPipeData::getTurber()
		{
			return mTurber;
		}

		void IPipeData::setClose(bool close)
		{
			mClose = close;
		}

		bool IPipeData::getClose()
		{
			return mClose;
		}

		void IPipeData::setOffset(CVector3 offset)
		{
			mOffset = offset;
		}

		CVector3 IPipeData::getOffset()
		{
			return mOffset;
		}

		void IPipeData::setRotation(CQuaternion q)
		{
			mQuat = q;
		}

		CQuaternion IPipeData::getRotation()
		{
			return mQuat;
		}

		void IPipeData::setPreferInstrance(bool instance)
		{
			mPreferInstance = instance;
		}

		bool IPipeData::getPreferInstance()
		{
			return mPreferInstance;
		}

		void IPipeData::setScale(CVector3 scale)
		{
			mScale = scale;
		}

		CVector3 IPipeData::getScale()
		{
			return mScale;
		}

		void IPipeData::setLoopC(unsigned short n)
		{
			mLoopc = n;
		}

		unsigned short IPipeData::getLoopC()
		{
			return mLoopc;
		}

		void IPipeData::setLoopR(unsigned short n)
		{
			mLoopr = n;
		}

		unsigned short IPipeData::getLoopR()
		{
			return mLoopr;
		}

		void IPipeData::setTextureImage(String image)
		{
			mTextureImage = image;
		}

		String IPipeData::getTextureImage()
		{
			return mTextureImage;
		}

		void IPipeData::setImageInternal(bool inter)
		{
			mImageInternal = inter;
		}

		bool IPipeData::getImageInternal()
		{
			return mImageInternal;
		}

		void IPipeData::mergeGeode(osg::Geode* geodeToMerge)
		{
			osg::ref_ptr<osg::Geode> g = geodeToMerge;
			if (mGeode.valid() && g.valid())
			{
				for (uint32 i = 0;i<geodeToMerge->getNumDrawables();i++)
				{
					mGeode->addDrawable(geodeToMerge->getDrawable(i));
				}
			}
		}

		osg::DrawElements* IPipeData::createDrawElements()
		{
#if 1
			return new osg::DrawElementsUShort(GL_TRIANGLES);
#else
			return new osg::DrawElementsUInt(GL_TRIANGLES);
#endif
		}

		Vector3List IPipeData::bezierCurve(CVector3 left,CVector3 center,CVector3 right)
		{
			Vector3List v3list;

			//move to origin
			CVector3 left1 = left - center;
			CVector3 center1 = center - center;
			CVector3 right1 = right - center;

			CVector3 planeNormal = (right1 -  center1).crossProduct(left1 - center1);

			CQuaternion quatToXOY = planeNormal.getRotationTo(CVector3::UNIT_Z);
			CQuaternion quatToOriginal = CVector3::UNIT_Z.getRotationTo(planeNormal);

			//rotate to XOY
			CVector3 left2 = quatToXOY * left1;
			CVector3 center2 = quatToXOY * center1;
			CVector3 right2 = quatToXOY * right1; 

			//beizier curve
			CVector2 pt0(left2.x,left2.y);
			CVector2 pt1(0,0);
			CVector2 pt2(right2.x,right2.y);

			unsigned short division = getLoopC();

			for(unsigned short i = 0;i<=division;i++)
			{
				float t = i / (float)division;
				CVector2 temp = pt0 * Math::Pow((1-t),2) + pt1*2*t*(1-t) + pt2 * Math::Pow(t,2);
				CVector3 tempV3 = quatToOriginal* CVector3(temp.x,temp.y,0) + center;
				v3list.push_back(tempV3);
			}

			return v3list;
		}

		Vector3List IPipeData::createSectionPoints(CVector3 center,CVector3 direction,CVector3 normal,PipeSection sect, unsigned short loopR)
		{
			CVector3 localAxisX = normal.crossProduct(direction).normalisedCopy();
			CVector3 localAxisY = direction.crossProduct(localAxisX).normalisedCopy();

			Vector3List resultV3List;
			if (sect.getType() == RECT_SECTION)
			{
				resultV3List.resize(4);

				CVector3 hei2 = localAxisY * (sect.getHeight() / 2.0f);
				CVector3 wid2 = localAxisX * (sect.getWidth() / 2.0f);

				resultV3List[0] = center - wid2 - hei2;
				resultV3List[1] = center + wid2 - hei2;
				resultV3List[2] = center + wid2 + hei2;
				resultV3List[3] = center - wid2 + hei2;
			}
			else if(sect.getType() == CIRCULAR_SECTION)
			{
				double radianStep = Math::TWO_PI / loopR;
				double radius = sect.getRadius();
				resultV3List.resize(loopR);
				for(int i = 0;i < loopR;i++)
				{
					double radian = radianStep * i;

					CVector3 localOffsetX = localAxisX * radius * Math::Cos(radian);
					CVector3 localOffsetY = localAxisY * radius * Math::Sin(radian);

					resultV3List[i] = center + localOffsetX + localOffsetY;
				}
			}

			return resultV3List;
		}
		//////////////////////////////////////////////////////////////////////////
		
		
		//////////////////////////////////////////////////////////////////////////
		CPipeData::CPipeData(void)
			:mRefNormal(CVector3::UNIT_Z)
		{

		}

		CPipeData::~CPipeData(void)
		{

		}

		TiXmlElement* CPipeData::toXmlElement()
		{
			TiXmlElement* element = new TiXmlElement("CPipeData");
			element->SetAttribute("RefNormal",StringConverter::toString(mRefNormal));
			return element;
		}
		
		void CPipeData::readXmlElement(TiXmlElement* elemennt)
		{
			mRefNormal = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"RefNormal"));
		}

		int CPipeData::getArcPointNum()
		{
			return (int)mArcPointSet.size();
		}

		CVector3 CPipeData::getArcPoint(int i)
		{
			if ( i > -1 && i < getArcPointNum())
			{
				return mArcPointSet[i];
			}

			return CVector3::ZERO;
		}

		void CPipeData::setRefNormal(CVector3 normal)
		{
			mRefNormal = normal;
		}

		CVector3 CPipeData::getRefNormal()
		{
			return mRefNormal;
		}

	}
}
