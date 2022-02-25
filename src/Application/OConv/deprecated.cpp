#include "ConvCommand.h"


void computeTexture(OC::Json::Value& root)
{
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	StringStream sstream;
	sstream << "noRotation" << std::endl;
	sstream << "noTesselateLargePolygons" << std::endl;
	sstream << "noTriStripPolygons" << std::endl;
	sstream << "noReverseFaces" << std::endl;
	options->setOptionString(sstream.str());

	CTextureVisitor v;
	String filename = JsonPackage::getString(root, "filename");
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename, options);
	v.traverse(*node);
	v.mBoundComputed = true;
	v.traverse(*node);

	String basename, extension, out_dir;
	StringUtil::splitFullFilename(filename, basename, extension, out_dir);
	String out_file = out_dir + basename + "_texture." + extension;
	osgDB::writeNodeFile(*node, out_file, options);
}
