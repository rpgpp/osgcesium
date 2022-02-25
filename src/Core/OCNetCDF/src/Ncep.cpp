#include "Ncep.h"


using namespace OC;

#define GFS_URL "https://nomads.ncep.noaa.gov/cgi-bin/filter_{DIR}_${RES}.pl?file={DIR}.t${GFS_TIME}z.pgrb2.${RES}.f000&${LEVEL}&subregion=&${BBOX}&dir=%2F{DIR}.${GFS_DATE}%2F${GFS_TIME}%2Fatmos"

Ncep::Ncep()
{

}

Ncep::~Ncep()
{

}

osg::Image* Ncep::readImage(String filename, String var_name)
{
    return NULL;
}


