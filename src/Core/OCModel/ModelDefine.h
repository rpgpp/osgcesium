#ifndef _OC_MODEL__DEFINE_H_
#define _OC_MODEL__DEFINE_H_

#if defined(_MSC_VER)
#ifndef OCModel_EXPORTS
#define _ModelExport  __declspec(dllimport)
#else
#define _ModelExport  __declspec(dllexport)
#endif
#else
#define _ModelExport
#endif

#include "OCUtility/MathUtil.h"
#include "OCUtility/tinyxml.h"
#include "OCUtility/Rectangle.h"
#include "OCUtility/Vector3.h"
#include "OCUtility/AxisAlignedBox.h"
#include "OCMain/Exception.h"
#include "OCLayer/IArchive.h"
#include "OCLayer/FeatureRecord.h"
#include "OCLayer/QueryInfo.h"
#include "OCLayer/IDatabaseObject.h"

#pragma warning(disable:4018)

#define LOG Singleton(LogManager).stream()

namespace OC
{
	namespace Modeling
	{
		static String PipeTableSplice = "_";
		static String NodeTableSubfix = PipeTableSplice + "NODE";
		static String LineTableSubfix = PipeTableSplice + "LINE";

		enum MeasureZ_Flag
		{
			MeasureZ_IS_TOP,
			MeasureZ_IS_CENTER,
			MeasureZ_IS_BOTTOM
		};

		class CDataFilter;
		class ProjectTool;
		class CSubShelfData;
		class CSubLineShelfTable;
		class GenericMoldData;
		class IGroupModelData;
		class ISimpleModelData;
		class IModelData;
		class IModelDatabase;
		class IModelTable;
		class IModelFactory;
		class IPipeDataHandler;
		class LineTable;
		class ModelDataManager;
		class NodeTable;
		class ModelNode;
		class PipeLine;
		class PipeNode;
		class PipeTable;
		class TableConfig;
		class SaveStrategy;
		class GenericMoldTable;
		class FieldMapping;
		
		typedef std::map<long,osg::ref_ptr<PipeLine>>		LineMap;
		typedef MapIterator<LineMap>						LineMapIterator;

		typedef std::map<long,osg::ref_ptr<PipeNode>>		NodeMap;
		typedef MapIterator<NodeMap>						NodeMapIterator;

		typedef std::map<long,IModelData*>					ModelDataMap;
		typedef MapIterator<ModelDataMap>					ModelDataMapIterator;

		typedef std::vector<PipeNode>			NodeList;
		typedef std::vector<PipeLine>			LineList;
		typedef std::vector<PipeNode*>			NodePtrList;
		typedef std::vector<PipeLine*>			LinePtrList;
		typedef std::vector<IModelData*>		IModelDataList;
	}
}
#endif