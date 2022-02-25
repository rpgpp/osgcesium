#include "glTFConverter.h"
#include "OCesium/CesiumTool.h"

namespace OC
{
	using namespace osgEarth;
	namespace Cesium
	{
		glTFConverter::glTFConverter()
		{

		}

		glTFConverter::~glTFConverter()
		{

		}

		void glTFConverter::readAttributeMap(String filename)
		{
			mAttributeCallbak = new gltfAttributeCallback;
			mAttributeCallbak->readFile(filename);
		}

		void glTFConverter::resetNode()
		{
			mNode = NULL;
		}

		void glTFConverter::init()
		{
			if (!mProjectTool.valid())
			{
				String prj;
				if (mPrjCS.isSet())
				{
					prj = mPrjCS.get();
				}
				mProjectTool = new MapProject(prj);
			}
		}

		void glTFConverter::optimize(osg::Node* node)
		{
			if (localOffset)
			{
				glTFVisitor visitor;
				osg::Vec3d model_center = node->getBound().center();
				model_center.z() = 0.0f;
				visitor.offset = model_center;
				node->accept(visitor);
				node->dirtyBound();
			}

			mNode = node;
		}

		class OCNodeReserve : public osg::NodeVisitor
		{
		public:
			OCNodeReserve()
				:osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
			{

			}
			~OCNodeReserve() {}

			void apply(osg::Geode& geode)
			{
				String name = geode.getName();
				if (StringUtil::startsWith(name, "wendu")
					|| StringUtil::startsWith(name, "yangan")
					|| StringUtil::startsWith(name, "jiankong"))
				{
					g->addChild(&geode);
				}
			}

			osg::ref_ptr<osg::Group> g = new osg::Group;
		};


		bool glTFConverter::readNodeFile(String filename, osgDB::Options* options)
		{
			mNode = osgDB::readNodeFile(filename, options);
			if (mNode.valid())
			{
				mNodeFilename = osgDB::convertFileNameToUnixStyle(filename);

				init();

				if (mPrjCS.isSet())
				{
					osg::Vec3d model_center = mNode->getBound().center();
					model_center.z() = 0.0f;
					CVector3 position = mPrjOffset + Vec3ToVector3(model_center);
					MapProject prjtool(mPrjCS.get());
					mLocation = prjtool.localTo4326(position);
				}

				optimize(mNode);

				if (rotate)
				{
					osg::ref_ptr<osg::Group> g = new osg::Group;
					osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
					transform->setMatrix(osg::Matrixd::rotate(osg::Vec3d(0.0, 0.0, 1.0), osg::Vec3d(0.0, 1.0, 0.0)));
					transform->addChild(mNode);
					g->addChild(transform);
					mNode = g;
				}

				return true;
			}

			return false;
		}

		void glTFConverter::readPrjFile(String filename)
		{
			ifstream ifs(filename.c_str());
			if (ifs)
			{
				String IputPrjCS;
				getline(ifs, IputPrjCS);
				ifs.close();
				mPrjCS = IputPrjCS;
			}
		}

		void glTFConverter::setLocaltion(CVector3 location)
		{
			mLocation = location;
		}

		void glTFConverter::writeToGltf()
		{
			if (!mNode.valid())
			{
				return;
			}

			String basename;
			String extension;
			String path;
			StringUtil::splitFullFilename(mNodeFilename, basename, extension, path);
			if (outputPath.empty())
			{
				outputPath = path;
			}
			else if (outputPath[outputPath.length() - 1] != '/')
			{
				outputPath += "/";
			}

			CesiumTool::writeGLB(*mNode, outputPath + basename + ".gltf");
		}

		void glTFConverter::writeToGlb()
		{
			if (!mNode.valid())
			{
				return;
			}

			String basename;
			String extension;
			String path;
			StringUtil::splitFullFilename(mNodeFilename, basename, extension, path);
			if (outputPath.empty())
			{
				outputPath = path;
			}
			else if (outputPath[outputPath.length() - 1] != '/')
			{
				outputPath += "/";
			}

			CesiumTool::writeGLB(*mNode, outputPath + basename + ".glb");
		}

		void glTFConverter::output()
		{
			if (!mNode.valid())
			{
				return;
			}

			String basename;
			String extension;
			String path;
			StringUtil::splitFullFilename(mNodeFilename, basename, extension, path);

			if (outputPath.empty())
			{
				outputPath = path;
			}
			else if (outputPath[outputPath.length() - 1] != '/')
			{
				outputPath += "/";
			}
			osgDB::makeDirectory(outputPath);

			String tilesetfile = outputPath + basename + ".json";

			osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
			if (!upAxisZ)
			{
				tileset->asset()->gltfUpAxis() = "Y";
			}

			osg::ref_ptr<B3dmWriter> b3dmWriter = new B3dmWriter();
			b3dmWriter->getFeatureBatchTable()->setAttributeCallback(mAttributeCallbak);
			b3dmWriter->getGltfObject()->convert(mNode);

			int index = 0;
			std::vector<osg::ref_ptr<B3dmWriter> > list = b3dmWriter->splitByCapcity();
			if (list.size() == 1)
			{
				String uri = basename + ".b3dm";
				tileset->root()->content()->uri() = uri;
				b3dmWriter->writeToFile(outputPath + uri);
			}
			else
			{
				for (auto i : list)
				{
					String uri = basename + "_" + StringConverter::toString(index++) + ".b3dm";;
					B3dmWriter* writer = i.get();
					writer->writeToFile(outputPath + uri);
					osg::ref_ptr<TDTILES::Tile> tile = new TDTILES::Tile;
					tile->boundingVolume()->box() = writer->getGltfObject()->boundingBox();
					tile->geometricError() = 0;
					tile->content()->uri() = uri;
					tileset->root()->children().push_back(tile);
				}
			}

			osg::BoundingBox box = CesiumTool::getBoundBox(mNode);
			osg::Matrix matrix = MapProject::computeTransform(mLocation);
			tileset->root()->transform() = matrix;
			tileset->root()->boundingVolume()->box() = box;
			tileset->root()->geometricError() = tileset->geometricError() = box.radius() * 2.0;
			_OC_WriteTileset(tileset, tilesetfile);
		}
	}
}//namespace