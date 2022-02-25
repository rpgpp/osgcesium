#include "ModelUtility.h"
#include "PipeTable.h"

namespace OC
{
	namespace Modeling
	{
		ModelUtility::ModelUtility(void)
		{
		}

		ModelUtility::~ModelUtility(void)
		{
		}

		osg::StateSet* ModelUtility::createTextureStateset(String imageurl,bool readimage)
		{
			osg::StateSet* stateset = new osg::StateSet;

			osg::ref_ptr<osg::Image> image;
			if (readimage)
			{
				image = osgDB::readImageFile(imageurl);
			}

			if (!image.valid())
			{
				image = new osg::Image;
			}

			image->setFileName(imageurl);
			osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D(image);
			tex2d->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
			tex2d->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
			tex2d->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);  
			stateset->setTextureAttributeAndModes(0,tex2d,osg::StateAttribute::ON);

			return stateset;
		}

		void ModelUtility::setTexture(osg::Node* node,String texture,bool tInternal)
		{
			if (node == NULL || texture.empty())
			{
				return;
			}

			osg::StateSet *state = node->getOrCreateStateSet();
			if (state != NULL)
			{
				osg::ref_ptr<osg::Image> image;
				if (tInternal)
				{
					image = osgDB::readImageFile(texture);
				}

				if (!image.valid())
				{
					image = new osg::Image;
				}

				image->setFileName(texture);
				osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D(image);
				tex2d->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
				tex2d->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
				tex2d->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);  
				state->setTextureAttributeAndModes(0,tex2d,osg::StateAttribute::ON);
			}
		}

		String ModelUtility::fixshelftable(String table)
		{
			if (table == IModelTable::PillarReservedMold)
			{
				return IModelTable::PillarMold;
			}
			else if (table == IModelTable::PipeRackColumnReservedMold)
			{
				return IModelTable::PipeRackColumnMold;
			}

			return table;
		}

		void ModelUtility::getMinMaxSection(LinePtrList linkLines,PipeSection& minSect,PipeSection& maxSect)
		{
			if (linkLines.size() > 0)
			{
				for (int i=0;i<linkLines.size();i++)
				{
					PipeLine* line = linkLines[i];

					PipeSection& section = line->getSection();

					if (section.getType() == UNKNOWM_SECTION)
					{
						continue;
					}

					if (minSect.getType() == UNKNOWM_SECTION)
					{
						minSect = section;
					}

					if (maxSect.getType() == UNKNOWM_SECTION)
					{
						maxSect = section;
					}

					if (section.getRadius() < minSect.getRadius())
					{
						minSect.setType(section.getType());
					}

					if (section.getRadius() > maxSect.getRadius())
					{
						maxSect.setType(section.getType());
					}

					minSect.setHeight(min(minSect.getHeight(),section.getHeight()));
					minSect.setWidth(min(minSect.getWidth(),section.getWidth()));
					minSect.setRadius(min(minSect.getRadius(),section.getRadius()));
					maxSect.setHeight(max(maxSect.getHeight(),section.getHeight()));
					maxSect.setWidth(max(maxSect.getWidth(),section.getWidth()));
					maxSect.setRadius(max(maxSect.getRadius(),section.getRadius()));
				}
			}
		}

		Vector3List ModelUtility::parseGeom(uchar* ptr)
		{
			Vector3List list;

			uint32 wkbtype=0;	
			int Dimension = 2;
			memcpy(&wkbtype, ptr + 1, sizeof( unsigned int ) );

			if(	wkbtype!=2/*WKBLineString*/ && 
				wkbtype!=0x80000002/*WKBLineString25D*/ &&
				wkbtype!=1002/*WKBSpliteLineStringXYZ*/)
			{
				return list;
			}

			if(wkbtype == 0x80000002||wkbtype == 1002)
				Dimension = 3;

			//先跳过5字节
			double x, y, z=0;
			ptr += 5;
			//读取线坐标点数
			int nPoints = *(( int* )ptr );
			ptr += 4;

			if(nPoints<1 || nPoints>2147483647)
			{
				return list;
			}

			list.resize(nPoints);

			for ( UINT i = 0; i < nPoints; ++i )
			{
				x = *(( double * ) ptr );
				y = *(( double * )( ptr + sizeof( double ) ) );

				ptr += 2 * sizeof( double );

				list[i] = CVector3(x,y,20);

				if ( Dimension==3 ) // ignore Z value
				{
					z = *(( double * ) ptr );
					ptr += sizeof( double );
					list[i].z = z;
				}		
			}

			return list;
		}

		Vector3List ModelUtility::parseSpatialField(String geomStr)
		{
			return OCGeomFieldInfo::parseSpatialGeom(geomStr);
		}

		PipeSection ModelUtility::parseSect(String sectStr)
		{
			PipeSection sect;

			StringVector sv = StringUtil::split(sectStr,"X");

			if (sv.size() != 2)
			{
				sv = StringUtil::split(sectStr,"x");
			}

			if (sv.size() != 2)
			{
				sv = StringUtil::split(sectStr,"*");
			}

			//毫米
			double unit = 0.001;

			if (sv.size() == 2)
			{
				sect.setType(RECT_SECTION);

				double width = StringConverter::parseReal(sv[0]) * unit;
				double height = StringConverter::parseReal(sv[1])* unit;

				//长方形的外包圆半径
				double r = pow((width*width + height*height),0.5) * 0.5;
				sect.setRadius(r);

				sect.setHeight(height);
				sect.setWidth(width);
			}
			else
			{
				double d = StringConverter::parseReal(sectStr) * unit;
				if (d>0.0f)
				{
					sect.setType(CIRCULAR_SECTION);
					sect.setRadius(d * 0.5);
					sect.setHeight(d);
					sect.setWidth(d);
				}
			}

			if (Math::RealEqual(0.0f,sect.getRadius()))
			{
				sect.setType(UNKNOWM_SECTION);
			}

			return sect;
		}
	}
}
