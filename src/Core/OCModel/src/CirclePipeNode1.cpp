
#include "PipeData.h"
#include "GeometryBuilder.h"

using namespace OC;
using namespace OC::Modeling;

String CCirclePipeNode1::TagName = "CirclePipeNode1";

TiXmlElement* CCirclePipeNode1::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("Direction",StringConverter::toString(mDirection));
	element->LinkEndChild(mCenter.toXmlElement());

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CCirclePipeNode1::readXmlElement(TiXmlElement* elemennt)
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
	mDirection = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"Direction"));
	mCenter = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName));
}

CCirclePipeNode1::CCirclePipeNode1()
{

}

CCirclePipeNode1::~CCirclePipeNode1()
{

}

void CCirclePipeNode1::setPoints(PipePoint center,CVector3 direction)
{
	mCenter = center;
	mDirection = direction;
	mDirection.normalise();
}

osg::Geode* CCirclePipeNode1::buildGeode()
{
	CCirclePipeLine pipe;
	pipe.setClose(true);
	CRay ray(mCenter.getPoint(),mDirection);

	CVector3 v1 = ray.getPoint(mCenter.getSection().getRadius() * 0.5);
	CVector3 v2 = ray.getPoint(mCenter.getSection().getRadius() * -0.5);;

	pipe.setStPoint(PipePoint(v1,mCenter.getSection().getRadius() * 1.1));
	pipe.setEdPoint(PipePoint(v2,mCenter.getSection().getRadius() * 1.1));
	mGeode = pipe.buildGeode();

	return IPipeData::buildGeode();
}

