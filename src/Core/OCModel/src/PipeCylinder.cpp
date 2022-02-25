
#include "PipeData.h"
#include "ModelUtility.h"
#include "BuildWell.h"

using namespace OC;
using namespace OC::Modeling;

String CPipeCylinder::TagName = "PipeCylinder";

TiXmlElement* CPipeCylinder::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("Position",StringConverter::toString(mPosition));
	element->SetAttribute("OffsetU",StringConverter::toString(mOffsetU));
	element->SetAttribute("OffsetD",StringConverter::toString(mOffsetD));
	element->SetAttribute("Radius",StringConverter::toString(mRadius));
	element->SetAttribute("mHeight",StringConverter::formatDoubletoString(mHeight));
	
	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CPipeCylinder::readXmlElement(TiXmlElement* elemennt)
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

	mPosition = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"Position"));
	mOffsetU = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"OffsetU"));
	mOffsetD = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"OffsetD"));
	mRadius = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"Radius"));
	mHeight = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"mHeight"));
}

void CPipeCylinder::setUpOffset(double offset)
{
	mOffsetU = offset;
}

void CPipeCylinder::setDownOffset(double offset)
{
	mOffsetD = offset;
}

void CPipeCylinder::setRadius(double r)
{
	mRadius = r;
}

void CPipeCylinder::setPosition(CVector3 position)
{
	mPosition = position;
}

void CPipeCylinder::setHeight(double h)
{
	mHeight = h;
}

osg::Geode* CPipeCylinder::buildGeode()
{
	mGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;
	osg::ref_ptr<osg::Geometry> geometry2 = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
	osg::ref_ptr<osg::DrawElementsUInt> index = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	osg::ref_ptr<osg::DrawElementsUInt> index1 = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	osg::ref_ptr<osg::DrawElementsUInt> index2 = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	geometry->setVertexArray(vertex);
	geometry->addPrimitiveSet(index);
	geometry1->setVertexArray(vertex);
	geometry1->addPrimitiveSet(index1);
	geometry2->setVertexArray(vertex);
	geometry2->addPrimitiveSet(index2);

	setRefNormal(CVector3::UNIT_Y);
	PipeSection sect0(mRadius);
	int loopR = getLoopR();
	CVector3 center0 = mPosition + CVector3::UNIT_Z * 0.5f * mHeight;
	CVector3 center1 = center0 + CVector3::NEGATIVE_UNIT_Z * mHeight;

	Vector3List circle0 = createSectionPoints(mPosition + CVector3::UNIT_Z * 0.5f * mHeight, CVector3::UNIT_Z, mRefNormal, sect0, loopR);
	Vector3List circle1 = createSectionPoints(mPosition + CVector3::NEGATIVE_UNIT_Z * 0.5f * mHeight,CVector3::UNIT_Z,mRefNormal,sect0,loopR);

	vertex->resize( 2 * loopR + 2);
	vertex->at(0) = Vector3ToVec3(center0);
	vertex->at(vertex->size() - 1) = Vector3ToVec3(center1);
	double radianStep = Math::TWO_PI / loopR;
	for (int i=0;i<loopR;i++)
	{
		vertex->at(i + 1) = Vector3ToVec3(circle0[i]);
		vertex->at(i+loopR + 1) = Vector3ToVec3(circle1[i]);

		int a = 1 + i;
		int b = a + 1;
		int c = 1 + i + loopR;
		int d = c + 1;
		if (i==loopR-1)
		{
			b = 1;
			d = loopR + 1;
		}

		//
		index1->push_back(0);index1->push_back(b);index1->push_back(a);

		//
		index->push_back(b);index->push_back(a);index->push_back(d);
		index->push_back(d);index->push_back(a);index->push_back(c);

		//
		index2->push_back(vertex->size() - 1);index2->push_back(c);index2->push_back(d);

	}

	osgUtil::SmoothingVisitor::smooth(*geometry);

	mGeode->addDrawable(geometry);
	mGeode->addDrawable(geometry1);
	mGeode->addDrawable(geometry2);

	osg::ref_ptr<osg::Vec3Array> normals1 = new osg::Vec3Array;
	normals1->push_back(osg::Z_AXIS);
	geometry1->setNormalArray(normals1,osg::Array::BIND_PER_PRIMITIVE_SET);

	osg::ref_ptr<osg::Vec3Array> normals2 = new osg::Vec3Array;
	normals2->push_back(-osg::Z_AXIS);
	geometry2->setNormalArray(normals2,osg::Array::BIND_PER_PRIMITIVE_SET);


	return mGeode.release();
}

osg::Node* CPipeCylinder::buildNode()
{
	return IPipeData::buildNode();
}