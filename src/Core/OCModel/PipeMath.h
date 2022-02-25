#ifndef _MODEL_MATH_H_
#define _MODEL_MATH_H_

#include "OCMain/osg.h"
#include "OCUtility/Ray.h"
#include "OCUtility/Matrix3.h"
#include "OCUtility/AxisAlignedBox.h"
#include "ISection.h"

#pragma warning(disable:4018)

namespace OC
{
	namespace Modeling
	{
		class _ModelExport CPipeMath
		{
		public:
			CPipeMath(void);
			~CPipeMath(void);
		public:
			//Rect Pipe
			static osg::Node* buildRectPipe(Vector3List v3list,PipeSection sect,CVector3 normal = CVector3::UNIT_Z,bool close = true);
			static osg::Node* buildRectConnec1(CVector3 center,CVector3 right,PipeSection sect);
			static osg::Node* buildRectConnec2(CVector3 left,CVector3 center,CVector3 right,PipeSection leftSect,PipeSection rightSect,bool buildTuber = false);
	
			//Circle Pipe
			static osg::Node* buildCirclePipe(Vector3List v3list,double dr,bool close = true);
			static osg::Node* buildCirclePipe(CVector3 stPoint,CVector3 edPoint,PipeSection sect,bool colse = true);
			static osg::Node* buildCirclePipe(CVector3 stPoint,CVector3 edPoint,double dr,bool close = true);
			static osg::Node* buildCirclePipe(CVector3 stPoint,CVector3 edPoint,double dr1,double dr2,bool close = true);
			static osg::Node* buildCircleConnec1(CVector3 center,CVector3 right,PipeSection sect);
			static osg::Node* buildCircleConnec2(CVector3 left,CVector3 center,CVector3 right,PipeSection leftSect,PipeSection rightSect,bool buildTuber = false);
			static osg::Node* buildCircleConnec3(CVector3 center,Vector3List list,doubleVector drList,bool tuber = true);

			static CVector3 getPointByDistance(CVector3 start,CVector3 end,double distance);

			/*
			生成井模型
			upR 上面的半径大小
			bottomR 下面的半径大小
			height 井的高度
			*/
			static osg::ref_ptr<osg::Node> BuildWell(double upR,double bottomR,double height);
		};

	}
}

#endif
