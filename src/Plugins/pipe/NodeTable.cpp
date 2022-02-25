#include "NodeTable.h"
#include "PipeTable.h"
#include "OCModel/ModelUtility.h"
#include "OCModel/IModelFactory.h"
#include "OCModel/ModelDataManager.h"

namespace OC{namespace Modeling{

	NodeTable::NodeTable(PipeTable* owner):mOwner(owner)
	{
	}

	NodeTable::~NodeTable(void)
	{
	}

	NodeMapIterator NodeTable::getIterator(void)
	{
		return NodeMapIterator(mNodeList.begin(),mNodeList.end());
	}

	void NodeTable::push(PipeNode* node)
	{
		long gid = node->getGID();
		TemplateRecord* templaterecord = node->getTemplateRecord();
		
		if (templaterecord->getHeadInfo()->exist(PipeNode::NameNodeNo))
		{
			String nodeNo= templaterecord->getFieldValue(PipeNode::NameNodeNo);
			if (!nodeNo.empty())
			{
				mCodeIDMap.insert(std::make_pair(nodeNo,node->getGID()));
				size_t count = mCodeIDMap.count(nodeNo);
				if (count > 1)
				{
					StringStream sstream;
					sstream<<"["<<getNameTable()<<"]"<<"ID号为[" << gid << "]"<<"的管点探测点号出现重复"<<nodeNo; 
					LOG<<sstream.str();
				}
			}
		}

		mNodeList[gid] = node;

		double const_elevation = mOwner->getConfig()->getDoubleValue(TableConfig::NameConstantElevation,-1000.0);
		if (const_elevation > -999.0)
		{
			node->getInfo().setGroundZ(const_elevation);
			node->position().z = (const_elevation);
		}

		mOwner->onPush(node);
	}

	long NodeTable::findID(String nodecode,CVector3 position)
	{
		std::multimap<String,long>::iterator it = mCodeIDMap.find(nodecode);
		size_t count = mCodeIDMap.count(nodecode);
		if (count == 1)
		{
			return it->second;
		}
		else
		{
			for (size_t i=0;i<count;i++,it++)
			{
				long gid = it->second;
				if (PipeNode* node = find(gid))
				{
					if (Math::RealEqual(position.x,node->position().x,1e-2)
						&&Math::RealEqual(position.y,node->position().y,1e-2))
					{
						return gid;
					}
				}
			}
		}
		return -1;
	}

	PipeNode* NodeTable::find(long gid)
	{
		NodeMap::iterator it = mNodeList.find(gid);
		if (it==mNodeList.end())
		{
			return NULL;
		}
		return it->second;
	}

	NodeMap& NodeTable::getNodeMap()
	{
		return mNodeList;
	}

	void NodeTable::doFeatureInfo(PipeNode* node)
	{
		node->setTable(getNameTable());

		String featureName = node->generateName();
		node->setFeatureName(featureName);
		node->setFeatureUrl(featureName);
	}

	void NodeTable::doModelType(PipeNode* pPipeNode)
	{
#ifdef _DEBUG
		long gid = pPipeNode->getGID();
#endif // _DEBUG

		//管点类型的字段值
		String fieldVal = pPipeNode->getInfo().getCategory();

		if (mOwner->getConfig()->isConfigModel(fieldVal))
		{
			//默认不构造孤立点
			if (!mOwner->getConfig()->getBoolValue(TableConfig::NameBuildIsolatePoint,true)
				&& pPipeNode->getConnective() == 0)
			{
				return;
			}

			//管点类型字段值对应的模型库模型
			ModelNode model = mOwner->getConfig()->getConfigModel(fieldVal);
			pPipeNode->setFeatureName(fieldVal);
			pPipeNode->setFeatureUrl("global://" + model.getStringValue(ModelNode::NameURL));
			pPipeNode->getInfo().setModelNode(model);
			pPipeNode->setType(GLOBAL_MODEL);
			
			//模型的坐标，如果已经做过偏移则加回来
			CVector3 pWorldPosition = pPipeNode->getOffset() + pPipeNode->position();
			if (model.isTouchGround())
			{
				//如果模型是贴地的，则Z值就是地面高程
				pWorldPosition.z = pPipeNode->getInfo().getGroundZ();
				
				//如果地下的管点需要抬高一点
				if (pPipeNode->position().z < 0)
				{
					pWorldPosition.z += mOwner->getConfig()->getDoubleValue(TableConfig::NamePlusUnderGroundNode,0.0);
				}
			}
			pPipeNode->setOffset(pWorldPosition);
			pPipeNode->getInfo().setModelPosition(pWorldPosition);

			//计算缩放
			CVector3 scale = model.getScale();
			if (model.getAutoScaleRadius())
			{
				//查找与此管点连接的管段的最大管径作为缩放因子
				PipeSection minSect,maxSect;
				LinePtrList linkLines = pPipeNode->getLinkLines();
				ModelUtility::getMinMaxSection(linkLines,minSect,maxSect);
				if (maxSect.getType() == UNKNOWM_SECTION)
				{
					pPipeNode->setScale(CVector3(scale.x,scale.y,scale.z));
				}
				else
				{
					double r = maxSect.getRadius();
					pPipeNode->setScale(CVector3(r / scale.x, r / scale.y,r / scale.z));
				}
			} 
			else if (model.isAutoScaleHeight() && model.isTouchGround())
			{
				//计算缩放的高度
				double modelHeight = model.getWidthLenghtHeight().z * scale.z;
				if (model.isOverGround())
				{
					//如果模型在地面上，则管点高度到地面高程的距离为缩放高度
					double realHeight = (pPipeNode->position().z - pPipeNode->getInfo().getGroundZ());
					if (realHeight > modelHeight * 0.3)
					{
						double scaleZ = realHeight / modelHeight;
						pPipeNode->setScale(CVector3(1,1,scaleZ));
					}
				}
				else
				{
					//如果模型在地下，则管点地面高程到管点的距离加上模型规格的一半作为缩放高度
					double realHeight = Math::Abs(pPipeNode->position().z - pPipeNode->getInfo().getGroundZ()) + modelHeight * 0.5;
					if (realHeight > modelHeight)
					{
						scale.z *= (realHeight / modelHeight);
					}
					pPipeNode->setScale(scale);
				}
			}
			else
			{
				pPipeNode->setScale(scale);
			}
			//end calc scale
		}
		else
		{
			//如果此管点不是映射的管点类型，则计算它的连接类型
			pPipeNode->calcType();
		}
	}

	void NodeTable::doMaterialInfo(PipeNode* node)
	{
		if (NULL == node)
		{
			return;
		}

		String strNameMaterialColor = mOwner->getConfig()->getStringValue(TableConfig::NameMaterialColor);

		StringMap sm;
		StringVector sv = StringUtil::split(strNameMaterialColor, "][");
		for (StringVector::iterator it = sv.begin(); it!=sv.end(); it++)
		{
			StringVector nv = StringUtil::split(*it,":");
			if (nv.size() == 2)
			{
				sm[nv[0]] = nv[1];
			}
		}

		String strMaterialString;
		StringMap::iterator itr_map = sm.find(node->getMaterial());
		if (itr_map != sm.end())
		{
			ColourValue color = StringConverter::parseColourValue(itr_map->second);

			stringstream stream;  
			stream<<color.r<<" "<<color.g<<" "<<color.b<<" 1.0";  
			strMaterialString = stream.str();
		}

		node->setMaterialString(strMaterialString);
	}

	void NodeTable::clear()
	{
		mNodeList.clear();
	}

	CRectangle& NodeTable::getExtent()
	{
		return mExtent;
	}

	String NodeTable::getNameTable()
	{
		return mTableName;
	}

	void NodeTable::setNameTable(String table)
	{
		mTableName = table;
	}

	String NodeTable::getAliasName()
	{
		return mAliasName;
	}

	void NodeTable::setAliasName(String name)
	{
		mAliasName = name;
	}

	bool NodeTable::checkData()
	{
		NodeMapIterator it = getIterator();
		while(it.hasMoreElements())
		{
			PipeNode* pNode = it.getNext();

			pNode->setOwnerTable(mOwner);
			pNode->checkData();

			doFeatureInfo(pNode);

			//处理管点模型映射
			doModelType(pNode);

			//处理材质（代码无效）
			doMaterialInfo(pNode);

			if (pNode->getState() != STATE_ERROR
				&& pNode->getType() != UNKNOWN_NODE)
			{
				CVector3 center = pNode->getCenter();
				mExtent.merge(CVector2(center.x, center.y));
			}
		}
		return true;
	}

	IModelDataList NodeTable::getBlockMap()
	{
		IModelDataList datalist;
		if (mOwner->getMateRecord()->getEntityMode() == CMetaRecord::EntityWithSingleRecord)
		{
			NodeMapIterator it = getIterator();
			while(it.hasMoreElements())
			{
				PipeNode* data = it.getNext();
				if (data->getState() == STATE_NORMAL)
				{
					datalist.push_back(data);
				}
			}
		}
		else
		{
			CQuadTreeGenerator generator(mOwner->getMateRecord()->getExtent(),mOwner->getMateRecord()->getLevel());

			NodeMapIterator it = getIterator();
			while(it.hasMoreElements())
			{
				PipeNode* data = it.getNext();
				if (data->getState() == STATE_NORMAL)
				{
					generator.append(data);
				}
			}

			datalist = generator.generate(mOwner,getNameTable());
		}

		return datalist;
	}

	void NodeTable::removeInvalidData()
	{
		
	}

	void NodeTable::fixData()
	{
		
	}

}}

