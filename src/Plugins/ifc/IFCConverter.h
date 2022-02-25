#pragma once

namespace osg
{
	class Node;
}
class IFCConverter
{
public:
	IFCConverter(void);
	~IFCConverter(void);


	osg::Node* read(const char* file);
};

