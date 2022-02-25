#ifndef _GLOBAL_ARCHIVE_H_
#define _GLOBAL_ARCHIVE_H_

#include "OCMain/Singleton.h"
#include "IArchive.h"
#include "LayerDefine.h"

namespace OC
{
	class _LayerExport OCResource : public CSingleton<OCResource>
	{
	public:
		OCResource(void);
		~OCResource(void);

		IArchive* getArchive();

		osg::Node* readNode(String file);
		osg::Image* readImage(String file);
	private:
		osg::ref_ptr<IArchive>	mArchive;
	};
}


#endif
