#ifndef _OSG_HEADER_H__
#define _OSG_HEADER_H__
#pragma warning(disable:4100)

#include <osg/Array>
#include <osg/AlphaFunc>
#include <osg/AutoTransform>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/BoundingBox>
#include <osg/ClipNode>
#include <osg/ComputeBoundsVisitor>
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/io_utils>
#include <osg/Image>
#include <osg/ImageSequence>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PolygonOffset>
#include <osg/Program>
#include <osg/ProxyNode>
#include <osg/PointSprite>
#include <osg/Referenced>
#include <osg/Sequence>
#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/Stencil>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osg/TexEnv>
#include <osg/TextureCubeMap>  
#include <osg/TriangleIndexFunctor>
#include <osg/Uniform>
#include <osg/Vec2>
#include <osg/ValueObject>



#include <osgAnimation/Sampler>
#include <osgAnimation/BasicAnimationManager>


#include <osgDB/Archive>
#include <osgDB/ConvertUTF>
#include <osgDB/DatabasePager>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
//#include <osgDB/pluginquery>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/ReaderWriter>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>  

#include <osgParticle/AccelOperator>
#include <osgParticle/BoxPlacer>
#include <osgParticle/DomainOperator>
#include <osgParticle/FireEffect>
#include <osgParticle/FluidProgram>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/range>
#include <osgParticle/Particle>
#include <osgParticle/ParticleEffect>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/RadialShooter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SinkOperator>


#include <osgManipulator/Command>
#include <osgManipulator/Dragger>
#include <osgManipulator/Projector>
#include <osgManipulator/RotateCylinderDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/TranslateAxisDragger>

#include <osgGA/GUIEventHandler>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/StandardManipulator>

#include <osgText/Text>
#include <osgText/Font>

#include <osgUtil/Optimizer>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/DelaunayTriangulator>
#include <osgUtil/ShaderGen>
#include <osgUtil/Tessellator>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/Simplifier>

#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Renderer>
#include <osgViewer/Scene>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerBase>
#include <osgViewer/ViewerEventHandlers>

#ifdef _WIN32
#include <osgViewer/api/win32/GraphicsWindowWin32>
#else
#endif

#include <osgFX/Scribe>
#include <osgFX/SpecularHighlights>
#include <osgFX/AnisotropicLighting>
#include <osgFX/Cartoon>

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/PrecipitationEffect>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

#include <osgSim/HeightAboveTerrain>
#include <osgSim/ShapeAttribute>

//osg terrain
#include <osgTerrain/Terrain>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>

struct EmptyComputeBoundingSphereCallback : public osg::Node::ComputeBoundingSphereCallback
{
	EmptyComputeBoundingSphereCallback() {}
	EmptyComputeBoundingSphereCallback(const EmptyComputeBoundingSphereCallback&,const osg::CopyOp&) {}
	virtual osg::BoundingSphere computeBound(const osg::Node&) const { return osg::BoundingSphere(); }
};



#endif
