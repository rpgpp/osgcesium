#ifndef _OC_DATABASE_H__
#define _OC_DATABASE_H__

#include "OCMain/stl.h"
#include "OCMain/osg.h"
#include "OCMain/IteratorWrapper.h"
#include "OCMain/DataStream.h"
#include "OCUtility/StringConverter.h"
#include "OCUtility/tinyxml.h"
#include "OCMain/CoreConfig.h"

struct sqlite3;
struct sqlite3_stmt;
namespace OC
{
	class CMetaRecord;
	class CQueryInfo;
	class FeatureRecord;
	class IDatabase;
	class LayerRecord;
	class IPipeDatabase;
	class TemplateRecord;
	class TileFeatureRecord;
	class OCFieldInfo;
	class OCHeaderInfo;
	class OCGeomFieldInfo;
	class OCJoinFieldInfo;
	class OCBinaryFieldInfo;
	class OCBinaryField;
	class SQLite;
}

typedef enum _OC_DATATYPE  
{  
	OC_DATATYPE_INTEGER,  
	OC_DATATYPE_FLOAT,  
	OC_DATATYPE_TEXT,  
	OC_DATATYPE_BLOB,  
	OC_DATATYPE_NULL,
	OC_DATATYPE_TIME,
	OC_DATATYPE_GEOM
}OC_DATATYPE;  

#endif