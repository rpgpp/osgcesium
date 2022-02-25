#pragma once
#include "OCMain/Singleton.h"
#include "ISection.h"
#include "PipeData.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport PipeGeomLibrary : public CSingleton<PipeGeomLibrary>
		{
		public:
			PipeGeomLibrary();
			~PipeGeomLibrary();

			osg::Node* createInstance(CCirclePipeLine* pipeline);
			osg::Node* createInstance(CRectPipeLine* pipeline);
		private:
			osg::Geode* createUnitCirclePipe();
			osg::Geode* createUnitRectPipe(bool close);
			osg::ref_ptr<osg::Geode>		mCirclePipe;
			osg::ref_ptr<osg::Geode>		mRectPipe;
			osg::ref_ptr<osg::Geode>		mRectPipeClose;
			osg::ref_ptr<osg::Program>   mPipeInstanceProgram;
			osg::ref_ptr<osg::StateSet>    mNormaizeState;
			RefObjectCacher<osg::StateSet> mInstanceTextures;
		};
	}
}