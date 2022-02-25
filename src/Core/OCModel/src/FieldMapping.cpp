#include "FieldMapping.h"
#include "PipeLine.h"
#include "PipeNode.h"

namespace OC
{
	namespace Modeling
	{
		String FieldMapping::TagFieldMap = "FiledAlias";
		FieldMapping::FieldMapping()
			:mDirty(false)
			,mTiXmlElement(NULL)
		{
			mNodeQueryeHeader = new OCHeaderInfo;
			mNodeQueryeHeader->add(new OCFieldInfo("gid",OC_DATATYPE_INTEGER,true));
			mNodeQueryeHeader->getFieldMapping()[PipeLine::NameID] = "gid";
			mNodeQueryeHeader->add(new OCGeomFieldInfo("geom","POINT",0));
			mNodeQueryeHeader->getFieldMapping()[PipeLine::NameSpatialColumn] = "geom";

			mLineQueryHeader = new OCHeaderInfo;
			mLineQueryHeader->add(new OCFieldInfo("gid",OC_DATATYPE_INTEGER,true));
			mLineQueryHeader->getFieldMapping()[PipeLine::NameID] = "gid";
			mLineQueryHeader->add(new OCGeomFieldInfo("geom","LINESTRING",0));
			mLineQueryHeader->getFieldMapping()[PipeLine::NameSpatialColumn] = "geom";
			mLineQueryHeader->add(new OCFieldInfo("stnod",OC_DATATYPE_INTEGER));
			mLineQueryHeader->getFieldMapping()[PipeLine::NameStnodNo] = "stnod";
			mLineQueryHeader->add(new OCFieldInfo("ednod",OC_DATATYPE_INTEGER));
			mLineQueryHeader->getFieldMapping()[PipeLine::NameEdnodNo] = "ednod";
		}

		FieldMapping::~FieldMapping()
		{

		}

		FieldMappingPair FieldMapping::getMappingPairLine()
		{
			FieldMappingPair fmp;

			StringVector sv = getKeyAlignFieldsLine();
			for (StringVector::iterator it = sv.begin();it!=sv.end();it++)
			{
				String keyfield = *it;
				StringMap& sm = mLineQueryHeader->getFieldMapping();
				StringMap::iterator smi = sm.find(keyfield);
				if (smi != sm.end())
				{
					fmp.push_back(std::make_pair(smi->first,smi->second));
				}
			}

			return fmp;
		}

		FieldMappingPair FieldMapping::getMappingPairNode()
		{
			FieldMappingPair fmp;

			StringVector sv = getKeyAlignFieldsNode();
			for (StringVector::iterator it = sv.begin();it!=sv.end();it++)
			{
				String keyfield = *it;
				StringMap& sm = mNodeQueryeHeader->getFieldMapping();
				StringMap::iterator smi = sm.find(keyfield);
				if (smi != sm.end())
				{
					fmp.push_back(std::make_pair(smi->first,smi->second));
				}
			}

			return fmp;
		}

		void FieldMapping::fromXmlElement(TiXmlElement* element)
		{
 			mTiXmlElement = element;

			if(TiXmlNode* lineTableNode = element->FirstChild("Line"))
			{
				for(TiXmlNode *tableNode = lineTableNode->FirstChild("Field");tableNode;tableNode = tableNode->NextSibling("Field"))
				{
					String name = TinyXml::read_attribute(tableNode->ToElement(),"name");
					if(tableNode->FirstChild())
					{
						String val = tableNode->ToElement()->GetText();
						if (!name.empty() && !val.empty())
						{
							addToHeader(mLineQueryHeader,name,val);
						}
					}
				}
			}

			if(TiXmlNode* nodeTableNode = element->FirstChild("Node"))
			{
				for(TiXmlNode *tableNode = nodeTableNode->FirstChild("Field");tableNode;tableNode = tableNode->NextSibling("Field"))
				{
					String name = TinyXml::read_attribute(tableNode->ToElement(),"name");
					if(tableNode->FirstChild())
					{
						String val = tableNode->ToElement()->GetText();
						if (!name.empty() && !val.empty())
						{
							addToHeader(mNodeQueryeHeader,name,val);
						}
					}
				}
			}
		}

		inline void compatible(String& name)
		{
			if (name == "Sect")
			{
				name = PipeLine::NameSect;
			}
			else if (name == "inter1")
			{
				name = PipeLine::NameStnodInter;
			}
			else if (name == "inter2")
			{
				name = PipeLine::NameEdnodInter;
			}
			else if (name == "direction")
			{
				name = PipeLine::NameDirection;
			}
			else if (name == "z")
			{
				name = PipeNode::NameNodeElevation;
			}
			else if (name == "GroundZ")
			{
				name = PipeNode::NameGroundElevation;
			}
			else if (name == "ModelType")
			{
				name = PipeNode::NameNodeModelType;
			}
		}

		void FieldMapping::addToHeader(OCHeaderInfo* header,String cfg,String fieldname)
		{
			String cfgname = cfg;
			compatible(cfgname);

			osg::ref_ptr<OCFieldInfo> field = new OCFieldInfo();

			StringMap::iterator mapping = header->getFieldMapping().find(cfgname);
			if (mapping != header->getFieldMapping().end())
			{
				osg::ref_ptr<OCFieldInfo> defaultField = header->getFieldInfo(mapping->second);
				if (OCGeomFieldInfo* gfield = defaultField->asGeomFieldInfo())
				{
					field = new OCGeomFieldInfo(fieldname,gfield->GeometryType,gfield->SRID);
				}
				else
				{
					field->IsPrimary = defaultField->IsPrimary;
					field->TYPE = defaultField->TYPE;
					field->Name = fieldname;
				}
				header->replaceFieldInfo(defaultField->Name,field);
			}
			else if (header->getFieldInfo(fieldname) == NULL)
			{
				if (cfgname == PipeLine::NameSpatialColumn)
				{
					field = new OCGeomFieldInfo("","",0);
				}
				field->IsPrimary = (cfgname == PipeLine::NameID);
				field->Name = fieldname;
				header->add(field);
			}
			header->getFieldMapping()[cfgname] = fieldname;
		}

		String FieldMapping::getName()
		{
			return mName;
		}

		void FieldMapping::setName(String name)
		{
			mName = name;
			if (mTiXmlElement)
			{
				mTiXmlElement->SetAttribute("name",name);
			}
		}

		void FieldMapping::toXmlElement(TiXmlElement* element)
		{
			TiXmlElement* root = element ? element : new TiXmlElement(TagFieldMap);
			root->RemoveChild(root->FirstChild("LineTable"));
			root->RemoveChild(root->FirstChild("NodeTable"));

			TiXmlElement* element_line = new TiXmlElement("LineTable");
			{
				StringMap sm = mLineQueryHeader->getFieldMapping();
				if (!sm.empty())
				{
					StringMap::iterator it = sm.begin();
					for(;it!=sm.end();it++)
					{
						String name = it->first;
						String field = it->second;
						if ((name == PipeLine::NameID && field == "gid")
							||(name == PipeLine::NameSpatialColumn && field == "geom")
							||(name == PipeLine::NameStnodNo && field == "stnod")
							||(name == PipeLine::NameEdnodNo && field == "ednod"))
						{
							continue;
						}
						if (OCFieldInfo* fieldinfo = mLineQueryHeader->getFieldInfo(it->second))
						{
							TiXmlElement* fieldelement = fieldinfo->toXmlElement();
							fieldelement->SetAttribute("name",it->first);
							fieldelement->RemoveAttribute("type");
							fieldelement->RemoveAttribute("PrimaryKey");
							fieldelement->RemoveAttribute("GeometryType");
							fieldelement->RemoveAttribute("SRID");
							element_line->LinkEndChild(fieldelement);
						}
					}
				}
			}

			TiXmlElement* element_node = new TiXmlElement("NodeTable");
			{
				StringMap sm = mNodeQueryeHeader->getFieldMapping();
				if (!sm.empty())
				{
					StringMap::iterator it = sm.begin();
					for(;it!=sm.end();it++)
					{
						String name = it->first;
						String field = it->second;
						if ((name == PipeLine::NameID && field == "gid")
							||(name == PipeLine::NameSpatialColumn && field == "geom"))
						{
							continue;
						}
						if (OCFieldInfo* fieldinfo = mNodeQueryeHeader->getFieldInfo(it->second))
						{
							TiXmlElement* fieldelement = fieldinfo->toXmlElement();
							fieldelement->SetAttribute("name",it->first);
							fieldelement->RemoveAttribute("type");
							fieldelement->RemoveAttribute("PrimaryKey");
							fieldelement->RemoveAttribute("GeometryType");
							fieldelement->RemoveAttribute("SRID");
							element_node->LinkEndChild(fieldelement);
						}
					}
				}
			}
			

			element->LinkEndChild(element_line);
			element->LinkEndChild(element_node);
		}

		void FieldMapping::fromNameValues(NameValuePairList lineNVs,NameValuePairList nodeNVs)
		{
			{
				NameValuePairList::iterator it;
				for (it = lineNVs.begin();it!=lineNVs.end();it++)
				{
					String fn = it->second;
					if (fn.empty())
					{
						continue;
					}
					addToHeader(mLineQueryHeader,it->first,fn);
				}
			}
			{
				NameValuePairList::iterator it;
				for (it = nodeNVs.begin();it!=nodeNVs.end();it++)
				{
					String fn = it->second;
					if (fn.empty())
					{
						continue;
					}
					addToHeader(mNodeQueryeHeader,it->first,fn);
				}
			}
		}

		void FieldMapping::setLineQueryHeader(OCHeaderInfo* header)
		{
			mLineQueryHeader = header;
		}

		OCHeaderInfo* FieldMapping::getLineQueryHeader()
		{
			return mLineQueryHeader.get();
		}

		void FieldMapping::setNodeQueryHeader(OCHeaderInfo* header)
		{
			mNodeQueryeHeader = header;
		}

		OCHeaderInfo* FieldMapping::getNodeQueryHeader()
		{
			return mNodeQueryeHeader.get();
		}

		StringVector FieldMapping::getKeyAlignFieldsLine()
		{
			StringVector sv;
			sv.push_back(PipeLine::NameID);
			sv.push_back(PipeLine::NameSpatialColumn);
			sv.push_back(PipeLine::NameStnodNo);
			sv.push_back(PipeLine::NameEdnodNo);
			sv.push_back(PipeLine::NameSect);
			sv.push_back(PipeLine::NameStnodInter);
			sv.push_back(PipeLine::NameEdnodInter);
			sv.push_back(PipeLine::NameDirection);
			//sv.push_back(PipeLine::NameMaterial);
			return sv;
		}

		StringVector FieldMapping::getKeyAlignFieldsNode()
		{
			StringVector sv;
			sv.push_back(PipeNode::NameID);
			sv.push_back(PipeNode::NameSpatialColumn);
			sv.push_back(PipeNode::NameNodeNo);
			sv.push_back(PipeNode::NameNodeInter);
			sv.push_back(PipeNode::NameNodeElevation);
			sv.push_back(PipeNode::NameGroundElevation);
			sv.push_back(PipeNode::NameNodeModelType);
			//sv.push_back(PipeNode::NameMaterial);
			return sv;
		}


	}
}