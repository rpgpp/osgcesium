#include "BuildingConverter.h"
#include <osgEarth/OGRFeatureSource>
#include <osgEarth/StyleSheet>
#include "OCUtility/FileUtil.h"
#include "OCUtility/StringConverter.h"

namespace OC
{
    using namespace Cesium;

    BuildingConverter::BuildingAttributeCallback::BuildingAttributeCallback(BuildingConverter* owner)
        :_owner(owner)
    {
    
    }

    BuildingConverter::BuildingAttributeCallback::~BuildingAttributeCallback()
    {

    }

    void BuildingConverter::BuildingAttributeCallback::onSetAttirubte(String name, Cesium::FeatureBatchTable* featurebatchtable)
    {
        if (!_owner->mWriteAttributeList.empty())
        {
            uint32 id = StringConverter::parseLong(name);
            if (Feature* f = _owner->mFeatureSource->getFeature(id))
            {
                const AttributeTable& table = f->getAttrs();
                for (auto attn : _owner->mWriteAttributeList)
                {
                    AttributeTable::const_iterator it = table.find(attn);
                    if (it != table.end())
                    {
                        if (it->second.first == ATTRTYPE_INT)
                        {
                            featurebatchtable->pushAttribute(attn, (long)it->second.getInt());
                        }
                        else if (it->second.first == ATTRTYPE_DOUBLE)
                        {

                            featurebatchtable->pushAttribute(attn, (float)it->second.getDouble());
                        }
                        else if (it->second.first == ATTRTYPE_BOOL)
                        {

                            featurebatchtable->pushAttribute(attn, it->second.getBool());
                        }
                        else
                        {
                            featurebatchtable->pushAttribute(attn, it->second.getString());
                        }
                    }
                }
            }
        }
    }

    struct FeatureData {
        double x, y;
        FeatureID fid;
    };

    struct SortByX {
        bool operator()(const FeatureData& lhs, const FeatureData& rhs) const {
            return lhs.x < rhs.x;
        }
    };

    struct SortByY {
        bool operator()(const FeatureData& lhs, const FeatureData& rhs) const {
            return lhs.y < rhs.y;
        }
    };

    struct FeatureTile
    {
        GeoExtent extent;
        FeatureList featureList;
        std::vector<FeatureData> dataList;
    };

    typedef std::vector<FeatureTile> FeatureTileList;

    auto tile2 = [](FeatureSource* mFeatureSource, const GeoExtent& extent, std::vector<FeatureData>& data)
    {
        FeatureTileList list;
        list.resize(2);

        bool isWide = extent.width() > extent.height();

        double median;

        if (isWide)
        {
            SortByX sortByX;
            std::sort(data.begin(), data.end(), sortByX);
            median = ((data.size() & 0x1) == 0) ?
                0.5 * (data[data.size() / 2].x + data[data.size() / 2].x) :
                data[data.size() / 2].x;
        }
        else
        {
            SortByY sortByY;
            std::sort(data.begin(), data.end(), sortByY);
            median = ((data.size() & 0x1) == 0) ?
                0.5 * (data[data.size() / 2].y + data[data.size() / 2].y) :
                data[data.size() / 2].y;
        }

        for (unsigned i = 0; i < data.size(); ++i)
        {
            const FeatureData& d = data[i];
            Feature* f = mFeatureSource->getFeature(d.fid);
            double x, y;
            f->getExtent().getCentroid(x, y);
            double side = isWide ? x : y;

            if (side < median)
            {
                list[0].featureList.push_back(f);
                list[0].dataList.push_back(d);
            }
            else
            {
                list[1].featureList.push_back(f);
                list[1].dataList.push_back(d);
            }
        }

        list[0].extent = GeoExtent(extent.getSRS());
        list[1].extent = GeoExtent(extent.getSRS());

        if (isWide)
        {
            list[0].extent.set(extent.xMin(), extent.yMin(), median, extent.yMax());
            list[1].extent.set(median, extent.yMin(), extent.xMax(), extent.yMax());
        }
        else
        {
            list[0].extent.set(extent.xMin(), extent.yMin(), extent.xMax(), median);
            list[1].extent.set(extent.xMin(), median, extent.xMax(), extent.yMax());
        }

        return list;
    };

    int BuildingConverter::split(const GeoExtent& extent, OC::Cesium::Tile* parentTile, unsigned depth, String destDir)
    {
        FeatureList features;

        Query query;
        query.bounds() = extent.bounds();
        osg::ref_ptr<FeatureCursor> cursor = mFeatureSource->createFeatureCursor(query, 0L);
        if (!cursor.valid())
        {
            OE_WARN << "Feature cursor error" << std::endl;
            return 0;
        }

        cursor->fill(features);
        if (features.empty())
            return 0;

        //ResampleFilter resample(error, DBL_MAX);
        //FilterContext fc(0L, env.input->getFeatureProfile(), extent);
        //resample.push(features, fc);

        // Just divide the error by 10 each level. Placeholder for something smarter.
        double error = mGeometricError[depth];
        std::vector<FeatureData> data;

        for (FeatureList::iterator i = features.begin(); i != features.end(); ++i)
        {
            Feature* f = i->get();

            const GeoExtent& fex = f->getExtent();

            double xmin,ymin,xmax,ymax;
            fex.getBounds(xmin, ymin, xmax, ymax);

            float wd = fex.width();
            float hd = fex.height();

            if (fex.getSRS()->isGeocentric() || fex.getSRS()->isGeographic()
                || fex.getSRS()->isGeodetic())
            {
                float w = GeoMath::distance(
                    osg::DegreesToRadians(ymin),
                    osg::DegreesToRadians(xmin),
                    osg::DegreesToRadians(ymax),
                    osg::DegreesToRadians(xmax + wd));
                float h = GeoMath::distance(
                    osg::DegreesToRadians(ymin),
                    osg::DegreesToRadians(xmin),
                    osg::DegreesToRadians(ymax + hd),
                    osg::DegreesToRadians(xmax));

                wd = w;
                hd = hd;
            }

            if (wd < error && hd < error)
                continue;

            FeatureID fid = f->getFID();
            if (mWritedFidMap.find(fid) != mWritedFidMap.end())
            {
                continue;
            }
            mWritedFidMap[fid] = 1;
            FeatureData d;
            fex.getCentroid(d.x, d.y);
            d.fid = fid;
            data.push_back(d);
        }

        if (data.empty())
        {
            parentTile->geometricError() = 0.0;
            return -1;
        }

        FeatureTileList featureTileList = tile2(mFeatureSource,extent,data);

        size_t i = 0;
        for (; i < featureTileList.size();i++)
        {
            FeatureTile& featureTile = featureTileList[i];
            FeatureList& list = featureTile.featureList;
            if (list.size() > mMaxNodeInTile)
            {
                FeatureTileList ft2 = tile2(mFeatureSource, featureTile.extent, featureTile.dataList);
                featureTile = ft2[0];
                featureTileList.push_back(ft2[1]);
                i--;
            }
        }

        for (auto& featureTile : featureTileList)
        {
            FeatureList& list = featureTile.featureList;

            if (list.empty())
            {
                continue;
            }

            int index = parentTile->children().size();

            String uri = Stringify() << destDir << index << ".b3dm";
            osg::ref_ptr<OC::Cesium::Tile> tile = buildTile(list, depth, uri);
            if (tile.valid())
            {
                tile->geometricError() = 0.0;
                if (depth < maxDepth)
                {
                    tile->geometricError() = parentTile->geometricError().get() * 0.5;
                    String nextDestDir = Stringify() << destDir << index << "/";
                    split(featureTile.extent, tile, depth + 1, nextDestDir);
                }

                parentTile->children().push_back(tile);
            }
        }

        return 0;
    }

    OC::Cesium::Tile* BuildingConverter::buildTile(FeatureList& list,int depth, String uri) {
        OC::Cesium::Tile* tile = NULL;
        
        osg::ref_ptr<osg::Node> node = extrude.push(list, mFilterContext);
        if (node.valid())
        {
            if (osg::Group* t = node->asGroup())
            {
                if (t->getNumChildren() == 0)
                {
                    return NULL;
                }

                tile = new OC::Cesium::Tile();
                //transform = t->asMatrixTransform()->getMatrix();
                osg::ref_ptr<osg::Group> g = new osg::Group;
                for (uint32 i = 0; i < t->getNumChildren(); i++)
                {
                    g->addChild(node->asGroup()->getChild(i));
                }

                String b3dmfilename = Stringify() << outputPath << "/" << uri;

                osgDB::makeDirectoryForFile(b3dmfilename);
                B3dmWriter writer;
                writer.getFeatureBatchTable()->setAttributeCallback(mBuildingAttributeCallback.get());
                writer.getGltfObject()->convert(g);
                if (!writer.writeToFile(b3dmfilename))
                {
                    tile = NULL;
                    return tile;
                }
                std::cout << "file:" << uri
                    << " size:" << StringConverter::formatDoubletoString(writer.bytesLength() / 1048576.0) << std::endl;

                osg::BoundingBox box = Cesium::CesiumTool::getBoundBox(g);
                mRootBoudingBox.expandBy(box);
                tile->boundingVolume()->box() = box;
                tile->content()->uri() = uri;
                tile->refine() = OC::Cesium::REFINE_ADD;
            }
        }

        return tile;
    };

	BuildingConverter::BuildingConverter()
	{
        mBuildingAttributeCallback = new BuildingAttributeCallback(this);
	}

	BuildingConverter::~BuildingConverter()
	{

	}

    void BuildingConverter::initStyleSheet()
    {
        Style style;
        ExtrusionSymbol* extrusion = style.getOrCreateSymbol<ExtrusionSymbol>();
        extrusion->height() = constantHeight;
        extrusion->heightExpression() = NumericExpression(heightExpression);
        extrude.setMergeGeometry(false);
        extrude.setFeatureNameExpr(StringExpression(nameExpression));

        if (FileUtil::FileExist(resource_lib))
        {
            // a style for the wall textures:
            Style wallStyle;
            wallStyle.setName("building-wall");
            SkinSymbol* wallSkin = wallStyle.getOrCreate<SkinSymbol>();
            wallSkin->library() = "resources";
            wallSkin->addTag("building");
            wallSkin->randomSeed() = 1;

            // a style for the rooftop textures:
            Style roofStyle;
            roofStyle.setName("building-roof");
            SkinSymbol* roofSkin = roofStyle.getOrCreate<SkinSymbol>();
            roofSkin->library() = "resources";
            roofSkin->addTag("rooftop");
            roofSkin->randomSeed() = 1;
            roofSkin->isTiled() = true;

            // assemble a stylesheet and add our styles to it:
            StyleSheet* styleSheet = new StyleSheet();
            styleSheet->addStyle(wallStyle);
            styleSheet->addStyle(roofStyle);

            extrusion->wallStyleName() = "building-wall";
            extrusion->roofStyleName() = "building-roof";
            // load a resource library that contains the building textures.
            ResourceLibrary* reslib = new ResourceLibrary("resources", resource_lib);
            styleSheet->addResourceLibrary(reslib);
            mFilterContext.getSession()->setStyles(styleSheet);
        }

        extrude.setStyle(style);
    }

	void BuildingConverter::readShp(String filename)
	{
        Cesium::gltfConfig::roughnessFactor = 0.7;
        Cesium::gltfConfig::metallicFactor = 0.1;
        Cesium::gltfConfig::GeodePerId = true;
        
        if (outputPath.empty())
        {
            outputPath = osgDB::getFilePath(filename) + "/3dtiles/";
        }

        if (outputPath[outputPath.length() - 1] != '/')
        {
            outputPath += '/';
        }

        osgDB::makeDirectory(outputPath);

        String errors;

        if (!mErrors.empty())
        {
            errors = mErrors;
        }
        else if (splitLevel == 3)
        {
            errors = String("1 80 200");
        }
        else
        {
            errors = String("1 80 160 200");
        }

        Config inputConf;
        inputConf.set("driver", "ogr");
        inputConf.set("url", filename);
        
        //mFeatureSource = FeatureSourceFactory::create(ConfigOptions(inputConf));

        mFeatureSource = new OGRFeatureSource();
        ((OGRFeatureSource*)mFeatureSource.get())->setURL(filename);
        assert(mFeatureSource != NULL);

        if (!mFeatureSource.valid())
            return;

        if (mFeatureSource->open().isError())
            return;

        std::vector<std::string> errorStrings;
        StringTokenizer(errors, errorStrings, " ");
        if (errorStrings.size() < 1)
            return;

        mGeometricError.resize(errorStrings.size());
        maxDepth = errorStrings.size() - 1;

        // generate level errors:
        for (int i = 0; i < errorStrings.size(); ++i)
        {
            int j = errorStrings.size() - 1 - i;
            mGeometricError[j] = osgEarth::as<double>(errorStrings[i], -1.0);
            if (mGeometricError[j] <= 0.0)
                return;
        }

        const FeatureProfile* inputFP = mFeatureSource->getFeatureProfile();
        if (inputFP && inputFP->getExtent().isValid())
        {
            const GeoExtent& fex = inputFP->getExtent();
            Profile::create(fex.getSRS(), fex.xMin(), fex.yMin(), fex.xMax(), fex.yMax());
        }

        static osg::ref_ptr<osgEarth::Map> map = new osgEarth::Map();
        mFilterContext = FilterContext(new Session(map), inputFP);
        
        initStyleSheet();
	}

	void BuildingConverter::output()
	{
        String outFile = Stringify() << outputPath << "/tileset.json";
        osg::ref_ptr<OC::Cesium::Tileset> tileset = _OC_NewTileset();

        const GeoExtent extent = mFeatureSource->getFeatureProfile()->getExtent();

        double error = Cesium::CesiumTool::computeGeometryError(extent) / 125.0;
        
        tileset->geometricError() = error * 2.0 * Cesium::gltfConfig::geometryErroRatio;
        tileset->root()->geometricError() = error * Cesium::gltfConfig::geometryErroRatio;

        int count = mFeatureSource->getFeatureCount();
        if (count > 0)
        {
            split(extent, tileset->root(), 0, "");
        }

        tileset->root()->boundingVolume()->box() = mRootBoudingBox;
        tileset->root()->transform() = transform;
        _OC_WriteTileset(tileset, outFile);
	}
}