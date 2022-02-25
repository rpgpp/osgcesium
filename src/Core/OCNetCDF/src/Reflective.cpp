#include "Reflective.h"

namespace OC
{
	class NC_Relective
	{
	public:
		NC_Relective()
			:lon(NULL)
			, lat(NULL)
			, dbz(NULL)
		{
		}
		~NC_Relective()
		{
			CORE_SAFE_DELETE_ARRAY(lon);
			CORE_SAFE_DELETE_ARRAY(lat);
			CORE_SAFE_DELETE_ARRAY(dbz);
		}

		long _dimz, _dimx, _dimy;
		double* lon;
		double* lat;
		char* dbz;
		int* level;
		int _Dimension = 3;
	};

	Reflective::Reflective()
	{

	}

	Reflective::~Reflective()
	{

	}

	osg::Image* Reflective::readImage(String filename, String var_name)
	{
		int _varid;
		NCFunction(nc_inq_varid(_ncid, var_name.c_str(), &_varid));

		int _lonid, _latid, _levelid;
		NCFunction(nc_inq_varid(_ncid, "lat", &_latid));
		NCFunction(nc_inq_varid(_ncid, "lon", &_lonid));
		NCFunction(nc_inq_varid(_ncid, "level", &_levelid));

		int dimids[NC_MAX_VAR_DIMS];
		memset(&dimids,0,sizeof(int)* NC_MAX_VAR_DIMS);
		nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		NC_Relective zq;
		char dname[128];
		ncdiminq(_ncid, dimids[0], dname, &zq._dimz);
		ncdiminq(_ncid, dimids[1], dname, &zq._dimy);
		ncdiminq(_ncid, dimids[2], dname, &zq._dimx);

		long xySize = zq._dimy * zq._dimx;
		long xyzSize = zq._dimz * zq._dimy * zq._dimx;

		zq.level = new int[zq._dimz];
		zq.lon = new double[zq._dimx];
		zq.lat = new double[zq._dimy];
		zq.dbz = new char[xyzSize];
		
		int imageS = zq._dimx;
		int imageT = zq._dimy;
		int imageR = zq._dimz;

		nc_get_var(_ncid, _lonid, zq.lon);
		nc_get_var(_ncid, _latid, zq.lat);
		nc_get_var(_ncid, _levelid, zq.level);
		nc_get_var(_ncid, _varid, zq.dbz);

		osg::ref_ptr<osg::Image> image = new osg::Image;
		image->allocateImage(imageS, imageT, imageR, GL_RGBA, GL_UNSIGNED_BYTE);
		int component = osg::Image::computeNumComponents(image->getPixelFormat());
		memset(image->data(0, 0, 0), 0, imageS * imageT * imageR * component);

		CRectangle rectangle;
		mClampMin = 0.0;
		mClampMax = 70.0;
		float scaler = mClampMax - mClampMin;
		long offset = 0;
		double d_meters = 0.0;
		for (long z = 0; z < imageR; z++)
		{
			d_meters = max(d_meters, zq.level[z]);
			for (long y = imageT - 1; y > -1; y--)
			{
				for (long x = 0; x < imageS; x++, offset++)
				{
					if (z == 0)
					{
						rectangle.merge(CVector2(zq.lon[x], zq.lat[y]));

					}
					char dbz = zq.dbz[offset];
					float v = 3.33 * dbz + 44.0;
					osg::Vec4 color = RGBAEncoder::encodeFloatRGBA(Math::Clamp((v - mClampMin) / scaler, 0.0f, 0.999999f)) * 255.0;

					unsigned char* data = (unsigned char*)image->data(x, y, z);
					data[0] = color.r();
					data[1] = color.g();
					data[2] = color.b();
					data[3] = color.a();
				}
			}
		}


		float w_meters = width_in_meter(rectangle.getMinimum().y, rectangle.getWidth());
		float h_meters = height_in_meter(rectangle.getHeight());

		setPosition(rectangle.getCenter().x, rectangle.getCenter().y);
		setMeter(w_meters, h_meters, d_meters);
		CVolume::setVolumeAttribute(image);
		return image.release();
	}

}
