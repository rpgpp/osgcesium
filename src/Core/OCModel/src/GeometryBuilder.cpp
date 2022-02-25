#include "GeometryBuilder.h"
#include "OCLayer/OCResource.h"

namespace OC
{
	namespace Modeling
	{
		CGeometryBuilder::CGeometryBuilder()
		{
			mVertices = new osg::Vec3Array;
			mIndice = new osg::DrawElementsUInt(GL_QUADS);
			mNormal = new osg::Vec3Array;
			mTexCoord = new osg::Vec2Array;
		}

		CGeometryBuilder::~CGeometryBuilder()
		{

		}

		osg::Geometry* CGeometryBuilder::buildGeometry()
		{
			if (mVertices->empty() || mIndice->empty())
			{
				return NULL;
			}

			osg::Geometry* geom = new osg::Geometry;

			//vertex
			geom->setVertexArray(mVertices);
			geom->addPrimitiveSet(mIndice);

			//index
			if(!mNormal->empty())
			{
				geom->setNormalArray(mNormal,osg::Array::BIND_PER_PRIMITIVE_SET);
				//geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
			}
			else
			{
				osgUtil::SmoothingVisitor::smooth(*geom);
			}


			if (!mTexCoord->empty())
			{
				geom->setTexCoordArray(0,mTexCoord,osg::Array::BIND_PER_VERTEX);
			}

			//state
			{
				//texture
				if (!texture.empty())
				{
					osg::StateSet *state = geom->getOrCreateStateSet();
					if (state != NULL)
					{
						osg::ref_ptr<osg::Image> image = new osg::Image;
						image->setFileName(texture);

						osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D(image);
						tex2d->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
						tex2d->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
						tex2d->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);  
					
						//osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
						//texenv->setMode(osg::TexEnv::BLEND);
						//texenv->setColor(osg::Vec4(0.0,0.0,0.0,0.0));
						//state->setTextureAttribute(0,texenv.get());

						state->setTextureAttributeAndModes(0,tex2d,osg::StateAttribute::ON);
					}
				}
			}

			return geom;
		}

		void CGeometryBuilder::addNormal(double x,double y,double z)
		{
			osg::Vec3 v1(x,y,z);
			mNormal->push_back(v1);
		}

		void CGeometryBuilder::setVertice(osg::ref_ptr<osg::Vec3Array> vertex)
		{
			mVertices = vertex;
		}

		void CGeometryBuilder::setIndice(osg::ref_ptr<osg::DrawElementsUInt> indice)
		{
			mIndice = indice;
		}

		void CGeometryBuilder::setNormal(osg::ref_ptr<osg::Vec3Array> normal)
		{
			mNormal = normal;
		}

		void CGeometryBuilder::setTexCoord(osg::ref_ptr<osg::Vec2Array> texCoord)
		{
			mTexCoord = texCoord;
		}
		
		osg::Vec2Array* CGeometryBuilder::getTexCoord()
		{
			return mTexCoord.get();
		}

		osg::Geode* CGeometryBuilder::buildPipe()
		{
			osg::Geode* geode = NULL;
			osg::Geometry* geom = buildGeometry();

			if (geom != NULL)
			{
				geode = new	osg::Geode;
				geode->addDrawable(geom);
			}

			return geode;
		}

		void CGeometryBuilder::addPoint(double x,double y,double z)
		{
			mVertices->push_back(osg::Vec3(x,y,z));
		}

		void CGeometryBuilder::addIndice(int a,int b,int c,int d)
		{
			mIndice->push_back(a);
			mIndice->push_back(b);
			mIndice->push_back(c);
			mIndice->push_back(d);
		}

		void CGeometryBuilder::addIndice(std::vector<int> intVector)
		{
			for(int i =0;i< (int)intVector.size();i++)
			{
				mIndice->push_back(intVector[i]);
			}
		}
	}
}
