
#include "PipeData.h"
#include "GeometryBuilder.h"

using namespace OC;
using namespace OC::Modeling;

String CRectPipeNode1::TagName = "RectPipeNode1";

TiXmlElement* CRectPipeNode1::toXmlElement()
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

void CRectPipeNode1::readXmlElement(TiXmlElement* elemennt)
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

CRectPipeNode1::CRectPipeNode1()
{

}

CRectPipeNode1::~CRectPipeNode1()
{

}

void CRectPipeNode1::setPoints(PipePoint center,CVector3 direction)
{
	mCenter = center;
	mDirection = direction;
	mDirection.normalise();
}

osg::Geode* CRectPipeNode1::buildGeode()
{
	CRectPipeLine pipe;
	pipe.setPreferInstrance(getPreferInstance());
	pipe.setRefNormal(getRefNormal());
	pipe.setClose(true);

	CRay ray(mCenter.getPoint(),mDirection);

	//CVector3 v1 = ray.getPoint(mCenter.getSection().getRadius() * 0.3);
	CVector3 v2 = ray.getPoint(mCenter.getSection().getRadius() * -0.3);;

	pipe.setStPoint(mCenter);
	pipe.setEdPoint(PipePoint(v2,mCenter.getSection()));
	
	mGeode = pipe.buildGeode();

	return IPipeData::buildGeode();
}

