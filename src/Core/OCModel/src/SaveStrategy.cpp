#include "SaveStrategy.h"

namespace OC
{
	namespace Modeling
	{
		SaveStrategy::SaveStrategy(void)
			:mCommitNumber(5000)
		{}

		SaveStrategy::~SaveStrategy(void)
		{}

		void SaveStrategy::setTable(IModelTable* table)
		{
			mRefTable = table;
		}

		void SaveStrategy::setWorkPath(String path)
		{
			replace(path.begin(),path.end(),'\\','/');
			mWorkPath = path;

			if (!mWorkPath.empty())
			{
				if (*mWorkPath.rbegin() != '/')
				{
					mWorkPath.push_back('/');
				}
			}
		}

		void SaveStrategy::setCommitNumber(int num)
		{
			mCommitNumber = num;
		}

		void SaveStrategy::insertMetadata(CMetaRecord* record)
		{
			if (mArchive.valid())
			{
				mArchive->getDatabase()->insert(record);
			}
		}

		FileSaveStrategy::FileSaveStrategy(){}

		FileSaveStrategy::~FileSaveStrategy()
		{
			
		}

		bool FileSaveStrategy::init()
		{
			return false;
		}

		bool FileSaveStrategy::writeFeature(FeatureRecord* record)
		{
			return false;
		}

		SqliteSaveStrategy::SqliteSaveStrategy()
		{
		
		}

		SqliteSaveStrategy::~SqliteSaveStrategy()
		{
			
		}

		bool SqliteSaveStrategy::init()
		{
			return false;
		}

		bool SqliteSaveStrategy::writeFeature(FeatureRecord* record)
		{
			return false;
		}

		XmlSaveStrategy::XmlSaveStrategy():mDocument(NULL),mLayerElement(NULL)
		{
		}

		XmlSaveStrategy::~XmlSaveStrategy()
		{
			
		}

		bool XmlSaveStrategy::init()
		{
			return false;
		}

		bool XmlSaveStrategy::writeFeature(FeatureRecord* record)
		{
			return false;
		}

		void XmlSaveStrategy::writeRecord(FeatureRecord* record)
		{			

		}

		ShpFileSaveStrategy::ShpFileSaveStrategy()
		{

		}

		ShpFileSaveStrategy::~ShpFileSaveStrategy()
		{
		}

		bool ShpFileSaveStrategy::init()
		{
			return false;
		}

		bool ShpFileSaveStrategy::writeFeature(FeatureRecord* record)
		{
			return false;
		}
	}
}
