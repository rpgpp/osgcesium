#ifndef _OC_VOLUME_CinRad_Reader_H__
#define _OC_VOLUME_CinRad_Reader_H__

#include "VolumeObject.h"
#include "CinRad.h"

namespace OC
{
    namespace Volume
    {
        class CinRadReader : public CinRad
        {
        public:
            CinRadReader(String name);
            virtual ~CinRadReader();
            virtual bool convert(std::istream& sstream);
        protected:
            void parseName(String name);
            void parseData(std::istream& sstream);
            void generateImage();
        private:
            typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
            typedef std::vector<CINRAD_RecordCommon> TList;
            typedef std::map<int, TList > TListMap;
            TListMap                mPitchRadialMapDBZ;
        };
    }
}



#endif // !_OC_VOLUME_CINRAD_FMT_H__
