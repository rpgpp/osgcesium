#ifndef _MODEL_PIPE_NODE_CLASS_H__
#define _MODEL_PIPE_NODE_CLASS_H__

#include "IModelData.h"
#include "ISection.h"
#include "NodeExInfo.h"

namespace OC
{
	namespace Modeling
	{
		enum NodeType{
			UNKNOWN_NODE,
			GLOBAL_MODEL,
			CIRCULAR_1_NODE,
			CIRCULAR_2_NORMAL_NODE,
			CIRCULAR_3_NORMAL_NODE,
			RECT_1_NODE,
			RECT_2_NORMAL_NODE,
			RECT_3_NORMAL_NODE,
			MIX_NODE,
			MIX_NODE_2,
			MIX_NODE_3,
			INSPECTION_WELL_NODE
		};

		class _ModelExport PipeNode : public ISimpleModelData
		{
		public:
			PipeNode(void);

			virtual PipeNode* asPipeNode();
		public:

			static PipeNode* create(long gid,int dno,double x,double y,NameValuePairList nameValues);
			static PipeNode* create(TemplateRecord* record);
			
			virtual void doOffset(CVector3 offset);
			
			virtual CVector3 getCenter();

			NodeType getType(){return m_type;}
			void setType(NodeType type){m_type = type;}

			String getMaterial();
			void setMaterial(String strMaterial);

			CVector3& position();

			PipePointList& getAroundPointList();
			Vector3List getAroundPoint();

			void addAroundPoint(PipePoint around);
			void addAroundPoint(CVector3 point,PipeSection sect);

			NodeExInfo& getInfo();

			void doMinusZ(MeasureZ_Flag flag);

			//lines link with this node
			void addLinkLine(PipeLine* line);
			void removeLinkLine(PipeLine* line);
			LinePtrList& getLinkLines();

			NodeType calcType();
			int getConnective();

			void checkData();

			static String NameID;
			static String NameNodeNo;
			static String NameNodeInter;
			static String NameNodeElevation;
			static String NameGroundElevation;
			static String NameNodeModelType;
			static String NameSpatialColumn;
			static String NameMaterial;
		protected:
			NodeType		m_type;
			CVector3		mPosition;
			String			m_strMaterial;
			NodeExInfo		mNodeInfo;
			LinePtrList		mLinkLines;
			PipePointList	mAroundPointList;

		private:
			bool			mDirty;
		};
	}
}

#endif
