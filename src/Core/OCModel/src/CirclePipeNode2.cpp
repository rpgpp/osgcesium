#include "PipeData.h"
#include "GeometryBuilder.h"

using namespace OC;
using namespace OC::Modeling;

String CCirclePipeNode2::TagName = "CirclePipeNode2";

TiXmlElement* CCirclePipeNode2::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("Center",StringConverter::toString(mCenter));
	element->SetAttribute("TuberLengthFactor",StringConverter::toString(mTuberLengthFactor));
	element->SetAttribute("TuberRadiusFactor",StringConverter::toString(mTuberRadiusFactor));
	element->LinkEndChild(mPoint1.toXmlElement());
	element->LinkEndChild(mPoint2.toXmlElement());

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CCirclePipeNode2::readXmlElement(TiXmlElement* elemennt)
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
	mTuberLengthFactor = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"TuberLengthFactor"));
	mTuberRadiusFactor = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"TuberRadiusFactor"));

	TiXmlElement* fc = elemennt->FirstChildElement(PipePoint::TagName);
	mPoint1 = PipePoint::parse(fc);
	mPoint2 = PipePoint::parse(fc->NextSiblingElement());

}

CCirclePipeNode2::CCirclePipeNode2()
	:mTuberLengthFactor(0.4)
	,mTuberRadiusFactor(1.1)
{

}

CCirclePipeNode2::~CCirclePipeNode2()
{

}

void CCirclePipeNode2::setPoints(CVector3 center,PipePoint p1,PipePoint p2)
{
	mCenter = center;
	mPoint1 = p1;
	mPoint2 = p2;
}

void CCirclePipeNode2::createArcPointSet()
{
	CVector3 dir1 = mPoint1.getPoint() - mCenter;
	CVector3 dir2 = mPoint2.getPoint() - mCenter;
	dir1.normalise();
	dir2.normalise();

	CVector3 normal = dir2.crossProduct(dir1);
	if (normal.length() > 1e-3 && normal.angleBetween(mRefNormal) > CRadian(CDegree(89.0f)))
	{
		mRefNormal = normal;
	}

	mArcPointSet =  bezierCurve(mPoint1.getPoint(),mCenter,mPoint2.getPoint());
}

void CCirclePipeNode2::createTuber()
{
	CCirclePipeLine leftTurber;

	CVector3 p1 = CPipeMath::getPointByDistance(mPoint1.getPoint(),mCenter,mPoint1.getSection().getRadius() * -mTuberLengthFactor);  
	CVector3 p2 = CPipeMath::getPointByDistance(mPoint1.getPoint(),mCenter,mPoint1.getSection().getRadius() * mTuberLengthFactor);  
	leftTurber.setRefNormal(mRefNormal);
	leftTurber.setStPoint(PipePoint(p1,mPoint1.getSection() * mTuberRadiusFactor));
	leftTurber.setEdPoint(PipePoint(p2,mPoint1.getSection() * mTuberRadiusFactor));
	leftTurber.setClose(true);

	CCirclePipeLine rightTurber;
	p1 = CPipeMath::getPointByDistance(mPoint2.getPoint(),mCenter,mPoint2.getSection().getRadius() * mTuberLengthFactor);  
	p2 = CPipeMath::getPointByDistance(mPoint2.getPoint(),mCenter,mPoint2.getSection().getRadius() * -mTuberLengthFactor);
	rightTurber.setRefNormal(mRefNormal);
	rightTurber.setStPoint(PipePoint(p1,mPoint2.getSection() * mTuberRadiusFactor));
	rightTurber.setEdPoint(PipePoint(p2,mPoint2.getSection() * mTuberRadiusFactor));
	rightTurber.setClose(true);
	mergeGeode(leftTurber.buildGeode());
	mergeGeode(rightTurber.buildGeode());
}

osg::Geode* CCirclePipeNode2::buildGeode()
{
	if (mCenter == mPoint1.getPoint() || mCenter == mPoint2.getPoint())
	{
		return NULL;
	}

	CVector3 dir1 = mCenter - mPoint1.getPoint();
	CVector3 dir2 = mCenter - mPoint2.getPoint();

	//插值中心点
	createArcPointSet();

	int pointNum = getArcPointNum();

	if (pointNum < 2)
	{
		return NULL;
	}

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
	osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
	osg::ref_ptr<osg::Vec2Array> textureCoord = new osg::Vec2Array;
	geometry->setVertexArray(vertex);
	geometry->addPrimitiveSet(drawelements);
	geometry->setTexCoordArray(0, textureCoord, osg::Array::BIND_PER_VERTEX);

	PipeSection stepSect = (mPoint2.getSection()- mPoint1.getSection()) / (double)pointNum;

	unsigned short loopR = getLoopR();

	//沿着中心点构建环绕点集
	for(int i = 0;i<pointNum;i++)
	{
		PipeSection dSect = stepSect * i + mPoint1.getSection();

		CVector3 direction;

		if ( i == 0)
		{
			direction = mCenter - mPoint1.getPoint();
		}
		else if (i==pointNum - 1)
		{
			direction = mPoint2.getPoint() - mCenter;
		}
		else
		{
			direction = mArcPointSet[i+1] - mArcPointSet[i];
		}

		Vector3List pCirclePoints = createSectionPoints(mArcPointSet[i], direction, mRefNormal, dSect, loopR);

		//vertex
		for (Vector3List::size_type i = 0;
			i< pCirclePoints.size();i++)
		{
			CVector3 point = pCirclePoints[i];
			vertex->push_back(Vector3ToVec3(point));
		}
		
		//indices
		if (i < pointNum - 1)
		{
			for (unsigned short j=0;j<loopR;j++)
			{
				int a = i * loopR + j;
				int b = a + 1;
				int c = (i+1) * loopR  + j;
				int d = c + 1;
				if ( j == loopR-1)
				{
					b -= loopR;
					d -= loopR;
				}
				drawelements->addElement(a);
				drawelements->addElement(b);
				drawelements->addElement(c);

				drawelements->addElement(b);
				drawelements->addElement(d);
				drawelements->addElement(c);
			}
		}
	}

	//calc texture coordinate
	textureCoord->resize(loopR * pointNum);
	double radianStep = Math::TWO_PI / loopR;
	double len = 0.0f;
	for(int i = 0;i<pointNum;i++)
	{
		if (i > 0)
		{
			len += (mArcPointSet[i] - mArcPointSet[i-1]).length();
		}

		for (int j=0;j<loopR;j++)
		{
			double radian = Math::PI + radianStep * j;
			double texX = Math::Cos(radian);
			textureCoord->at(i*loopR + j) = osg::Vec2(len, texX);
		}
	}

	mGeode = new osg::Geode;
	mGeode->addDrawable(geometry);

	if(mTurber)
	{
		createTuber();
	}

	osgUtil::SmoothingVisitor::smooth(*geometry);

	return IPipeData::buildGeode();
}
