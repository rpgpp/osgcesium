#include "PipeData.h"
#include "PipeGeomLibrary.h"
#include "ModelUtility.h"

using namespace OC;
using namespace OC::Modeling;

String CCirclePipeLine::TagName = "CirclePipeLine";

TiXmlElement* CCirclePipeLine::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);
	element->LinkEndChild(mStPoint.toXmlElement());
	element->LinkEndChild(mEdPoint.toXmlElement());

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CCirclePipeLine::readXmlElement(TiXmlElement* elemennt)
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
	
	mStPoint = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName));
	mEdPoint = PipePoint::parse(elemennt->FirstChildElement(PipePoint::TagName)->NextSiblingElement());
}

void CCirclePipeLine::setStPoint(PipePoint point)
{
	mStPoint = point;
}

PipePoint CCirclePipeLine::getStPoint()
{
	return mStPoint;
}

void CCirclePipeLine::setEdPoint(PipePoint point)
{
	mEdPoint = point;
}

PipePoint CCirclePipeLine::getEdPoint()
{
	return mEdPoint;
}

osg::Geode* CCirclePipeLine::buildGeode()
{
	mGeode = new osg::Geode;

	CVector3 stPoint = mStPoint.getPoint();
	CVector3 edPoint = mEdPoint.getPoint();

	osg::ref_ptr<osg::Vec3Array> pVertexs = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> pTexCoord = new osg::Vec2Array;

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geom->setTexCoordArray(0,pTexCoord,osg::Array::BIND_PER_VERTEX);
	geom->setVertexArray(pVertexs);
	CVector3 direction = edPoint - stPoint;

	osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
	geom->addPrimitiveSet(drawelements);

	double len = direction.length();

	if (Math::RealEqual(len,0.0f))
	{
		return mGeode = NULL;
	}

	direction.normalise();

	if (direction.directionEquals(mRefNormal,CRadian(CDegree(1.0))) || direction.directionEquals(-mRefNormal,CRadian(CDegree(1.0))))
	{
		mRefNormal = CVector3::UNIT_Y;
	}

	int loopR = getLoopR();

	Vector3List c1List = createSectionPoints(stPoint, direction, mRefNormal, mStPoint.getSection(), loopR);
	Vector3List c2List = createSectionPoints(edPoint, direction, mRefNormal, mEdPoint.getSection(), loopR);

	int geomPointCount = loopR * 2;
	pTexCoord->resize(geomPointCount);
	pVertexs->resize(geomPointCount);

	double radianStep = Math::TWO_PI / loopR;

	//vertex
	//index
	for(int i = 0;i<loopR;i++)
	{
		pVertexs->at(i) = Vector3ToVec3(c1List[i]);
		pVertexs->at(i + loopR) = Vector3ToVec3(c2List[i]);

		int a,b,c,d;

		if(i==loopR-1)
		{
			a = i;
			b = 0;
			c = i + 1;
			d = 2 * loopR - 1;
		}
		else
		{
			a = i;
			b = a + 1;
			c = i + loopR + 1;
			d = c - 1;
		}

		drawelements->addElement(a);
		drawelements->addElement(b);
		drawelements->addElement(c);

		drawelements->addElement(a);
		drawelements->addElement(c);
		drawelements->addElement(d);

		//UV
		double radian = Math::PI + radianStep * i;
		double texX = Math::Cos(radian);
		(*pTexCoord)[i] = osg::Vec2(0,texX);
		(*pTexCoord)[i + loopR] = osg::Vec2(1 * len,texX);
	}

	osgUtil::SmoothingVisitor::smooth(*geom);
	mGeode->addDrawable(geom);

	if(getClose())
	{
		osg::Geometry* circle1 = new osg::Geometry;
		osg::Geometry* circle2 = new osg::Geometry;
		osg::ref_ptr<osg::DrawElements> drawelements1 = createDrawElements();
		osg::ref_ptr<osg::DrawElements> drawelements2 = createDrawElements();
		osg::ref_ptr<osg::Vec3Array> pVertexs1 = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> pVertexs2 = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> pNormals1 = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> pNormals2 = new osg::Vec3Array;

		pVertexs1->push_back(osg::Vec3(stPoint.x,stPoint.y,stPoint.z));
		pVertexs2->push_back(osg::Vec3(edPoint.x,edPoint.y,edPoint.z));

		for(int i = 0; i < loopR;i++)
		{
			pVertexs1->push_back(osg::Vec3(c1List[i].x,c1List[i].y,c1List[i].z));
			pVertexs2->push_back(osg::Vec3(c2List[i].x,c2List[i].y,c2List[i].z));
		}
				
		for(int i = 0;i < loopR;i++)
		{
			int a,b,c;
			a = 0;
			if ( i == loopR - 1)
			{
				b = i + 1;
				c = i + 2 - loopR;
			}
			else
			{
				b = i + 1;
				c = i + 2;
			}

			drawelements1->addElement(a);
			drawelements1->addElement(c);
			drawelements1->addElement(b);

			drawelements2->addElement(a);
			drawelements2->addElement(b);
			drawelements2->addElement(c);
		}

		pNormals1->push_back(osg::Vec3(direction.x,direction.y,direction.z) * -1.0);
		pNormals2->push_back(osg::Vec3(direction.x,direction.y,direction.z));

		circle1->setVertexArray(pVertexs1);
		circle2->setVertexArray(pVertexs2);
		circle1->addPrimitiveSet(drawelements1);
		circle2->addPrimitiveSet(drawelements2);
		circle1->setNormalArray(pNormals1,osg::Vec3Array::BIND_PER_PRIMITIVE_SET);
		circle2->setNormalArray(pNormals2,osg::Vec3Array::BIND_PER_PRIMITIVE_SET);
		mGeode->addDrawable(circle1);
		mGeode->addDrawable(circle2);
	}

	return IPipeData::buildGeode();
}

osg::Node* CCirclePipeLine::buildNode()
{
	osg::Node* node = NULL;

	if (getPreferInstance())
	{
		if (PipeGeomLibrary* instranceFactory = SingletonPtr(PipeGeomLibrary))
		{
			node = instranceFactory->createInstance(this);
		}
	}

	if (node == NULL)
	{
		node = buildGeode();
	}
	
	return node;
}
