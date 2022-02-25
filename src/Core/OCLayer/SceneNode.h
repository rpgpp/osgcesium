#ifndef _OC_SceneNode_h__
#define _OC_SceneNode_h__

#include "FeatureEffect.h"
#include "ScenePass.h"
#include "BaseNodeMask.h"

namespace OC
{
	class _LayerExport CSceneNode : public osg::MatrixTransform
	{
	public:
		typedef std::set<osg::ref_ptr<osg::Node> > QueueNode;
		typedef std::set<osg::ref_ptr<CScenePassNode> > QueuePass;
		typedef std::set<osg::ref_ptr<osg::Camera> > RenderPass;

		CSceneNode(bool hasswitch = true);
		virtual ~CSceneNode(void);

		virtual bool init();

		virtual bool add(osg::Node* child);
		virtual bool remove(osg::Node* child);
		virtual void update(osg::NodeVisitor* nv);
		
		virtual bool attach(CSceneNode* parent);
		virtual void detach();

		virtual bool addChild(osg::Node* child);
		virtual bool removeChild(osg::Node* child);

		bool removeAllChild();

		void setVisible(bool visible);

		osg::Switch* getSwitch();

		bool getPickable();
		void setPickable(bool enable);

		virtual CFeatureNode* asFeatureNode(){return NULL;}
		virtual void setEffect(IFeatureEffect* effect);
		virtual IFeatureEffect* getEffect();

		//render pass
		void addScenePassNode(CScenePassNode* node);
		void removeScenePassNode(CScenePassNode* node);
		void removeAllScenePassNode();
		bool contains_scene_pass(CScenePassNode* node);

		//
		virtual void traverse(osg::NodeVisitor& nv);
	protected:
		void takeAddQueue(QueueNode& queue);
		void takeRemoveQueue(QueueNode& queue);
		void takeAddScenePassNodeQueue(QueuePass& queue);
		void takeRemoveScenePassNodeQueue(QueuePass& queue);
		void dirty();
	protected:
		bool							mIsVisible;
		bool							mDirty;
		bool							mAttached;
		bool							mInitialize;
		unsigned int mLastUpdate;
		osg::ref_ptr<osg::Switch>		mSwitch;
		osg::ref_ptr<IFeatureEffect>	mEffect;
		osg::ref_ptr<osg::StateSet>		mPassRootStateSet;
	private:		
		QueuePass						mAddScenePassChildQueue;
		QueuePass						mRemoveScenePassChildQueue;

		QueueNode						mAddChildQueue;
		QueueNode						mRemoveChildQueue;
		ScenePassNodeList			mScenePassNodes;
		osg::observer_ptr<CSceneNode>	mParentNode;
	protected:
		CORE_MUTEX(CSceneNode_MUTEX)
		CORE_MUTEX(RenderPass_MUTEX)
	};
}

#endif
