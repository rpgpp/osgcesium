
#include "PipeData.h"

using namespace OC;
using namespace OC::Modeling;

String CCirclePipeNode3::TagName = "CirclePipeNode3";

TiXmlElement* CCirclePipeNode3::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("Center",StringConverter::toString(mCenter));
	element->LinkEndChild(mPoint1.toXmlElement());
	element->LinkEndChild(mPoint2.toXmlElement());
	element->LinkEndChild(mPoint3.toXmlElement());

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CCirclePipeNode3::readXmlElement(TiXmlElement* elemennt)
{
	if (elemennt == NULL || elemennt->Value() != TagName)
	{
		return;
	}

	if (TiXmlNode* pipe = elemennt->Parent())
	{
		CPipeData::readXmlElement(pipe->ToElement());
		if (TiXmlNode* root = pipe->Parent())
		{
			IPipeData::readXmlElement(root->ToElement());
		}
	}

	mCenter = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"Center"));
	mPoint1 = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName));
	mPoint2 = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName)->NextSiblingElement());
	mPoint3 = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName)->NextSiblingElement()->NextSiblingElement());
}

void CCirclePipeNode3::setPoints(CVector3 center,PipePoint p1,PipePoint p2,PipePoint p3)
{
	mCenter = center;
	mPoint1 = p1;
	mPoint2 = p2;
	mPoint3 = p3;
}

osg::Geode* CCirclePipeNode3::buildGeode()
{
	mGeode = new osg::Geode;

	CVector3 v1 = mPoint1.getPoint() - mCenter;
	CVector3 v2 = mPoint2.getPoint() - mCenter;
	CVector3 v3 = mPoint3.getPoint() - mCenter;

	CRadian angle1 = v1.angleBetween(v2);
	CRadian angle2 = v1.angleBetween(v3);
	CRadian angle3 = v2.angleBetween(v3);

	PipeSection sect1 = mPoint1.getSection();
	PipeSection sect2 = mPoint2.getSection();
	PipeSection sect3 = mPoint3.getSection();

	CRadian maxRadian = max(max(angle1,angle2),angle3);
	
	//如果是一个立管的三通，如立方体角落
	static CRadian right_angle_factor = CRadian(CDegree(90.0f));
	if (Math::RealEqual(angle1.valueAngleUnits(),right_angle_factor.valueAngleUnits(),0.1f)
		&& Math::RealEqual(angle2.valueAngleUnits(),right_angle_factor.valueAngleUnits(),0.1f)
		&& Math::RealEqual(angle3.valueAngleUnits(),right_angle_factor.valueAngleUnits(),0.1f))
	{
		{
			CCirclePipeNode2 pipe2;
			pipe2.setTurber(false);
			pipe2.setPoints(mCenter,mPoint1,mPoint2);
			mergeGeode(pipe2.buildGeode());
		}
		{
			CCirclePipeNode2 pipe2;
			pipe2.setTurber(false);
			pipe2.setPoints(mCenter,mPoint1,mPoint3);
			mergeGeode(pipe2.buildGeode());
		}
		{
			CCirclePipeNode2 pipe2;
			pipe2.setTurber(false);
			pipe2.setPoints(mCenter,mPoint2,mPoint3);
			mergeGeode(pipe2.buildGeode());
		}
	}
	else
	{
		//如果三通不是垂直的，使用接近直线（角度最大）作为弯管，另一头插入
		CCirclePipeNode2 pipe2;
		pipe2.setTurber(mTurber);
		CCirclePipeLine line;
		if(maxRadian == angle1)
		{
			pipe2.setPoints(mCenter,mPoint1,mPoint2);
			line.setStPoint(PipePoint(mCenter,min(mPoint1.getSection(),mPoint2.getSection())));
			line.setEdPoint(mPoint3);
		}
		else if(maxRadian == angle2)
		{
			pipe2.setPoints(mCenter,mPoint1,mPoint3);
			line.setStPoint(PipePoint(mCenter,min(mPoint1.getSection(),mPoint3.getSection())));
			line.setEdPoint(mPoint2);
		}   
		else 
		{
			pipe2.setPoints(mCenter,mPoint2,mPoint3);
			line.setStPoint(PipePoint(mCenter,min(mPoint2.getSection(),mPoint3.getSection())));
			line.setEdPoint(mPoint1);
		}
		mergeGeode(pipe2.buildGeode());
		mergeGeode(line.buildGeode());
	}

	return IPipeData::buildGeode();
}