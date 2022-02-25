#include "IRecord.h"

namespace OC
{
	IRecord::IRecord()
		:mID(-9999)
		,mOwner(NULL)
		,mDirty(false)
	{

	}

	IRecord::~IRecord()
	{

	}

	long IRecord::getID()
	{
		return mID;
	}

	void IRecord::setID(long id)
	{
		mID = id;
	}

	String IRecord::getFeatureName()
	{
		return mFeatureName;
	}

	void IRecord::setFeatureName(String name)
	{
		mFeatureName = name;
	}

	IFeature* IRecord::getOwnerFeature()
	{
		return mOwner;
	}

	void IRecord::setOwnerFeature(IFeature* feature)
	{
		mOwner = feature;
	}

	void IRecord::update()
	{
		if (!mDirty)
		{
			return;
		}
		//
	}

	void IRecord::dirty(bool _dirty)
	{
		mDirty = _dirty;
	}
}
