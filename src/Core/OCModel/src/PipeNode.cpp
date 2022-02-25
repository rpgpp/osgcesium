#include "PipeNode.h"
#include "ModelUtility.h"
#include "PipeLine.h"
#include "IModelTable.h"

namespace OC
{
	namespace Modeling
	{
		String PipeNode::NameID = "ID";
		String PipeNode::NameNodeNo = "点号";
		String PipeNode::NameNodeInter = "埋深";
		String PipeNode::NameNodeElevation = "管点高程";
		String PipeNode::NameGroundElevation = "地面高程";
		String PipeNode::NameNodeModelType = "管点类型";
		String PipeNode::NameSpatialColumn = "空间字段";
		String PipeNode::NameMaterial = "井盖材质";

		PipeNode::PipeNode(void)
			:mNodeInfo(this)
		{
			m_type = UNKNOWN_NODE;
		}

		void PipeNode::doMinusZ(MeasureZ_Flag flag)
		{
			if (flag != MeasureZ_IS_CENTER)
			{
				LinePtrList linkLines = getLinkLines();
				PipeSection minSect,maxSect;
				ModelUtility::getMinMaxSection(linkLines,minSect,maxSect);
				double half_h = maxSect.getType() == RECT_SECTION ? maxSect.getHeight() * 0.5 : maxSect.getRadius();
				if (flag == MeasureZ_IS_BOTTOM)
				{
					//埋深为管底，管中心需要加上截面一半高度
					position() += getWorldZVector() * half_h ;
				}
				if (flag == MeasureZ_IS_TOP)
				{
					//埋深为管顶，管中心需要减去截面一半高度
					position() -= getWorldZVector() * half_h;
				}
			}
		}

		CVector3 PipeNode::getCenter()
		{
			return mPosition;
		}

		void PipeNode::doOffset(CVector3 offset)
		{
			mPosition -= offset;

			setOffset(offset);

			PipePointList::iterator it = mAroundPointList.begin();
			for (;it!=mAroundPointList.end();it++)
			{
				it->Position -= offset;
			}
		}

		PipeNode* PipeNode::create(TemplateRecord* record)
		{
			Vector3List points = ModelUtility::parseSpatialField(record->getGeometry());

			if (points.empty())
			{
				return NULL;
			}

			PipeNode* node = new PipeNode;
			node->setGID(record->getID());
			node->setDNO(record->getIntValue("dno"));
			node->position() = points[0];
			node->setTemplateRecord(record);
			node->position().z = record->getDoubleValue(NameNodeElevation);
			node->setTime(record->getFieldValue(TableConfig::NameTimeStamp));
			node->getInfo().setCategory(record->getFieldValue(NameNodeModelType));
			node->getInfo().setGroundZ(record->getDoubleValue(NameGroundElevation));
			node->setMaterial(record->getFieldValue(NameMaterial));

			StringMap sm = record->getHeadInfo()->getFieldMapping();
			if (sm[NameNodeElevation].empty())
			{
				node->position().z = node->getInfo().getGroundZ();
			}
			else if (sm[NameGroundElevation].empty())
			{
				node->getInfo().setGroundZ(record->getDoubleValue(NameNodeElevation));
			}

			node->position().z -= record->getDoubleValue(NameNodeInter);

			return node;
		}

		PipeNode* PipeNode::create(long gid,int dno,double x,double y,NameValuePairList nameValues)
		{			
			osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo;

			header->add(new OCFieldInfo("gid",OC_DATATYPE_INTEGER,true));
			header->add(new OCGeomFieldInfo("geom","POINT",0));
			header->add(new OCFieldInfo("dno",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("stnod",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("ednod",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo(PipeNode::NameNodeInter,OC_DATATYPE_TEXT));

			osg::ref_ptr<TemplateRecord> templaterecord = new TemplateRecord(header);

			templaterecord->setFieldValue("gid",StringConverter::toString(gid));
			templaterecord->setGeometry(CVector2(x,y));
			templaterecord->setFieldValue("dno",StringConverter::toString(dno));

			NameValuePairList::iterator it = nameValues.begin();
			for(;it!=nameValues.end();it++)
			{
				header->add(new OCFieldInfo(it->first,OC_DATATYPE_TEXT));
				templaterecord->setFieldValue(it->first,it->second);
			}

			return create(templaterecord);
		}

		Vector3List PipeNode::getAroundPoint()
		{
			Vector3List list;

			PipePointList::iterator it = mAroundPointList.begin();
			for (;it!=mAroundPointList.end();it++)
			{
				list.push_back(it->Position);
			}

			return list;
		}

		PipePointList& PipeNode::getAroundPointList()
		{
			return mAroundPointList;
		}

		void PipeNode::addAroundPoint(CVector3 point,PipeSection sect)
		{
			addAroundPoint(PipePoint(point,sect));
		}

		void PipeNode::addAroundPoint(PipePoint around)
		{
			mAroundPointList.push_back(around);
		}

		CVector3& PipeNode::position()
		{
			return mPosition;
		}

		String PipeNode::getMaterial()
		{
			return m_strMaterial;
		}

		void PipeNode::setMaterial(String strMaterial)
		{
			m_strMaterial = strMaterial;
		}

		NodeExInfo& PipeNode::getInfo()
		{
			return mNodeInfo;
		}

		PipeNode* PipeNode::asPipeNode()
		{ 
			return static_cast<PipeNode*>(this);
		}

		int PipeNode::getConnective()
		{
			return (int)mLinkLines.size();
		}

		NodeType PipeNode::calcType()
		{
			int connectCount = getConnective();

			if (connectCount == 0)
			{
				setType(UNKNOWN_NODE);
			}
			else
			{
				SectionType defaultSect = mLinkLines[0]->getSection().getType();

				for (int i=1;i<connectCount;i++)
				{
					PipeLine* line = mLinkLines[i];

					SectionType lineType = line->getSection().getType();

					//如果管点连接的管段具备不同的管径
					if (lineType != defaultSect)
					{
						defaultSect = UNKNOWM_SECTION;
					}
				}

				//根据连接度分类
				switch(connectCount)
				{
				case 1:
					{
						if (CIRCULAR_SECTION == defaultSect)
						{
							setType(CIRCULAR_1_NODE);
						}
						else if (RECT_SECTION == defaultSect)
						{
							setType(RECT_1_NODE);
						}
					}
					break;
				case 2:
					{
						if (CIRCULAR_SECTION == defaultSect)
						{
							setType(CIRCULAR_2_NORMAL_NODE);
						}
						else if (RECT_SECTION == defaultSect)
						{							
							setType(RECT_2_NORMAL_NODE);
						}
						else
						{
							setType(MIX_NODE_2);
						}
					}
					break;
				case 3:
					{
						if (CIRCULAR_SECTION == defaultSect)
						{
							setType(CIRCULAR_3_NORMAL_NODE);
						}
						else if (RECT_SECTION == defaultSect)
						{
							setType(RECT_3_NORMAL_NODE);
						}
						else
						{
							setType(MIX_NODE_3);
						}
					}
					break;
				default:
					{
						setType(MIX_NODE);
					}
				}
			}

			return getType();
		}

		void PipeNode::addLinkLine(PipeLine* line)
		{
			removeLinkLine(line);
			mLinkLines.push_back(line);
		}

		void PipeNode::removeLinkLine(PipeLine* line)
		{
			LinePtrList::iterator it = find(mLinkLines.begin(),mLinkLines.end(),line);
			if (it != mLinkLines.end())
			{
				mLinkLines.erase(it);
			}
		}

		LinePtrList& PipeNode::getLinkLines()
		{
			return mLinkLines;
		}

		//need do sth!!! 11.29
		void PipeNode::checkData()
		{
			long nodeGID = getGID();

			std::map<double,int> interMap;

			//计算管点高程,遍历与本管点连接的管段,挂点的高度减去起点、终点埋深
			LinePtrList::iterator it = mLinkLines.begin();
			while(it!=mLinkLines.end())
			{
				PipeLine* pLine = *it;

				double z1 = position().z;
			
				if (nodeGID == pLine->getStID())
				{						
					z1 -= pLine->getStInter();
				}
				else if (nodeGID == pLine->getEdID())
				{						
					z1 -= pLine->getEdInter();
				}

				interMap[z1] = z1;
				it++;
			}

			//如果存在不同的埋深，则取最低的
			if(!interMap.empty())
			{
				position().z = (interMap.begin()->first);
			}

			//根据埋深预设方式计算管点的中心高程
			if (mOwnerTable)
			{
				doMinusZ(mOwnerTable->getConfig()->getMeasureZ_Flag());
			}
		}

	}
}

