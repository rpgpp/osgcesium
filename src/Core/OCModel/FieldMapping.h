#ifndef _OC_FIELD_MAPPING_H__
#define _OC_FIELD_MAPPING_H__

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		typedef std::vector<osg::ref_ptr<FieldMapping> > FieldMappingList;
		typedef VectorIterator<FieldMappingList> FieldMappingVectorIterator;

		typedef std::map<String,osg::ref_ptr<FieldMapping> > FieldMappingMap;
		typedef MapIterator<FieldMappingMap> FieldMappingIterator;

		typedef std::vector<std::pair<String,String> > FieldMappingPair;
		class _ModelExport FieldMapping : public osg::Referenced
		{
		public:
			FieldMapping();
			~FieldMapping();

			String getName();
			void setName(String name);

			void fromNameValues(NameValuePairList lineNVs,NameValuePairList nodeNVs);
			void fromXmlElement(TiXmlElement* element);
			void toXmlElement(TiXmlElement* element);

			void setLineQueryHeader(OCHeaderInfo* header);
			OCHeaderInfo* getLineQueryHeader();

			void setNodeQueryHeader(OCHeaderInfo* header);
			OCHeaderInfo* getNodeQueryHeader();

			FieldMappingPair getMappingPairLine();
			FieldMappingPair getMappingPairNode();

			static StringVector getKeyAlignFieldsLine();
			static StringVector getKeyAlignFieldsNode();
			static void addToHeader(OCHeaderInfo* header,String cfg,String fieldname);
			static String TagFieldMap;
			bool isDirty(){return mDirty;}
			void setDirty(bool isChange = true){mDirty = isChange;}
		protected:
			osg::ref_ptr<OCHeaderInfo> mLineQueryHeader;
			osg::ref_ptr<OCHeaderInfo> mNodeQueryeHeader;
			bool		mDirty;
			String	mName;
			TiXmlElement* mTiXmlElement;
		};
	}
}



#endif
