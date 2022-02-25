#ifndef _OC_MODELUTIL_H__
#define _OC_MODELUTIL_H__

#include "LayerDefine.h"

namespace OC
{
	template<class T>
	inline bool core_math_isclockwise(T polygon)
	{
 		int n = (int)polygon.size();

		if ( n > 2)
		{
			if (polygon[0] != polygon[n-1])
			{
				polygon.push_back(polygon[0]);
				n++;
			}

			int clockwise = 0;

			for (int i=0;i<n-1;i++)
			{
				CVector2 v1;
				
				if (i==0)
				{
					v1 = CVector2(polygon[i][0] - polygon[n-2][0],
						polygon[i][1] - polygon[n-2][1]);
				}
				else
				{
					v1 = CVector2(polygon[i][0] - polygon[i-1][0],
						polygon[i][1] - polygon[i-1][1]);
				}

				CVector2 v2(polygon[i+1][0] - polygon[i][0],
					polygon[i+1][1] - polygon[i][1]);

				v1.crossProduct(v2) < 0 ? clockwise++ : clockwise--;
			}

			return clockwise > 0;
		}

		return false;
	}

	template<class T>
	inline T core_math_resemble(T polygon,double len)
	{
		int n = (int)polygon.size();

		if ( n > 2)
		{
			T t;

			if (polygon[0] != polygon[n-1])
			{
				polygon.push_back(polygon[0]);
				n++;
			}

			for (int i=0;i<n-1;i++)
			{
				CVector2 v1;
				CVector2 v2;

				if (i==0)
				{
					v1 = CVector2(polygon[i][0] - polygon[n-2][0],
						polygon[i][1] - polygon[n-2][1]);

					v2 = CVector2(polygon[i+1][0] - polygon[i][0],
						polygon[i+1][1] - polygon[i][1]);
				}
				else
				{
					v1 = CVector2(polygon[i][0] - polygon[i-1][0],
						polygon[i][1] - polygon[i-1][1]);

					v2 = CVector2(polygon[i+1][0] - polygon[i][0],
						polygon[i+1][1] - polygon[i][1]);
				}


				bool clockwise = v1.crossProduct(v2) < 0;

				CVector3 refNormal = clockwise ? CVector3::UNIT_Z : CVector3::NEGATIVE_UNIT_Z;

				CVector3 n1 = CVector3(v1.x,v1.y,0).crossProduct(refNormal);
				CVector3 n2 = CVector3(v2.x,v2.y,0).crossProduct(refNormal);

				n1.normalise();
				n2.normalise();

				v1 = -v1;

				v1.normalise();
				v2.normalise();

				v1 = CVector2(n1.x,n1.y);
				v2 = CVector2(n2.x,n2.y);

				double m = len / Math::Sin(Math::ACos(v1.dotProduct(v2)) * 0.5);

				CVector2 pos = (v1 + v2).normalisedCopy() * m;

				typename T::value_type v;
				v[0] = polygon[i][0] + pos.x;
				v[1] = polygon[i][1] + pos.y;
				t.push_back(v);
			}

			return t;
		}

		return polygon;
	}

	template<class T>
	inline T core_math_resemble(T polygon,std::vector<double> len)
	{
		unsigned int n = (unsigned int)polygon.size();

		if(len.size() == 0)
		{
			len.push_back(0);
		}

		if(len.size() < n)
		{
			len.insert(len.end() - 1,n - len.size(),len.back());
		}

		if ( n > 2)
		{
			T t;

			if (polygon[0] != polygon[n-1])
			{
				polygon.push_back(polygon[0]);
				n++;
			}

			bool initialClockwise = core_math_isclockwise(polygon);

			for (unsigned int i=0;i<n-1;i++)
			{
				CVector2 v1;
				CVector2 v2;

				if (i==0)
				{
					v1 = CVector2(polygon[i][0] - polygon[n-2][0],
						polygon[i][1] - polygon[n-2][1]);

					v2 = CVector2(polygon[i+1][0] - polygon[i][0],
						polygon[i+1][1] - polygon[i][1]);
				}
				else
				{
					v1 = CVector2(polygon[i][0] - polygon[i-1][0],
						polygon[i][1] - polygon[i-1][1]);

					v2 = CVector2(polygon[i+1][0] - polygon[i][0],
						polygon[i+1][1] - polygon[i][1]);
				}


				bool clockwise = v1.crossProduct(v2) < 0;
				//if(i == 0)
				//	initialClockwise = clockwise;

				CVector3 refNormal = clockwise ? CVector3::UNIT_Z : CVector3::NEGATIVE_UNIT_Z;

				CVector3 n1 = CVector3(v1.x,v1.y,0).crossProduct(refNormal);
				CVector3 n2 = CVector3(v2.x,v2.y,0).crossProduct(refNormal);

				n1.normalise();
				n2.normalise();

				v1 = -v1;

				v1.normalise();
				v2.normalise();

				double m = len[i] / Math::Sin(Math::ACos(v1.dotProduct(v2)) * 0.5);

				if(clockwise != initialClockwise)
					m = -m;

				CVector2 pos = (v1 + v2).normalisedCopy() * m;

				typename T::value_type v;
				v[0] = polygon[i][0] + pos.x;
				v[1] = polygon[i][1] + pos.y;
				t.push_back(v);
			}

			return t;
		}

		return polygon;
	}

	class _LayerExport TriangleSurface
	{
	public:
		TriangleSurface();
		~TriangleSurface();

		void addPoints(osg::ref_ptr<osg::Vec3dArray> points);

		osg::Geometry* build();
	private:
		osg::ref_ptr<osg::Vec3Array>	mPoints;
	};

	class _LayerExport ModelUtil
	{
	public:
		static osg::Node* buildWall(std::vector<osg::Vec3d> up_points,std::vector<osg::Vec3d> down_points);
		static osg::Node* buildSurface(std::vector<osg::Vec3d> points);

		static osg::Geometry* buildPipe();
		static void setPipeState(osg::Node* node,String texName);
		static void setPipeState(osg::Node* node,osg::Vec4 clr);
		static void setPipeClrMat(osg::Node* node,osg::Vec4 clr);
		static void setPipeClrMat(osg::Node* node,ColourValue clr);
		static osgTerrain::TerrainTile* getDefaultTile(osg::Group* group);

		//only triangles
		static double calculateArea(osg::Geometry* geometry);
		static double calculateArea(osg::Vec3Array* vectex,osg::DrawElementsUInt* primitive);
	};
}

#endif
