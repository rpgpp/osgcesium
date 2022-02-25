#include "IFeature.h"

namespace OC
{
	String IFeature::LabelFeatureType = "IFeature::Label";
	String IFeature::ModelFeatureType = "IFeature::Model";

	IFeature::IFeature(void)
		:mParent(NULL)
		,mRecord(NULL)
	{
		mRecord = new IRecord;
	}


	IFeature::~IFeature(void)
	{
	}

	void IFeature::setID(long id)
	{
		if (mRecord.valid())
		{
			mRecord->setID(id);
		}
	}

	long IFeature::getID()
	{
		if (mRecord.valid())
		{
			return mRecord->getID();
		}
		return -9999;
	}

	void IFeature::setFeatureName(String name)
	{
		if (mRecord.valid())
		{
			mRecord->setFeatureName(name);
		}
	}

	String IFeature::getFeatureName()
	{
		if (mRecord.valid())
		{
			return mRecord->getFeatureName();
		}
		return StringUtil::BLANK;
	}

	ILayer* IFeature::getParent()
	{
		return mParent;
	}

	void IFeature::setParent(ILayer* layer)
	{
		mParent = layer;
	}

	void IFeature::setRecord(IRecord* record)
	{
		mRecord = record;
	}

	IRecord* IFeature::getRecrod()
	{
		return mRecord.get();
	}

	String IFeature::getType()
	{
		return mFeatureType;
	}

}
