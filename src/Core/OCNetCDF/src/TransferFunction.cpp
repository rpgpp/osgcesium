#include "TransferFunction.h"

namespace OC
{

	CTransferFunction::CTransferFunction()
	{

	}

	CTransferFunction::~CTransferFunction()
	{

	}

	void CTransferFunction::read(String filename)
	{

	}

	CImageTransferFunction::CImageTransferFunction()
	{

	}

	CImageTransferFunction::~CImageTransferFunction()
	{

	}

	void CImageTransferFunction::read(String filename)
	{
		tfImage = osgDB::readImageFile(filename);
	}

	osg::Vec4 CImageTransferFunction::getColor(float v)
	{
		bool vertical = true;

		unsigned char* data = NULL;

		if (vertical)
		{
			int pos = tfImage->t() * v;
			data = tfImage->data(0, pos, 0);
		}
		else
		{
			int pos = tfImage->s() * v;
			data = tfImage->data(pos, 0, 0);
		}

		const osg::Vec4 color1 = osg::Vec4(0.0f, 0.0f, 255.0f, 25.0f);
		const osg::Vec4 color2 = osg::Vec4(255.0f, 0.0f, 0.0f, 255.0f);

		osg::Vec4 color = color1 * (1.0 - v) + color2 * v;

		color.x() = data[0];
		color.y() = data[1];
		color.z() = data[2];
		color.w() = v * 255.0;

		return color;
	}

	CFileTransferFunction::CFileTransferFunction()
	{

	}

	CFileTransferFunction::~CFileTransferFunction()
	{

	}

	void CFileTransferFunction::read(String filename)
	{
		osgDB::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
		while (fin)
		{
			char readline[4096];
			*readline = 0;
			fin.getline(readline, sizeof(readline));

			if (*readline == 0) continue;

			if (*readline == '#')
			{
				OSG_NOTICE << "comment = [" << readline << "]" << std::endl;
			}
			else
			{
				std::stringstream str(readline);

				std::string value;
				str >> value;

				{

					std::string red, green, blue, alpha;
					str >> red >> green >> blue >> alpha;

					*readline = 0;
					str.getline(readline, sizeof(readline));

					char* comment = readline;
					while (*comment == ' ' || *comment == '\t') ++comment;

					_colorMap[osg::asciiToFloat(value.c_str())] = osg::Vec4(osg::asciiToFloat(red.c_str()), osg::asciiToFloat(green.c_str()), osg::asciiToFloat(blue.c_str()), osg::asciiToFloat(alpha.c_str()));
				}
			}
		}
	}

	osg::Vec4 CFileTransferFunction::getColor(float v)
	{
		if (_colorMap.empty()) return osg::Vec4(255.0f, 255.0f, 255.0f, 255.0f);
		if (_colorMap.size() == 1) return _colorMap.begin()->second;

		if (v <= _colorMap.begin()->first) return _colorMap.begin()->second;
		if (v >= _colorMap.rbegin()->first) return _colorMap.rbegin()->second;

		std::pair<osg::TransferFunction1D::ColorMap::const_iterator, osg::TransferFunction1D::ColorMap::const_iterator>
			range = _colorMap.equal_range(v);

		return range.first->second;
	}

}