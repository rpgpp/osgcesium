#include "SceneNode.h"

namespace OC
{
	struct SceneNodeCallback : public osg::NodeCallback
	{
		SceneNodeCallback():mLastUpdate(0){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
			if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR &&
				nv->getFrameStamp() &&
				nv->getFrameStamp()->getFrameNumber() != mLastUpdate) {
					mLastUpdate = nv->getFrameStamp()->getFrameNumber();
					static_cast<CSceneNode*>(node)->update(nv);
			}
			traverse(node, nv);
		}
		unsigned int mLastUpdate;
	};

	CSceneNode::CSceneNode(bool hasswitch)
		:mInitialize(false)
		,mDirty(false)
		,mAttached(false)
		,mLastUpdate(0)
	{
		if (hasswitch)
		{
			osg::Switch* s = new osg::Switch;
			osg::Group::addChild(s);
			mSwitch = s;
		}
		setUpdateCallback(new SceneNodeCallback);
	}


	CSceneNode::~CSceneNode(void)
	{
	}

	void CSceneNode::takeAddQueue(QueueNode& queue)
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)
		std::swap(mAddChildQueue,queue);
	}

	void CSceneNode::takeRemoveQueue(QueueNode& queue)
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)
		std::swap(mRemoveChildQueue,queue);
	}

	void CSceneNode::takeAddScenePassNodeQueue(QueuePass& queue)
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)
		std::swap(mAddScenePassChildQueue,queue);
	}

	void CSceneNode::takeRemoveScenePassNodeQueue(QueuePass& queue)
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)
		std::swap(mRemoveScenePassChildQueue,queue);
	}

	bool CSceneNode::init()
	{
		return true;
	}

	void CSceneNode::setEffect(IFeatureEffect* effect)
	{
		mEffect = NULL;
		mEffect = effect;
	}

	IFeatureEffect* CSceneNode::getEffect()
	{
		return mEffect.get();
	}

	void CSceneNode::update(osg::NodeVisitor* nv)
	{
		if (mDirty)
		{
			mDirty = false;

			//remove child
			CSceneNode::QueueNode removeQueue;
			takeRemoveQueue(removeQueue);

			osg::Group* g = mSwitch.valid() ? mSwitch.get()->asGroup() : this;

			if(!removeQueue.empty())
			{
				CSceneNode::QueueNode::iterator it = removeQueue.begin();

				for (;it!=removeQueue.end();it++)
				{
					g->removeChild(*it);
				}
			}

			//add child
			CSceneNode::QueueNode addQueue;
			takeAddQueue(addQueue);
			CSceneNode::QueueNode::iterator it = addQueue.begin();

			for (;it!=addQueue.end();it++)
			{
				osg::Node* pNode = it->get();
				g->addChild(pNode);
			}

			//pass
			{
				//remove child
				CSceneNode::QueuePass removeQueue;
				takeRemoveScenePassNodeQueue(removeQueue);

				if (!removeQueue.empty())
				{
					CSceneNode::QueuePass::iterator it = removeQueue.begin();

					for (; it != removeQueue.end(); it++)
					{
						ScenePassNodeList::iterator jt = mScenePassNodes.begin();
						for (; jt != mScenePassNodes.end(); jt++)
						{
							if (it->get() == jt->get())
							{
								mScenePassNodes.erase(jt);
								break;
							}
						}
					}
				}

				//add child
				CSceneNode::QueuePass addQueue;
				takeAddScenePassNodeQueue(addQueue);
				CSceneNode::QueuePass::iterator it = addQueue.begin();

				for (; it != addQueue.end(); it++)
				{
					mScenePassNodes.push_back(*it);
				}
			}

		}

		osg::ref_ptr<IFeatureEffect> effect = mEffect;
		if (effect.valid())
		{

		}
	}

	void CSceneNode::dirty()
	{
		mDirty = true;
	}

	bool CSceneNode::getPickable()
	{
		return getNodeMask() != SkyBoxNodeMask;
	}

	void CSceneNode::setPickable(bool enable)
	{
		if (enable)
		{
			setNodeMask(SceneNodeMask);
		}
		else
		{
			setNodeMask(SkyBoxNodeMask);
		}
	}


	osg::Switch* CSceneNode::getSwitch()
	{
		return mSwitch.get();
	}

	void CSceneNode::setVisible(bool visible)
	{
		if (!mSwitch.valid())
		{
			return;
		}

		if (visible)
		{
			mSwitch->setAllChildrenOn();
			mSwitch->setNodeMask(0xFFFFFF);
		}
		else
		{
			mSwitch->setAllChildrenOff();
			mSwitch->setNodeMask(0L);
		}
		mSwitch->setNewChildDefaultValue(visible);
	}

	bool CSceneNode::attach(CSceneNode* parent)
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)

		if (mAttached)
		{
			return true;
		}

		mAttached = true;
		mParentNode = parent;
		return parent->add(this);
	}

	void CSceneNode::detach()
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)

		if (!mAttached)
		{
			return;
		}

		mAttached = false;

		osg::ref_ptr<CSceneNode> parent;
		if (mParentNode.lock(parent))
		{
			parent->remove(this);
		}

		mParentNode = NULL;
	}

	bool CSceneNode::addChild(osg::Node* child)
	{
		return mSwitch.valid() ? mSwitch->addChild(child) : osg::Group::addChild(child);
	}

	bool CSceneNode::removeChild(osg::Node* child)
	{
		return mSwitch.valid() ? mSwitch->removeChild(child) : osg::Group::removeChild(child);
	}

	bool CSceneNode::add(osg::Node* child)
	{
		if (child == NULL)
		{
			return false;
		}

		CORE_LOCK_MUTEX(CSceneNode_MUTEX)

		osg::ref_ptr<osg::Node> node = child;

		QueueNode::iterator it = mRemoveChildQueue.find( node );
		if( it != mRemoveChildQueue.end() )
			mRemoveChildQueue.erase(it);

		if( mAddChildQueue.find( node ) == mAddChildQueue.end() )
			mAddChildQueue.insert(node);

		dirty();

		return true;
	}

	bool CSceneNode::remove(osg::Node* child)
	{
		if (child == NULL)
		{
			return false;
		}

		CORE_LOCK_MUTEX(CSceneNode_MUTEX)

		osg::ref_ptr<osg::Node> node = child;

		QueueNode::iterator it = mAddChildQueue.find( node );
		if( it != mAddChildQueue.end() )
			mAddChildQueue.erase(it);

		if( mRemoveChildQueue.find( node ) == mRemoveChildQueue.end() )
			mRemoveChildQueue.insert(node);

		dirty();

		return true;
	}

	bool CSceneNode::removeAllChild()
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)

		mAddChildQueue.clear();

		mRemoveChildQueue.clear();

		osg::Group* g = mSwitch.valid() ? mSwitch.get()->asGroup() : this;
		for (unsigned int i = 0;i< g->getNumChildren();i++)
		{
			mRemoveChildQueue.insert(g->getChild(i));
		}

		dirty();

		return true;
	}
	
	void CSceneNode::addScenePassNode(CScenePassNode* child)
	{
		if (child != NULL)
		{
			CORE_LOCK_MUTEX(CSceneNode_MUTEX)
			osg::ref_ptr<CScenePassNode> node = child;

			QueuePass::iterator it = mRemoveScenePassChildQueue.find( node );
			if( it != mRemoveScenePassChildQueue.end() )
				mRemoveScenePassChildQueue.erase(it);

			if( mAddScenePassChildQueue.find( node ) == mAddScenePassChildQueue.end() )
				mAddScenePassChildQueue.insert(node);

			dirty();

			if (!mPassRootStateSet.valid())
			{
				mPassRootStateSet = new osg::StateSet;
				mPassRootStateSet->addUniform(new osg::Uniform( "texture_0", 0 ) );
				mPassRootStateSet->addUniform(new osg::Uniform( "texture_1", 1 ) );

				osg::ref_ptr<osg::Shader> vertShader = osgDB::readShaderFile(osg::Shader::VERTEX,"global://shader/PassRoot.vert");
				osg::ref_ptr<osg::Shader> fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT,"global://shader/PassRoot.frag");
			}
		}
	}

	void CSceneNode::removeScenePassNode(CScenePassNode* child)
	{
		if (child != NULL)
		{
			CORE_LOCK_MUTEX(CSceneNode_MUTEX)
			osg::ref_ptr<CScenePassNode> node = child;

			QueuePass::iterator it = mAddScenePassChildQueue.find( node );
			if( it != mAddScenePassChildQueue.end() )
				mAddScenePassChildQueue.erase(it);

			if( mRemoveScenePassChildQueue.find( node ) == mRemoveScenePassChildQueue.end() )
				mRemoveScenePassChildQueue.insert(node);

			dirty();
		}
	}

	void CSceneNode::removeAllScenePassNode()
	{
		CORE_LOCK_MUTEX(CSceneNode_MUTEX)
		mAddScenePassChildQueue.clear();
		mRemoveScenePassChildQueue.clear();
		ScenePassNodeList::iterator it = mScenePassNodes.begin();
		for (;it!=mScenePassNodes.end();it++)
		{
			mRemoveScenePassChildQueue.insert(it->get());
		}
		dirty();
	}

	bool CSceneNode::contains_scene_pass(CScenePassNode* node)
	{
		return false;
	}

	void CSceneNode::traverse(osg::NodeVisitor& nv)
	{
		//if (nv.getFrameStamp() &&
		//	nv.getFrameStamp()->getFrameNumber() != mLastUpdate) {
		//		mLastUpdate = nv.getFrameStamp()->getFrameNumber();
		//		this->update(&nv);
		//}

		switch(nv.getVisitorType())
		{
		case osg::NodeVisitor::UPDATE_VISITOR:
			{
				osg::Group::traverse(nv);
				break;
			}
		case osg::NodeVisitor::CULL_VISITOR:
			{
				osg::ref_ptr<osgUtil::CullVisitor> cv = osg::dynamic_pointer_cast<osgUtil::CullVisitor,osg::NodeVisitor>(&nv);

				bool travel_pass = (cv->getTraversalMask() & PassMask) != 0;
				//travel_pass = false;
				if (travel_pass && !mScenePassNodes.empty())
				{
					ScenePassNodeList::iterator it = mScenePassNodes.begin();
					for (;it!=mScenePassNodes.end();it++)
					{
						CScenePassNode* pass =  it->get();
						osg::StateSet* stateset = pass->getPassStateSet();
						//stateset->getUniform("enable_pass")->set(false);
						pass->accept(nv);
						cv->pushStateSet(stateset);
					}

					cv->pushStateSet(mPassRootStateSet);
					osg::Group::traverse(nv);
					cv->popStateSet();

					it = mScenePassNodes.begin();
					for (;it!=mScenePassNodes.end();it++)
					{
						cv->popStateSet();
					}
				}
				else
				{
					osg::Group::traverse(nv);
				}
				break;
			}
		default:
			osg::Group::traverse(nv);
		}

	}
}


