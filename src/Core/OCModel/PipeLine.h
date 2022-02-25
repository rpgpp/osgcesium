#ifndef _MODEL_PIPE_LINE_CLASS_H__
#define _MODEL_PIPE_LINE_CLASS_H__

#include "IModelData.h"
#include "ISection.h"
#include "PipeNode.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport PipeLine : public ISimpleModelData
		{
		public:
			PipeLine();
			virtual ~PipeLine();
			virtual PipeLine* asPipeLine();
			virtual PipeLine* clone();
		public:

			static PipeLine* create(long gid,int dno,int stnod,int ennod,String geomStr,String sectStr,NameValuePairList nameValues);
			static PipeLine* create(TemplateRecord* record);

			virtual void doOffset(CVector3 offset);

			virtual CVector3 getCenter();

			void setLineZ(double stElev,double edElev);

			void setLinPoint(Vector3List lin){mPointList = lin;}
			Vector3List& getLinPoint(){return mPointList;}

			Vector3List getTwoPointFromNode(long nodeID);
			bool substractFromNode(long nodeID,double factor,CVector3& newPoint);

			void setStNode(PipeNode* refN);
			PipeNode* getStNode();

			void setEdNode(PipeNode* refN);
			PipeNode* getEdNode();

			long getStID();
			void setStID(long id);

			long getEdID();
			void setEdID(long id);

			bool setStartPoint(CVector3 v);
			CVector3 getStartPoint();

			CVector3 getStartNextPoint();
			CVector3 getEndNextPoint();

			bool setEndPoint(CVector3 v);
			CVector3 getEndPoint();

			CVector3 getPointByID(long gid);
			CVector3 getPointNextById(long gid);

			void setSection(PipeSection sect);
			PipeSection& getSection();

			LinePtrList getLinkLines();

			double getStInter();
			void setStInter(double inter);

			double getEdInter();
			void setEdInter(double inter);

			String getMaterial();
			void setMaterial(String strMaterial);

			//流向是否与点集序列相反
			bool isFlowNegative();
			CVector3 getDirectionFromNode(long nodeGID);

			std::vector<Vector3List>& getExtraLine();

			bool fixSectNull2Near();
			
			static String NameID;
			static String NameSect;
			static String NameDirection;
			static String NameStnodNo;
			static String NameEdnodNo;
			static String NameStnodInter;
			static String NameEdnodInter;
			static String NameSpatialColumn;
			static String NameMaterial;
		protected:			
			void setIsGeographicData(bool isGeographicData)
			{
				mIsGeographicData = isGeographicData;
			}

			bool isGeographicData()
			{
				return mIsGeographicData;
			}

		protected:						
			bool							  mIsGeographicData;
			long							  mStNodeID;
			long							  mEdNodeID;
			double							  mStInter;
			double							  mEdInter;
			String							  m_strMaterial;
			Vector3List						  mPointList;
			PipeSection						  mSection;
			osg::ref_ptr<PipeNode>			  mStNode;
			osg::ref_ptr<PipeNode>			  mEdNode;			
			std::vector<Vector3List>		  mExtraLine;
			osg::ref_ptr<osg::EllipsoidModel> mEllipsoidModel;
		};
	}
}


#endif

