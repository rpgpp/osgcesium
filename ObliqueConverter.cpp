#include "ObliqueConverter.h"
#include "OCesium/CesiumTool.h"

namespace OC
{
	namespace Optimize
	{
		ObliqueConverter::ObliqueAttributeCallback::ObliqueAttributeCallback(ObliqueConverter* owner)
			:_owner(owner)
		{

		}

		ObliqueConverter::ObliqueAttributeCallback::~ObliqueAttributeCallback()
		{

		}

		void ObliqueConverter::ObliqueAttributeCallback::onSetAttirubte(String name, Cesium::FeatureBatchTable* featurebatchtable)
		{
			if(!_name.empty())
				featurebatchtable->pushAttribute("id",_name);
		}

		ObliqueConverter::ObliqueConverter(int threadNum)
		:workqueue(threadNum)
		{
			
		}

		ObliqueConverter::~ObliqueConverter()
		{

		}

		class FindPagedlod : public osg::NodeVisitor
		{
		public:
			FindPagedlod(osg::Group* g = NULL)
				:osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
				,_g(g)
			{
			}
			~FindPagedlod() {}

			virtual void apply(osg::PagedLOD& lod)
			{
				if (_g.valid())
				{
					String databasePath = lod.getDatabasePath();
					osg::LOD::RangeList list = lod.getRangeList();
					for (unsigned i = 0; i < list.size(); i++)
					{
						String childname = lod.getFileName(i);
						if (!childname.empty())
						{
							osg::Node* node = osgDB::readNodeFile(databasePath + childname);
							if (node)
								_g->addChild(node);
						}
					}
				}

				traverse(lod);
			}

			virtual void apply(osg::Geode& node)
			{
				_geodeCount++;
			}

			int _geodeCount = 0;
			osg::ref_ptr<osg::Group> _g;
		};

		class ConvertPagedlod : public osg::NodeVisitor
		{
		public:
			ConvertPagedlod(ObliqueConverter* converter,
				int level,
				String destDir,
				TDTILES::Tileset* tileset)
				:osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
				, _converter(converter)
				, _level(level)
				, _destDir(destDir)
				, _tileset(tileset)
			{
			}
			~ConvertPagedlod() {}

			virtual void apply(osg::PagedLOD& lod)
			{
				String databasePath = lod.getDatabasePath();
				osg::LOD::RangeList list = lod.getRangeList();
				for (unsigned i = 0; i < list.size(); i++)
				{
					String childname = lod.getFileName(i);
					if (!childname.empty())
					{
						String filename = databasePath + childname;
						osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);
						if (node.valid())
						{
							bool ret = _converter->convert(databasePath, _destDir, childname, _level + 1, _tileset->root(), node);
							if (!ret)
							{
								_tileset->geometricError() = _tileset->root()->geometricError() = 0.0;
							}
						}
					}
				}
			}
			int _level;
			String _destDir;
			TDTILES::Tileset* _tileset;
			ObliqueConverter* _converter;
		};

		bool ObliqueConverter::convert(String srcDir, String destDir, String file,int level, TDTILES::Tile* parentTile, osg::Node* parentNode)
		{
			osg::ref_ptr<osg::Node> node = parentNode;

			String url = osgDB::getNameLessAllExtensions(file);
			double geometryError = parentTile->geometricError().get() * 0.5;

			osg::BoundingBox box = Cesium::CesiumTool::getBoundBox(node);

			//insert this child to parent tile
			osg::ref_ptr<TDTILES::Tile> tile = new TDTILES::Tile;
			tile->geometricError() = geometryError;
			tile->boundingVolume()->box() = box;
			if (level == 0)
			{
				tile->content()->uri() = url + "/" + url + ".json"; 
				CORE_LOCK_AUTO_MUTEX
				parentTile->children().push_back(tile);
			}
			else
			{
				tile->content()->uri() = url + ".json";
				parentTile->children().push_back(tile);
			}

			//new child tileset
			osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
			tileset->root()->content()->uri() = url + ".b3dm";
			tileset->geometricError() = tileset->root()->geometricError() = geometryError;
			tileset->root()->boundingVolume()->box() = box;

			osgDB::makeDirectory(destDir);

			String b3dmfile = destDir + url + ".b3dm";
			if (mRWriteB3dm || !FileUtil::FileExist(b3dmfile))
			{
				Cesium::B3dmWriter writer;
				writer.getGltfObject()->convert(node);
				writer.writeToFile(b3dmfile);
			}

			//convert children
			osg::ref_ptr< ConvertPagedlod> _ConvertPagedlod = new ConvertPagedlod(this, level + 1, destDir, tileset);
			node->accept(*_ConvertPagedlod);

			String jsonfile = destDir + url + ".json";
			if (mRWriteJson || !FileUtil::FileExist(jsonfile))
			{
				OC_WRITE_Tiltset(tileset, jsonfile);
			}
			std::cout << jsonfile << std::endl;
			return true;
		}

		osg::Group* tile4(osg::Group* g)
		{
			osg::ref_ptr<osg::Group> splitGroup = new osg::Group;

			{
				osg::BoundingBox box = Cesium::CesiumTool::getBoundBox(g);

				float w = box.xMax() - box.xMin();
				float h = box.yMax() - box.yMin();

				osg::BoundingBox box1 = box;
				osg::BoundingBox box2 = box;
				osg::BoundingBox box3 = box;
				osg::BoundingBox box4 = box;

				box1.xMax() = box2.xMax() = box.xMin() + w * 0.5;
				box1.yMax() = box2.yMin() = box.yMin() + h * 0.5;

				box3.xMin() = box4.xMin() = box.xMin() + w * 0.5;
				box3.yMax() = box4.yMin() = box.yMin() + h * 0.5;

				osg::ref_ptr<osg::Group> g1 = new osg::Group;
				osg::ref_ptr<osg::Group> g2 = new osg::Group;
				osg::ref_ptr<osg::Group> g3 = new osg::Group;
				osg::ref_ptr<osg::Group> g4 = new osg::Group;
				for (int i = 0; i < (int)g->getNumChildren(); i++)
				{
					osg::Node* child = g->getChild(i);
					if (box1.contains(child->getBound().center()))
						g1->addChild(child);
					else if (box2.contains(child->getBound().center()))
						g2->addChild(child);
					else if (box3.contains(child->getBound().center()))
						g3->addChild(child);
					else
						g4->addChild(child);
				}
				splitGroup->addChild(g1);
				splitGroup->addChild(g2);
				splitGroup->addChild(g3);
				splitGroup->addChild(g4);
			}

			return splitGroup.release();
		}

		osg::Group* tile2(osg::Group* g,int maxNode, bool shouldSplit)
		{
			osg::ref_ptr<osg::Group> splitGroup = new osg::Group;

			FindPagedlod fpl;
			g->accept(fpl);

			if (g->getNumChildren() < maxNode && fpl._geodeCount < maxNode && !shouldSplit)
			{
				splitGroup->addChild(g);
			}
			else
			{
				shouldSplit = (fpl._geodeCount / maxNode >= 2) && (fpl._geodeCount / g->getNumChildren()) >= 4;
				
				osg::BoundingBox box = Cesium::CesiumTool::getBoundBox(g);

				float w = box.xMax() - box.xMin();
				float h = box.yMax() - box.yMin();

				bool splitH = w < h;

				osg::BoundingBox box1 = box;
				osg::BoundingBox box2 = box;
				if (splitH)
				{
					box1.yMax() = box2.yMin() = box.yMin() + h * 0.5;
				}
				else
				{
					box1.xMax() = box2.xMin() = box.xMin() + w * 0.5;
				}

				osg::ref_ptr<osg::Group> g1 = new osg::Group;
				osg::ref_ptr<osg::Group> g2 = new osg::Group;
				for (int i = 0; i < (int)g->getNumChildren(); i++)
				{
					osg::Node* child = g->getChild(i);
					if (box1.contains(child->getBound().center()))
						g1->addChild(child);
					else
						g2->addChild(child);
				}

				if (g1->getNumChildren() > maxNode || (shouldSplit && g1->getNumChildren() > 1))
				{
					g1 = tile2(g1, maxNode, shouldSplit);
					for (int i = 0; i < (int)g1->getNumChildren(); i++)
					{
						splitGroup->addChild(g1->getChild(i));
					}
				}
				else
				{
					splitGroup->addChild(g1);
				}
				if (g2->getNumChildren() > maxNode || (shouldSplit && g2->getNumChildren() > 1))
				{
					g2 = tile2(g2, maxNode, shouldSplit);
					for (int i = 0; i < (int)g2->getNumChildren(); i++)
					{
						splitGroup->addChild(g2->getChild(i));
					}
				}
				else
				{
					splitGroup->addChild(g2);
				}
			}
			
			return splitGroup.release();
		}

		inline void pushWriteRequest(ConvertWorkqueue& workqueue,String filename,osg::Node* node)
		{
			ConvertWorkqueue::TaskRequest request;
			request.type = ConvertWorkqueue::RT_Writeb3dm;
			request.file = filename;
			request.rootNode = node;
			workqueue.addRequest(request);
		}

		inline void pushSplitRequest(ConvertWorkqueue& workqueue, ObliqueConverter* converter, String destDir, osg::Node* node,TDTILES::Tile* parentTile,int code)
		{
			ConvertWorkqueue::TaskRequest request;
			request.type = ConvertWorkqueue::RT_SplitAndWrite;
			request.converter = converter;
			request.destDir = destDir;
			request.rootNode = node;
			request.parentTile = parentTile;
			request.code = code;
			workqueue.addRequest(request);
		}

		void ObliqueConverter::split(osg::Group* node,TDTiles::Tile* parentTile,bool shouldSplit ,int depth,String destDir,int code)
		{
			destDir = Stringify() << destDir << code << "/";

			int maxTileNodeNum = (std::max)(mMaxTileNodeNum,mMaxRootTileNodeNum / (depth + 1));

			osg::ref_ptr<osg::Group> splitGroup = tile2(node, maxTileNodeNum, shouldSplit);
			
			for (int i = 0; i < (int)splitGroup->getNumChildren(); i++)
			{
				float r = splitGroup->getNumChildren() > 4 ? 0.25:0.5;
				osg::Node* child = splitGroup->getChild(i);
				String uri = Stringify() << destDir << i << ".b3dm";
				osg::ref_ptr<TDTiles::Tile> tile = new TDTiles::Tile;
				parentTile->children().push_back(tile);
				tile->geometricError() = parentTile->geometricError().get() * r;
				tile->boundingVolume()->box() = Cesium::CesiumTool::getBoundBox(child);
				tile->content()->uri() = uri;
				String filename = mDestDir + uri;
				osgDB::makeDirectoryForFile(filename);

				FindPagedlod fpl(new osg::Group);
				child->accept(fpl);

				pushWriteRequest(workqueue,filename,child);

				//std::cout << fpl._geodeCount << std::endl;
				if (fpl._g->getNumChildren() == 0 || depth == mDebugDepth /*debug*/)
				{
					tile->geometricError() = 0.0;
					continue;
				}
				split(fpl._g, tile, fpl._geodeCount > mMaxTileNodeNum, depth + 1, destDir, i);
			}
		}

		void ObliqueConverter::readNode(String directory)
		{
			directory = osgDB::convertFileNameToUnixStyle(directory);
			if (StringUtil::endsWith(directory, "/", false))
			{
				directory = directory.substr(0, directory.length() - 1);
			}

			if (mDestDir.empty())
			{
				String name, ext, path;
				StringUtil::splitFullFilename(directory, name, ext, path);
				mDestDir = path + name + "_OC/";
			}
			else if(!StringUtil::endsWith(mDestDir, "/", false))
			{
				mDestDir += "/";
			}

			directory += "/";

			StringVector sv;
			osg::ref_ptr<osg::Group> group = new osg::Group;
			osgDB::DirectoryContents contents = osgDB::getDirectoryContents(directory);
			osgDB::DirectoryContents::iterator it = contents.begin();
			for (; it != contents.end(); it++)
			{
				String subPath = *it;
				if (subPath == ".." || subPath == ".")
				{
					continue;
				}
				else
				{
					if (osgDB::DIRECTORY == osgDB::fileType(directory + "/" + subPath))
					{
						String featureUrl = subPath + "/" + subPath + ".osgb";
						String file = directory + featureUrl;
						if (FileUtil::FileExist(file))
						{
							sv.push_back(subPath);
						}
					}
				}
			}

			osgDB::makeDirectory(mDestDir);

			osg::ref_ptr<osg::Group> root = new osg::Group;
			for (auto i = sv.begin();i< sv.end();i++)
			{
				String name = *i;
				String file = directory + name + "/" + name + ".osgb";
				osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(file);
				if (node.valid())
				{
					osg::BoundingBox box = Cesium::CesiumTool::getBoundBox(node);
					mRootBoundBox.expandBy(box);
					root->addChild(node);
				}
				else
				{
					i = sv.erase(i);
				}
			}

			osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
			tileset->root()->transform() = Modeling::ProjectTool::computeTransform(mLocation);
			tileset->root()->boundingVolume()->box() = mRootBoundBox;
			double error = mRootBoundBox.radius() * 16.0 * Cesium::gltfConfig::geometryErroRatio;
			error /= 125.0;
			tileset->geometricError() = error * 4;
			tileset->root()->geometricError() = error;

			if (mergeTop)
			{
				osg::ref_ptr<osg::Group> splitGroup = tile2(root, mMaxRootTileNodeNum, false);

				for (int i = 0; i < (int)splitGroup->getNumChildren(); i++)
				{
					osg::Node* child = splitGroup->getChild(i);

					osg::ref_ptr<TDTiles::Tile> tile;
					if (splitGroup->getNumChildren() == 1)
					{
						tile = tileset->root();
					}
					else
					{
						tile = new TDTiles::Tile;
						tile->geometricError() = tileset->root()->geometricError().get() * 0.5;
						tile->boundingVolume()->box() = Cesium::CesiumTool::getBoundBox(child);
						tileset->root()->children().push_back(tile);
					}

					String uri = Stringify() << i << ".b3dm";
					tile->content()->uri() = uri;
					String filename = mDestDir + uri;
					osgDB::makeDirectoryForFile(filename);

					FindPagedlod fpl(new osg::Group);
					child->accept(fpl);

					pushWriteRequest(workqueue, filename, child);

					if (fpl._g->getNumChildren() == 0)
					{
						tile->geometricError() = 0.0;
						continue;
					}

					pushSplitRequest(workqueue, this, String(), fpl._g, tile, i);
				}
			}
			else
			{
				for (int i = 0; i < (int)root->getNumChildren(); i++)
				{
					String name = sv[i];
					ConvertWorkqueue::TaskRequest request;
					request.type = ConvertWorkqueue::RT_WriteDirectory;
					request.converter = this;
					request.srcDir = directory + name + "/";
					request.destDir = mDestDir + name + "/";
					request.file = name + ".osgb";
					request.parentTile = tileset->root();
					request.rootNode = root->getChild(i);
					workqueue.addRequest(request);
				}
			}

			while (workqueue.finished != 0) {
				workqueue.mWorkQueue->processResponses();
				Sleep(100);
			}

			String tilesetFile = mDestDir + "tileset.json";
			OC_WRITE_Tiltset(tileset, tilesetFile);
		}
	}
}