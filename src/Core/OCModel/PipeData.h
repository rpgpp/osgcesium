#ifndef _OC_PIPE_DATA_H__
#define _OC_PIPE_DATA_H__

#include "PipeMath.h"
#include "Modelnode.h"

namespace OC
{
	namespace Modeling
	{
		class IPipeData;

		class _ModelExport IPipeDataBuilder
		{
		public:
			IPipeDataBuilder(){}
			virtual ~IPipeDataBuilder(){}
			virtual String className() = 0;
			virtual IPipeData* create() = 0;

			static unsigned short LOOP_R;
			static unsigned short LOOP_C;

			static void setLowQuality();
		};

		typedef std::map<String,IPipeDataBuilder*> IPipeBuilderMap;

		class _ModelExport PipeGeometryFactory : public CSingleton<PipeGeometryFactory>
		{
		public:
			PipeGeometryFactory(){}

			~PipeGeometryFactory()
			{
				CORE_DELETE_SECOND(mBuilderMap,IPipeBuilderMap)	;
			}

			void addBuilder(String className,IPipeDataBuilder* builder)
			{
				mBuilderMap[className] = builder;
			}

			IPipeDataBuilder* getBuilder(String className)
			{
				IPipeBuilderMap::iterator it = mBuilderMap.find(className);
				if(it == mBuilderMap.end())
				{
					return NULL;
				}
				return it->second;
			}

			static IPipeData* createPipeData(String name);
		private:
			IPipeBuilderMap mBuilderMap;
		};


		class _ModelExport PipePoint
		{
		public:
			PipePoint(CVector3 point,PipeSection sect)
				:Position(point)
				,Section(sect)
			{}
			PipePoint(){}
			~PipePoint(){}
			static String TagName;

			void setPoint(CVector3 point){Position = point;}
			CVector3 getPoint(){return Position;}

			void setSection(PipeSection sect){Section = sect;}
			PipeSection getSection(){return Section;}

			CVector3	Position;
			PipeSection Section;

			TiXmlElement* toXmlElement();
			static PipePoint parse(TiXmlElement* element);
		};

		typedef std::vector<PipePoint> PipePointList;

		class _ModelExport IPipeData : public osg::Referenced
		{
		public:
			IPipeData();
			virtual ~IPipeData();

			virtual osg::Geode* buildGeode();
			virtual osg::Node* buildNode(){return buildGeode();}

			void setTurber(bool turber);
			bool getTurber();

			void setClose(bool close);
			bool getClose();

			void setImageInternal(bool inter);
			bool getImageInternal();

			void setTextureImage(String image);
			String getTextureImage();

			void mergeGeode(osg::Geode* geodeToMerge);

			virtual void doOffset(CVector3 offset){}

			virtual void setOffset(CVector3 offset);
			virtual CVector3 getOffset();

			void setRotation(CQuaternion q);
			CQuaternion getRotation();

			void setScale(CVector3 scale);
			CVector3 getScale();

			void setLoopC(unsigned short n);
			unsigned short getLoopC();

			void setLoopR(unsigned short n);
			unsigned short getLoopR();

			void setPreferInstrance(bool instance);
			bool getPreferInstance();

			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			static Vector3List createSectionPoints(CVector3 center,CVector3 direction,CVector3 normal,PipeSection sect,unsigned short loopR);
		protected:
			Vector3List bezierCurve(CVector3 left,CVector3 center,CVector3 right);
			osg::DrawElements* createDrawElements();
		protected:
			bool						mPreferInstance;
			bool						mTurber;
			bool						mClose;
			bool						mImageInternal;
			unsigned short				mLoopc;
			unsigned short				mLoopr;
			String						mTextureImage;
			CVector3					mOffset;
			CVector3					mScale;
			CQuaternion					mQuat;
			osg::ref_ptr<osg::Geode>	mGeode;
		};

		typedef std::vector<osg::ref_ptr<IPipeData>> IPipeDataList;

		class _ModelExport CPipeData : public IPipeData
		{
		public:
			CPipeData();
			virtual ~CPipeData();
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode(){return IPipeData::buildGeode();}

			int getArcPointNum();
			CVector3 getArcPoint(int i);

			void setRefNormal(CVector3 normal);
			CVector3 getRefNormal();

		protected:
			CVector3	mRefNormal;
			Vector3List mArcPointSet;
		};

		class _ModelExport CGlobalNode : public CPipeData
		{
		public:
			CGlobalNode();
			~CGlobalNode();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Node* buildNode();
			
			void calculateRotation(PipeNode* pPipeNode);
			void setFeatureUrl(String url);
			void setUseProxyNode(bool prox);
		private:
			bool		mUseProxyNode;
			String		mFeatureUrl;
			String		mEnName;
		};

		class _ModelExport CPipeCylinder : public CPipeData
		{
		public:
			CPipeCylinder(){}
			~CPipeCylinder(){}
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);

			void setUpOffset(double offset);
			void setDownOffset(double offset);
			void setRadius(double r);
			void setPosition(CVector3 position);
			void setHeight(double h);
			virtual osg::Geode* buildGeode();
			virtual osg::Node* buildNode();
		private:
			double		mHeight;
			double		mOffsetU;
			double		mOffsetD;
			double		mRadius;
			CVector3	mPosition;
		};

		class _ModelExport CPipeWell : public CPipeData
		{
		public:
			CPipeWell(){}
			~CPipeWell(){}
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);

			virtual osg::Node* buildNode();

			void setCoverImage(String url);
			void setWallImage(String url);

			void setMatrix(osg::Matrix position);
			void setHeight(double h1,double h2);
		private:
			double		mHeight1;
			double		mHeight2;
			String		mWallImage;
			String		mCoverImage;
			osg::Matrix	mMatrix;
		};

		class _ModelExport CCirclePipeNode1 : public CPipeData
		{
		public:
			CCirclePipeNode1();
			~CCirclePipeNode1();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();
			void setPoints(PipePoint center,CVector3 direction);
		protected:
			CVector3  mDirection;
			PipePoint mCenter;
		};

		class _ModelExport CCirclePipeNode2 : public CPipeData
		{
		public:
			CCirclePipeNode2();
			~CCirclePipeNode2();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();
			void setPoints(CVector3 center,PipePoint p1,PipePoint p2);
		protected:
			CVector3  mCenter;
			PipePoint mPoint1;
			PipePoint mPoint2;
		private:
			void createArcPointSet();
			void createTuber();
			float mTuberLengthFactor;
			float mTuberRadiusFactor;
		};

		class _ModelExport CCirclePipeNode3 : public CPipeData
		{
		public:
			CCirclePipeNode3(){}
			~CCirclePipeNode3(){}
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();
			void setPoints(CVector3 center,PipePoint p1,PipePoint p2,PipePoint p3);
		protected:
			CVector3  mCenter;
			PipePoint mPoint1;
			PipePoint mPoint2;
			PipePoint mPoint3;
		};

		class _ModelExport CCirclePipeLine : public CPipeData
		{
		public:
			CCirclePipeLine(){}
			~CCirclePipeLine(){}
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();
			virtual osg::Node* buildNode();

			void setStPoint(PipePoint point);
			PipePoint getStPoint();

			void setEdPoint(PipePoint point);
			PipePoint getEdPoint();
		protected:
			PipePoint mStPoint;
			PipePoint mEdPoint;
		};

		class _ModelExport CRectPipeNode1 : public CPipeData
		{
		public:
			CRectPipeNode1();
			~CRectPipeNode1();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();

			void setPoints(PipePoint center,CVector3 direction);

		protected:
			CVector3  mDirection;
			PipePoint mCenter;
		};

		class _ModelExport CRectPipeNode2 : public CPipeData
		{
		public:
			CRectPipeNode2(){}
			~CRectPipeNode2(){}
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();

			void setPoints(CVector3 center,PipePoint point1,PipePoint point2);

		protected:
			CVector3  mCenter;
			PipePoint mPoint1;
			PipePoint mPoint2;
		};

		class _ModelExport CRectPipeLine : public CPipeData
		{
		public:
			CRectPipeLine();
			~CRectPipeLine();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();
			virtual osg::Node* buildNode();

			void setStPoint(PipePoint point);
			PipePoint getStPoint();

			void setEdPoint(PipePoint point);
			PipePoint getEdPoint();

		protected:
			PipePoint mStPoint;
			PipePoint mEdPoint;
		};

		class _ModelExport CPipeWell1 : public CPipeData
		{
		public:
			CPipeWell1();
			~CPipeWell1();
			static String TagName;
			virtual TiXmlElement* toXmlElement();
			virtual void readXmlElement(TiXmlElement* elemennt);
			virtual osg::Geode* buildGeode();

			double mHatR,mRoomR,mRoomD;
		};
	}
}



#endif
