#ifndef _OC_Base_Writer_H
#define _OC_Base_Writer_H

#include "CesiumDefine.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport BaseWriter : public osg::Object
        {
        public:
            BaseWriter() {}
            virtual ~BaseWriter() {};

            BaseWriter(const BaseWriter& eh, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
            META_Object(OCesium, BaseWriter);

            virtual bool writeToFile(String filename) { return false; }
            virtual bool writeToStream(std::ostream& output) { return false; }
            virtual uint32 bytesLength(){return mBytesLength;}

        protected:
            uint32							mBytesLength = 0;
        };
}}


#endif