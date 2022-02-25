#ifndef _OC_VOLUME_TRANSFER_H_
#define _OC_VOLUME_TRANSFER_H_

#include "NetCDFDefine.h"

namespace OC
{
	class CTransferFunction : public osg::Referenced
	{
	public:
		CTransferFunction();
		virtual ~CTransferFunction();

		virtual void read(String filename);
		virtual osg::Vec4 getColor(float v) { return osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f); }
	};

	class CImageTransferFunction : public CTransferFunction
	{
	public:
		CImageTransferFunction();
		~CImageTransferFunction();

		virtual void read(String filename);
		virtual osg::Vec4 getColor(float v);

		osg::ref_ptr<osg::Image> tfImage;
	};

	class CFileTransferFunction : public CTransferFunction
	{
	public:
		CFileTransferFunction();
		~CFileTransferFunction();

		virtual void read(String filename);
		virtual osg::Vec4 getColor(float v);

		typedef std::map<float, osg::Vec4> ColorMap;
		ColorMap _colorMap;
	};
}

#endif