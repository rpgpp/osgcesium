#include "ConvCommand.h"

void regiter(String jsonStr)
{
	osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("license");
	String encodeFunc = StringUtil::replaceAll(jsonStr, "regiter", "encodekey");
	String licenseKey = rw->readObject(encodeFunc).message();

	String countStr = rw->readObject(jsonStr).message();
	int count = StringConverter::parseInt(countStr);
	if (count < 1)
	{
		std::cout << "register code is not valid" << std::endl;
		return;
	}

	std::cout << licenseKey << std::endl;
}

void decodekey(String jsonStr)
{
	osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("license");
	String result = rw->readObject(jsonStr).message();
	std::cout << result << std::endl;
}

void encodekey(String jsonStr)
{
	decodekey(jsonStr);
}

void biosuuid(String jsonStr)
{
	osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("license");
	String uuid = rw->readObject("{\"function\":\"uuid\"}").message();
	std::cout << uuid << std::endl;
}