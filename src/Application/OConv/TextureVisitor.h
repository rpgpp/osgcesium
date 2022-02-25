#pragma once

#include "OCUtility/MathUtil.h"
#include "OCUtility/Rectangle.h"
#include "OCModel/ModelUtility.h"

using namespace OC;

class CTextureVisitor : public osg::NodeVisitor
{
public:
	CTextureVisitor()
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}
	~CTextureVisitor(){}

	void apply(osg::Geode& geode)
	{
		if (mBoundComputed)
			Modeling::ModelUtility::setTexture(&geode, "tdt.jpg", false);
		unsigned int n = geode.getNumDrawables();

		for (unsigned int i = 0; i < n; i++)
		{
			if (osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i)))
			{
				osg::Vec3Array* vertex = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
				if (vertex == NULL)
				{
					continue;
				}
				if (mBoundComputed == false)
				{
					for (size_t t = 0; t < vertex->size(); t++)
					{
						osg::Vec3d v = vertex->at(t);
						mRectangle.merge(CVector2(v.x(), v.y()));
					}
				}
				else
				{
					osg::ref_ptr<osg::Vec2Array> textureCoord = new osg::Vec2Array;
					geom->setTexCoordArray(0, textureCoord, osg::Array::BIND_PER_VERTEX);
					textureCoord->resize(vertex->size());

					for (size_t t = 0; t < vertex->size(); t++)
					{
						osg::Vec3d v = vertex->at(t);
						CVector2 pos(v.x(), v.y());
						pos -= mRectangle.getMinimum();
						textureCoord->at(t) = osg::Vec2(pos.x / mRectangle.getWidth(), pos.y / mRectangle.getHeight());
					}
				}
			}
		}
	}

	void apply(osg::Node& node)
	{
		traverse(node);
	}

	CRectangle mRectangle;
	bool mBoundComputed = false;
};