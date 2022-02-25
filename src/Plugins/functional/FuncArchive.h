#ifndef _OC_FuncArchive_H__
#define _OC_FuncArchive_H__

#include "OCMain/FunctionalArchive.h"

using namespace OC;

class FuncArchive : public FunctionalArchive
{
public:
	FuncArchive(void);
	~FuncArchive(void);

	CVector2 gauss2LH(CVector2 coord);
	String invoke(StringVector params);
	osgDB::ReaderWriter::ReadResult openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int dep, const osgDB::Options* options) const;
};

#endif
