#include "PipeLine.h"
#include "PipeMath.h"
#include "ModelUtility.h"
#include "TableConfig.h"

namespace OC
{
	namespace Modeling
	{
		String PipeLine::NameID = "ID";
		String PipeLine::NameSect = "管径";
		String PipeLine::NameDirection = "流向";
		String PipeLine::NameStnodInter = "起点埋深";
		String PipeLine::NameEdnodInter = "终点埋深";
		String PipeLine::NameStnodNo = "起点号";
		String PipeLine::NameEdnodNo = "终点号";
		String PipeLine::NameSpatialColumn = "空间字段";
		String PipeLine::NameMaterial = "管线材质";

		PipeLine::PipeLine()
			:mStInter(0.0f)
			,mEdInter(0.0f)
			,mStNodeID(-1)
			,mEdNodeID(-1)
		{
			mStNode = mEdNode = NULL;
			mIsGeographicData = false;
		}

		PipeLine::~PipeLine()
		{
			
		}

		PipeLine* PipeLine::clone()
		{
			PipeLine* line = new PipeLine;

			line->setGID(getGID());
			line->setDNO(getDNO());
			line->setTable(getTable());
			line->setFeatureName(getFeatureName());
			line->setFeatureUrl(getFeatureUrl());
			line->setStNode(getStNode());
			line->setEdNode(getEdNode());

			line->setOffset(getOffset());
			line->setScale(getScale());

			line->setSection(mSection);

			line->setStID(mStNodeID);
			line->setEdID(mEdNodeID);

			return line;
		}

		CVector3 PipeLine::getCenter()
		{
			return 0.5 * (getStartPoint() + getEndPoint());
		}

		void PipeLine::doOffset(CVector3 offset)
		{
			Vector3List& v3list = getLinPoint();
			for(int i =0 ;i<v3list.size();i++)
			{
				v3list[i] -= offset;
			}

			{
				std::vector<Vector3List>::iterator it = mExtraLine.begin();
				for (;it!=mExtraLine.end();it++)
				{
					for(int i=0;i<(int)it->size();i++)
					{
						it->at(i) -=offset;
					}
				}
			}

			setOffset(offset);
		}

		PipeLine* PipeLine::create(TemplateRecord* record)
		{
			Vector3List linepoints = ModelUtility::parseSpatialField(record->getGeometry());

			if (linepoints.size() < 2)
			{
				return NULL;
			}

			PipeLine* line = new PipeLine;
			line->setLinPoint(linepoints);
			line->setGID(record->getID());
			line->setDNO(record->getIntValue("dno"));

			
			StringMap sm =  record->getHeadInfo()->getFieldMapping();

			String stnod = sm[NameStnodNo];
			String ednod = sm[NameEdnodNo];
			
			StringUtil::toLowerCase(stnod);
			StringUtil::toLowerCase(ednod);
			if (stnod == "stnod" && ednod == "ednod")
			{
				line->setStID(record->getLongValue(NameStnodNo));
				line->setEdID(record->getLongValue(NameEdnodNo));
			}

			String sect = record->getFieldValue(NameSect);
			line->setSection(ModelUtility::parseSect(sect));
			line->setStInter(record->getDoubleValue(NameStnodInter));
			line->setEdInter(record->getDoubleValue(NameEdnodInter));
			line->setMaterial(record->getFieldValue(NameMaterial));
			line->setTime(record->getFieldValue(TableConfig::NameTimeStamp));
			line->setTemplateRecord(record);

			return line;
		}

		PipeLine* PipeLine::create(long gid,int dno,int stnod,int ennod,String geomStr,String sectStr,NameValuePairList nameValues)
		{
			osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo;

			header->add(new OCFieldInfo("gid",OC_DATATYPE_INTEGER,true));
			header->add(new OCGeomFieldInfo("geom","LINESTRING",0));
			header->add(new OCFieldInfo("dno",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("stnod",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("ednod",OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo(PipeLine::NameSect,OC_DATATYPE_TEXT));

			osg::ref_ptr<TemplateRecord> templaterecord = new TemplateRecord(header);

			templaterecord->setFieldValue("gid",StringConverter::toString(gid));
			templaterecord->setGeometry(geomStr);
			templaterecord->setFieldValue("dno",StringConverter::toString(dno));
			templaterecord->setFieldValue("stnod",StringConverter::toString(stnod));
			templaterecord->setFieldValue("ednod",StringConverter::toString(ennod));
			templaterecord->setFieldValue(PipeLine::NameSect,sectStr);

			NameValuePairList::iterator it = nameValues.begin();
			for(;it!=nameValues.end();it++)
			{
				header->add(new OCFieldInfo(it->first,OC_DATATYPE_TEXT));
				templaterecord->setFieldValue(it->first,it->second);
			}

			return create(templaterecord);
		}

		void PipeLine::setLineZ(double stElev,double edElev)
		{
			int pointNum = (int)mPointList.size();
			
			if (pointNum > 2)
			{
				double lineLengh = 0.0f;
				for(int i=0;i<pointNum-1;i++)
				{
					lineLengh += (mPointList[i+1] - mPointList[i]).length();
				}

				double sum  = 0.0f;

				for(int i=1;i<pointNum-1;i++)
				{
					CVector3 p1 = mPointList[i-1];
					CVector3 p2 = mPointList[i];
					p1.z = p2.z = 0.0f;
					double len = (p2 - p1).length();
					sum += len;
					mPointList[i].z = (sum / lineLengh) * (edElev - stElev)  + stElev;
				}
			}

			if (pointNum > 0)
			{
				mPointList.begin()->z = stElev;
				mPointList.rbegin()->z = edElev;
			}

			//double step = (edElev - stElev) / (pointNum - 1);
			//for (int i=0;i<pointNum;i++)
			//{
			//	mPointList[i].z = stElev + step * i;
			//}
		}

		bool PipeLine::isFlowNegative()
		{
			if (mTemplateRecord.valid())
			{
				String flow = mTemplateRecord->getFieldValue(NameDirection);
				if (flow.empty())
				{
					return false;
				}

				if (flow.find("顺") != String::npos || flow.find("正") != String::npos)
				{
					return false;
				}
				else if (flow.find("逆") != String::npos || flow.find("反") != String::npos)
				{
					return true;
				}
				else
				{
					return StringConverter::parseBool(flow) == false;
				}
			}

			return false;
		}

		CVector3 PipeLine::getDirectionFromNode(long nodeGID)
		{
			CVector3 direction;
			if(getStID() == nodeGID)
			{
				direction = getEndPoint() - getStartPoint();
			}
			else if(getEdID() == nodeGID)
			{
				direction = getStartPoint() - getEndPoint();
			}
			return direction.normalisedCopy();
		}

		std::vector<Vector3List>& PipeLine::getExtraLine()
		{
			return mExtraLine;
		}

		void PipeLine::setStInter(double inter)
		{
			mStInter = inter;
		}

		double PipeLine::getStInter()
		{
			return mStInter;
		}

		double PipeLine::getEdInter()
		{
			return mEdInter;
		}

		void PipeLine::setEdInter(double inter)
		{
			mEdInter = inter;
		}

		String PipeLine::getMaterial()
		{
			return m_strMaterial;
		}

		void PipeLine::setMaterial(String strMaterial)
		{
			m_strMaterial = strMaterial;
		}

		PipeNode* PipeLine::getStNode()
		{
			return mStNode;
		}

		void PipeLine::setStNode(PipeNode* refN)
		{
			mStNode = refN;
		}

		void PipeLine::setEdNode(PipeNode* refN)
		{
			mEdNode = refN;
		}

		PipeNode* PipeLine::getEdNode()
		{
			return mEdNode;
		}

		long PipeLine::getStID()
		{
			return mStNodeID;
		}

		long PipeLine::getEdID()
		{
			return mEdNodeID;
		}

		void PipeLine::setStID(long id)
		{
			mStNodeID = id;
		}

		void PipeLine::setEdID(long id)
		{
			mEdNodeID = id;
		}

		PipeLine* PipeLine::asPipeLine()
		{
			return static_cast<PipeLine*>(this);
		}

		bool PipeLine::substractFromNode(long nodeID,double factor,CVector3& newPoint)
		{
			long size = mPointList.size();

			if (size < 1)
			{
				return false;
			}

			if(nodeID == mStNodeID)
			{
				CVector3 p1 = mPointList[0];
				CVector3 p2 = mPointList[1];

				CVector3 dir = p2 - p1;
				double len = dir.length();
				dir.normalise();

				double subLen = getSection().getRadius() * factor;

				newPoint = CPipeMath::getPointByDistance(p1,p2,subLen);
				mPointList[0] = newPoint;
				
				return true;
			}
			else if(nodeID == mEdNodeID)
			{
				CVector3 p1 = mPointList[size-1];
				CVector3 p2 = mPointList[size-2];

				CVector3 dir = p2 - p1;
				double len = dir.length();
				dir.normalise();

				double subLen = getSection().getRadius() * factor;

				newPoint = CPipeMath::getPointByDistance(p1,p2,subLen);
				mPointList[size-1] = newPoint;

				return true;
			}

			return false;
		}


		Vector3List PipeLine::getTwoPointFromNode(long nodeID)
		{
			Vector3List list;
			list.resize(2);
			
			long pointSize = mPointList.size();

			if( pointSize > 1)
			{
				if (nodeID == mStNodeID)
				{
					list[0] = mPointList[0];
					list[1] = mPointList[1];
				}
				else if (nodeID == mEdNodeID)
				{
					list[0] = mPointList[pointSize-1];
					list[1] = mPointList[pointSize-2];
				}
			}

			return list;
		}

		bool PipeLine::setStartPoint(CVector3 start)
		{
			if(!mPointList.empty())
			{
				mPointList[0] = start;
				return true;
			}
			return false;
		}

		CVector3 PipeLine::getStartNextPoint()
		{
			if(mPointList.size() >= 2)
			{
				return mPointList[1];
			}
			else
				return CVector3::ZERO;
		}


		CVector3 PipeLine::getEndNextPoint()
		{
			int size = mPointList.size();
			if(size >= 2)
			{
				return mPointList[size - 2];
			}
			else
				return CVector3::ZERO;
		}

		CVector3 PipeLine::getStartPoint()
		{
			if(!mPointList.empty())
			{
				return mPointList[0];
			}
			else
				return CVector3::ZERO;
		}

		bool PipeLine::setEndPoint(CVector3 start)
		{
			if(!mPointList.empty())
			{
				int size = mPointList.size();
				mPointList[size - 1] = start;
				return true;
			}
			return false;
		}

		CVector3 PipeLine::getEndPoint()
		{
			if(!mPointList.empty())
			{
				int size = mPointList.size();
				return mPointList[size - 1];
			}
			return CVector3::ZERO;
		}

		CVector3 PipeLine::getPointByID(long gid)
		{
			int size = mPointList.size();
			if(mStNodeID == gid)
			{
				return mPointList[0];
			}
			else if(mEdNodeID == gid)
			{
				return mPointList[size - 1];
			}
			else
				return CVector3::ZERO;
		}

		CVector3 PipeLine::getPointNextById(long gid)
		{
			int size = mPointList.size();
			if(mStNodeID == gid)
			{
				return mPointList[1];

			}
			else if(mEdNodeID == gid)
			{
				return mPointList[size-2];
			}
			else
				return CVector3::ZERO;
		}

		void PipeLine::setSection(PipeSection sect)
		{
			mSection = sect;
		}

		PipeSection& PipeLine::getSection()
		{
			return mSection;
		}

		LinePtrList PipeLine::getLinkLines()
		{
			LinePtrList outList;

			if(mStNode)
			{
				LinePtrList list = mStNode->getLinkLines();
				outList.insert(outList.begin(),list.begin(),list.end());
			}

			if (mEdNode)
			{
				LinePtrList list = mEdNode->getLinkLines();
				outList.insert(outList.begin(),list.begin(),list.end());
			}

			return outList;
		}

		bool PipeLine::fixSectNull2Near()
		{
			if(getSection().getType() == UNKNOWM_SECTION)
			{
				LinePtrList linVertor = getLinkLines();

				double dr = 1e+6;

				PipeLine* bestline = NULL;

				LinePtrList::iterator it = linVertor.begin();
				for (;it!=linVertor.end();it++)
				{
					PipeLine* line = *it;

					if (this == line)
					{
						continue;
					}

					PipeSection section = line->getSection();
					if (UNKNOWM_SECTION == section.getType())
					{
						continue;
					}
					double radius = section.getRadius();

					if (radius < dr && radius > 0.0f)
					{
						bestline = line;
						dr = radius;
					}
				}

				if (bestline != NULL)
				{
					setSection(bestline->getSection());
					StringStream sstream;
					sstream<<"["<<getTable()<<"]"<<"ID号为["<<getGID()<<"]的管段管径为空,使用ID["
						<<bestline->getGID() << "]的管径赋值:"<<bestline->getSection().getRadius();
					LOG<<sstream.str();
				}
			}

			return UNKNOWM_SECTION != getSection().getType();
		}

	}
}
