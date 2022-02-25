
#include "Modelnode.h"


namespace OC
{
	namespace Modeling
	{
		String ModelNode::NameURL				= "url";
		String ModelNode::NameEnName			= "enname";
		String ModelNode::NameName				= "name";
		String ModelNode::NameOffset			= "offset";
		String ModelNode::NameScaleHeight		= "AutoScaleZ";
		String ModelNode::NameDirection			= "direction";
		String ModelNode::NameTouchGround		= "ground";
		String ModelNode::NameModelScale		= "scale";
		String ModelNode::NameScaleRadius		= "AutoScaleRadius";
		String ModelNode::NameIsAboveGround		= "AboveGround";
		String ModelNode::NameWidthLengthHeight = "WLH";
		String ModelNode::NameMustHorizontal	= "MustHorizontal";
		String ModelNode::NameMustInBeeLine		= "MustInBeeLine";

		ModelNode::ModelNode()
		{

		}

		ModelNode::~ModelNode()
		{

		}

		void ModelNode::fromXmlElement(TiXmlElement* element)
		{
			String name = TinyXml::read_attribute(element,ModelNode::NameName);
			setName(name);
			setConfig(ModelNode::NameURL,TinyXml::read_attribute(element,ModelNode::NameURL));
			setConfig(ModelNode::NameEnName,TinyXml::read_attribute(element,ModelNode::NameEnName));
			setConfig(ModelNode::NameScaleHeight,TinyXml::read_attribute(element,ModelNode::NameScaleHeight));
			setConfig(ModelNode::NameOffset,TinyXml::read_attribute(element,ModelNode::NameOffset));
			setConfig(ModelNode::NameTouchGround,TinyXml::read_attribute(element,ModelNode::NameTouchGround));
			setConfig(ModelNode::NameDirection,TinyXml::read_attribute(element,ModelNode::NameDirection));
			setConfig(ModelNode::NameModelScale,TinyXml::read_attribute(element,ModelNode::NameModelScale));
			setConfig(ModelNode::NameScaleRadius,TinyXml::read_attribute(element,ModelNode::NameScaleRadius));
			setConfig(ModelNode::NameIsAboveGround,TinyXml::read_attribute(element,ModelNode::NameIsAboveGround));
			setConfig(ModelNode::NameWidthLengthHeight,TinyXml::read_attribute(element,ModelNode::NameWidthLengthHeight));
			setConfig(ModelNode::NameMustHorizontal,TinyXml::read_attribute(element,ModelNode::NameMustHorizontal));
		}

		TiXmlElement* ModelNode::toXmlElement()
		{
			TiXmlElement* element = new TiXmlElement("Model");
			
			
			return element;
		}

		void ModelNode::setName(String name)
		{
			setConfig(NameName,name);
		}

		String ModelNode::getName()
		{
			return getStringValue(NameName);
		}

		bool ModelNode::isTouchGround()
		{
			return getBoolValue(NameTouchGround,true);
		}

		std::pair<bool,CVector3> ModelNode::getDefineFaceDirection()
		{
			std::pair<bool,CVector3> result;
			
			String dir = getStringValue(NameDirection);

			if (!dir.empty())
			{
				result.first = true;
				result.second = StringConverter::parseVector3(dir);
			}

			return result;
		}

		CVector3 ModelNode::getScale()
		{
			return getVector3Value(NameModelScale,CVector3::UNIT_SCALE);
		}

		bool ModelNode::isAutoScaleHeight()
		{
			return getBoolValue(NameScaleHeight);
		}

		bool ModelNode::getAutoScaleRadius()
		{
			return getBoolValue(NameScaleRadius);
		}

		CVector3 ModelNode::getWidthLenghtHeight()
		{
			return getVector3Value(NameWidthLengthHeight,CVector3::UNIT_SCALE);
		}

		bool ModelNode::isOverGround()
		{
			return getBoolValue(NameIsAboveGround,true);
		}

	}
}