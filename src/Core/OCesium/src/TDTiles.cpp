#include "TDTiles.h"
#include <osgDB/FileNameUtils>

using namespace osgEarth;

namespace OC {
    namespace Cesium
    {
        void
            Asset::fromJSON(const OC::Json::Value& value)
        {
            if (value.isMember("version"))
                version() = value.get("version", "").asString();
            if (value.isMember("tilesetVersion"))
                tilesetVersion() = value.get("tilesetVersion", "").asString();
            if (value.isMember("gltfUpAxis"))
                gltfUpAxis() = value.get("gltfUpAxis", "").asString();
        }

        OC::Json::Value
            Asset::getJSON() const
        {
            Json::Value value(OC::Json::objectValue);
            if (version().isSet())
                value["version"] = version().get();
            if (tilesetVersion().isSet())
                value["tilesetVersion"] = tilesetVersion().get();
            if (gltfUpAxis().isSet())
                value["gltfUpAxis"] = gltfUpAxis().get();
            return value;
        }

        //........................................................................

        void
            BoundingVolume::fromJSON(const OC::Json::Value& value)
        {
            if (value.isMember("region"))
            {
                const OC::Json::Value& a = value["region"];
                if (a.isArray() && a.size() == 6)
                {
                    OC::Json::Value::const_iterator i = a.begin();
                    region()->xMin() = (*i++).asDouble();
                    region()->yMin() = (*i++).asDouble();
                    region()->xMax() = (*i++).asDouble();
                    region()->yMax() = (*i++).asDouble();
                    region()->zMin() = (*i++).asDouble();
                    region()->zMax() = (*i++).asDouble();
                }
                else OE_WARN << "Invalid region array" << std::endl;
            }

            if (value.isMember("sphere"))
            {
                const OC::Json::Value& a = value["sphere"];
                if (a.isArray() && a.size() == 4)
                {
                    OC::Json::Value::const_iterator i = a.begin();
                    sphere()->center().x() = (*i++).asDouble();
                    sphere()->center().y() = (*i++).asDouble();
                    sphere()->center().z() = (*i++).asDouble();
                    sphere()->radius() = (*i++).asDouble();
                }
            }
            if (value.isMember("box"))
            {
                const OC::Json::Value& a = value["box"];
                if (a.isArray() && a.size() == 12)
                {
                    OC::Json::Value::const_iterator i = a.begin();
                    osg::Vec3 center((*i++).asDouble(), (*i++).asDouble(), (*i++).asDouble());
                    osg::Vec3 xvec((*i++).asDouble(), (*i++).asDouble(), (*i++).asDouble());
                    osg::Vec3 yvec((*i++).asDouble(), (*i++).asDouble(), (*i++).asDouble());
                    osg::Vec3 zvec((*i++).asDouble(), (*i++).asDouble(), (*i++).asDouble());
                    box()->expandBy(center + xvec);
                    box()->expandBy(center - xvec);
                    box()->expandBy(center + yvec);
                    box()->expandBy(center - yvec);
                    box()->expandBy(center + zvec);
                    box()->expandBy(center - zvec);
                }
                else OE_WARN << "Invalid box array" << std::endl;
            }
        }

        OC::Json::Value
            BoundingVolume::getJSON() const
        {
            OC::Json::Value value(OC::Json::objectValue);

            if (region().isSet())
            {
                OC::Json::Value a(OC::Json::arrayValue);
                a.append(region()->xMin());
                a.append(region()->yMin());
                a.append(region()->xMax());
                a.append(region()->yMax());
                a.append(region()->zMin());
                a.append(region()->zMax());
                value["region"] = a;
            }
            else if (sphere().isSet())
            {
                OC::Json::Value a(OC::Json::arrayValue);
                a.append(sphere()->center().x());
                a.append(sphere()->center().y());
                a.append(sphere()->center().z());
                a.append(sphere()->radius());
                value["sphere"] = a;
            }
            else if (box().isSet())
            {
                OC::Json::Value a(OC::Json::arrayValue);
                a.append(box()->center().x());
                a.append(box()->center().y());
                a.append(box()->center().z());
                a.append((box()->xMax() - box()->xMin()) * 0.5);
                a.append(0.0);
                a.append(0.0);
                a.append(0.0);
                a.append((box()->yMax() - box()->yMin()) * 0.5);
                a.append(0.0);
                a.append(0.0);
                a.append(0.0);
                a.append((box()->zMax() - box()->zMin()) * 0.5);
                value["box"] = a;
            }
            return value;
        }

        osg::BoundingSphere
            BoundingVolume::asBoundingSphere() const
        {
            const SpatialReference* epsg4979 = SpatialReference::get("epsg:4979");
            if (!epsg4979)
                return osg::BoundingSphere();

            if (region().isSet())
            {
                GeoExtent extent(epsg4979,
                    osg::RadiansToDegrees(region()->xMin()),
                    osg::RadiansToDegrees(region()->yMin()),
                    osg::RadiansToDegrees(region()->xMax()),
                    osg::RadiansToDegrees(region()->yMax()));

                osg::BoundingSphered bs = extent.createWorldBoundingSphere(region()->zMin(), region()->zMax());
                return osg::BoundingSphere(bs.center(), bs.radius());
            }

            else if (sphere().isSet())
            {
                return sphere().get();
            }

            else if (box().isSet())
            {
                return osg::BoundingSphere(box()->center(), box()->radius());
            }

            return osg::BoundingSphere();
        }

        //........................................................................

        void
            TileContent::fromJSON(const OC::Json::Value& value, LoadContext& lc)
        {
            if (value.isMember("boundingVolume"))
                boundingVolume() = BoundingVolume(value.get("boundingVolume", OC::Json::nullValue));
            if (value.isMember("uri"))
                uri() = URI(value.get("uri", "").asString(), lc._uc);
            if (value.isMember("url"))
                uri() = URI(value.get("url", "").asString(), lc._uc);
        }

        OC::Json::Value
            TileContent::getJSON() const
        {
            OC::Json::Value value(OC::Json::objectValue);
            if (boundingVolume().isSet())
                value["boundingVolume"] = boundingVolume()->getJSON();
            if (uri().isSet())
                value["uri"] = uri()->base();
            return value;
        }

        //........................................................................

        void
            Tile::fromJSON(const OC::Json::Value& value, LoadContext& uc)
        {
            if (value.isMember("boundingVolume"))
                boundingVolume() = value["boundingVolume"];
            if (value.isMember("viewerRequestVolume"))
                viewerRequestVolume() = value["viewerRequestVolume"];
            if (value.isMember("geometricError"))
                geometricError() = value.get("geometricError", 0.0).asDouble();
            if (value.isMember("content"))
                content() = TileContent(value["content"], uc);

            if (value.isMember("refine"))
            {
                refine() = osgEarth::ciEquals(value["refine"].asString(), "add") ? REFINE_ADD : REFINE_REPLACE;
                uc._defaultRefine = refine().get();
            }
            else
            {
                refine() = uc._defaultRefine;
            }

            if (value.isMember("transform"))
            {
                const OC::Json::Value& digits = value["transform"];
                double c[16];
                if (digits.isArray() && digits.size() == 16)
                {
                    unsigned k = 0;
                    for (OC::Json::Value::const_iterator i = digits.begin(); i != digits.end(); ++i)
                        c[k++] = (*i).asDouble();
                    transform() = osg::Matrix(c);
                }
            }

            if (value.isMember("children"))
            {
                const OC::Json::Value& a = value["children"];
                if (a.isArray())
                {
                    for (OC::Json::Value::const_iterator i = a.begin(); i != a.end(); ++i)
                    {
                        osg::ref_ptr<Tile> tile = new Tile(*i, uc);
                        children().push_back(tile.get());
                    }
                }
            }
        }

        OC::Json::Value
            Tile::getJSON() const
        {
            OC::Json::Value value(OC::Json::objectValue);

            if (transform().isSet())
            {
                const osg::Matrix::value_type* ptr = transform()->ptr();

                OC::Json::Value a(OC::Json::arrayValue);
                a.append(ptr[0]);
                a.append(ptr[1]);
                a.append(ptr[2]);
                a.append(ptr[3]);
                a.append(ptr[4]);
                a.append(ptr[5]);
                a.append(ptr[6]);
                a.append(ptr[7]);
                a.append(ptr[8]);
                a.append(ptr[9]);
                a.append(ptr[10]);
                a.append(ptr[11]);
                a.append(ptr[12]);
                a.append(ptr[13]);
                a.append(ptr[14]);
                a.append(ptr[15]);
                value["transform"] = a;
            }

            if (boundingVolume().isSet())
                value["boundingVolume"] = boundingVolume()->getJSON();
            if (viewerRequestVolume().isSet())
                value["viewerRequestVolume"] = viewerRequestVolume()->getJSON();
            if (geometricError().isSet())
                value["geometricError"] = geometricError().get();
            if (refine().isSet())
                value["refine"] = (refine().get() == REFINE_ADD) ? "add" : "replace";
            if (content().isSet())
                value["content"] = content()->getJSON();


            if (!children().empty())
            {
                OC::Json::Value collection(OC::Json::arrayValue);
                for (unsigned i = 0; i < children().size(); ++i)
                {
                    Tile* child = children()[i].get();
                    if (child)
                    {
                        collection.append(child->getJSON());
                    }
                }
                value["children"] = collection;
            }

            return value;
        }

        //........................................................................

        void
            Tileset::fromJSON(const OC::Json::Value& value, LoadContext& uc)
        {
            if (value.isMember("asset"))
                asset() = Asset(value.get("asset", OC::Json::nullValue));
            if (value.isMember("boundingVolume"))
                boundingVolume() = BoundingVolume(value.get("boundingVolume", OC::Json::nullValue));
            if (value.isMember("geometricError"))
                geometricError() = value.get("geometricError", 0.0).asDouble();
            if (value.isMember("root"))
                root() = new Tile(value["root"], uc);
        }

        OC::Json::Value
            Tileset::getJSON() const
        {
            OC::Json::Value value(OC::Json::objectValue);
            if (asset().isSet())
                value["asset"] = asset()->getJSON();
            if (boundingVolume().isSet())
                value["boundingVolume"] = boundingVolume()->getJSON();
            if (geometricError().isSet())
                value["geometricError"] = geometricError().get();
            if (root().valid())
                value["root"] = root()->getJSON();
            return value;
        }

        Tileset*
            Tileset::create(const std::string& json, const URIContext& uc)
        {
            OC::Json::Reader reader;
            OC::Json::Value root(OC::Json::objectValue);
            if (!reader.parse(json, root, false))
                return NULL;

            LoadContext lc;
            lc._uc = uc;
            lc._defaultRefine = REFINE_REPLACE;

            return new Tileset(root, lc);
        }

    }
}