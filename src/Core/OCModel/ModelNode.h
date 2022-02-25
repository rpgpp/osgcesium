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

			//�Ƿ�����
			bool isTouchGround();

			//ģ�͵�����
			std::pair<bool,CVector3> getDefineFaceDirection();

			//��ȡ���õ�����ֵ
			CVector3 getScale();

			//���ģ���Ƿ���Ҫ�Զ��������ŵĸ߶�
			bool isAutoScaleHeight();

			//��ȡģ�͵Ĺ�񣬿� �� �ߣ���λΪ��
			CVector3 getWidthLenghtHeight();

			//���ģ���Ƿ�һ���ڵ���
			bool isOverGround();

			//���ģ���Ƿ���Ҫ�Զ��������ŵİ뾶
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