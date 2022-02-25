#include "FuncArchive.h"
#include "OCMain/osgearth.h"

FuncArchive::FuncArchive(void)
{
	supportsExtension("func", "OC func");
}

FuncArchive::~FuncArchive(void)
{

}

String FuncArchive::invoke(StringVector params)
{
	String command = params.size() > 0 ? params[0] : StringUtil::BLANK;

	return StringUtil::BLANK;
}

CVector2 FuncArchive::gauss2LH(CVector2 coord)
{
	static String a = "+proj=tmerc +lat_0=0 +lon_0={%0} +k=1 +x_0=500000 +y_0=0 +ellps=IAU76 +units=m +no_defs";
	int dno = (int)coord.x - (int)coord.x % 1000000;
	int L0 = 114;
	if (dno == 0)
	{

	}
	else
	{
		dno /= 1000000;
		L0 = dno > 35 ? dno * 3 : dno * 6 - 3;
	}
	coord.x -= dno * 1000000;
	String prjcs = StringUtil::replaceAll(a,"{%0}",StringConverter::toString(L0));
	osgEarth::SpatialReference* local_srs = osgEarth::SpatialReference::get(prjcs);
	osgEarth::SpatialReference* wgs84_srs = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint local_point(local_srs, coord.x, coord.y);
	osgEarth::GeoPoint wgs84_point;
	bool ret = local_point.transform(wgs84_srs, wgs84_point);
	return CVector2(wgs84_point.x(), wgs84_point.y());
}

osgDB::ReaderWriter::ReadResult FuncArchive::openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int dep, const osgDB::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;
	return new FuncArchive;
}

REGISTER_OSGPLUGIN(func, FuncArchive)
