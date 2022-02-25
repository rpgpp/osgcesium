#ifndef _IFEATURE_CLASS_
#define _IFEATURE_CLASS_

#include "OCMain/osg.h"
#include "IRecord.h"

namespace OC
{
	class _LayerExport IFeature
	{
	public:
		static String LabelFeatureType;
		static String ModelFeatureType;
	public:
		IFeature(void);
		virtual ~IFeature(void);

		virtual void setFeatureName(String name);
		virtual String getFeatureName();

		virtual void setID(long id);
		virtual long getID();

		virtual	ILayer* getParent();
		virtual void setParent(ILayer* layer);

		void setRecord(IRecord* record);
		IRecord* getRecrod();

		String getType();

	protected:
		String					mFeatureType;
		ILayer*					mParent;
		osg::ref_ptr<IRecord>	mRecord;
	};
}

#endif