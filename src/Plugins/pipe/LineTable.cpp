#include "LineTable.h"
#include "PipeTable.h"
#include "OCModel/ModelUtility.h"


namespace OC{namespace Modeling{

	inline bool isFirePlug(PipeNode* node)
	{
		if (node->getType() == GLOBAL_MODEL && 
			(StringUtil::endsWith(node->getFeatureUrl(),"消防栓.osgb")
			||StringUtil::endsWith(node->getFeatureUrl(),"消防栓7.osgb")))
		{
			return true;
		}
		return false;
	}

	inline bool isCollectorBox(PipeNode* node)
	{
		if (node->getType()==GLOBAL_MODEL && StringUtil::endsWith(node->getFeatureUrl(), "集线箱.osgb"))
		{
			return true;
		}
		return false;
	}

	inline bool isLamp(PipeNode* node)
	{
		if(node->getType() == GLOBAL_MODEL
			&& StringUtil::endsWith(node->getFeatureUrl(),"路灯.osgb"))
		{
			return true;
		}
		return false;
	} 

	inline void fixErrorLine(LinePtrList errorLineList,int maxNum)
	{
		if (maxNum--<0)
		{
			return;
		}

		LinePtrList outErrorLines;

		LinePtrList::iterator it = errorLineList.begin();
		for (;it!=errorLineList.end();it++)
		{
			PipeLine* pLine = *it;
			if (!pLine->fixSectNull2Near())
			{
				outErrorLines.push_back(*it);
			}
		}

		if (outErrorLines.size() < errorLineList.size())
		{
			fixErrorLine(outErrorLines,maxNum);
		}
	}

	LineTable::LineTable(PipeTable* owner):mOwner(owner)
	{
	}

	LineTable::~LineTable(void)
	{
		
	}

	IModelDataList LineTable::getBlockMap()
	{
		IModelDataList datalist;
		if (mOwner->getMateRecord()->getEntityMode() == CMetaRecord::EntityWithSingleRecord)
		{
			LineMapIterator it = getIterator();
			while(it.hasMoreElements())
			{
				PipeLine* data = it.getNext();
				if (data->getState() == STATE_NORMAL)
				{
					datalist.push_back(data);
				}
			}
		}
		else
		{
			CQuadTreeGenerator generator(mOwner->getMateRecord()->getExtent(),mOwner->getMateRecord()->getLevel());

			LineMapIterator it = getIterator();
			while(it.hasMoreElements())
			{
				PipeLine* data = it.getNext();
				if (data->getState() == STATE_NORMAL)
				{
					generator.append(data);
				}
			}

			datalist = generator.generate(mOwner,getNameTable());
		}

		return datalist;
	}

	void LineTable::push(PipeLine* line)
	{
		//if (OC::Math::RealEqual(line->getSection().getRadius(),0.0f))
		//{
		//	StringStream sstream;
		//	sstream<<"["<<getNameTable()<<"]"<<"ID号为["<<line->getGID()<<"]的管段管径为空";
		//	line->getSection().setType(UNKNOWM_SECTION);
		//	LOG<<sstream.str();
		//}

		line->setTable(mOwner->getName() + LineTableSubfix);
		mLineList[line->getGID()] = line;

		mOwner->onPush(line);
	}

	PipeLine* LineTable::find(long gid)
	{
		LineMap::iterator it = mLineList.find(gid);
		if (it==mLineList.end())
		{
			return NULL;
		}
		return it->second;
	}

	void LineTable::clear()
	{
		mLineList.clear();
	}

	CRectangle& LineTable::getExtent()
	{
		return mExtent;
	}

	String LineTable::getNameTable()
	{
		return mTableName;
	}

	void LineTable::setNameTable(String table)
	{
		mTableName = table;
	}

	String LineTable::getAliasName()
	{
		return mAliasName;
	}

	void LineTable::setAliasName(String name)
	{
		mAliasName = name;
	}

	LineMapIterator LineTable::getIterator(void)
	{
		return LineMapIterator(mLineList.begin(),mLineList.end());
	}

	void LineTable::fixData()
	{
		LinePtrList pUnknownLines;

		LineMapIterator lineIt = getIterator();
		while(lineIt.hasMoreElements())
		{
			PipeLine* line = lineIt.getNext();
			line->setOwnerTable(mOwner);
#if _DEBUG
			long gid = line->getGID();
#endif
			long stnod = line->getStID();
			long ennod = line->getEdID();
			NodeTable& nodetable = mOwner->getNodeTable();
			if (stnod < 1)
			{
				CVector3 st_point = line->getStartPoint();
				stnod = nodetable.findID(line->getTemplateRecord()->getFieldValue(PipeLine::NameStnodNo),st_point);
				line->setStID(stnod);
			}
			if (ennod < 1)
			{
				CVector3 ed_point = line->getEndPoint();
				ennod = nodetable.findID(line->getTemplateRecord()->getFieldValue(PipeLine::NameEdnodNo),ed_point);
				line->setEdID(ennod);
			}
			PipeNode* stNode = nodetable.find(stnod);
			PipeNode* edNode = nodetable.find(ennod);
			if (stNode != NULL)
			{
				stNode->addLinkLine(line);
			}
			if (edNode != NULL)
			{
				edNode->addLinkLine(line);
			}
			line->setStNode(stNode);
			line->setEdNode(edNode);
			if (stNode == NULL || edNode == NULL)
			{
				line->setState(STATE_ERROR);
			}
			if (stNode != NULL && edNode != NULL)
			{
				CVector3 stPoint = line->getStartPoint();
				CVector3 edPoint = line->getEndPoint();
				CVector3 stPosition = stNode->position();
				CVector3 edPosition =edNode->position();
				stPoint.z = edPoint.z = stPosition.z =edPosition.z;
				if (stPoint == edPosition && edPoint == stPosition)
				{
					reverse(line->getLinPoint().begin(),line->getLinPoint().end());
				}
			}

			if (line->getSection().getType() == UNKNOWM_SECTION)
			{
				pUnknownLines.push_back(line);
			}
		}

		//
		bool fix_data = mOwner->getConfig()->getBoolValue(TableConfig::NameFixNullSectLine, true);
		if (fix_data)
		{
			PipeSection defaultPipeSect;

			String defaultSectString = mOwner->getConfig()->getStringValue(TableConfig::NameDefaultSect);
			if (!defaultSectString.empty())
			{
				defaultPipeSect = ModelUtility::parseSect(defaultSectString);
			}

			fixErrorLine(pUnknownLines,15);
			if (defaultPipeSect.getType() != UNKNOWM_SECTION)
			{
				for(LinePtrList::iterator it = pUnknownLines.begin();
					it!=pUnknownLines.end();it++)
				{
					PipeLine* line = *it;
					if(line->getSection().getType() == UNKNOWM_SECTION)
					{
						line->setSection(defaultPipeSect);
						StringStream sstream;
						sstream<<"["<<line->getTable()<<"]"<<"ID号为["<<line->getGID()<<"]的管段管径为空,使用默认值"<<defaultSectString;
						LOG<<sstream.str();
					}
				}
			}
		}
	}

	void LineTable::doFeatureInfo(PipeLine* line)
	{
		line->setTable(getNameTable());

		String featureName = line->generateName();
		line->setFeatureName(featureName);
		line->setFeatureUrl(featureName);
	}

	void LineTable::doFixLineZ(PipeLine* line)
	{
		PipeNode* stNode = line->getStNode();
		PipeNode* edNode = line->getEdNode();

		if (stNode != NULL && edNode != NULL)
		{
			line->setLineZ(stNode->position().z,edNode->position().z);
		}
	}

	void LineTable::doMaterialInfo(PipeLine* line)
	{
		if (NULL == line)
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
		StringMap::iterator itr_map = sm.find(line->getMaterial());
		if (itr_map != sm.end())
		{
			ColourValue color = StringConverter::parseColourValue(itr_map->second);

			stringstream stream;  
			stream<<color.r<<" "<<color.g<<" "<<color.b<<" 1.0";  
			strMaterialString = stream.str();
		}

		line->setMaterialString(strMaterialString);
	}

	inline void oper1(PipeNode* node)
	{
		LinePtrList linkLines = node->getLinkLines();
		LinePtrList::iterator it = linkLines.begin();
		for (;it!=linkLines.end();it++)
		{
			PipeLine* line = *it;
			Vector3List linePt = line->getTwoPointFromNode(node->getGID());
			node->addAroundPoint(linePt[1],line->getSection());
		}
	}

	inline void oper2(PipeNode* node)
	{
		long nodeGID = node->getGID();

		double m_subDistanceFactor = 1.5f;

		LinePtrList linkLines = node->getLinkLines();

		if (linkLines.size() < 1)
		{
			node->setType(MIX_NODE);
			return;
		}

		PipeLine* line1 = linkLines[0];
		PipeLine* line2 = linkLines[1];

		Vector3List line1Pt = line1->getTwoPointFromNode(node->getGID());
		Vector3List line2Pt = line2->getTwoPointFromNode(node->getGID());

		CVector3 dir1 = (line1Pt[1] - line1Pt[0]);
		CVector3 dir2 = (line2Pt[1] - line2Pt[0]);

		double len1 = dir1.length();
		double len2 = dir2.length();

		dir1.normalise();
		dir2.normalise();

		//检查管径和管段的长度，假如管径大于管段的长度，则无法插值弯头，只能建井
		PipeSection minSect, maxSect;
		ModelUtility::getMinMaxSection(linkLines,minSect,maxSect);
		double subLen = maxSect.getRadius();
		if (maxSect.getType() == RECT_SECTION)
		{
			CRadian angle = dir1.angleBetween(dir2);

			if (angle > CRadian(CDegree(160)))
			{
				m_subDistanceFactor *= 0.5;

				if (m_subDistanceFactor * subLen > min(len1,len2))
				{
					m_subDistanceFactor *= 0.5;

					if (m_subDistanceFactor * subLen > min(len1,len2))
					{
						node->setType(MIX_NODE);
						return;
					}
				}
			}
		}

		if (m_subDistanceFactor * subLen > min(len1,len2))
		{
			m_subDistanceFactor = 1.0;

			if (m_subDistanceFactor * subLen > min(len1,len2))
			{
				m_subDistanceFactor = 0.5;
				if (m_subDistanceFactor * subLen > min(len1,len2))
				{
					node->setType(MIX_NODE);
					return;
				}
			}
		}


		//使用参数裁剪连接的管段，将裁剪出的新点赋予管点
		CVector3 newPoint1,newPoint2;

		bool result1 = line1->substractFromNode(node->getGID(),m_subDistanceFactor,newPoint1);
		bool result2 = line2->substractFromNode(node->getGID(),m_subDistanceFactor,newPoint2);
		double new_len1 = (newPoint1 - node->position()).length();
		double new_len2 = (newPoint2 - node->position()).length();

		static double rf = 101.0f;
		if (!result1 || !result2/* || new_len1 > rf || new_len2 > rf || new_len1 > new_len2 * rf || new_len2 > new_len1 * rf*/)
		{
			//should reset the sub line
			node->setType(MIX_NODE);
		}
		else
		{
			if (
				(line1->getStID() == nodeGID && line1->isFlowNegative())||
				(line1->getEdID() == nodeGID && !line1->isFlowNegative())||
				(line2->getStID() == nodeGID && !line2->isFlowNegative())||
				(line2->getEdID() == nodeGID && line2->isFlowNegative())
				)
			{
				node->addAroundPoint(newPoint1,line1->getSection());
				node->addAroundPoint(newPoint2,line2->getSection());
			}
			else
			{
				node->addAroundPoint(newPoint2,line2->getSection());
				node->addAroundPoint(newPoint1,line1->getSection());
			}
		}
	}

	inline void oper3(PipeNode* node)
	{
		double m_subDistanceFactor = 1.5f;

		int connective = node->getConnective();

		LinePtrList linkLines = node->getLinkLines();

		if (linkLines.size() < 2)
		{
			node->setType(MIX_NODE);
			return;
		}
		
		PipeLine* line1 = linkLines[0];
		PipeLine* line2 = linkLines[1];
		PipeLine* line3 = linkLines[2];

		Vector3List line1Pt = line1->getTwoPointFromNode(node->getGID());
		Vector3List line2Pt = line2->getTwoPointFromNode(node->getGID());
		Vector3List line3Pt = line3->getTwoPointFromNode(node->getGID());

		CVector3 dir1 = (line1Pt[1] - line1Pt[0]);
		CVector3 dir2 = (line2Pt[1] - line2Pt[0]);
		CVector3 dir3 = (line3Pt[1] - line3Pt[0]);

		double len1 = dir1.length();
		double len2 = dir2.length();
		double len3 = dir3.length();

		double r1 = linkLines[0]->getSection().getRadius();
		double r2 = linkLines[1]->getSection().getRadius();
		double r3 = linkLines[2]->getSection().getRadius();

		dir1.normalise();
		dir2.normalise();
		dir3.normalise();

		PipeSection minSect,maxSect;
		ModelUtility::getMinMaxSection(linkLines,minSect,maxSect);

		CRadian angle1 = dir1.angleBetween(dir2);
		CRadian angle2 = dir1.angleBetween(dir3);
		CRadian angle3 = dir2.angleBetween(dir3);

		static CRadian inline_angle_factor = CRadian(CDegree(160.0f));
		static CRadian right_angle_factor = CRadian(CDegree(90.0f));

		if (angle1 > inline_angle_factor || angle2 > inline_angle_factor || angle3 > inline_angle_factor)
		{
			if (maxSect.getType() == RECT_SECTION)
			{
				m_subDistanceFactor *= 0.5;
			}
		}
		else if (Math::RealEqual(angle1.valueAngleUnits(),right_angle_factor.valueAngleUnits(),0.1f)
			&& Math::RealEqual(angle2.valueAngleUnits(),right_angle_factor.valueAngleUnits(),0.1f)
			&& Math::RealEqual(angle3.valueAngleUnits(),right_angle_factor.valueAngleUnits()),0.1f)
		{
			m_subDistanceFactor = 1.0f;
		}


		double subLen1 = r1 * m_subDistanceFactor;
		double subLen2 = r2 * m_subDistanceFactor;
		double subLen3 = r3 * m_subDistanceFactor;

		if (subLen1 > len1 || subLen2 > len2 || subLen3 > len3)
		{
			node->setType(MIX_NODE);
			return;
		}

		CVector3 newPoint1,newPoint2,newPoint3;

		bool result1 = line1->substractFromNode(node->getGID(),m_subDistanceFactor,newPoint1);
		bool result2 = line2->substractFromNode(node->getGID(),m_subDistanceFactor,newPoint2);
		bool result3 = line3->substractFromNode(node->getGID(),m_subDistanceFactor,newPoint3);

		if (result1 && result2 && result3)
		{
			//哪个是尾点，则流向为从这个管点的第一个点开始
			if (node->getGID() == line1->getEdID())
			{
				node->addAroundPoint(newPoint1,line1->getSection());
				node->addAroundPoint(newPoint2,line2->getSection());
				node->addAroundPoint(newPoint3,line3->getSection());
			}
			else if (node->getGID() == line2->getEdID())
			{
				node->addAroundPoint(newPoint2,line2->getSection());
				node->addAroundPoint(newPoint1,line1->getSection());
				node->addAroundPoint(newPoint3,line3->getSection());
			}
			else if (node->getGID() == line3->getEdID())
			{
				node->addAroundPoint(newPoint3,line3->getSection());
				node->addAroundPoint(newPoint1,line1->getSection());
				node->addAroundPoint(newPoint2,line2->getSection());
			}
			else
			{
				node->addAroundPoint(newPoint1,line1->getSection());
				node->addAroundPoint(newPoint2,line2->getSection());
				node->addAroundPoint(newPoint3,line3->getSection());
			}
		}
		else
		{
			node->setType(MIX_NODE);

			//should reset the sub line ...
		}
	}

	inline void oper_mix2(PipeNode* node)
	{
		LinePtrList linkLines = node->getLinkLines();
		PipeLine* line1 = linkLines[0];
		PipeLine* line2 = linkLines[1];

		Vector3List linePt1 = line1->getTwoPointFromNode(node->getGID());
		Vector3List linePt2 = line2->getTwoPointFromNode(node->getGID());

		CVector3 dir1 = (linePt1[1] - linePt1[0]).normalisedCopy();
		CVector3 dir2 = (linePt2[1] - linePt2[0]).normalisedCopy();

		PipeSection sect1 = line1->getSection();
		PipeSection sect2 = line2->getSection();

		PipeSection minSect = sect1 < sect2 ? sect1 : sect2;

		CVector3 lpoint = linePt1[0] + dir1 * minSect.getRadius() * 0.5;
		CVector3 rpoint = linePt2[0] + dir2 * minSect.getRadius() * 0.5;

		node->addAroundPoint(lpoint,minSect);
		node->addAroundPoint(rpoint,minSect);
	}

	inline void oper_mix3(PipeNode* node)
	{
		LinePtrList& linkLines = node->getLinkLines();
		PipeLine* line1 = linkLines[0];
		PipeLine* line2 = linkLines[1];
		PipeLine* line3 = linkLines[2];

		Vector3List linePt1 = line1->getTwoPointFromNode(node->getGID());
		Vector3List linePt2 = line2->getTwoPointFromNode(node->getGID());
		Vector3List linePt3 = line3->getTwoPointFromNode(node->getGID());

		CVector3 dir1 = (linePt1[0] - linePt1[1]).normalisedCopy();
		CVector3 dir2 = (linePt2[0] - linePt2[1]).normalisedCopy();
		CVector3 dir3 = (linePt3[0] - linePt3[1]).normalisedCopy();

		PipeSection sect1 = line1->getSection();
		PipeSection sect2 = line2->getSection();
		PipeSection sect3 = line3->getSection();

		double r1 = sect1.getRadius();
		double r2 = sect2.getRadius();
		double r3 = sect3.getRadius();

		CRadian ra1 = dir1.angleBetween(dir2);
		CRadian ra2 = dir1.angleBetween(dir3);
		CRadian ra3 = dir2.angleBetween(dir3);

		//如果接近直线角度的2个管段截面相同，则是一个弯头，原来的管段则不裁减，
		//径直插入该弯头，所以只裁剪弯头连接的两端即可
		if (ra1 > ra2 && ra1 > ra3)
		{
			if(sect1 == sect2)
			{
				node->setType(sect1.getType() == RECT_SECTION ? RECT_2_NORMAL_NODE : CIRCULAR_2_NORMAL_NODE);
				oper2(node);
			}
		}
		else if (ra2 > ra1 && ra2 > ra3)
		{
			if(sect1 == sect3)
			{
				node->setType(sect1.getType() == RECT_SECTION ? RECT_2_NORMAL_NODE : CIRCULAR_2_NORMAL_NODE);
				linkLines[1] = line3;
				linkLines[2] = line2;
				oper2(node);
			}
		}
		else if (ra3 > ra1 && ra3 > ra2)
		{
			if(sect2 == sect3)
			{
				node->setType(sect2.getType() == RECT_SECTION ? RECT_2_NORMAL_NODE : CIRCULAR_2_NORMAL_NODE);
				linkLines[0] = line3;
				linkLines[2] = line1;
				oper2(node);
			}
		}
	}

	inline void oper_mixn(PipeNode* node)
	{
		LinePtrList linkLines = node->getLinkLines();

		PipeSection minSect,maxSect;
		ModelUtility::getMinMaxSection(linkLines,minSect,maxSect);
		
		int lineNum = (int)linkLines.size();

		if (lineNum < 1)
		{
			return;
		}

		PipeLine* pLine1 = NULL;
		PipeLine* pLine2 = NULL;

		for (int i=0;i<lineNum;i++)
		{
			pLine1 = linkLines[i];

 			if (pLine1->getSection().getType() != maxSect.getType())
 			{
 				continue;
 			}

			for (int j=i+1;j<lineNum;j++)
			{
				PipeLine* line = linkLines[j];

				PipeSection sect = line->getSection();

				if (sect == pLine1->getSection())
				{
					pLine2 = line;
					break;
				}
			}

			if (pLine1 !=NULL && pLine2 != NULL)
			{
				break;
			}
		}

		if (pLine1 !=NULL && pLine2 != NULL)
		{
			node->getLinkLines().clear();
			node->getLinkLines().push_back(pLine1);
			node->getLinkLines().push_back(pLine2);

			oper2(node);

			node->setType(pLine1->getSection().getType() == RECT_SECTION ? RECT_2_NORMAL_NODE : CIRCULAR_2_NORMAL_NODE);
		}
	}

	inline void LineTable::doSubtract()
	{
		double m_subDistanceFactor = 1.5;

		NodeMapIterator nodit = mOwner->getNodeTable().getIterator();
		while(nodit.hasMoreElements())
		{
			PipeNode* node = nodit.getNext();

			long gid = node->getGID();

			NodeType nodeType = node->getType();

			switch(nodeType)
			{
			case GLOBAL_MODEL:
				{
				}
				break;
			case CIRCULAR_1_NODE:
			case RECT_1_NODE:
				{
					//端点，使用小距离的管段建模
					oper1(node);
				}
				break;
			case RECT_2_NORMAL_NODE:
			case CIRCULAR_2_NORMAL_NODE:
				{
					//弯头，使用插值建模
					oper2(node);
				}
				break;
			case CIRCULAR_3_NORMAL_NODE:
				{
					//圆管3通，使用双通弯头与一段管段组合
					oper3(node);
				}
				break;
			case MIX_NODE_2:
				{
					//一边连接圆截面、一边连接方截面，则使用截面最大的截面建双通弯头
					oper_mix2(node);
				}
				break;
			case MIX_NODE_3:
				{
					//连接度为三的混合连接截面，先判断能不能用最大的截面双通建模，不行则建井
					oper_mix3(node);
				}
				break;
			case MIX_NODE:
				{
					//先判断能不能用最大的截面双通建模，不行则建井
					oper_mixn(node);
				}
				break;
			default:
				break;
			}
		}
	}

	inline void LineTable::doJS_Strategy()
	{
		LineMapIterator lineIt = getIterator();
		while(lineIt.hasMoreElements())
		{
			PipeLine* line = lineIt.getNext();

			PipeNode* stNode = line->getStNode();
			PipeNode* edNode = line->getEdNode();

			PipeNode* xfsNode = NULL;

			if (isFirePlug(stNode) || isCollectorBox(stNode))
			{
				xfsNode = stNode;
			}
			else if(isFirePlug(edNode) || isCollectorBox(edNode))
			{
				xfsNode = edNode;
			}

			if (xfsNode!=NULL)
			{
				double distance = xfsNode->getInfo().getGroundZ() - xfsNode->position().z;

				Vector3List twoPoint = line->getTwoPointFromNode(xfsNode->getGID());
				CVector3 dir = twoPoint[1] - twoPoint[0];
				double len = dir.length();
				dir.normalise();

				double inter = line->getSection().getRadius() * 1.5;

				if (distance > inter && len > inter)
				{
					CVector3 newPoint;
					line->substractFromNode(xfsNode->getGID(),1.5,newPoint);

					Vector3List list;
					list.resize(3);
					list[0] = newPoint;
					list[1] = xfsNode->position();
					list[2] = xfsNode->position() + CVector3::UNIT_Z * inter;

					line->getExtraLine().push_back(list);

					Vector3List list2;
					list2.resize(2);
					list2[0] = xfsNode->position() + CVector3::UNIT_Z * inter;
					list2[1] = xfsNode->position() + CVector3::UNIT_Z * distance;
					line->getExtraLine().push_back(list2);

				}
				else
				{
					CVector3 dir = line->getDirectionFromNode(xfsNode->getGID());
					Vector3List list;
					list.resize(2);
					list[0] = xfsNode->position();
					list[1] = xfsNode->position() + CVector3::UNIT_Z * distance;
					line->getExtraLine().push_back(list);
				}
			}
		}
	}

	inline void LineTable::doGD_Strategy()
	{
		{
			LineMapIterator lineIt = getIterator();
			while(lineIt.hasMoreElements())
			{
				PipeLine* line = lineIt.getNext();

				PipeNode* stNode = line->getStNode();
				PipeNode* edNode = line->getEdNode();

				if (stNode->getType() == GLOBAL_MODEL && edNode->getType() == GLOBAL_MODEL)
				{
					if (StringUtil::endsWith(stNode->getFeatureUrl(),"电杆.osgb")
						&&StringUtil::endsWith(edNode->getFeatureUrl(),"电杆.osgb"))
					{
						stNode->getInfo().setConstDirection(line->getDirectionFromNode(stNode->getGID()));
						edNode->getInfo().setConstDirection(line->getDirectionFromNode(edNode->getGID()));
					}
				}
			}
		}

		LineMapIterator lineIt = getIterator();
		while(lineIt.hasMoreElements())
		{
			PipeLine* line = lineIt.getNext();

			if (line->getState() == STATE_ERROR)
			{
				continue;
			}

			PipeNode* stNode = line->getStNode();
			PipeNode* edNode = line->getEdNode();

			if (stNode->getType() == GLOBAL_MODEL && edNode->getType() == GLOBAL_MODEL)
			{
				if (StringUtil::endsWith(stNode->getFeatureUrl(),"电杆.osgb")
					&&StringUtil::endsWith(edNode->getFeatureUrl(),"电杆.osgb") 
					&& stNode->getInfo().getGroundZ() < stNode->position().z
					&& edNode->getInfo().getGroundZ() < edNode->position().z)
				{
					CVector3 stDir = stNode->getInfo().getConstDirection();
					CVector3 edDir = edNode->getInfo().getConstDirection();

					if (edDir.angleBetween(stDir) > CRadian(Math::HALF_PI))
					{
						edDir *= -1;
					}

					if (stDir != CVector3::ZERO)
					{
						Vector3List points = line->getLinPoint();
						Vector3List points1;
						Vector3List points2;

						points1.clear();
						points2.clear();

						CVector3 stOffsetX = stDir.crossProduct(CVector3::UNIT_Z).normalisedCopy();
						CVector3 edOffsetX = edDir.crossProduct(CVector3::UNIT_Z).normalisedCopy();

						if (points.size() == 2)
						{
							points1.push_back(points[0] + stOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);
							points2.push_back(points[0] - stOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);

							points1.push_back(points[1] + edOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);
							points2.push_back(points[1] - edOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);
						}
						else
						{
							for (int i=0;i<points.size();i++)
							{
								points1.push_back(points[i] + stOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);
								points2.push_back(points[i] - stOffsetX * 1.0 + CVector3::UNIT_Z * 0.2);
							}
						}

						line->getExtraLine().push_back(points1);
						line->getExtraLine().push_back(points2);
					}
				}
			}
		}
	}

	void LineTable::removeInvalidData()
	{
		LineMap::iterator it = mLineList.begin();

		while (it!=mLineList.end())
		{
			PipeLine* line = it->second;

			if (line->getState() == STATE_ERROR
				|| line->getSection().getType() == UNKNOWM_SECTION)
			{
				if (PipeNode* stNode = line->getStNode())
				{
					stNode->removeLinkLine(line);
				}

				if (PipeNode* edNode = line->getEdNode())
				{
					edNode->removeLinkLine(line);
				}

				it = mLineList.erase(it);
				continue;
			}

			it++;
		}
	}

	bool LineTable::checkData()
	{
		LineMapIterator it = getIterator();

		while (it.hasMoreElements())
		{
			PipeLine* line = it.getNext();
#if _DEBUG
			long gid = line->getGID();
#endif
			doFeatureInfo(line);
			doFixLineZ(line);
			doMaterialInfo(line);

			CVector3 center = line->getCenter();
			mExtent.merge(CVector2(center.x, center.y));
		}

		doSubtract();

		if (mOwner->getConfig()->getBoolValue(TableConfig::NameHasDianGanNode))
		{
			doGD_Strategy();
		}

		if(mOwner->getConfig()->getBoolValue(TableConfig::NameHasXiaoFangShuanNode))
		{
			doJS_Strategy();
		}
		return true;
	}

}}
