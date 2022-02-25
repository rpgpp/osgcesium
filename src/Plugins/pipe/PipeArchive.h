#ifndef _OC_FuncArchive_H__
#define _OC_FuncArchive_H__

#include "OCModel/ModelArchive.h"

using namespace OC;
using namespace OC::Modeling;

class PipeArchive : public ModelArchive
{
public:
	PipeArchive(void);
	~PipeArchive(void);
	virtual IModelTable* getModelTable(ModelDataManager* owner);

	osgDB::ReaderWriter::ReadResult openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int dep, const osgDB::Options* options) const;
};

#endif
