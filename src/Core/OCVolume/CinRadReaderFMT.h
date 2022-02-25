#ifndef _OC_VOLUME_CINRAD_FMT_H__
#define _OC_VOLUME_CINRAD_FMT_H__

#include "VolumeObject.h"
#include "CinRadHeader.h"
#include "CinRad.h"

namespace OC
{
    namespace Volume
    {
        class CinRadReaderFMT : public CinRad
        {
        public:
            CinRadReaderFMT();
            ~CinRadReaderFMT();
            bool convert(std::istream& sstream);
        protected:
            void parseData(std::istream& sstream);
            void generateImage();
        private:
            typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
            typedef std::vector<CINRAD_DataFMT> TList;
            typedef std::map<int, TList > TListMap;
            TListMap                mPitchRadialMapDBZ;
        };
    }
}



#endif // !_OC_VOLUME_CINRAD_FMT_H__
