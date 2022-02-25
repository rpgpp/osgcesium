
#include "PipeData.h"
#include "OCLayer/OCResource.h"
#include "PipeNode.h"
#include "IModelTable.h"

using namespace OC;
using namespace OC::Modeling;

String CGlobalNode::TagName = "GlobalNode";

CGlobalNode::CGlobalNode()
	:mUseProxyNode(true)
{
		
}

CGlobalNode::~CGlobalNode()
{

}

TiXmlElement* CGlobalNode::toXmlElement()
{
	TiXmlElement* element = new TiXmlElement(TagName);
	element->SetAttribute("FeatureUrl",mFeatureUrl);
	element->SetAttribute("UseProxyNode",StringConverter::toString(mUseProxyNode));

	TiXmlElement* root = IPipeData::toXmlElement();
	TiXmlElement* pipe = CPipeData::toXmlElement();
	pipe->LinkEndChild(element);
	root->LinkEndChild(pipe);

	return root;
}

void CGlobalNode::readXmlElement(TiXmlElement* elemennt)
{
	if (elemennt == NULL || elemennt->Value() != TagName)
	{
		return;
	}

	if (TiXmlNode* pipe = elemennt->Parent())
	{
		CPipeData::readXmlElement(pipe->ToElement());
		if (TiXmlNode* root = pipe->Parent())
		{
			IPipeData::readXmlElement(root->ToElement());
		}
	}

	mUseProxyNode = StringConverter::parseBool(TinyXml::read_attribute(elemennt,"UseProxyNode"),true);
	mFeatureUrl = TinyXml::read_attribute(elemennt,"FeatureUrl");
}

void CGlobalNode::setFeatureUrl(String url)
{
	mFeatureUrl = osgDB::getSimpleFileName(url);
}

void CGlobalNode::setUseProxyNode(bool prox)
{
	mUseProxyNode = prox;
}

void CGlobalNode::calculateRotation(PipeNode* pPipeNode)
{
	if (pPipeNode->getInfo().getModelNode().getDefineFaceDirection().first)
	{
		CVector3 faceDirection = pPipeNode->getInfo().getModelNode().getDefineFaceDirection().second;
		CVector3 direction = pPipeNode->getInfo().getConstDirection();
		if (CVector3::ZERO == direction)
		{
			direction = pPipeNode->getInfo().calcModelDirection();
		}

		if(pPipeNode->getInfo().getModelNode().getBoolValue(ModelNode::NameMustHorizontal,true))
		{
			direction.z = 0;
		}

		direction.normalise();
		CMatrix3 faceMatrix = CMatrix3::IDENTITY;
		CQuaternion faceQuat = faceDirection.getRotationTo(direction);
		faceQuat.ToRotationMatrix(faceMatrix);

		pPipeNode->setRotateString(StringConverter::toString(faceQuat));
	}

	pPipeNode->setOffset(pPipeNode->getInfo().getModelPosition());

	if (pPipeNode->getOwnerTable()->getConfig()->getStringValue(TableConfig::NameExtension) == "osgjs")
	{
		setUseProxyNode(false);
		mEnName = pPipeNode->getInfo().getModelNode().getStringValue(ModelNode::NameEnName);
	}
}

osg::Node* CGlobalNode::buildNode()
{
	if (SingletonPtr(OCResource) == NULL)
	{
		return NULL;
	}

	IDatabase* database = Singleton(OCResource).getArchive()->getDatabase();

	if (database)
	{
		FeatureRecord record;
		record.setFeatureUrl(mFeatureUrl);
		database->query(&record);
		String boundStr = record.getBoundString();
		if (!boundStr.empty())
		{
			osg::ref_ptr<osg::Node> model;
			if (mUseProxyNode)
			{
				osg::ref_ptr<osg::ProxyNode> node = new osg::ProxyNode;
				CVector4 bound = StringConverter::parseVector4(boundStr);
				node->setRadius(bound.w);
				node->setCenter(osg::Vec3(bound.x,bound.y,bound.z));
				node->setCenterMode(osg::ProxyNode::USER_DEFINED_CENTER);
				node->setDatabasePath("global://");
				node->setFileName(0,mFeatureUrl);
				node->setLoadingExternalReferenceMode(osg::ProxyNode::DEFER_LOADING_TO_DATABASE_PAGER);
				model = node;
			}
			else
			{
				osg::ref_ptr<osg::PagedLOD> node = new osg::PagedLOD;
				CVector4 bound = StringConverter::parseVector4(boundStr);
				node->setRadius(bound.w);
				node->setCenter(osg::Vec3(bound.x,bound.y,bound.z));
				node->setCenterMode(osg::PagedLOD::USER_DEFINED_CENTER);
				node->setFileName(0,"global/" + mEnName + ".osgjs");
				node->setRange(0,0,5000);
				model = node;
			}

			if (mOffset == CVector3::ZERO
				&& mScale == CVector3::UNIT_SCALE
				&& mQuat == CQuaternion::IDENTITY)
			{
				return model.release();
			}
			else
			{
				osg::MatrixTransform* trans = new osg::MatrixTransform;
				trans->setMatrix(osg::Matrix::scale(mScale.x,mScale.y,mScale.z) 
					* osg::Matrix::rotate(osg::Quat(mQuat.x,mQuat.y,mQuat.z,mQuat.w))
					* osg::Matrix::translate(osg::Vec3d(mOffset.x,mOffset.y,mOffset.z)));
				trans->addChild(model);
				return trans;
			}
		}
	}

	//miss something

	return NULL;
}