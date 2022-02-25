#include "PipeData.h"
#include "GeometryBuilder.h"

using namespace OC;
using namespace OC::Modeling;

String CRectPipeNode2::TagName = "RectPipeNode2";

TiXmlElement* CRectPipeNode2::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("Center",StringConverter::toString(mCenter));
	element->LinkEndChild(mPoint1.toXmlElement());
	element->LinkEndChild(mPoint2.toXmlElement());

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CRectPipeNode2::readXmlElement(TiXmlElement* elemennt)
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
}

void CRectPipeNode2::setPoints(CVector3 center,PipePoint point1,PipePoint point2)
{
	mCenter = center;
	mPoint1 = point1;
	mPoint2 = point2;
}

#if 1

osg::Geode* CRectPipeNode2::buildGeode()
{
	int loopC = getLoopC();
	int loopR = 4;

	mGeode = new osg::Geode;

	osg::Geometry* geometry0 = new osg::Geometry;
	osg::Geometry* geometry1 = new osg::Geometry;
	osg::Geometry* geometry2 = new osg::Geometry;
	osg::Geometry* geometry3 = new osg::Geometry;

	mGeode->addDrawable(geometry0);
	mGeode->addDrawable(geometry1);
	mGeode->addDrawable(geometry2);
	mGeode->addDrawable(geometry3);

	osg::Vec3Array* vertex0 = new osg::Vec3Array;
	osg::Vec3Array* vertex1 = new osg::Vec3Array;
	osg::Vec3Array* vertex2 = new osg::Vec3Array;
	osg::Vec3Array* vertex3 = new osg::Vec3Array;

	geometry0->setVertexArray(vertex0);
	geometry1->setVertexArray(vertex1);
	geometry2->setVertexArray(vertex2);
	geometry3->setVertexArray(vertex3);

	osg::Vec2Array* texCoord0 = new osg::Vec2Array;
	osg::Vec2Array* texCoord1 = new osg::Vec2Array;
	osg::Vec2Array* texCoord2 = new osg::Vec2Array;
	osg::Vec2Array* texCoord3 = new osg::Vec2Array;

	geometry0->setTexCoordArray(0, texCoord0);
	geometry1->setTexCoordArray(0, texCoord1);
	geometry2->setTexCoordArray(0, texCoord2);
	geometry3->setTexCoordArray(0, texCoord3);

	Vector3List axlePoints = bezierCurve(mPoint1.getPoint(), mCenter, mPoint2.getPoint());
	int axleSize = (int)axlePoints.size();

	CVector3 dir1 = mPoint1.getPoint() - mCenter;
	CVector3 dir2 = mPoint2.getPoint() - mCenter;

	dir1.normalise();
	dir2.normalise();

	CVector3 refNormal = getRefNormal();

	CRadian t = CRadian(CDegree(5.0));
	if (dir1.directionEquals(refNormal, t)
		|| dir1.directionEquals(-refNormal, t)
		|| dir2.directionEquals(refNormal, t)
		|| dir2.directionEquals(-refNormal, t)
		)
	{
		refNormal = dir1.crossProduct(dir2);
		refNormal = refNormal.crossProduct(dir1);
	}

	PipeSection stepSect = (mPoint2.getSection() - mPoint1.getSection()) / (double)axleSize;

	double len = 0.0f;
	double fullLen = 0.0f;

	for (int i = 0; i < axleSize; i++)
	{
		if (i < axleSize - 1)
		{
			CVector3 direction = axlePoints[i + 1] - axlePoints[i];
			fullLen += direction.length();
		}

		PipeSection dSect = stepSect * i + mPoint1.getSection();

		CVector3 direction;

		if (i == 0)
		{
			direction = mCenter - mPoint1.getPoint();
		}
		else if (i == axleSize - 1)
		{
			direction = mPoint2.getPoint() - mCenter;
		}
		else
		{
			direction = axlePoints[i + 1] - axlePoints[i];
		}

		Vector3List pCirclePoints = createSectionPoints(axlePoints[i], direction, refNormal, dSect, loopR);

		osg::Vec3 p0 = Vector3ToVec3(pCirclePoints[0]);
		osg::Vec3 p1 = Vector3ToVec3(pCirclePoints[1]);
		osg::Vec3 p2 = Vector3ToVec3(pCirclePoints[2]);
		osg::Vec3 p3 = Vector3ToVec3(pCirclePoints[3]);

		vertex0->push_back(p0);
		vertex0->push_back(p1);
		vertex1->push_back(p1);
		vertex1->push_back(p2);
		vertex2->push_back(p2);
		vertex2->push_back(p3);
		vertex3->push_back(p3);
		vertex3->push_back(p0);

		//calc texture coordinate
		if (i > 0)
		{
			len += (axlePoints[i] - axlePoints[i - 1]).length();
		}

		texCoord0->push_back(osg::Vec2(len, 0));
		texCoord0->push_back(osg::Vec2(len, 1));
		texCoord1->push_back(osg::Vec2(len, 0));
		texCoord1->push_back(osg::Vec2(len, 1));
		texCoord2->push_back(osg::Vec2(len, 0));
		texCoord2->push_back(osg::Vec2(len, 1));
		texCoord3->push_back(osg::Vec2(len, 0));
		texCoord3->push_back(osg::Vec2(len, 1));
	}

	osg::ref_ptr<osg::DrawElements> drawelements0 = new osg::DrawElementsUShort(GL_TRIANGLES);
	osg::ref_ptr<osg::DrawElements> drawelements1 = new osg::DrawElementsUShort(GL_TRIANGLES);
	osg::ref_ptr<osg::DrawElements> drawelements2 = new osg::DrawElementsUShort(GL_TRIANGLES);
	osg::ref_ptr<osg::DrawElements> drawelements3 = new osg::DrawElementsUShort(GL_TRIANGLES);
	geometry0->addPrimitiveSet(drawelements0);
	geometry1->addPrimitiveSet(drawelements1);
	geometry2->addPrimitiveSet(drawelements2);
	geometry3->addPrimitiveSet(drawelements3);

	for (unsigned int j = 0; j < loopC; j++)
	{
		unsigned int a = 0;
		unsigned int b = 1;
		unsigned int c = 2;
		unsigned int d = 3;

		a += j * 2;
		b += j * 2;
		c += j * 2;
		d += j * 2;
		drawelements0->addElement(a); drawelements0->addElement(b); drawelements0->addElement(c);
		drawelements0->addElement(c); drawelements0->addElement(b); drawelements0->addElement(d);
		drawelements1->addElement(a); drawelements1->addElement(b); drawelements1->addElement(c);
		drawelements1->addElement(c); drawelements1->addElement(b); drawelements1->addElement(d);
		drawelements2->addElement(a); drawelements2->addElement(b); drawelements2->addElement(c);
		drawelements2->addElement(c); drawelements2->addElement(b); drawelements2->addElement(d);
		drawelements3->addElement(a); drawelements3->addElement(b); drawelements3->addElement(c);
		drawelements3->addElement(c); drawelements3->addElement(b); drawelements3->addElement(d);
	}

	return IPipeData::buildGeode();
}
#else

osg::Geode* CRectPipeNode2::buildGeode()
{
	PipeBuildCompile builder;

	int loopC = getLoopC();
	int loopR = 4;

	mGeode = new osg::Geode;
	mGeode->addChild(builder.geometry());

	builder.normal()->resize(4);

	Vector3List axlePoints = bezierCurve(mPoint1.getPoint(), mCenter, mPoint2.getPoint());
	int axleSize = (int)axlePoints.size();

	CVector3 dir1 = mPoint1.getPoint() - mCenter;
	CVector3 dir2 = mPoint2.getPoint() - mCenter;

	dir1.normalise();
	dir2.normalise();

	CVector3 refNormal = getRefNormal();

	CRadian t = CRadian(CDegree(5.0));
	if (dir1.directionEquals(refNormal, t)
		|| dir1.directionEquals(-refNormal, t)
		|| dir2.directionEquals(refNormal, t)
		|| dir2.directionEquals(-refNormal, t)
		)
	{
		refNormal = dir1.crossProduct(dir2);
		refNormal = refNormal.crossProduct(dir1);
	}

	PipeSection stepSect = (mPoint2.getSection() - mPoint1.getSection()) / (double)axleSize;

	double len = 0.0f;
	double fullLen = 0.0f;

	for (int i = 0; i < axleSize; i++)
	{
		if (i < axleSize - 1)
		{
			CVector3 direction = axlePoints[i + 1] - axlePoints[i];
			fullLen += direction.length();
		}

		PipeSection dSect = stepSect * i + mPoint1.getSection();

		CVector3 direction;

		if (i == 0)
		{
			direction = mCenter - mPoint1.getPoint();
		}
		else if (i == axleSize - 1)
		{
			direction = mPoint2.getPoint() - mCenter;
		}
		else
		{
			direction = axlePoints[i + 1] - axlePoints[i];
		}

		Vector3List pCirclePoints = createSectionPoints(axlePoints[i], direction, refNormal, dSect, loopR);

		Vector3List::iterator it = pCirclePoints.begin();
		for (; it != pCirclePoints.end(); it++)
		{
			builder.vertex()->push_back(osg::Vec3(it->x, it->y, it->z));
		}

		//calc texture coordinate
		if (i > 0)
		{
			len += (axlePoints[i] - axlePoints[i - 1]).length();
		}

		builder.texCoord()->push_back(osg::Vec2(len, -1));
		builder.texCoord()->push_back(osg::Vec2(len, 0));
		builder.texCoord()->push_back(osg::Vec2(len, 1));
		builder.texCoord()->push_back(osg::Vec2(len, 0));
	}

	bool drawQuad = getPreferInstance();
	osg::ref_ptr<osg::DrawElements> drawelements;

	for (int i = 0; i < loopR; i++)
	{
		if (drawQuad)
		{
			drawelements = new osg::DrawElementsUShort(GL_QUADS);
			builder.geometry()->addPrimitiveSet(drawelements);
		}
		else if (!drawelements.valid())
		{
			drawelements = new osg::DrawElementsUShort(GL_TRIANGLES);
			builder.geometry()->addPrimitiveSet(drawelements);
		}

		osg::Vec3 nor;

		for (unsigned int j = 0; j < loopC; j++)
		{
			unsigned int a = i;
			unsigned int b = a + 1;
			unsigned int c = b + loopR;
			unsigned int d = c - 1;

			if (i == loopR - 1)
			{
				a = 3;
				b = 0;
				c = 4;
				d = 7;
			}

			a += j * loopR;
			b += j * loopR;
			c += j * loopR;
			d += j * loopR;

			if (drawQuad)
			{
				drawelements->addElement(a);
				drawelements->addElement(b);
				drawelements->addElement(c);
				drawelements->addElement(d);
			}
			else
			{
				drawelements->addElement(a); drawelements->addElement(b); drawelements->addElement(c);
				drawelements->addElement(a); drawelements->addElement(c); drawelements->addElement(d);
			}

			nor += builder.computeNormal(a, b, c, d);
		}

		nor.normalize();
		builder.normal()->at(i) = nor;
	}

	builder.setupNormal();

	return IPipeData::buildGeode();
}
#endif
