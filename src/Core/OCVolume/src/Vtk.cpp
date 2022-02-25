#include "Vtk.h"
#include "OCMain/ConfigManager.h"
#include "OCMain/OCPluginTool.h"

using namespace OC;
using namespace OC::Volume;

Vtk::Vtk(String varname)
	:mVarname(varname)
{
	mNeedStatistic = true;
	mFillValue = -9999.0;
	mUseXYZ = mVarname == "xyz";
	mConvertGauss = false;
	if (SingletonPtr(ConfigManager))
	{
		mConvertGauss = Singleton(ConfigManager).getBoolValue("VtkConvertGauss", false);
		mUseXYZ = Singleton(ConfigManager).getBoolValue("VtkUseXYZ", true);
		mNeedStatistic = Singleton(ConfigManager).getBoolValue("StatisticVolume", true);
		mFillValue = Singleton(ConfigManager).getDoubleValue("FillValue", -999999);
	}
}

Vtk::~Vtk()
{

}

bool Vtk::convert(std::string filename)
{
	osg::Vec3d center(116.0, 29.0, 0.0);
	mClampMin = 99999.0;
	mClampMax = -mClampMin;
	std::ifstream ifs(filename.c_str());
	if (ifs)
	{
		uint32 dim_x = 0;
		uint32 dim_y = 0;
		uint32 dim_z = 0;
		uint32 point_data = 0;
		osg::Vec3 origin(1.0, 1.0, 1.0);
		osg::Vec3 aspect_ratio(1.0, 1.0, 1.0);
		int headerLines = 10;
		String line;
		for (int i = 0; i < headerLines; i++)
		{
			getline(ifs, line);
			if (line.empty())
			{
				break;
			}
			else if (StringUtil::startsWith(line, "DIMENSIONS", false))
			{
				StringVector sv = StringUtil::split(line);
				if (sv.size() == 4)
				{
					dim_x = StringConverter::parseInt(sv[1]);
					dim_y = StringConverter::parseInt(sv[2]);
					dim_z = StringConverter::parseInt(sv[3]);
				}
			}
			else if (StringUtil::startsWith(line, "POINT_DATA", false))
			{
				StringVector sv = StringUtil::split(line);
				if (sv.size() == 2)
				{
					point_data = StringConverter::parseInt(sv[1]);
				}
			}
			else if (StringUtil::startsWith(line, "ASPECT_RATIO", false))
			{
				StringVector sv = StringUtil::split(line);
				if (sv.size() == 4)
				{
					aspect_ratio.x() = StringConverter::parseInt(sv[1]);
					aspect_ratio.y() = StringConverter::parseInt(sv[2]);
					aspect_ratio.z() = StringConverter::parseInt(sv[3]);
				}
			}
			else if (StringUtil::startsWith(line, "ORIGIN", false))
			{
				StringVector sv = StringUtil::split(line);
				if (sv.size() == 4)
				{
					origin.x() = StringConverter::parseInt(sv[1]);
					origin.y() = StringConverter::parseInt(sv[2]);
					origin.z() = StringConverter::parseInt(sv[3]);
					if (SingletonPtr(OCPluginTool))
					{
						CVector2 LH = Singleton(OCPluginTool).getFunctional()->gauss2LH(CVector2(origin.x(), origin.y()));
						center.x() = LH.x;
						center.y() = LH.y;
					}
				}
			}
		}
		uint32 num = dim_x * dim_y * dim_z;

		if (point_data != num)
		{
			return false;
		}
		
		double sum = 0.0;
		double averageVal = 0.0;
		uint32 count = 0;
		doubleVector dv;
		for (uint32 i = 0; i < num; i++)
		{
			getline(ifs, line);
			if (line.empty())
			{
				return false;
			}
			float value = StringConverter::parseReal(line);
			if (Math::RealEqual(mFillValue, value))
			{
				value = 0.0;
			}
			else
			{
				mClampMin = (std::min)(mClampMin, value);
				mClampMax = (std::max)(mClampMax, value);
			}
			sum += value;
			dv.push_back(value);
		}

		if (mNeedStatistic)
		{
			doubleVector pdDisDifData;
			pdDisDifData.resize(num);
			averageVal = sum / num;

			for (int i = 0; i < num; i++)
			{
				pdDisDifData[i] = dv[i] - averageVal;
			}
			double dDisDifSqrSum = 0.0;
			for (int i = 0; i < num; i++)
			{
				dDisDifSqrSum += pdDisDifData[i] * pdDisDifData[i];
			}
			double m_dDifDisSqrSumVal = dDisDifSqrSum;
			double m_dSqrDifVal = m_dDifDisSqrSumVal / num;
			double m_dAverDifVal = sqrt(m_dSqrDifVal);

			setStatistic("min", mClampMin);
			setStatistic("max", mClampMax);
			setStatistic("aver", averageVal);
			setStatistic("variance", m_dAverDifVal);
		}
		

		float scaler = mClampMax - mClampMin;
		int imageS = dim_x;
		int imageT = dim_y;
		int imageR = dim_z;
		if (!mUseXYZ)
		{
			imageS = dim_z;
			imageR = dim_x;
		}
		osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);
		long offset = 0;
		for (long z = 0; z < imageR; z++)
		{
			for (long y = 0; y < imageT; y++)
			{
				for (long x = 0; x < imageS; x++)
				{
					float v = dv[offset];
					osg::Vec4ub color = _EncodeValue(v);
					unsigned char* data = (unsigned char*)image->data(x, y, z);
					data[0] = color.r();
					data[1] = color.g();
					data[2] = data[3] = 0;

					if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
					{
						float px = dv[offset + 1];
						float nx = dv[offset - 1];
						float py = dv[offset + imageS];
						float ny = dv[offset - imageS];

						osg::Vec2 octVec = computeNormal(v, px, nx, py, ny);
						data[2] = octVec.x();
						data[3] = octVec.y();
					}

					offset++;
				}
			}
		}

		int w_meters = dim_x * aspect_ratio.x();
		int h_meters = dim_y * aspect_ratio.y();
		int d_meters = dim_z * aspect_ratio.z();
		setPosition(center);
		setMeter(osg::Vec3d(w_meters, h_meters, d_meters));
		setUint8Image(image);
	}

	return true;
}
