#include "BuildWell.h"
#include "OCLayer/ModelUtil.h"
#include "OCModel/PipeMath.h"
#include "GeometryBuilder.h"

using namespace OC;
using namespace Modeling;

Vector3List BuildWell::setCircle(CVector3 center,CVector3 normal,double dr,int loop,Vector2List & texturecoor,double TextureYvalue)
{
	static unsigned int textmode=1;

	Vector3List resultV3List;
	double radianStep = Math::TWO_PI / loop;
	CVector3 origin(0,0,0);

	CVector3 originAxle(0,0,1);
	CQuaternion quat =  originAxle.getRotationTo(normal);

	for(int i = 0;i < loop;i++)
	{
		double radian = radianStep*i;
		CVector3 temp;
		CVector2 texturetemp;
		temp.x = dr * Math::Cos(radian);
		temp.y = dr * Math::Sin(radian);
		temp.z = 0;
		temp = quat * temp + center;
		resultV3List.push_back(temp);
		int Yvalue=textmode%2;		
		if(i%4==0)
		{
			/*texturetemp.x=i*(2.0/(double)loop);
			texturetemp.y=Yvalue;*/			
			texturetemp.x=0;
			texturetemp.y=TextureYvalue;
		}
		else if(i%4==1||i%4==3)
		{
			/*texturetemp.x=-1+i*(2.0/(double)loop);*/
			texturetemp.x=0.5;
			texturetemp.y=TextureYvalue;				
		}
		else
		{
			texturetemp.x=1;
			texturetemp.y=TextureYvalue;
		}
		texturecoor.push_back(texturetemp);
	}
	textmode++;
	return resultV3List;
}

osg::ref_ptr<osg::Group> BuildWell::createWellHatCylinder()
{
	osg::ref_ptr<osg::Geometry> wellhat = BuildWell::createWellHat();
	osg::ref_ptr<osg::MatrixTransform> wellhattranfrom = new osg::MatrixTransform;
	osg::Matrix mat1,mat2;
	mat1 = osg::Matrix::rotate(Math::HALF_PI,osg::Vec3d(-1.0,0.0,0.0));
	wellhattranfrom->setMatrix(mat1);
	wellhattranfrom->addChild(wellhat.get());
	return wellhattranfrom;	
}

osg::ref_ptr< osg::Geometry > BuildWell::createWellHat()
{
	double radius = mTopRadius*0.8;
	osg::ref_ptr<osg::Vec3Array> vertices=new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoords=new osg::Vec2Array;
	osg::ref_ptr<osg::DrawElementsUShort> index = new osg::DrawElementsUShort(osg::DrawElements::TRIANGLES);

	int loopc = 20;
	double stepRad = Math::TWO_PI / loopc;

	for(int i=0;i<loopc;i++)
	{
		osg::Vec3 p;
		p.x() = radius * Math::Cos(stepRad * i);
		p.z() = radius * Math::Sin(stepRad * i);

		osg::Vec2 t;
		t.x() = 0.5 * Math::Cos(stepRad * i) + 0.5;
		t.y() = 0.5 * Math::Sin(stepRad * i) + 0.5;

		vertices->push_back(p);
		texcoords->push_back(t);
	}

	vertices->push_back(osg::Vec3(0,0,0));
	texcoords->push_back(osg::Vec2(0.5,0.5));

	for(int i=0;i<loopc;i++)
	{
		if(i == loopc - 1)
		{
			index->push_back(loopc);
			index->push_back(loopc - 1);
			index->push_back(0);
		}
		else
		{
			index->push_back(loopc);
			index->push_back(i);
			index->push_back(i+1);
		}
	}
	
	osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
	quad->setVertexArray(vertices.get());
	quad->addPrimitiveSet(index);
	quad->setTexCoordArray(0,texcoords.get());	

	osgUtil::SmoothingVisitor::smooth(*quad.get());
	//osg::ref_ptr<osg::Vec3Array> normals=new osg::Vec3Array;
	//normals->push_back(osg::Vec3f(0.0f,-1.0f,0.0f));
	//quad->setNormalArray(normals.get());
	//quad->setNormalBinding(osg::Geometry::BIND_OVERALL);


	osg::ref_ptr<osg::Image> image;
	if (mImageInternal)
	{
		image = osgDB::readImageFile(mWellCoverImage);
	}
	else
	{
		image = new osg::Image;
		image->setFileName(mWellCoverImage);
	}

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture.get());
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::DECAL);
	quad->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
	
	return quad;
}

osg::ref_ptr<osg::MatrixTransform> BuildWell::build()
{
	Vector3List v1,v2,v3,v4,v5;
	Vector2List texture1,texture2,texture3,texture4,texture5;
	Vector3List	vlist;
	Vector2List texturefinal;
	
	int precision = 20;

	CVector3 center1 = CVector3(0,0,-mBottomHeight - mMiddleHeight - mTopHeight);
	CVector3 center2 = CVector3(0,0,-mBottomHeight - mMiddleHeight * 0.5);
	CVector3 center3 = CVector3(0,0,-mBottomHeight);
	CVector3 center4 = CVector3(0,0,0);
	CVector3 center5 = CVector3(0,0,0);

	v1 = setCircle(center1,CVector3::UNIT_Z,mBottomRadius,precision,texture1,0);
	v2 = setCircle(center2,CVector3::UNIT_Z,mBottomRadius,precision,texture2,1);
	v3 = setCircle(center3,CVector3::UNIT_Z,mTopRadius,precision,texture3,0.3);
	v4 = setCircle(center4,CVector3::UNIT_Z,mTopRadius,precision,texture4,1.2);
	v5 = setCircle(center5,CVector3::UNIT_Z,mTopRadius * 0.8,precision,texture5,1);

	vlist.insert(vlist.end(),v1.begin(),v1.end());
	vlist.insert(vlist.end(),v2.begin(),v2.end());
	vlist.insert(vlist.end(),v3.begin(),v3.end());
	vlist.insert(vlist.end(),v4.begin(),v4.end());
	vlist.insert(vlist.end(),v5.begin(),v5.end());
	
	texturefinal.insert(texturefinal.end(),texture1.begin(),texture1.end());
	texturefinal.insert(texturefinal.end(),texture2.begin(),texture2.end());
	texturefinal.insert(texturefinal.end(),texture3.begin(),texture3.end());
	texturefinal.insert(texturefinal.end(),texture4.begin(),texture4.end());
	texturefinal.insert(texturefinal.end(),texture5.begin(),texture5.end());

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
	for(int i=0;i<texturefinal.size();i++)
	{
		texcoords->push_back(osg::Vec2f(texturefinal[i].x,texturefinal[i].y));
	}

	bool bottomClose = true;

	int cicleNum = vlist.size()/precision;	
	CGeometryBuilder pd2;

	for(int i = 0;i<vlist.size();i++)
	{
		pd2.addPoint(vlist[i].x,vlist[i].y,vlist[i].z);
	}

	for(int i = 0;i<(cicleNum - 1) * precision;i++)
	{
		if((i +1)%precision != 0)
			pd2.addIndice(i,i+1,i+precision+1,i + precision);
		else
			pd2.addIndice(i,i - precision +1,i+1,i + precision);
	}	
	int base = vlist.size()  - 1;

	for(int i = 0;i<precision;i++)
	{
		if(i != (precision - 1))
			pd2.addIndice(base + 1,i + 1,i,base + 1);
		else
			pd2.addIndice(base + 1,i - precision + 1,i,base + 1);
	}

	int k = 0;

	if(bottomClose)
	{
		pd2.addPoint(center1.x,center1.y,center1.z);	
		for(int i = 0;i<precision-1;i++)
		{
			pd2.addIndice(vlist.size() + k,i+1,i,vlist.size()+k);
		}
		pd2.addIndice(vlist.size() + k,0,precision - 1,vlist.size()+k);	


		texcoords->push_back(osg::Vec2f(0,0));
	}

	pd2.setTexCoord(texcoords);
	osg::ref_ptr<osg::Geometry> quad = pd2.buildGeometry();

	osg::ref_ptr<osg::Image> image1 = new osg::Image;

	if (mImageInternal)
	{
		image1 = osgDB::readImageFile(mWellWallImage);
	}
	else
	{
		image1 = new osg::Image;
		image1->setFileName(mWellWallImage);
	}

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image1);
	texture->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_R,osg::Texture::REPEAT);
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture.get());
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::DECAL);
	quad->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

	osg::ref_ptr<osg::Group> wellhatcylinder = createWellHatCylinder();
	osg::ref_ptr<osg::MatrixTransform> root = new osg::MatrixTransform;
	root->addChild(quad.get());
	root->addChild(wellhatcylinder.get() );
	
	return root;
}
 

