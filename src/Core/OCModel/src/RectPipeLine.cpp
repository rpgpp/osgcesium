#include "PipeData.h"
#include "GeometryBuilder.h"
#include "PipeGeomLibrary.h"
#include "ModelUtility.h"

using namespace OC;
using namespace OC::Modeling;

String CRectPipeLine::TagName = "RectPipeLine";

TiXmlElement* CRectPipeLine::toXmlElement()
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

void CRectPipeLine::readXmlElement(TiXmlElement* elemennt)
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

CRectPipeLine::CRectPipeLine()
{

}

CRectPipeLine::~CRectPipeLine()
{

}

void CRectPipeLine::setStPoint(PipePoint point)
{
	mStPoint = point;
}

PipePoint CRectPipeLine::getStPoint()
{
	return mStPoint;
}


void CRectPipeLine::setEdPoint(PipePoint point)
{
	mEdPoint = point;
}

PipePoint CRectPipeLine::getEdPoint()
{
	return mEdPoint;
}

#if 1

osg::Geode* CRectPipeLine::buildGeode()
{
	mGeode = new osg::Geode;

	CVector3 p1 = mStPoint.getPoint();
	CVector3 p2 = mEdPoint.getPoint();

	CVector3 direction = p2 - p1;
	CVector3 directionNor = direction.normalisedCopy();

	if (directionNor.directionEquals(mRefNormal, CRadian(0.1)))
	{
		mRefNormal = CVector3::UNIT_Y;
	}

	double len = direction.length();

	int loopR = 4;

	Vector3List sectPoints = createSectionPoints(p1, direction, mRefNormal, mStPoint.getSection(), loopR);

	for (int i = 0; i < loopR; i++)
	{
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
		vertex->resize(4);
		geometry->setVertexArray(vertex);

		osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
		drawelements->addElement(0);
		drawelements->addElement(1);
		drawelements->addElement(2);

		drawelements->addElement(1);
		drawelements->addElement(3);
		drawelements->addElement(2);

		geometry->addPrimitiveSet(drawelements);

		CVector3 pt = sectPoints[i];
		CVector3 pt2 = sectPoints[(i + 1)%4];

		//points begin
		vertex->at(0) = osg::Vec3(pt.x, pt.y, pt.z);
		vertex->at(1) = osg::Vec3(pt2.x, pt2.y, pt2.z);

		//points end
		pt += direction;
		pt2 += direction;
		vertex->at(2) = osg::Vec3(pt.x, pt.y, pt.z);
		vertex->at(3) = osg::Vec3(pt2.x, pt2.y, pt2.z);

		osg::ref_ptr<osg::Vec2Array> texCoord = new osg::Vec2Array;
		texCoord->resize(4);
		texCoord->at(0) = osg::Vec2(0, 0);
		texCoord->at(1) = osg::Vec2(0, 1);
		texCoord->at(2) = osg::Vec2(len, 0);
		texCoord->at(3) = osg::Vec2(len, 1);
		geometry->setTexCoordArray(0, texCoord);

		mGeode->addDrawable(geometry);
	}

	if (getClose())
	{
		for (int i = 0; i < 2; i++)
		{
			osg::Geometry* geometry = new osg::Geometry;
			osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
			geometry->addPrimitiveSet(drawelements);

			CVector3 pt0 = sectPoints[0] + direction * i;
			CVector3 pt1 = sectPoints[1] + direction * i;
			CVector3 pt2 = sectPoints[2] + direction * i;
			CVector3 pt3 = sectPoints[3] + direction * i;

			osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
			vertex->resize(4);
			geometry->setVertexArray(vertex);
			vertex->at(0) = Vector3ToVec3(pt0);
			vertex->at(1) = Vector3ToVec3(pt1);
			vertex->at(2) = Vector3ToVec3(pt2);
			vertex->at(3) = Vector3ToVec3(pt3);

			if (i == 1)
			{
				drawelements->addElement(0);
				drawelements->addElement(1);
				drawelements->addElement(2);

				drawelements->addElement(0);
				drawelements->addElement(2);
				drawelements->addElement(3);
			}
			else
			{
				drawelements->addElement(0);
				drawelements->addElement(2);
				drawelements->addElement(1);

				drawelements->addElement(0);
				drawelements->addElement(3);
				drawelements->addElement(2);
			}
			osg::ref_ptr<osg::Vec2Array> texCoord = new osg::Vec2Array;
			texCoord->resize(4);
			texCoord->at(0) = osg::Vec2(0, 0);
			texCoord->at(1) = osg::Vec2(0, 1);
			texCoord->at(2) = osg::Vec2(1, 0);
			texCoord->at(3) = osg::Vec2(1, 1);
			geometry->setTexCoordArray(0, texCoord);
			mGeode->addDrawable(geometry);
		}
	}

	return IPipeData::buildGeode();
}

#else

osg::Geode* CRectPipeLine::buildGeode()
{
	PipeBuildCompile builder;

	mGeode = new osg::Geode;
	mGeode->addDrawable(builder.geometry());

	CVector3 p1 = mStPoint.getPoint();
	CVector3 p2 = mEdPoint.getPoint();

	CVector3 direction = p2 - p1;
	CVector3 directionNor = direction.normalisedCopy();

	if (directionNor.directionEquals(mRefNormal, CRadian(0.1)))
	{
		mRefNormal = CVector3::UNIT_Y;
	}

	double len = direction.length();

	int loopR = 4;

	Vector3List sectPoints = createSectionPoints(p1, direction, mRefNormal, mStPoint.getSection(), loopR);

	builder.vertex()->resize(loopR * 2);

	//Vertex
	for (int i = 0; i < loopR; i++)
	{
		CVector3 pt = sectPoints[i];

		//points begin
		builder.vertex()->at(i) = osg::Vec3(pt.x, pt.y, pt.z);

		//points end
		pt += direction;
		builder.vertex()->at(i + loopR) = osg::Vec3(pt.x, pt.y, pt.z);

	}

	bool drawQuad = getPreferInstance();
	//indices and normal
	for (int i = 0; i < loopR; i++)
	{
		int a, b, c, d;

		if (i < loopR - 1)
		{
			a = i;
			b = a + 1;
			d = i + loopR;
			c = d + 1;
		}
		else
		{
			a = i;
			b = i - loopR + 1;
			c = i + 1;
			d = i + loopR;
		}

		if (drawQuad)
		{
			//in osg
			builder.addQuadDrawElements(a, b, c, d);

			osg::Vec3 d1 = builder.vertex()->at(b) - builder.vertex()->at(a);
			osg::Vec3 d2 = builder.vertex()->at(d) - builder.vertex()->at(a);
			osg::Vec3 d3 = d1 ^ d2;
			d3.normalize();
			builder.normal()->push_back(d3);
		}
		else
		{
			osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
			drawelements->addElement(a);
			drawelements->addElement(b);
			drawelements->addElement(c);

			drawelements->addElement(a);
			drawelements->addElement(c);
			drawelements->addElement(d);

			builder.geometry()->addPrimitiveSet(drawelements);
		}
	}

	//Texture Coordinate
	builder.texCoord()->push_back(osg::Vec2(0, -1));
	builder.texCoord()->push_back(osg::Vec2(0, 0));
	builder.texCoord()->push_back(osg::Vec2(0, 1));
	builder.texCoord()->push_back(osg::Vec2(0, 0));

	builder.texCoord()->push_back(osg::Vec2(1.0 * len, -1));
	builder.texCoord()->push_back(osg::Vec2(1.0 * len, 0));
	builder.texCoord()->push_back(osg::Vec2(1.0 * len, 1));
	builder.texCoord()->push_back(osg::Vec2(1.0 * len, 0));

	if (getClose())
	{
		if (drawQuad)
		{
			builder.addQuadDrawElements(0, 1, 2, 3);

			int a = builder.vertex()->size() - loopR;
			int b = builder.vertex()->size() - loopR + 1;
			int c = builder.vertex()->size() - loopR + 2;
			int d = builder.vertex()->size() - loopR + 3;

			builder.addQuadDrawElements(a, b, c, d);

			builder.normal()->resize(6);
			builder.normal()->at(4) = -osg::Vec3(directionNor.x, directionNor.y, directionNor.z);
			builder.normal()->at(5) = osg::Vec3(directionNor.x, directionNor.y, directionNor.z);
		}
		else
		{
			osg::ref_ptr<osg::DrawElements> drawelements = createDrawElements();
			builder.geometry()->addPrimitiveSet(drawelements);

			drawelements->addElement(0);
			drawelements->addElement(1);
			drawelements->addElement(2);

			drawelements->addElement(0);
			drawelements->addElement(2);
			drawelements->addElement(3);

			drawelements->addElement(4);
			drawelements->addElement(5);
			drawelements->addElement(6);

			drawelements->addElement(4);
			drawelements->addElement(6);
			drawelements->addElement(7);
		}
	}

	builder.setupNormal();

	return IPipeData::buildGeode();
}

#endif

osg::Node* CRectPipeLine::buildNode()
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
