#include "ShpDrawer.h"

using namespace OC;

CShpDrawer::CShpDrawer()
{
	osgEarth::Registry::instance();
}

CShpDrawer::~CShpDrawer()
{

}

void CShpDrawer::pushPoint(CVector2 point)
{
	osgEarth::PointSet* p = new osgEarth::PointSet();
	p->push_back(point.x, point.y);
	mPoints.push_back(p);
}

osgEarth::LineString* CShpDrawer::pushLine()
{
	mLines.push_back(new osgEarth::LineString());
	return mLines[mLines.size() - 1];
}

osgEarth::Polygon* CShpDrawer::pushPolygon()
{
	mPolygons.push_back(new osgEarth::Polygon());
	return mPolygons[mPolygons.size() - 1];
}

void CShpDrawer::setExtent(float xmin, float ymin, float xmax, float ymax)
{
	mExtent.setMinimum(xmin, ymin);
	mExtent.setMaximum(xmax, ymax);
}


void CShpDrawer::output(String path)
{
	CVector2 minimum = mExtent.getMinimum();
	CVector2 maximum = mExtent.getMaximum();

	osgDB::makeDirectory(path);
	const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("epsg:4326");
	osgEarth::GeoExtent extent(srs, minimum.x, minimum.y, maximum.x, maximum.y);
	osgEarth::FeatureSchema schema;
	schema["name"] = osgEarth::ATTRTYPE_STRING;
	osg::ref_ptr<osgEarth::FeatureProfile> featureProfile = new osgEarth::FeatureProfile(extent);
	if(!mLines.empty())
	{
		osg::ref_ptr<osgEarth::OGRFeatureSource> output = new osgEarth::OGRFeatureSource;
		output->setURL(path + "/line.shp");
		output->setOpenWrite(true);
		const osgEarth::Status& outputStatus = output->create(
			featureProfile,
			schema,
			osgEarth::Geometry::TYPE_UNKNOWN,
			NULL);
		if (outputStatus.isOK())
		{
			for (auto i : mLines)
			{
				osg::ref_ptr<osgEarth::Feature> f = new osgEarth::Feature(i.get(), extent.getSRS());
				output->insertFeature(f);
			}
		}
	}
	if (!mPolygons.empty())
	{
		osg::ref_ptr<osgEarth::OGRFeatureSource> output = new osgEarth::OGRFeatureSource;
		output->setURL(path + "/polygon.shp");
		output->setOpenWrite(true);
		const osgEarth::Status& outputStatus = output->create(
			featureProfile,
			schema,
			osgEarth::Geometry::TYPE_UNKNOWN,
			NULL);
		if (outputStatus.isOK())
		{
			int i = 0;
			for (auto g : mPolygons)
			{
				osg::ref_ptr<osgEarth::Feature> f = new osgEarth::Feature(g.get(), extent.getSRS());
				if (mPolygonNames.size() == mPolygons.size())
				{
					f->set("name", mPolygonNames[i]);
				}
				output->insertFeature(f);
				i++;
			}
		}
	}

	if (!mPoints.empty())
	{
		osg::ref_ptr<osgEarth::OGRFeatureSource> output = new osgEarth::OGRFeatureSource;
		output->setURL(path + "/point.shp");
		output->setOpenWrite(true);
		const osgEarth::Status& outputStatus = output->create(
			featureProfile,
			schema,
			osgEarth::Geometry::TYPE_UNKNOWN,
			NULL);
		if (outputStatus.isOK())
		{
			int i = 0;
			for (auto g : mPoints)
			{
				osg::ref_ptr<osgEarth::Feature> f = new osgEarth::Feature(g.get(), extent.getSRS());
				if (mPointNames.size() == mPoints.size())
				{
					f->set("name", mPointNames[i]);
				}
				output->insertFeature(f);
				i++;
			}
		}
	}
}

void CShpDrawer::test(Vector2List list, String outputfile)
{

	const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("epsg:4326");
	osgEarth::GeoExtent extent(srs, 0, 0, 180, 90);
	osg::ref_ptr<osgEarth::FeatureProfile> featureProfile = new osgEarth::FeatureProfile(osgEarth::GeoExtent(srs, 0, 0, 90, 90));
	osgEarth::FeatureSchema schema;

	osg::ref_ptr<osgEarth::OGRFeatureSource> output = new osgEarth::OGRFeatureSource;
	output->setURL(outputfile);
	output->setOpenWrite(true);
	const osgEarth::Status& outputStatus = output->create(
		featureProfile,
		schema,
		osgEarth::Geometry::TYPE_UNKNOWN,
		NULL);

	for (auto i : list)
	{
		osg::ref_ptr<osgEarth::PointSet> pointset = new osgEarth::PointSet();
		pointset->push_back(osg::Vec3d(i.x, i.y, 0));
		osg::ref_ptr<osgEarth::Feature> f = new osgEarth::Feature(pointset.get(), extent.getSRS());
		output->insertFeature(f);
	}
}

void CShpDrawer::test(Vector3List list, String outputfile)
{
	const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("epsg:4326");
	osgEarth::GeoExtent extent(srs, 0, 0, 180, 90);
	osg::ref_ptr<osgEarth::FeatureProfile> featureProfile = new osgEarth::FeatureProfile(osgEarth::GeoExtent(srs, 0, 0, 90, 90));
	osgEarth::FeatureSchema schema;
	schema["z"] = osgEarth::ATTRTYPE_DOUBLE;

	osg::ref_ptr<osgEarth::OGRFeatureSource> output = new osgEarth::OGRFeatureSource;
	output->setURL(outputfile);
	output->setOpenWrite(true);
	const osgEarth::Status& outputStatus = output->create(
		featureProfile,
		schema,
		osgEarth::Geometry::TYPE_UNKNOWN,
		NULL);

	for (auto i : list)
	{
		osg::ref_ptr<osgEarth::PointSet> pointset = new osgEarth::PointSet();
		pointset->push_back(osg::Vec3d(i.x, i.y, 0));
		osg::ref_ptr<osgEarth::Feature> f = new osgEarth::Feature(pointset.get(), extent.getSRS());
		f->set("z",i.z);
		output->insertFeature(f);
	}
}