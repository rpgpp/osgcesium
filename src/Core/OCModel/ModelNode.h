#ifndef _MODEL_NODE_CLASS_H_
#define _MODEL_NODE_CLASS_H_

#include "ModelDefine.h"
#include "OCMain/CoreConfig.h"

namespace OC
{
	namespace Modeling
	{
		typedef std::map<String,ModelNode>	ModelConfigMap;
		typedef MapIterator<ModelConfigMap> ModelConfigIterator;

		class _ModelExport ModelNode : public CCoreConfig
		{
		public:
			ModelNode();
			~ModelNode();

			void fromXmlElement(TiXmlElement* element);
			TiXmlElement* toXmlElement();

			void setName(String name);
			String getName();

			//是否贴地
			bool isTouchGround();

			//模型的正面
			std::pair<bool,CVector3> getDefineFaceDirection();

			//获取配置的缩放值
			CVector3 getScale();

			//这个模型是否需要自动计算缩放的高度
			bool isAutoScaleHeight();

			//获取模型的规格，宽 长 高，单位为米
			CVector3 getWidthLenghtHeight();

			//这个模型是否一定在地上
			bool isOverGround();

			//这个模型是否需要自动计算缩放的半径
			bool getAutoScaleRadius();

			static String NameURL;
			static String NameName;
			static String NameEnName;
			static String NameOffset;
			static String NameScaleHeight;
			static String NameDirection;
			static String NameTouchGround;
			static String NameModelScale;
			static String NameScaleRadius;
			static String NameIsAboveGround;
			static String NameWidthLengthHeight;
			static String NameMustHorizontal;
			static String NameMustInBeeLine;

		};
	}
}


#endif