#ifndef _OC_SHP_DRAWER_H__
#define _OC_SHP_DRAWER_H__

#include "OCUtility/StringConverter.h"
#include "OCMain/osg.h"
#include "OCMain/osgearth.h"
#include "OCMain/JsonPackage.h"

namespace OC
{
	class CShpDrawer
	{
	public:
		CShpDrawer();
		~CShpDrawer();

		void pushPoint(float x, float y) { pushPoint(CVector2(x, y)); }
		void pushPoint(CVector2 point);
		osgEarth::LineString* pushLine();
		osgEarth::Polygon* pushPolygon();
		void pushPointName(String name) { mPointNames.push_back(name); }
		void pushPolygonName(String name) { mPolygonNames.push_back(name); }
		void output(String path);
		void test(Vector2List list,String outputfile);
		void test(Vector3List list,String outputfile);

		void setExtent(float  xmin,float ymin,float xmax,float ymax);
	private:
		CRectangle											mExtent = CRectangle(-180, -90, 180, 90);
		std::vector<String>									mPolygonNames;
		std::vector<String>									mPointNames;
		std::vector<osg::ref_ptr<osgEarth::Polygon> >		mPolygons;
		std::vector<osg::ref_ptr<osgEarth::LineString> >	mLines;
		std::vector<osg::ref_ptr<osgEarth::PointSet> >		mPoints;

	};
}


#endif // !_OC_SHP_DRAWER_H__




