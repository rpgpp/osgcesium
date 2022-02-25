#ifndef _GEOMETRY_BUILDER_CLASS_H_
#define _GEOMETRY_BUILDER_CLASS_H_

#include "PipeMath.h"

namespace OC
{
	namespace Modeling
	{
		class CGeometryBuilder
		{
		public:
			CGeometryBuilder();
			~CGeometryBuilder();

			osg::Geode* buildPipe();
			osg::Geometry* buildGeometry();

			void setVertice(osg::ref_ptr<osg::Vec3Array> vertex);
			void setIndice(osg::ref_ptr<osg::DrawElementsUInt> indice);
			void setNormal(osg::ref_ptr<osg::Vec3Array> normal);
			void setTexCoord(osg::ref_ptr<osg::Vec2Array> texCoord);
			osg::Vec2Array* getTexCoord();

			void addPoint(double x,double y,double z);

			void addIndice(int a,int b,int c,int d);

			void addIndice(std::vector<int> intVector);

			void setTexture(String name){texture = name;}
			String getTexture(){return texture;}

			void addNormal(double x,double y,double z);
		private:
			String texture;
			osg::ref_ptr<osg::Vec3Array>		mVertices;
			osg::ref_ptr<osg::Vec3Array>		mNormal;
			osg::ref_ptr<osg::Vec2Array>		mTexCoord;
			osg::ref_ptr<osg::DrawElementsUInt> mIndice;
		};


		class PipeBuildCompile : public osg::Referenced
		{
		public:
			PipeBuildCompile()
			{
				mVertice = new osg::Vec3Array;
				mNormals = new osg::Vec3Array;
				mTextureCoords = new osg::Vec2Array;
				mGeometry = new osg::Geometry;
				mGeometry->setVertexArray(mVertice);
				mGeometry->setTexCoordArray(0, mTextureCoords, osg::Array::BIND_PER_VERTEX);
			}

			~PipeBuildCompile() {}

			osg::Vec3Array* vertex()
			{
				return mVertice.get();
			}

			osg::Vec3Array* normal()
			{
				return mNormals.get();
			}

			osg::Vec2Array* texCoord()
			{
				return mTextureCoords.get();
			}

			osg::Geometry* geometry()
			{
				return mGeometry.get();
			}

			void addIndice(int a, int b, int c)
			{
				if (!mDrawElements.valid())
				{
					mDrawElements = new osg::DrawElementsUShort(GL_TRIANGLES);
					geometry()->addPrimitiveSet(mDrawElements);
				}
				mDrawElements->addElement(a);
				mDrawElements->addElement(b);
				mDrawElements->addElement(c);
			}

			void addIndice(int a, int b, int c, int d)
			{
				if (!mDrawElements.valid())
				{
					mDrawElements = new osg::DrawElementsUShort(GL_QUADS);
					geometry()->addPrimitiveSet(mDrawElements);
				}
				mDrawElements->addElement(a);
				mDrawElements->addElement(b);
				mDrawElements->addElement(c);
				mDrawElements->addElement(d);
			}

			void addQuadDrawElements(int a, int b, int c, int d)
			{
				osg::ref_ptr<osg::DrawElements> drawelements = new osg::DrawElementsUShort(GL_QUADS);
				drawelements->addElement(a);
				drawelements->addElement(b);
				drawelements->addElement(c);
				drawelements->addElement(d);
				geometry()->addPrimitiveSet(drawelements);
			}

			void setupNormal()
			{
				if (normal()->size() == vertex()->size())
				{
					geometry()->setNormalArray(normal(), osg::Array::BIND_PER_VERTEX);
				}
				else if (normal()->size() == geometry()->getNumPrimitiveSets())
				{
					geometry()->setNormalArray(normal(), osg::Array::BIND_PER_PRIMITIVE_SET);
				}
				else
				{
					geometry()->setNormalArray(NULL);
				}
			}

			osg::Vec3 computeNormal(int a, int b, int c, int d)
			{
				osg::Vec3 d1 = vertex()->at(b) - vertex()->at(a);
				osg::Vec3 d2 = vertex()->at(d) - vertex()->at(a);
				osg::Vec3 d3 = d1 ^ d2;
				d3.normalize();
				return d3;
			}
		protected:
			GLenum							mMode;
			osg::ref_ptr<osg::Geometry>		mGeometry;
			osg::ref_ptr<osg::Vec3Array>	mVertice;
			osg::ref_ptr<osg::Vec3Array>	mNormals;
			osg::ref_ptr<osg::Vec2Array>	mTextureCoords;
			osg::ref_ptr<osg::DrawElements> mDrawElements;
		};

	}
}

#endif