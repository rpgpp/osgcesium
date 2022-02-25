#include "PipeData.h"
#include "ModelUtility.h"

using namespace OC;
using namespace OC::Modeling;

String CPipeWell1::TagName = "PipeWell1";

CPipeWell1::CPipeWell1()
	:mHatR(0.8f)
	,mRoomR(0.5f)
	,mRoomD(1.0f)
{

}

CPipeWell1::~CPipeWell1()
{

}

TiXmlElement* CPipeWell1::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	element->SetAttribute("mHatR",StringConverter::formatDoubletoString(mHatR));
	element->SetAttribute("mRoomR",StringConverter::formatDoubletoString(mRoomR));
	element->SetAttribute("mRoomD",StringConverter::formatDoubletoString(mRoomD));

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CPipeWell1::readXmlElement(TiXmlElement* elemennt)
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

	mHatR = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"mHatR"));
	mRoomR = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"mRoomR"));
	mRoomD = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"mRoomD"));
}
osg::Geode* CPipeWell1::buildGeode()
{
	mGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry0 = new osg::Geometry;
	osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;
	osg::ref_ptr<osg::Geometry> geometry2 = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexBody = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> vertexHatTop = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> vertexHatSide = new osg::Vec3Array;
	osg::ref_ptr<osg::DrawElementsUInt> indexBody = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	osg::ref_ptr<osg::DrawElementsUInt> indexHatTop = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	osg::ref_ptr<osg::DrawElementsUInt> indexHatSide = new osg::DrawElementsUInt(osg::DrawElements::TRIANGLES);
	osg::ref_ptr<osg::Vec2Array> textureCoordBody = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> textureCoordHatTop = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> textureCoordHatSide = new osg::Vec2Array;

	geometry0->setVertexArray(vertexBody);
	geometry1->setVertexArray(vertexHatTop);
	geometry2->setVertexArray(vertexHatSide);
	geometry0->addPrimitiveSet(indexBody);
	geometry1->addPrimitiveSet(indexHatTop);
	geometry2->addPrimitiveSet(indexHatSide);
	geometry0->setTexCoordArray(0,textureCoordBody,osg::Array::BIND_PER_VERTEX);
	geometry1->setTexCoordArray(0,textureCoordHatTop,osg::Array::BIND_PER_VERTEX);
	geometry2->setTexCoordArray(0,textureCoordHatSide,osg::Array::BIND_PER_VERTEX);

	setRefNormal(CVector3::UNIT_Y);

	PipeSection sect0(mHatR);
	PipeSection sect1(mRoomR);

	double hatH = 0.01f;
	double tx = 1.0f;
	double ty = 4.0f;

	int pn = getLoopR();
	Vector3List circle0 = createSectionPoints(CVector3::UNIT_Z * hatH,CVector3::NEGATIVE_UNIT_Z,mRefNormal,sect0,pn);
	Vector3List circle1 = createSectionPoints(CVector3::NEGATIVE_UNIT_Z * 0.01f,CVector3::NEGATIVE_UNIT_Z,mRefNormal,sect0,pn);
	Vector3List circle2 = createSectionPoints(CVector3::NEGATIVE_UNIT_Z * 0.01f,CVector3::NEGATIVE_UNIT_Z,mRefNormal,sect1,pn);
	Vector3List circle3 = createSectionPoints(CVector3::NEGATIVE_UNIT_Z * mRoomD,CVector3::NEGATIVE_UNIT_Z,mRefNormal,sect1,pn);

	vertexBody->resize(pn*3);
	vertexHatTop->resize(pn);
	vertexHatSide->resize(pn*2);

	textureCoordBody->resize(pn*3);
	textureCoordHatTop->resize(pn);
	textureCoordHatSide->resize(pn*2);

	double radianStep = Math::TWO_PI / pn;
	for (int i=0;i<pn;i++)
	{
		vertexHatTop->at(i) = Vector3ToVec3(circle0[i]);
		vertexHatSide->at(i) = Vector3ToVec3(circle0[i]);
		vertexHatSide->at(i+pn) = Vector3ToVec3(circle1[i]);
		vertexBody->at(i) = Vector3ToVec3(circle1[i]);
		vertexBody->at(i+pn) = Vector3ToVec3(circle2[i]);
		vertexBody->at(i+pn*2) = Vector3ToVec3(circle3[i]);

		int a = i;
		int b = i + 1;
		int c = i + pn;
		int d = c + 1;
		int e = i + pn*2;
		int f = e + 1;

		if (i==pn-1)
		{
			b = 0;
			d = pn;
			f = pn*2;
		}

		indexBody->push_back(a);indexBody->push_back(b);indexBody->push_back(d);
		indexBody->push_back(a);indexBody->push_back(d);indexBody->push_back(c);

		indexBody->push_back(c);indexBody->push_back(d);indexBody->push_back(f);
		indexBody->push_back(c);indexBody->push_back(f);indexBody->push_back(e);

		//bottom
		if (i!=0)
		{
			indexBody->push_back(pn*2);indexBody->push_back(e);indexBody->push_back(f);
			indexHatTop->push_back(0);indexHatTop->push_back(a);indexHatTop->push_back(b);
		}

		indexHatSide->push_back(a);indexHatSide->push_back(b);indexHatSide->push_back(d);
		indexHatSide->push_back(a);indexHatSide->push_back(d);indexHatSide->push_back(c);

		double radian = radianStep * i - Math::HALF_PI;
		double texX = Math::Sin(radian) * mRoomR * Math::TWO_PI;

		textureCoordBody->at(i) = osg::Vec2(texX,0.0f);
		textureCoordBody->at(i + pn) = osg::Vec2(texX,1.0f * (mRoomR - mHatR));
		textureCoordBody->at(i + pn*2) = textureCoordBody->at(i + pn) + osg::Vec2(0.0f,1.0f * mRoomD);

		textureCoordBody->at(i).x() *= tx;
		textureCoordBody->at(i + pn).x() *= tx;
		textureCoordBody->at(i + pn*2).x() *= tx;

		textureCoordBody->at(i).y() *= ty;
		textureCoordBody->at(i + pn).y() *= ty;
		textureCoordBody->at(i + pn*2).y() *= ty;

		textureCoordHatSide->at(i) = osg::Vec2(texX,0.0f);
		textureCoordHatSide->at(i+1) = osg::Vec2(texX,1.0f);

		osg::Vec2 t(-Math::Cos(radian),Math::Sin(radian));
		t *= 0.5f;
		t += osg::Vec2(0.5f,0.5f);
		textureCoordHatTop->at(i) = t;
	}

	//image
	ModelUtility::setTexture(geometry0.get(),"global://images/gyj001.dds",mImageInternal);
	ModelUtility::setTexture(geometry1.get(),"global://images/gyj002.dds",mImageInternal);
	ModelUtility::setTexture(geometry2.get(),"global://images/JT001.dds",mImageInternal);

	//normals
	osgUtil::SmoothingVisitor::smooth(*geometry0);
	osgUtil::SmoothingVisitor::smooth(*geometry2);
	osg::ref_ptr<osg::Vec3Array> pNormals = new osg::Vec3Array;
	pNormals->push_back(osg::Z_AXIS);
	geometry1->setNormalArray(pNormals,osg::Array::BIND_PER_PRIMITIVE_SET);

	mGeode->addDrawable(geometry0);
	mGeode->addDrawable(geometry1);
	mGeode->addDrawable(geometry2);

	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::DECAL);
	mGeode->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

	return mGeode.release();
}
