#include "OCRegister.h"

namespace OC
{
    String OC::Server::OCRegister::getRegister(StringVector params)
    {
        String code = params[1];
        String bios = params[2];
        String year = params[3];
        String month = params[4];
        String day = params[5];

        String jsonStr1 = "{\"function\":\"encodekey\",\"uuid\":\"{0}\",\"year\":{1},\"month\":{2},\"day\":{3}}";
        String jsonStr2 = "{\"function\":\"regiter\",\"code\":\"{0}\",\"year\":{1},\"month\":{2},\"day\":{3}}";

        jsonStr1 = StringUtil::replaceAll(jsonStr1, "{0}", bios);
        jsonStr1 = StringUtil::replaceAll(jsonStr1, "{1}", year);
        jsonStr1 = StringUtil::replaceAll(jsonStr1, "{2}", month);
        jsonStr1 = StringUtil::replaceAll(jsonStr1, "{3}", day);

        jsonStr2 = StringUtil::replaceAll(jsonStr2, "{0}", code);
        jsonStr2 = StringUtil::replaceAll(jsonStr2, "{1}", year);
        jsonStr2 = StringUtil::replaceAll(jsonStr2, "{2}", month);
        jsonStr2 = StringUtil::replaceAll(jsonStr2, "{3}", day);

        osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("license");
        String licenseKey = rw->readObject(jsonStr1).message();

        String countStr = rw->readObject(jsonStr2).message();
        int count = StringConverter::parseInt(countStr);
        if (count < 1)
        {
            return "register code is not valid";
        }

        return licenseKey;
    }
}

