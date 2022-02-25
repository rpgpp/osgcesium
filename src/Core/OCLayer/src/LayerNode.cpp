#include "OCBase/BaseNodeMask.h"
#include "OCBase/ModelUtil.h"
#include "LayerNode.h"
#include "LayerManager.h"
#include "FeatureEffect.h"

namespace OC
{
	CLayerNode::CLayerNode(ILayer* owner)
		:mOwnerLayer(owner)
	{
		setDataVariance(osg::Object::DYNAMIC);
		setNodeMask(FeatureNodeMask);
	}

	CLayerNode::~CLayerNode(void)
	{
	}

	void CLayerNode::updateConfig()
	{
		if (mOwnerLayer)
		{
			CLayerConfig& config = mOwnerLayer->getConfig();

			setNodeMask(config.getBoolValue(CLayerConfig::NamePickable,true) ? FeatureNodeMask : SkyBoxNodeMask);

			String defColor = config.getStringValue(CLayerConfig::NameColor);
			if (!defColor.empty())
			{
				ModelUtil::setPipeClrMat(this,StringConverter::parseColourValue(defColor));
			}

			CQuaternion q = StringConverter::parseQuaternion(mOwnerLayer->getConfig().getStringValue("Rotate"));
			osg::Matrix rotate = osg::Matrix::rotate(QuaternionToQuat(q));
			osg::Matrix offset = osg::Matrix::translate(Vector3ToVec3(mOwnerLayer->getConfig().getVector3Value(CLayerConfig::NameOffset)));
			setMatrix(rotate * offset);

			if (mLayerStateSet.valid())
			{
				mLayerStateSet->setRenderBinDetails(config.getIntValue(CLayerConfig::NameRenderBin), "RenderBin");

				CVector2 polygon_offset = mOwnerLayer->getConfig().getVector2Value(CLayerConfig::NamePolygonOffset);
				osg::PolygonOffset* polyoffset = static_cast<osg::PolygonOffset*>(mLayerStateSet->getAttribute(osg::StateAttribute::POLYGONOFFSET));
				if (polyoffset == NULL)
				{
					if (polygon_offset != CVector2::ZERO)
					{
						polyoffset = new osg::PolygonOffset;  
						polyoffset->setFactor(polygon_offset[0]);  
						polyoffset->setUnits(polygon_offset[1]);
						mLayerStateSet->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
					}
				}
				else
				{
					polyoffset->setFactor(polygon_offset[0]);  
					polyoffset->setUnits(polygon_offset[1]);
				}

				if (!config.getBoolValue(CLayerConfig::NameLighting,true))
				{
					mLayerStateSet->setAttribute(new osg::Program,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
					mLayerStateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF |osg::StateAttribute::OVERRIDE);
				}
			}
		}
	}

	bool CLayerNode::init()
	{
		CORE_MUTEX(SCENE_NODE_MUTEX)

		if (!mInitialize)
		{
			setStateSet(mLayerStateSet = new osg::StateSet);
			updateConfig();
			mInitialize = true;
		}

		return mInitialize;
	}

	void CLayerNode::update(osg::NodeVisitor* nv)
	{
		CSceneNode::update(nv);
	}

	void CLayerNode::setTextureMove()
	{
		
	}

	void CLayerNode::setEffect(IFeatureEffect* effect)
	{
		
	}

	void CLayerNode::setTranseparentColor(ColourValue color)
	{
		if(!mInitialize)
		{
			init();
		}

		CFeatureEffect_Transparent* effect = new CFeatureEffect_Transparent();
		effect->setMaterial(ColorToVec4(color));
		setEffect(effect);
	}

	void CLayerNode::setTransparent(double opacity)
	{
		if(!mInitialize)
		{
			init();
		}

		if (Math::RealEqual(opacity,1.0))
		{
			setEffect(NULL);
			return;
		}
		
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		if (mOwnerLayer->getConfig().getBoolValue(CLayerConfig::NameIsGround))
		{
			CVector2 offset = mOwnerLayer->getConfig().getVector2Value(CLayerConfig::NamePolygonOffset);
			osg::PolygonOffset* polyoffset = new osg::PolygonOffset;  
			polyoffset->setFactor(offset.x);  
			polyoffset->setUnits(offset.y);
			stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
		}

		osg::Vec4 constancolor(0.0f,0.0f,0.0f,opacity);

		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

		osg::BlendFunc*  blendFunc = new osg::BlendFunc;
		osg::BlendColor* blendColor = new osg::BlendColor(constancolor);

		blendFunc->setSource(osg::BlendFunc::CONSTANT_ALPHA);
		blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);

		stateset->setAttributeAndModes(blendFunc,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);  
		stateset->setAttributeAndModes(blendColor,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		setStateSet(stateset);
	}

}



