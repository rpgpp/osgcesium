#ifndef OC_TDTILES_H_
#define OC_TDTILES_H_

#include "CesiumDefine.h"
#include "OCMain/osgearth.h"

using namespace osgEarth;

namespace OC { namespace Cesium
{
    enum RefinePolicy
    {
        REFINE_REPLACE,
        REFINE_ADD
    };

    struct LoadContext
    {
        URIContext _uc;
        RefinePolicy _defaultRefine;
    };

    class _CesiumExport Asset
    {
        OE_OPTION(std::string, version);
        OE_OPTION(std::string, tilesetVersion);
		OE_OPTION(std::string, gltfUpAxis);

        Asset() { }
        Asset(const OC::Json::Value& value) { fromJSON(value); }
        void fromJSON(const OC::Json::Value&);
        OC::Json::Value getJSON() const;
    };

    class _CesiumExport BoundingVolume
    {
        OE_OPTION(osg::BoundingBox, box);
        OE_OPTION(osg::BoundingBox, region);
        OE_OPTION(osg::BoundingSphere, sphere);

        BoundingVolume() { }
        BoundingVolume(const OC::Json::Value& value) { fromJSON(value); }
        void fromJSON(const OC::Json::Value&);
        OC::Json::Value getJSON() const;

        osg::BoundingSphere asBoundingSphere() const;
    };

    class _CesiumExport TileContent
    {
        OE_OPTION(BoundingVolume, boundingVolume);
        OE_OPTION(URI, uri);

        TileContent() { }
        TileContent(const OC::Json::Value& value, LoadContext& uc) { fromJSON(value, uc); }
        void fromJSON(const OC::Json::Value&, LoadContext&);
        OC::Json::Value getJSON() const;
    };

    class _CesiumExport Tile : public osg::Referenced
    {
    public:
        OE_OPTION(BoundingVolume, boundingVolume);
        OE_OPTION(BoundingVolume, viewerRequestVolume);
        OE_OPTION(double, geometricError);
        OE_OPTION(RefinePolicy, refine);
        OE_OPTION(osg::Matrix, transform);
        OE_OPTION(TileContent, content);
        OE_OPTION_VECTOR(osg::ref_ptr<Tile>, children);

        Tile() : _refine(REFINE_ADD) { }
        Tile(const OC::Json::Value& value, LoadContext& uc) { fromJSON(value, uc); }
        void fromJSON(const OC::Json::Value&, LoadContext& uc);
        OC::Json::Value getJSON() const;
    };

    class _CesiumExport Tileset : public osg::Referenced
    {
    public:
        OE_OPTION(Asset, asset);
        OE_OPTION(BoundingVolume, boundingVolume);
        //todo: properties
        OE_OPTION(double, geometricError);
        OE_OPTION_REFPTR(Tile, root);

        Tileset() { }
        Tileset(const OC::Json::Value& value, LoadContext& uc) { fromJSON(value, uc); }
        void fromJSON(const OC::Json::Value&, LoadContext& uc);
        OC::Json::Value getJSON() const;

        static Tileset* create(const std::string& tilesetJSON, const URIContext& uc);
    };
} }

inline OC::Cesium::Tileset* _OC_NewTileset()
{
    OC::Cesium::Tileset* tileset = new OC::Cesium::Tileset;
    tileset->asset()->version() = "1.0";
    tileset->asset()->tilesetVersion() = "1.0.1-OC23dtiles";
    tileset->asset()->gltfUpAxis() = "Z";
    tileset->root() = new OC::Cesium::Tile;
    return tileset;
}

inline void _OC_WriteTileset(OC::Cesium::Tileset* tileset, OC::String filename)
{
    std::ofstream fout(filename.c_str());
    OC::Json::Value json = tileset->getJSON();
    OC::Json::StyledStreamWriter writer;
    writer.write(fout, json);
    fout.close();
}

#endif //
