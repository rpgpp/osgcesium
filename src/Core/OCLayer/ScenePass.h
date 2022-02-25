#ifndef _OC_ScenePassNode_H__
#define _OC_ScenePassNode_H__

#include "LayerDefine.h"
#include "OCMain/osg.h"

namespace OC
{
	class _LayerExport CScenePassNode : public osg::Group
	{
	public:
		CScenePassNode()
		{
			mPassStateSet= new osg::StateSet;
			//mPassStateSet->addUniform(new osg::Uniform("enable_pass",osg::Uniform::BOOL,1));
		}
		virtual ~CScenePassNode(){}

		osg::StateSet* getPassStateSet(){return mPassStateSet.get();}

	protected:
		osg::ref_ptr<osg::StateSet> mPassStateSet;
	};

	typedef std::vector<osg::ref_ptr<CScenePassNode> > ScenePassNodeList;

	class _LayerExport CScenePassCamera : public osg::Camera
	{
	public:
		CScenePassCamera(void);
		~CScenePassCamera(void);
		virtual void cull(osgUtil::CullVisitor& cv);
		void setCullNode(osg::Group* g);
		virtual void traverse(osg::NodeVisitor& nv);		
		bool isDirty()
		{
			return !_boundingSphereComputed;
		}
	private:
		osg::Group* mGroup;
	};

	class _LayerExport CScenePass : public osg::Referenced
	{
	public:
		CScenePass(void);
		virtual ~CScenePass(void);

		virtual void traverse(osg::Node* node, osg::NodeVisitor* nv){}

		osg::Camera* getCamera(){return mCamera.get();}
		void setCamera(osg::Camera* camera){mCamera = camera;}

		osg::StateSet* getStateSet(){return mStateSet.get();}
		void setStateSet(osg::StateSet* stateset){mStateSet = stateset;}
	protected:
		osg::ref_ptr<osg::Camera> mCamera;
		osg::ref_ptr<osg::StateSet> mStateSet;
	};

	typedef std::set<osg::ref_ptr<CScenePass> > ScenePassSet;
}

#endif
