#pragma once

#include "NcVolume.h"
#include "OCMain/DataStream.h"
#include "OCVolume/VolumeObject.h"

namespace OC
{
    namespace Volume
    {
        class npyTemprature : public Volume::CVolumeObject
        {
        public:
            npyTemprature(String varname);
            ~npyTemprature();
            virtual bool convert(std::string filename);
        private:
            String mVarname;
        };
    }
}


