#include "ModelUtil.h"

namespace OC
{
	// Compute the circumcircle of a triangle (only x and y coordinates are used),
	// return (Cx, Cy, r^2)
	inline osg::Vec3 compute_circumcircle(
		const osg::Vec3 &a,
		const osg::Vec3 &b,
		const osg::Vec3 &c)
	{
		float D =
			(a.x() - c.x()) * (b.y() - c.y()) -
			(b.x() - c.x()) * (a.y() - c.y());

		float cx, cy, r2;

		if(D==0.0)
		{
			// (Nearly) degenerate condition - either two of the points are equal (which we discount)
			// or the three points are colinear. In this case we just determine the average of
			// the three points as the centre for correctness, but squirt out a zero radius.
			// This method will produce a triangulation with zero area, so we have to check later
			cx = (a.x()+b.x()+c.x())/3.0;
			cy = (a.y()+b.y()+c.y())/3.0;
			r2 = 0.0;
		}
		else
		{
			cx =
				(((a.x() - c.x()) * (a.x() + c.x()) +
				(a.y() - c.y()) * (a.y() + c.y())) / 2 * (b.y() - c.y()) -
				((b.x() - c.x()) * (b.x() + c.x()) +
				(b.y() - c.y()) * (b.y() + c.y())) / 2 * (a.y() - c.y())) / D;

			cy =
				(((b.x() - c.x()) * (b.x() + c.x()) +
				(b.y() - c.y()) * (b.y() + c.y())) / 2 * (a.x() - c.x()) -
				((a.x() - c.x()) * (a.x() + c.x()) +
				(a.y() - c.y()) * (a.y() + c.y())) / 2 * (b.x() - c.x())) / D;

			//  r2 = (c.x() - cx) * (c.x() - cx) + (c.y() - cy) * (c.y() - cy);
			// the return r square is compared with r*r many times in an inner loop
			// so for efficiency use the inefficient sqrt once rather than 30* multiplies later.
			r2 = sqrt((c.x() - cx) * (c.x() - cx) + (c.y() - cy) * (c.y() - cy));
		}
		return osg::Vec3(cx, cy, r2);
	}

	// Test whether a point (only the x and y coordinates are used) lies inside
	// a circle; the circle is passed as a vector: (Cx, Cy, r).

	inline bool point_in_circle(const osg::Vec3 &point, const osg::Vec3 &circle)
	{
		float r2 =
			(point.x() - circle.x()) * (point.x() - circle.x()) +
			(point.y() - circle.y()) * (point.y() - circle.y());
		return r2 <= circle.z()*circle.z();
		//    return r2 <= circle.z();
	}

	typedef GLuint Vertex_index;
	class Edge {
	public:

		// Comparison object (for sorting)
		struct Less
		{
			inline bool operator()(const Edge &e1, const Edge &e2) const
			{
				if (e1.ibs() < e2.ibs()) return true;
				if (e1.ibs() > e2.ibs()) return false;
				if (e1.ies() < e2.ies()) return true;
				return false;
			}
		};

		Edge(): ib_(0), ie_(0), ibs_(0), ies_(0), duplicate_(false) {}
		Edge(Vertex_index ib, Vertex_index ie) : ib_(ib), ie_(ie), ibs_(osg::minimum(ib, ie)), ies_(osg::maximum(ib, ie)), duplicate_(false) {}

		// first endpoint
		inline Vertex_index ib() const { return ib_; }

		// second endpoint
		inline Vertex_index ie() const { return ie_; }

		// first sorted endpoint
		inline Vertex_index ibs() const { return ibs_; }

		// second sorted endpoint
		inline Vertex_index ies() const { return ies_; }

		// get the "duplicate" flag
		inline bool get_duplicate() const { return duplicate_; }

		// set the "duplicate" flag
		inline void set_duplicate(bool v) { duplicate_ = v; }

	private:
		Vertex_index ib_, ie_;
		Vertex_index ibs_, ies_;
		bool duplicate_;
	};


	// CLASS: Triangle

	class Triangle
	{
	public:

		Triangle():
		  a_(0),
			  b_(0),
			  c_(0) {}


		  Triangle(Vertex_index a, Vertex_index b, Vertex_index c, osg::Vec3Array *points)
			  :    a_(a),
			  b_(b),
			  c_(c),
			  cc_(compute_circumcircle((*points)[a_], (*points)[b_], (*points)[c_]))
		  {
			  edge_[0] = Edge(a_, b_);
			  edge_[1] = Edge(b_, c_);
			  edge_[2] = Edge(c_, a_);
		  }

		  Triangle& operator = (const Triangle& rhs)
		  {
			  if (&rhs==this) return *this;

			  a_ = rhs.a_;
			  b_ = rhs.b_;
			  c_ = rhs.c_;
			  cc_ = rhs.cc_;
			  edge_[0]  = rhs.edge_[0];
			  edge_[1]  = rhs.edge_[1];
			  edge_[2]  = rhs.edge_[2];

			  return *this;
		  }

		  inline Vertex_index a() const { return a_; }
		  inline Vertex_index b() const { return b_; }
		  inline Vertex_index c() const { return c_; }
		  inline void incrementa(const int delta) { a_+=delta; }
		  inline void incrementb(const int delta) { b_+=delta; }
		  inline void incrementc(const int delta) { c_+=delta; }

		  inline const Edge &get_edge(int idx) const { return edge_[idx];    }
		  inline const osg::Vec3 &get_circumcircle() const { return cc_; }

		  inline osg::Vec3 compute_centroid(const osg::Vec3Array *points) const
		  {
			  return ((*points)[a_] +(*points)[b_] + (*points)[c_])/3;
		  }

		  inline osg::Vec3 compute_normal(osg::Vec3Array *points) const
		  {
			  osg::Vec3 N = ((*points)[b_] - (*points)[a_]) ^ ((*points)[c_] - (*points)[a_]);
			  return N / N.length();
		  }

		  bool isedge(const unsigned int ip1,const unsigned int ip2) const
		  { // is one of the edges of this triangle from ip1-ip2
			  bool isedge=ip1==a() && ip2==b();
			  if (!isedge)
			  {
				  isedge=ip1==b() && ip2==c();
				  if (!isedge)
				  {
					  isedge=ip1==c() && ip2==a();
				  }
			  }
			  return isedge;
		  }
		  // GWM July 2005 add test for triangle intersected by p1-p2.
		  // return true for unused edge

		  bool intersected(const unsigned int ip1,const unsigned int ip2,const osg::Vec2 p1 ,const osg::Vec2 p2,const int iedge, osg::Vec3Array *points) const
		  {
			  // return true if edge iedge of triangle is intersected by ip1,ip2
			  Vertex_index ie1,ie2;
			  if (iedge==0)
			  {
				  ie1=a();
				  ie2=b();
			  }
			  else if (iedge==1)
			  {
				  ie1=b();
				  ie2=c();
			  }
			  else if (iedge==2)
			  {
				  ie1=c();
				  ie2=a();
			  }
			  if (ip1==ie1 || ip2==ie1) return false;
			  if (ip1==ie2 || ip2==ie2) return false;

			  osg::Vec2 tp1((*points)[ie1].x(),(*points)[ie1].y());
			  osg::Vec2 tp2((*points)[ie2].x(),(*points)[ie2].y());
			  return intersect(tp1,tp2,p1,p2);
		  }

		  bool intersectedby(const osg::Vec2 p1,const osg::Vec2 p2,osg::Vec3Array *points) const {
			  // true if line [p1,p2] cuts at least one edge of this triangle
			  osg::Vec2 tp1((*points)[a()].x(),(*points)[a()].y());
			  osg::Vec2 tp2((*points)[b()].x(),(*points)[b()].y());
			  osg::Vec2 tp3((*points)[c()].x(),(*points)[c()].y());
			  bool ip=intersect(tp1,tp2,p1,p2);
			  if (!ip)
			  {
				  ip=intersect(tp2,tp3,p1,p2);
				  if (!ip)
				  {
					  ip=intersect(tp3,tp1,p1,p2);
				  }
			  }
			  return ip;
		  }
		  int whichEdge(osg::Vec3Array *points,const osg::Vec2 p1, const osg::Vec2 p2,
			  const unsigned int e1,const unsigned int e2) const
		  {
			  int icut=0;
			  // find which edge of triangle is cut by line (p1-p2) and is NOT e1-e2 indices.
			  // return 1 - cut is on edge b-c; 2==c-a
			  osg::Vec2 tp1((*points)[a()].x(),(*points)[a()].y()); // triangle vertices
			  osg::Vec2 tp2((*points)[b()].x(),(*points)[b()].y());
			  osg::Vec2 tp3((*points)[c()].x(),(*points)[c()].y());
			  bool ip=intersect(tp2,tp3,p1,p2);
			  if (ip && (a()==e1 || a()==e2)) { return 1;}
			  ip=intersect(tp3,tp1,p1,p2);
			  if (ip && (b()==e1 || b()==e2)) { return 2;}
			  ip=intersect(tp1,tp2,p1,p2);
			  if (ip && (c()==e1 || c()==e2)) { return 3;}
			  return icut;
		  }

		  bool usesVertex(const unsigned int ip) const
		  {
			  return ip==a_ || ip==b_ || ip==c_;
		  }

		  int lineBisectTest(const osg::Vec3 apt,const osg::Vec3 bpt,const osg::Vec3 cpt, const osg::Vec2 p2) const
		  {
			  osg::Vec2 vp2tp=p2-osg::Vec2(apt.x(), apt.y()); // vector from p1 to a.
			  // test is: cross product (z component) with ab,ac is opposite signs
			  // AND dot product with ab,ac has at least one positive value.
			  osg::Vec2 vecba=osg::Vec2(bpt.x(), bpt.y())-osg::Vec2(apt.x(), apt.y());
			  osg::Vec2 vecca=osg::Vec2(cpt.x(), cpt.y())-osg::Vec2(apt.x(), apt.y());
			  float cprodzba=vp2tp.x()*vecba.y() - vp2tp.y()*vecba.x();
			  float cprodzca=vp2tp.x()*vecca.y() - vp2tp.y()*vecca.x();
			  //    OSG_WARN << "linebisect test " << " tri " << a_<<","<< b_<<","<< c_<<std::endl;
			  if (cprodzba*cprodzca<0)
			  {
				  // more tests - check dot products are at least partly parallel to test line.
				  osg::Vec2 tp1(bpt.x(),bpt.y()); // triangle vertices
				  osg::Vec2 tp2(cpt.x(),cpt.y());
				  osg::Vec2 tp3(apt.x(),apt.y());
				  bool ip=intersect(tp1,tp2,tp3,p2);
				  if (ip) return 1;
			  }
			  return 0;
		  }

		  int lineBisects(osg::Vec3Array *points, const unsigned int ip1, const osg::Vec2 p2) const
		  {
			  // return true if line starts at vertex <ip1> and lies between the 2 edges which meet at vertex
			  // <vertex> is that which uses index ip1.
			  // line is <vertex> to p2
			  //    return value is 0 - no crossing; 1,2,3 - which edge of the triangle is cut.
			  if (a_==ip1)
			  {
				  // first vertex is the vertex - test that a_ to p2 lies beteen edges a,b and a,c
				  osg::Vec3 apt=(*points)[a_];
				  osg::Vec3 bpt=(*points)[b_];
				  osg::Vec3 cpt=(*points)[c_];
				  return lineBisectTest(apt,bpt,cpt,p2)?1:0;
			  }
			  else if (b_==ip1)
			  {
				  // second vertex is the vertex - test that b_ to p2 lies beteen edges a,b and a,c
				  osg::Vec3 apt=(*points)[b_];
				  osg::Vec3 bpt=(*points)[c_];
				  osg::Vec3 cpt=(*points)[a_];
				  return lineBisectTest(apt,bpt,cpt,p2)?2:0;
			  }
			  else if (c_==ip1)
			  {
				  // 3rd vertex is the vertex - test that c_ to p2 lies beteen edges a,b and a,c
				  osg::Vec3 apt=(*points)[c_];
				  osg::Vec3 bpt=(*points)[a_];
				  osg::Vec3 cpt=(*points)[b_];
				  return lineBisectTest(apt,bpt,cpt,p2)?3:0;
			  }
			  return 0;
		  }

	private:


		bool intersect(const osg::Vec2 p1,const osg::Vec2 p2,const osg::Vec2 p3,const osg::Vec2 p4) const
		{
			// intersection point of p1,p2 and p3,p4
			// test from http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/
			// the intersection must be internal to the lines, not an end point.
			float det=((p4.y()-p3.y())*(p2.x()-p1.x())-(p4.x()-p3.x())*(p2.y()-p1.y()));
			if (det!=0)
			{
				// point on line is P=p1+ua.(p2-p1) and Q=p3+ub.(p4-p3)
				float ua=((p4.x()-p3.x())*(p1.y()-p3.y())-(p4.y()-p3.y())*(p1.x()-p3.x()))/det;
				float ub=((p2.x()-p1.x())*(p1.y()-p3.y())-(p2.y()-p1.y())*(p1.x()-p3.x()))/det;
				if (ua> 0.00 && ua< 1 && ub> 0.0000  && ub< 1)
				{
					return true;
				}
			}
			return false;
		}

		Vertex_index a_;
		Vertex_index b_;
		Vertex_index c_;
		osg::Vec3 cc_;
		Edge edge_[3];
	};

	struct GeodeNodeVisitor : public osg::NodeVisitor
	{
		GeodeNodeVisitor(String tex)
			: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
			,texName(tex)
		{}
		String texName;
		virtual void apply(osg::Geode& geode)
		{
			int num = geode.getNumDrawables();

			for (int i=0;i<num;i++)
			{
				osg::Drawable *draww = geode.getDrawable(i);

				osg::StateSet *state = draww->getOrCreateStateSet();
				if (state != NULL)
				{
					osg::Texture2D* tex2d = new osg::Texture2D;
					osg::ref_ptr<osg::Image> image = new osg::Image;
					image->setFileName(texName);
					state->setTextureAttributeAndModes(0,new osg::Texture2D(image),osg::StateAttribute::ON);
				}
			}
		}
	};

	struct ChangeColorVisitor : public osg::NodeVisitor
	{
		ChangeColorVisitor(osg::Vec4 clr)
			: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
			,mColor(clr)
		{}
		osg::Vec4 mColor;
		virtual void apply(osg::Geode& geode)
		{
			int num = geode.getNumDrawables();

			for (int i=0;i<num;i++)
			{
				osg::Drawable *draww = geode.getDrawable(i);
				
				osg::Geometry* geom = draww->asGeometry();
				
				if (geom != NULL)
				{
					osg::Vec4Array* colors = new osg::Vec4Array(1);
					(*colors)[0] = mColor;
					geom->setColorArray(colors, osg::Array::BIND_OVERALL);
				}

			}
		}
	};

	osgTerrain::TerrainTile* ModelUtil::getDefaultTile(osg::Group* group)
	{
		String clasName = group->className();

		if (clasName == "TerrainTile")
		{
			return (osgTerrain::TerrainTile*)group;
		}

		if (group != NULL)
		{
			int num = group->getNumChildren();
			for (int i=0;i<num;i++)
			{
				osg::Group* child = group->getChild(i)->asGroup();
				osgTerrain::TerrainTile* tile = getDefaultTile(child);
				if (tile)
				{
					return tile;
				}
			}
		}

		return NULL;
	}

	/*
	p0 - p1
	 |   |
	p3 - p2
	*/
	inline osg::Geometry* createOneWall(osg::Vec3 p0,osg::Vec3 p1,osg::Vec3 p2,osg::Vec3 p3)
	{
		osg::Geometry* geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> texture = new osg::Vec2Array;
		osg::ref_ptr<osg::DrawElementsUInt> indice = new osg::DrawElementsUInt(GL_TRIANGLES);
		vertex->resize(4);
		indice->resize(6);
		normal->resize(1);
		texture->resize(4);

		(*vertex)[0] = p0;
		(*vertex)[1] = p1;
		(*vertex)[2] = p3;
		(*vertex)[3] = p2;

		(*indice)[0] = 0;
		(*indice)[1] = 1;
		(*indice)[2] = 3;
		(*indice)[3] = 1;
		(*indice)[4] = 2;
		(*indice)[5] = 3;

		osg::Vec3 nor = (*normal)[0] = (p1 - p0) ^ (p2 - p0);
		nor.normalize();

		double minz = min(p1.z(),p2.z());
		double maxh = max(p0.z(),p1.z()) - minz;

		(*texture)[0] = osg::Vec2(0,(p0.z() - minz)/ maxh);
		(*texture)[1] = osg::Vec2(1,(p1.z() - minz)/ maxh);
		(*texture)[2] = osg::Vec2(0,0);
		(*texture)[3] = osg::Vec2(1,0);

		geom->setVertexArray(vertex);
		geom->setNormalArray(normal,osg::Array::BIND_PER_PRIMITIVE_SET);
		geom->setTexCoordArray(0,texture);
		geom->addPrimitiveSet(indice);

		return geom;
	}

	inline osg::Geometry* createOneWall(osg::Vec3 p0,osg::Vec3 p1,double minz,double depth,double angle,osg::Vec3& o2,osg::Vec3& o3)
	{
		osg::Geometry* geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> texture = new osg::Vec2Array;
		osg::ref_ptr<osg::DrawElementsUInt> indice = new osg::DrawElementsUInt(GL_TRIANGLES);
		vertex->resize(4);
		indice->resize(6);
		normal->resize(1);
		texture->resize(4);

		/*
		p0 - p1
		 |   |
		p3 - p2
		*/

		osg::Vec3 p2(p1.x(),p1.y(),minz);
		osg::Vec3 p3(p0.x(),p0.y(),minz);

		(*vertex)[0] = p0;
		(*vertex)[1] = p1;
		(*vertex)[2] = p2;
		(*vertex)[3] = p3;

		(*indice)[0] = 0;
		(*indice)[1] = 1;
		(*indice)[2] = 3;
		(*indice)[3] = 1;
		(*indice)[4] = 2;
		(*indice)[5] = 3;

		osg::Vec3 nor = (*normal)[0] = (p1 - p0) ^ (p3 - p0);
		nor.normalize();

		double len = depth / Math::Tan(angle);

		//o2 = (*vertex)[2] = p2 + nor * len;
		//o3 = (*vertex)[3] = p3 + nor * len;

		double h = max(p0.z(),p1.z()) - minz;

		(*texture)[0] = osg::Vec2(0,(p0.z() - minz)/ h);
		(*texture)[1] = osg::Vec2(1,(p1.z() - minz)/ h);
		(*texture)[2] = osg::Vec2(0,0);
		(*texture)[3] = osg::Vec2(1,0);

		geom->setVertexArray(vertex);
		geom->setNormalArray(normal,osg::Array::BIND_PER_PRIMITIVE_SET);
		geom->setTexCoordArray(0,texture);
		geom->addPrimitiveSet(indice);

		return geom;
	}

	osg::Node* ModelUtil::buildWall(std::vector<osg::Vec3d> up_points,std::vector<osg::Vec3d> down_points)
	{
		int n = (int)up_points.size();

		if ( n < 3 || n != (int)down_points.size())
		{
			return NULL;
		}

		osg::ref_ptr<osg::Geode> geode = new osg::Geode;

		if ((up_points.front()) != (*up_points.rbegin()))
		{
			up_points.push_back(up_points.front());
			down_points.push_back(down_points.front());
			n++;
		}

		for (int i=0;i<n - 1;i++)
		{
			osg::Geometry* geom = createOneWall(up_points[i],up_points[i+1],down_points[i],down_points[i+1]);
			geode->addDrawable(geom);
		}

		osg::ref_ptr<osg::Image> image = osgDB::readImageFile("global://images/TextureSide.jpg");
		if (image.valid())
		{
			osg::Texture2D* texture = new osg::Texture2D;
			texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
			texture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
			texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
			texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
			texture->setImage(image);
			geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		}

		return geode.release();
	}

	osg::Node* ModelUtil::buildSurface(std::vector<osg::Vec3d> points)
	{
		osg::ref_ptr<osg::Geode> geod = new osg::Geode;

		double minZ,maxZ,minX,maxX,minY,maxY;
		minX = minY = minZ = 1e+6;
		maxX = maxY = maxZ = -1e+6;

		std::vector<osg::Vec3d>::iterator it = points.begin();
		for (;it!=points.end();it++)
		{
			double x = it->x();
			double y = it->y();
			double z = it->z();

			//
			minZ = min(minZ,z);
			maxZ = max(maxZ,z);

			minX = min(minX,x);
			maxX = max(maxX,x);

			minY = min(minY,y);
			maxY = max(maxY,y);
		}

		double tw = maxX - minX;
		double th = maxY - minY;

		osg::ref_ptr<osg::Vec3dArray> vertex = new osg::Vec3dArray;
		osg::ref_ptr<osg::Vec2Array> texture = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
		normal->push_back(osg::Vec3(0,0,1));

		for (it = points.begin();it!=points.end();it++)
		{
			osg::Vec3d p = *it;

			vertex->push_back(p);

			p.x() -= minX;
			p.y() -= minY;

			osg::Vec2 t(p.x(),p.y());

			t.x() /= tw;
			t.y() /= th;
			t *= 4.0f;

			texture->push_back(t);
		}

		TriangleSurface surface;
		surface.addPoints(vertex);

		osg::Geometry* geom = surface.build();
		geom->setTexCoordArray(0,texture);
		geom->setNormalArray(normal,osg::Array::BIND_PER_PRIMITIVE_SET);

		osg::ref_ptr<osg::Image> image = osgDB::readImageFile("global://images/TextureBottom.jpg");

		if (image.valid())
		{
			osg::Texture2D* texture = new osg::Texture2D;
			texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
			texture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
			texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
			texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
			texture->setImage(image);
			geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			//geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF |osg::StateAttribute::PROTECTED);
		}

		geod->addDrawable(geom);

		return geod.release();
	}

	void ModelUtil::setPipeClrMat(osg::Node* node,osg::Vec4 clr)
	{
		osg::StateSet* stateset = node->getOrCreateStateSet();  
		osg::Material* material = new osg::Material();
		osg::Vec4 ambient = clr * 0.2f;
		ambient.a() = clr.a();
		osg::Vec4 diffuse = clr * 0.8f;
		diffuse.a() = clr.a();
		material->setAmbient(osg::Material::FRONT, ambient);
		material->setDiffuse(osg::Material::FRONT, diffuse);
		material->setSpecular(osg::Material::FRONT,clr);
		material->setShininess(osg::Material::FRONT,32);
		stateset->setAttribute(material);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON |osg::StateAttribute::PROTECTED);
	}

	void ModelUtil::setPipeClrMat(osg::Node* node,ColourValue clr)
	{
		setPipeClrMat(node,osg::Vec4(clr.r,clr.g,clr.b,clr.a));
	}

	void ModelUtil::setPipeState(osg::Node* node,String texName)
	{
		osg::Geode* geode = node->asGeode();
		if (geode != NULL)
		{
			int num = geode->getNumDrawables();

			for (int i=0;i<num;i++)
			{
				osg::Drawable *draww = geode->getDrawable(i);

				osg::StateSet *state = draww->getOrCreateStateSet();
				if (state != NULL)
				{
					osg::Texture2D* tex2d = new osg::Texture2D;
					osg::ref_ptr<osg::Image> image = new osg::Image;
					image->setFileName(texName);
					state->setTextureAttributeAndModes(0,new osg::Texture2D(image),osg::StateAttribute::ON);
				}
			}

			return;
		}

		{
			GeodeNodeVisitor visitor(texName);
			node->traverse(visitor);
		}
	}

	void ModelUtil::setPipeState(osg::Node* node,osg::Vec4 clr)
	{
		{
			ChangeColorVisitor visitor(clr);
			node->traverse(visitor);
		}
	}

	double ModelUtil::calculateArea(osg::Geometry* geometry)
	{
		double area = 0;

		osg::Vec3Array* vertex = osg::static_pointer_cast<osg::Vec3Array,osg::Array>(geometry->getVertexArray());				

		if (vertex == NULL)
		{
			return area;
		}

		osg::Geometry::PrimitiveSetList list = geometry->getPrimitiveSetList();
		for (osg::Geometry::PrimitiveSetList::iterator it = list.begin();
			it!=list.end();it++)
		{
			if (osg::DrawElementsUInt* primitive = osg::static_pointer_cast<osg::DrawElementsUInt,osg::PrimitiveSet>(*it))
			{
				area = calculateArea(vertex,primitive);
			}
		}

		return area;
	}

	double ModelUtil::calculateArea(osg::Vec3Array* vectex,osg::DrawElementsUInt* primitive)
	{
		double area = 0;

		if (vectex == NULL || primitive == NULL)
		{
			return area;
		}

		for(unsigned int i = 0 ; i < primitive->getNumIndices(); i += 3)
		{			
			osg::Vec3d p1 = vectex->at(primitive->getElement(i));
			osg::Vec3d p2 = vectex->at(primitive->getElement(i + 1));
			osg::Vec3d p3 = vectex->at(primitive->getElement(i + 2));

			double a = (p1 - p2).length();
			double b = (p3 - p2).length();
			double c = (p1 - p3).length();

			double l = a + b + c;
			l *= 0.5f;
			area += Math::Sqrt(l * (l - a) * (l - b) * (l - c));
		}

		return area;
	}

	//
	osg::Geometry* ModelUtil::buildPipe()
	{
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////

	TriangleSurface::TriangleSurface()
	{
		mPoints = new osg::Vec3Array;
	}

	TriangleSurface::~TriangleSurface()
	{

	}

	void TriangleSurface::addPoints(osg::ref_ptr<osg::Vec3dArray> points)
	{
		mPoints->clear();
		mPoints->insert(mPoints->begin(),points->begin(),points->end());
	}

	osg::Geometry* TriangleSurface::build()
	{
		//if (*mPoints->begin() != *mPoints->rbegin())
		//{
		//	mPoints->push_back(*mPoints->begin());
		//}
		if (*mPoints->begin() == *mPoints->rbegin())
		{
			mPoints->erase(mPoints->begin());
		}

		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

		osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator();
		dt->setInputPointArray(mPoints);
		dt->setOutputNormalArray(normals);

		osg::ref_ptr<osg::Vec3Array> pConstrainit = new osg::Vec3Array;

		for (int i=0;i<(int)mPoints->size();i++)
		{
			pConstrainit->push_back(mPoints->at(i));
		}

		osg::ref_ptr<osgUtil::DelaunayConstraint> dc = new osgUtil::DelaunayConstraint;
		dc->setVertexArray(pConstrainit);
		dc->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,pConstrainit->size()) );
		//dt->addInputConstraint(dc.get());

		dt->triangulate();

		dt->removeInternalTriangles(dc);

		geometry->setVertexArray(dc->getPoints(mPoints));

		geometry->addPrimitiveSet(dc->getTriangles());

		normals->push_back(osg::Vec3(0,0,1));

		//osgUtil::SmoothingVisitor::smooth(*geometry);

		return geometry.release();
	}
}