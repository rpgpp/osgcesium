#ifndef _OC_FEATURE_H__
#define _OC_FEATURE_H__

#include "IFeature.h"
#include "FeatureNode.h"

namespace OC
{
	class _LayerExport CFeature : public IFeature
	{
	public:
		static String ClassGUID;

		CFeature();
		~CFeature();

		bool load();

		virtual void loadImpl(void);
		virtual void unloadImpl(void){}

		virtual void preUnloadImpl(void) {}
		
		virtual void postUnloadImpl(void) {}

        virtual void prepareImpl(void);
       
        virtual void unprepareImpl(void) {}

		size_t calculateSize(void) const { return 0; }
	private:
		bool							mLoaded;
		bool							mAttached;
		osg::ref_ptr<CFeatureNode>		mFeatureNode;
		osg::ref_ptr<FeatureRecord>		mFeatureRecord;
	};

	typedef SharedPtr<CFeature> FeaturePtr;
}


#endif
