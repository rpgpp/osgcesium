#ifndef _OC_IRECORD_H__
#define _OC_IRECORD_H__

#include "LayerDefine.h"

namespace OC
{
	class _LayerExport IRecord : public osg::Referenced
	{
	public:
		IRecord();
		~IRecord();

		virtual long getID();
		virtual void setID(long id);

		virtual String getFeatureName();
		virtual void setFeatureName(String name);

		virtual IFeature* getOwnerFeature();
		virtual void setOwnerFeature(IFeature* feature);

		virtual void update();

		void dirty(bool _dirty = true);

	protected:
		long		mID;
		bool		mDirty;
		String		mFeatureName;
		IFeature*	mOwner;
	};
}

#endif
