#include "glTFOptimize.h"

namespace OC
{
	namespace Cesium
	{
		glTFVisitor::glTFVisitor()
			: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
			, removeStateset(false)
		{
		}

		glTFVisitor::~glTFVisitor()
		{
		}

		void glTFVisitor::apply(osg::Node& node)
		{
			traverse(node);
		}

		void glTFVisitor::apply(osg::StateSet& stateset)
		{

		}

		void glTFVisitor::apply(osg::Geode &geode)
		{
			if (geode.getStateSet()) apply(*geode.getStateSet());

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

					if (offset.isSet())
					{
						for (auto& v : *vertex)
						{
							v -= offset.get();
						}
						geom->dirtyBound();
					}

					if (removeStateset)
					{
						geom->setStateSet(NULL);
					}
				}
			}
		}
	}
}
