#ifndef _MERGETRIANGLEOPERATOR_H_
#define _MERGETRIANGLEOPERATOR_H_

#include <osg/TriangleLinePointIndexFunctor>

namespace OC
{
    class GltfTriangleOperator
    {
    public:
        osg::ref_ptr<osg::DrawElementsUInt> set;
        unsigned int offset = 0;
        std::map<osg::Vec2i, int> mapRepeat2;
        std::map<osg::Vec3i, int> mapRepeat3;

        inline void operator()(unsigned int p1)
        {

        }

        inline void operator()(unsigned int p1, unsigned int p2)
        {
            osg::Vec2i tri(p1 + offset, p2 + offset);
            if (mapRepeat2.find(tri) != mapRepeat2.end())
            {
                return;
            }

            mapRepeat2[tri] = 1;

            set->addElement(tri.x());
            set->addElement(tri.y());
        }

        inline void operator()(unsigned int p1, unsigned int p2, unsigned int p3)
        {
            osg::Vec3i tri(p1 + offset, p2 + offset, p3 + offset);
            if (mapRepeat3.find(tri) != mapRepeat3.end())
            {
                return;
            }

            mapRepeat3[tri] = 1;

            set->addElement(tri.x());
            set->addElement(tri.y());
            set->addElement(tri.z());
        }
    };

    typedef osg::TriangleLinePointIndexFunctor<GltfTriangleOperator> GltfTriangleIndexFunctor;
}

#endif // !1
