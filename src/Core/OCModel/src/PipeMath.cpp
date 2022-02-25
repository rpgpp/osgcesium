#include "PipeMath.h"
#include "PipeData.h"

namespace OC
{
	namespace Modeling
	{
		CPipeMath::CPipeMath(void)
		{
		}

		CPipeMath::~CPipeMath(void)
		{
		}

		osg::Node* CPipeMath::buildCirclePipe(CVector3 stPoint,CVector3 edPoint,double dr1,double dr2,bool close)
		{
			CCirclePipeLine pipe;
			pipe.setStPoint(PipePoint(stPoint,PipeSection(dr1)));
			pipe.setEdPoint(PipePoint(edPoint,PipeSection(dr2)));
			pipe.setClose(close);
			return pipe.buildGeode();
		}

		osg::Node* CPipeMath::buildCirclePipe(CVector3 stPoint,CVector3 edPoint,double dr,bool close)
		{
			return buildCirclePipe(stPoint,edPoint,dr,dr,close);
		}

		osg::Node* CPipeMath::buildRectConnec2(CVector3 left,CVector3 center,CVector3 right,PipeSection leftSect,PipeSection rightSect,bool buildTuber)
		{
			CRectPipeNode2 pipe;
			pipe.setTurber(buildTuber);
			pipe.setPoints(center,PipePoint(left,leftSect),PipePoint(right,rightSect));
			return pipe.buildGeode();
		}

		osg::Node* CPipeMath::buildCircleConnec2(CVector3 left,CVector3 center,CVector3 right,PipeSection leftSect,PipeSection rightSect,bool buildTuber)
		{
			CCirclePipeNode2 pipe;
			pipe.setPoints(center,PipePoint(left,leftSect),PipePoint(right,rightSect));
			pipe.setTurber(buildTuber);
			return pipe.buildGeode();
		}

		CVector3 CPipeMath::getPointByDistance(CVector3 start,CVector3 end,double distance)
		{
			CRay ray(start,(end - start).normalisedCopy());

			return ray.getPoint(distance);
		}

		osg::Node* CPipeMath::buildCircleConnec3(CVector3 center,Vector3List list,doubleVector drList,bool tuber)
		{
			if (list.size() == 3 && drList.size() == 3)
			{
				CCirclePipeNode3 pipe;
				PipePoint p1 = PipePoint(list[0],PipeSection(drList[0]));
				PipePoint p2 = PipePoint(list[1],PipeSection(drList[1]));
				PipePoint p3 = PipePoint(list[2],PipeSection(drList[2]));
				pipe.setPoints(center,p1,p2,p3);
				pipe.setClose(tuber);
				return pipe.buildGeode();
			}
 			
			return NULL;
		}

		osg::Node* CPipeMath::buildCircleConnec1(CVector3 center,CVector3 right,PipeSection sect)
		{
			CCirclePipeNode1 pipe;
			pipe.setPoints(PipePoint(center,sect),right - center);
			return pipe.buildGeode();
		}

		osg::Node* CPipeMath::buildRectConnec1(CVector3 center,CVector3 right,PipeSection sect)
		{
			CRectPipeNode1 pipe;
			pipe.setPoints(PipePoint(center,sect),right - center);
			return pipe.buildGeode();
		}

		osg::Node* CPipeMath::buildCirclePipe(CVector3 stPoint,CVector3 edPoint,PipeSection sect,bool colse)
		{
			return buildCirclePipe(stPoint,edPoint,sect.getRadius());
		}

		osg::Node* CPipeMath::buildRectPipe(Vector3List v3list,PipeSection sect,CVector3 normal,bool close)
		{
			CRectPipeLine pipe;
			pipe.setRefNormal(normal);
			pipe.setStPoint(PipePoint(v3list[0],sect));
			pipe.setEdPoint(PipePoint(v3list[1],sect));
			pipe.setClose(close);
			return pipe.buildGeode();
		}
		
	}
}

