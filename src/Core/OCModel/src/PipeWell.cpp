
#include "PipeData.h"
#include "ModelUtility.h"
#include "BuildWell.h"

using namespace OC;
using namespace OC::Modeling;

String CPipeWell::TagName = "PipeWell";

TiXmlElement* CPipeWell::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);

	osg::Quat r = mMatrix.getRotate();
	osg::Vec3d t = mMatrix.getTrans();
	osg::Vec3d s = mMatrix.getScale();

	CQuaternion q;
	q.w = r.w();
	q.x = r.x();
	q.y = r.y();
	q.z = r.z();

	element->SetAttribute("Position",StringConverter::toString(CVector3(t.x(),t.y(),t.z())));
	element->SetAttribute("Scale",StringConverter::toString(CVector3(s.x(),s.y(),s.z())));
	element->SetAttribute("Rotation",StringConverter::toString(q));

	element->SetAttribute("Height1",StringConverter::toString(mHeight1));
	element->SetAttribute("Height2",StringConverter::toString(mHeight2));

	element->SetAttribute("CoverImage",mCoverImage);
	element->SetAttribute("WallImage",mWallImage);

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CPipeWell::readXmlElement(TiXmlElement* elemennt)
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

	CVector3 Position = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"Position"));
	CVector3 Scale = StringConverter::parseVector3(TinyXml::read_attribute(elemennt,"Scale"),CVector3::UNIT_SCALE);
	CQuaternion Rotation = StringConverter::parseQuaternion(TinyXml::read_attribute(elemennt,"Rotation"));

	mMatrix = osg::Matrix(osg::Matrix::scale(Scale.x,Scale.y,Scale.z)
		*osg::Matrix::rotate(osg::Quat(Rotation.x,Rotation.y,Rotation.z,Rotation.w))
		*osg::Matrix::translate(Position.x,Position.y,Position.z)
		);

	mHeight1 = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"Height1"));
	mHeight2 = StringConverter::parseReal(TinyXml::read_attribute(elemennt,"Height2"));

	mCoverImage = TinyXml::read_attribute(elemennt,"CoverImage");
	mWallImage = TinyXml::read_attribute(elemennt,"WallImage");
}

void CPipeWell::setMatrix(osg::Matrix position)
{
	mMatrix = position;
}

void CPipeWell::setHeight(double h1,double h2)
{
	mHeight1 = h1;
	mHeight2 = h2;
}

void CPipeWell::setCoverImage(String url)
{
	mCoverImage = url;
}

void CPipeWell::setWallImage(String url)
{
	mWallImage = url;
}

osg::Node* CPipeWell::buildNode()
{
	BuildWell well(mHeight1,mHeight2,mWallImage,mCoverImage);
	well.setImageInternal(mImageInternal);
	osg::ref_ptr<osg::MatrixTransform> pWellNode = well.build();
	pWellNode->setMatrix(mMatrix);
	return pWellNode.release();
}