#include "ScenePass.h"

namespace OC
{
	CScenePassCamera::CScenePassCamera(void)
		:mGroup(NULL)
	{

	}

	void CScenePassCamera::setCullNode(osg::Group* g)
	{
		mGroup = g;
	}

	CScenePassCamera::~CScenePassCamera(void)
	{

	}

	void CScenePassCamera::cull(osgUtil::CullVisitor& cv)
	{
		mGroup->osg::Group::traverse(cv);
	}

	void CScenePassCamera::traverse(osg::NodeVisitor& nv)
	{
		if (mGroup/* && nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR*/)
		{
			mGroup->osg::Group::traverse(nv);
		}
		else
		{
			osg::Camera::traverse(nv);
		}
	}

	CScenePass::CScenePass(void)
	{
	}


	CScenePass::~CScenePass(void)
	{
	}

}