#include "Volume.h"
#include "CinRadReader.h"
#include "CinRadReaderFMT.h"
#include "Swan.h"
#include "Vtk.h"
#include "FyAwx.h"
#include "OCZip/BZip2.h"

namespace OC
{
    namespace Volume
    {
        CVolume::CVolume()
        {

        }

        CVolume::~CVolume()
        {

        }

        inline bool isRadarFMT(String name)
        {
            return StringUtil::contains(name, "FMT", false);
        }

        CVolumeObject* CVolume::convert(String filename, String varname)
        {
            String name, ext, path;
            StringUtil::splitFullFilename(filename, name, ext, path);
            StringUtil::toUpperCase(name);
            StringUtil::toLowerCase(ext);
            if (varname.empty())
            {
                varname = ext;
            }

            osg::ref_ptr<CVolumeObject> volume;

            if (varname == "EF6A0A74" || varname == "CD25FDE6")
            {
                if (isRadarFMT(name) || varname == "CD25FDE6")
                {
                    volume = new CinRadReaderFMT;
                }
                else
                {
                    volume = new CinRadReader(name);
                }

                if (ext == "bz2")
                {
                    std::stringstream sstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
                    size_t size = BZip2::openBZ2(filename, sstream);
                    if (size > 0)
                    {
                        volume->convert(sstream);
                    }
                }
                else if (ext == "bin")
                {
                    std::ifstream ifs(filename.c_str(), std::ios::binary);
                    if (ifs)
                    {
                        volume->convert(ifs);
                        ifs.close();
                    }
                }
            }
            else if (varname == "DBZ" && ext == "bz2")
            {
                osg::ref_ptr<CSwan> volumeObject;
                std::stringstream sstream;
                size_t size = BZip2::openBZ2(filename, sstream);
                if (size > 0)
                {
                    volume = volumeObject = new CSwan;
                    volumeObject->convert(sstream);
                }
            }
            else if (ext == "awx")
            {
                volume = new FyAwx;
                volume->convert(filename);
            }
            else if (ext == "vtk")
            {
                osg::ref_ptr<Vtk> volumeObject;
                volume = volumeObject = new Vtk;
                volumeObject->convert(filename);
            }
            else
            {
                if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("nc"))
                {
                    osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
                    options->setPluginStringData("varname",varname);
                    volume = dynamic_cast<CVolumeObject*>(rw->readObject(filename, options).takeObject());
                }
            }

            if (volume.valid())
            {
                volume->setVarName(varname);
                if (volume->uint8Image() == NULL)
                {
                    volume = NULL;
                }
            }
            return volume.release();
        }
    }
}