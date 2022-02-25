#include "PipeModelFactory.h"
#include "OCModel/PipeMath.h"
#include "OCModel/ModelUtility.h"
#include "PipeTable.h"

namespace OC
{
	namespace Modeling
	{
		inline IPipeDataList createJS_lineEx(PipeLine* line);
		inline IPipeDataList createGD_lineEx(PipeLine* line);

		bool PipeModelFactory::build(IModelData* data)
		{
			bool result = false;
			if (data->asPipeLine())
			{
 				result = createLine(data->asPipeLine());
			}
			else if (data->asPipeNode())
			{
				result = createNode(data->asPipeNode());
			}
		
			if (result || data->isGroupModelData())
			{
				result = data->build();
			}

			return result;
		}

		//Create Line
		bool PipeModelFactory::createLine(PipeLine* pLine)
		{
			long gid = pLine->getGID();

			osg::ref_ptr<osg::Node> node;

			PipeSection& lineSect = pLine->getSection();
			Vector3List linePoints = pLine->getLinPoint();

			if (linePoints.size() < 2)
			{
				return false;
			}

			if (pLine->isFlowNegative())
			{
				reverse(linePoints.begin(),linePoints.end());
			}

			if (lineSect.getType() == UNKNOWM_SECTION
				|| pLine->getState() == STATE_ERROR)
			{
				return false;
			}

			String ext = pLine->getOwnerTable()->getConfig()->getStringValue(TableConfig::NameExtension);

			String WS_Image;
			if (ext == "osgjs")
			{
				WS_Image = "global/images/ws_wall.jpg";
			}
			else
			{
				WS_Image = "global://images/ws_wall.jpg";
			}

			switch(lineSect.getType())
			{
			case CIRCULAR_SECTION:
				{
					if (pLine->getOwnerTable()->getConfig()->getBoolValue(TableConfig::NameHasDianGanNode))
					{
						pLine->getDataList() = createGD_lineEx(pLine);
					}
					else if (pLine->getOwnerTable()->getConfig()->getBoolValue(TableConfig::NameHasXiaoFangShuanNode))
					{
						pLine->getDataList() = createJS_lineEx(pLine);
					}
					else
					{
						if(linePoints.size() == 2)
						{
							CCirclePipeLine* pipe = new CCirclePipeLine;	
							pipe->setRefNormal(pLine->getWorldZVector());
							pipe->setStPoint(PipePoint(linePoints[0],lineSect));
							pipe->setEdPoint(PipePoint(linePoints[1],lineSect));
							pLine->getDataList().push_back(pipe);
						}
					}
				}
				break;
			case RECT_SECTION:
				{
					CVector3 up = pLine->getWorldZVector();
					CVector3 down = -up;

					CVector3 normal = up;
					CVector3 lineDir = linePoints[1] - linePoints[0];
					lineDir.normalise();
#if _DEBUG
					CRadian ra1 = lineDir.angleBetween(CVector3::UNIT_Z);
					CRadian ra2 = lineDir.angleBetween(CVector3::NEGATIVE_UNIT_Z);
#endif
					CRadian t = CRadian(CDegree(5.0));
					if (lineDir.directionEquals(up,t)
						||lineDir.directionEquals(down,t))
					{
						PipeNode* stNode = pLine->getStNode();
						PipeNode* edNode = pLine->getEdNode();

						Vector3List pNodePoints1 = stNode->getAroundPoint();
						Vector3List pNodePoints2 = edNode->getAroundPoint();

						if (pNodePoints1.size() == 2)
						{
							CVector3 dir1 = pNodePoints1[0] - stNode->position();
							CVector3 dir2 = pNodePoints1[1] - stNode->position();
							CVector3 nodePlaneNormal = dir1.crossProduct(dir2);
							normal = nodePlaneNormal.crossProduct(lineDir);
						}
						else if (pNodePoints2.size() == 2)
						{
							CVector3 dir1 = pNodePoints2[0] - edNode->position();
							CVector3 dir2 = pNodePoints2[1] - edNode->position();
							CVector3 nodePlaneNormal = dir1.crossProduct(dir2);
							normal = nodePlaneNormal.crossProduct(lineDir);
						}
						else
						{
							normal = CVector3::UNIT_Y;
						}

					}

					bool isClose = pLine->getStNode()->getType() != RECT_2_NORMAL_NODE
						|| pLine->getEdNode()->getType() != RECT_2_NORMAL_NODE;
					
					CRectPipeLine* pipe = new CRectPipeLine;
					pipe->setRefNormal(normal);
					pipe->setStPoint(PipePoint(linePoints[0],lineSect));
					pipe->setEdPoint(PipePoint(linePoints[1],lineSect));
					pipe->setClose(isClose);
					if (pLine->getOwnerTable()->getConfig()->getBoolValue(TableConfig::NameUseWSTexture))
					{
						pipe->setTextureImage(WS_Image);
					}
					pLine->getDataList().push_back(pipe);
				}
				break;
			case UNKNOWM_SECTION:
			default:
				{

				}
				break;
			}
 
			return true;
		}

		bool PipeModelFactory::createNode(PipeNode* pNode)
		{
			long nodeGID = pNode->getGID();

			osg::ref_ptr<osg::Node> node;

			CVector3 pNodePosition = pNode->position();

			PipePointList nodeAroundList = pNode->getAroundPointList();
			
			LinePtrList linkLines = pNode->getLinkLines();

			bool buildTurber = pNode->getOwnerTable()->getConfig()->getBoolValue(TableConfig::NameTuber);

			bool useWSTexture = pNode->getOwnerTable()->getConfig()->getBoolValue(TableConfig::NameUseWSTexture);

			String ext = pNode->getOwnerTable()->getConfig()->getStringValue(TableConfig::NameExtension);

			String WS_Image;
			if (ext == "osgjs")
			{
				WS_Image = "global/images/ws_wall.jpg";
			}
			else
			{
				WS_Image = "global://images/ws_wall.jpg";
			}

			switch(pNode->getType())
			{
			case MIX_NODE_2:
				{
					if (nodeAroundList[0].Section.getType() == RECT_SECTION)
					{
						CRectPipeNode2* pipe = new CRectPipeNode2;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setTurber(false);
						pipe->setPoints(pNodePosition, nodeAroundList[0], nodeAroundList[1]);
						pNode->getDataList().push_back(pipe);
					}
					else
					{
						CCirclePipeNode2* pipe = new CCirclePipeNode2;
						pipe->setPoints(pNodePosition,nodeAroundList[0],nodeAroundList[1]);
						pipe->setTurber(buildTurber);
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case CIRCULAR_1_NODE:
				{
					if (nodeAroundList.size() == 1)
					{
						CCirclePipeNode1* pipe = new CCirclePipeNode1;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setPoints(PipePoint(pNodePosition, linkLines[0]->getSection()), nodeAroundList[0].Position - pNodePosition);
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case CIRCULAR_2_NORMAL_NODE:
				{
					if (nodeAroundList.size() == 2)
					{
						CCirclePipeNode2* pipe = new CCirclePipeNode2;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setPoints(pNodePosition, nodeAroundList[0], nodeAroundList[1]);
						pipe->setTurber(buildTurber);
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case CIRCULAR_3_NORMAL_NODE:
				{
					if (nodeAroundList.size() == 3)
					{
						CCirclePipeNode3* pipe = new CCirclePipeNode3;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setPoints(pNodePosition, nodeAroundList[0], nodeAroundList[1], nodeAroundList[2]);
						pipe->setTurber(buildTurber);
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case RECT_1_NODE:
				{
					if (nodeAroundList.size() == 1)
					{
						CRectPipeNode1* pipe = new CRectPipeNode1;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setPoints(PipePoint(pNodePosition, linkLines[0]->getSection()), nodeAroundList[0].Position - pNodePosition);
						if (useWSTexture)
						{
							pipe->setTextureImage(WS_Image);
						}
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case RECT_2_NORMAL_NODE:
				{
					if (nodeAroundList.size() == 2)
					{
						CRectPipeNode2* pipe = new CRectPipeNode2;
						pipe->setRefNormal(pNode->getWorldZVector());
						pipe->setPoints(pNodePosition, nodeAroundList[0], nodeAroundList[1]);
						if (useWSTexture)
						{
							pipe->setTextureImage(WS_Image);
						}
						pNode->getDataList().push_back(pipe);
					}
				}
				break;
			case RECT_3_NORMAL_NODE:
			case MIX_NODE:
			case MIX_NODE_3:
			case INSPECTION_WELL_NODE:
				{
					{
						if (ext == "b3dm" || ext == "gltf" || ext == "glb")
						{
							return false;
						}
					}
					PipeSection maxSect,minSect;
					ModelUtility::getMinMaxSection(pNode->getLinkLines(),minSect,maxSect);
					double maxHeight = maxSect.getHeight();
					double r = (maxSect.getType() == RECT_SECTION ? maxSect.getWidth() * 0.5f : maxSect.getRadius());
					CVector3 offset = pNode->getOffset();
					CVector3 pWorldPosition = offset + pNodePosition;

					if (pWorldPosition.z < pNode->getInfo().getGroundZ())
					{
						CPipeWell1* pipe = new CPipeWell1;
						offset.z = pNode->getInfo().getGroundZ();
						pNode->setOffset(offset);
						double h = maxHeight + (pNode->getInfo().getGroundZ() - pWorldPosition.z);
						pipe->mRoomD = h;
						pipe->mRoomR = r * 1.1f;
						if (pipe->mHatR >= pipe->mRoomR)
						{
							pipe->mHatR = pipe->mRoomR - 0.1;
						}
						pNode->getDataList().push_back(pipe);
					}
					else
					{
						if (pNode->getConnective() > 1)
						{
							CPipeCylinder* pipe = new CPipeCylinder;
							double up = pNodePosition.z + maxHeight * 0.5;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
							double down = pNodePosition.z - maxHeight * 0.5;

							pipe->setRadius(r * 1.1);
							pipe->setUpOffset(up);
							pipe->setDownOffset(down);
							pipe->setPosition(pNodePosition);

							pipe->setHeight(maxHeight + 0.1);

							pNode->getDataList().push_back(pipe);
						}
						//miss something
					}
				}
				break;
			case GLOBAL_MODEL:
				{
					CGlobalNode* pipe = new CGlobalNode;

					pipe->calculateRotation(pNode);
					pipe->setFeatureUrl(pNode->getFeatureUrl());
					pNode->getDataList().push_back(pipe);
				}
				break;
			default:
				break;
			}

			return true;
		}



		inline IPipeDataList createJS_lineEx(PipeLine* line)
		{
			IPipeDataList pPipeList;

			std::vector<Vector3List>& extraLines = line->getExtraLine();
			int num = (int)extraLines.size();
			if (num > 0)
			{
				PipeSection section = line->getSection();

				osg::ref_ptr<osg::Group> root = new osg::Group;
				for(int i=0;i<num;i++)
				{
					Vector3List list = extraLines[i];
					if (list.size() == 3)
					{	
						CCirclePipeNode2* pipe = new CCirclePipeNode2;
						pipe->setRefNormal(line->getWorldZVector());
						pipe->setPoints(list[1], PipePoint(list[0], section), PipePoint(list[2], section));
						pipe->setTurber(true);
						pPipeList.push_back(pipe);
					}
					else if (list.size() == 2)
					{
						CCirclePipeLine* pipe = new CCirclePipeLine;
						pipe->setRefNormal(line->getWorldZVector());
						pipe->setStPoint(PipePoint(list[0], section));
						pipe->setEdPoint(PipePoint(list[1],section));
						pPipeList.push_back(pipe);
					}
				}

				CCirclePipeLine* pipe = new CCirclePipeLine;
				pipe->setRefNormal(line->getWorldZVector());
				pipe->setStPoint(PipePoint(line->getLinPoint()[0], section));
				pipe->setEdPoint(PipePoint(line->getLinPoint()[1],section));
				pPipeList.push_back(pipe);
			}
			else
			{
				CCirclePipeLine* pipe = new CCirclePipeLine;
				pipe->setRefNormal(line->getWorldZVector());
				pipe->setStPoint(PipePoint(line->getLinPoint()[0], line->getSection()));
				pipe->setEdPoint(PipePoint(line->getLinPoint()[1],line->getSection()));
				pPipeList.push_back(pipe);
			}

			return pPipeList;
		}

		inline IPipeDataList createGD_lineEx(PipeLine* line)
		{
			IPipeDataList list;

			std::vector<Vector3List>& extraLines = line->getExtraLine();
			int num = (int)extraLines.size();


			if (num > 0)
			{
				PipeSection defaultSection(0.01);
				//defaultSection = line->getSection();

				osg::ref_ptr<osg::Group> root = new osg::Group;
				for (int i=0;i<num;i++)
				{
					CVector3 stPoint = extraLines[i][0];
					CVector3 edPoint = extraLines[i][1];
					CVector3 center = (stPoint + edPoint) / 2.0f;

					double len = (stPoint - edPoint).length();
					double m = 1;

					if (len < 15)
					{
						m = 0.5;
					}
					else if (len < 50)
					{
						m = 1.0;
					}
					else if (len > 50)
					{
						m = 3;
					}
					center.z -= m;

					CCirclePipeNode2* pipe = new CCirclePipeNode2;
					pipe->setRefNormal(line->getWorldZVector());
					pipe->setPoints(center, PipePoint(stPoint, defaultSection), PipePoint(edPoint, defaultSection));
					list.push_back(pipe);
				}

				CVector3 stPoint = line->getLinPoint()[0];
				CVector3 edPoint = line->getLinPoint()[1];
				CVector3 center = (stPoint + edPoint) * 0.5f;
				double len = (stPoint - edPoint).length();

				double m = 1;
				if (len < 15)
				{
					m = 0.5;
				}
				else if (len < 50)
				{
					m = 1.0;
				}
				else if (len > 50)
				{
					m = 3;
				}
				center.z -= m;

				CCirclePipeNode2* pipe = new CCirclePipeNode2;
				pipe->setRefNormal(line->getWorldZVector());
				pipe->setPoints(center, PipePoint(stPoint, defaultSection), PipePoint(edPoint, defaultSection));
				list.push_back(pipe);
			}
			else
			{
				CCirclePipeLine* pipe = new CCirclePipeLine;
				pipe->setRefNormal(line->getWorldZVector());
				pipe->setStPoint(PipePoint(line->getLinPoint()[0], line->getSection()));
				pipe->setEdPoint(PipePoint(line->getLinPoint()[1],line->getSection()));
				pipe->setClose(true);
				list.push_back(pipe);
			}

			return list;
		}

	}
}
