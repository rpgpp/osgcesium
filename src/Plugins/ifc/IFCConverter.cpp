#include "IFCConverter.h"
//
#include <ifcpp/model/BasicTypes.h>
#include <ifcpp/model/BuildingObject.h>
#include <ifcpp/model/BuildingException.h>
#include <ifcpp/model/BuildingGuid.h>
#include <ifcpp/reader/ReaderSTEP.h>

#include <ifcpp/IFC4/include/IfcAxis2Placement3D.h>
#include <ifcpp/IFC4/include/IfcBuilding.h>
#include <ifcpp/IFC4/include/IfcBuildingStorey.h>
#include <ifcpp/IFC4/include/IfcGloballyUniqueId.h>
#include <ifcpp/IFC4/include/IfcProductDefinitionShape.h>
#include <ifcpp/IFC4/include/IfcShapeRepresentation.h>
#include <ifcpp/IFC4/include/IfcSite.h>
#include <ifcpp/IFC4/include/IfcText.h>
#include <ifcpp/IFC4/include/IfcWall.h>
#include "ifcpp\model\BuildingModel.h"
#include "ifcpp\reader\ReaderSTEP.h"
#include "ifcpp\geometry\Carve\GeometryConverter.h"
#include "ifcpp\geometry\Carve\ConverterOSG.h"

#include <carve/carve.hpp>

#include "OCMain/osg.h"
#include "OCUtility/StringUtil.h"
#include "OCLayer/IArchive.h"

using namespace OC;

//#define DEF_EPSILON 1.4901161193847656e-08
//
//namespace carve {
//	double EPSILON = DEF_EPSILON;
//	double EPSILON2 = DEF_EPSILON * DEF_EPSILON;
//}


IFCConverter::IFCConverter(void)
{

}


IFCConverter::~IFCConverter(void)
{
}

void resolveTreeItems(shared_ptr<BuildingObject> obj, std::unordered_set<int>& set_visited,IArchive* archive)
{

	std::vector<shared_ptr<IfcObjectDefinition> >::iterator it_object_def;
	std::vector<shared_ptr<IfcProduct> >::iterator it_product;

	shared_ptr<IfcObjectDefinition> obj_def = dynamic_pointer_cast<IfcObjectDefinition>(obj);
	if (obj_def)
	{
		if (set_visited.find(obj_def->m_entity_id) != set_visited.end())
		{
			return;
		}

		set_visited.insert(obj_def->m_entity_id);


		//osg::ref_ptr< FeatureRecord> item = new FeatureRecord();
		//item->setGID(obj_def->m_entity_id);

		if (obj_def->m_Name)
		{
			if (obj_def->m_Name->m_value.size() > 0)
			{
				String name = U2A(obj_def->m_Name->m_value.c_str());
				//item->setFeatureName(name);
			}
		}

		//archive->getDatabase()->insert(item,IDatabase::OP_ENTITY);

		//item->setText(1, QString::number(obj_def->m_entity_id));
		//item->setText(2, obj_def->className());

		if (obj_def->m_IsDecomposedBy_inverse.size() > 0)
		{
			std::vector<weak_ptr<IfcRelAggregates> >& vec_IsDecomposedBy = obj_def->m_IsDecomposedBy_inverse;
			std::vector<weak_ptr<IfcRelAggregates> >::iterator it;
			for (it = vec_IsDecomposedBy.begin(); it != vec_IsDecomposedBy.end(); ++it)
			{
				shared_ptr<IfcRelAggregates> rel_agg(*it);
				std::vector<shared_ptr<IfcObjectDefinition> >& vec = rel_agg->m_RelatedObjects;

				for (it_object_def = vec.begin(); it_object_def != vec.end(); ++it_object_def)
				{
					shared_ptr<IfcObjectDefinition> child_obj_def = (*it_object_def);
					resolveTreeItems(child_obj_def, set_visited, archive);
				}
			}
		}

		shared_ptr<IfcSpatialStructureElement> spatial_ele = dynamic_pointer_cast<IfcSpatialStructureElement>(obj_def);
		if (spatial_ele)
		{
			std::vector<weak_ptr<IfcRelContainedInSpatialStructure> >& vec_contained = spatial_ele->m_ContainsElements_inverse;
			if (vec_contained.size() > 0)
			{
				std::vector<weak_ptr<IfcRelContainedInSpatialStructure> >::iterator it_rel_contained;
				for (it_rel_contained = vec_contained.begin(); it_rel_contained != vec_contained.end(); ++it_rel_contained)
				{
					shared_ptr<IfcRelContainedInSpatialStructure> rel_contained(*it_rel_contained);
					std::vector<shared_ptr<IfcProduct> >& vec_related_elements = rel_contained->m_RelatedElements;
					std::vector<shared_ptr<IfcProduct> >::iterator it;

					for (it = vec_related_elements.begin(); it != vec_related_elements.end(); ++it)
					{
						shared_ptr<IfcProduct> related_product = (*it);

						resolveTreeItems(related_product, set_visited, archive);
					}
				}
			}
		}
	}

	return;
}

inline osg::Group* find(osg::Group* p)
{
	if (p->getNumChildren() > 1)
	{
		return p;
	}

	if (p->getNumChildren() == 1)
	{
		return find(p->getChild(0)->asGroup());
	}

	return NULL;
}

osg::Node* IFCConverter::read(const char* m_file_path)
{
	shared_ptr<ReaderSTEP>						m_step_reader;
	shared_ptr<BuildingModel>					m_ifc_model;
	m_ifc_model = shared_ptr<BuildingModel>( new BuildingModel() );
	m_step_reader = shared_ptr<ReaderSTEP>( new ReaderSTEP() );

	shared_ptr<GeometryConverter>				m_geometry_converter;
	m_geometry_converter = shared_ptr<GeometryConverter>( new GeometryConverter( m_ifc_model ) );

	m_geometry_converter->clearMessagesCallback();
	m_geometry_converter->resetModel();
	m_geometry_converter->getGeomSettings()->setNumVerticesPerCircle(16);
	m_geometry_converter->getGeomSettings()->setMinNumVerticesPerArc(4);

	OC::String file(m_file_path);
	m_step_reader->loadModelFromFile( A2U(file), m_geometry_converter->getBuildingModel() );
	const double length_in_meter = m_geometry_converter->getBuildingModel()->getUnitConverter()->getLengthInMeterFactor();
	m_geometry_converter->setCsgEps(1.5e-08 * length_in_meter);
	m_geometry_converter->convertGeometry();

	osg::ref_ptr<osg::Switch> model_switch = new osg::Switch;
	
	if (m_geometry_converter->getShapeInputData().size() > 0)
	{
		shared_ptr<ConverterOSG> converter_osg( new ConverterOSG( m_geometry_converter->getGeomSettings() ) );
		converter_osg->convertToOSG( m_geometry_converter->getShapeInputData(), model_switch );

		// in case there are IFC entities that are not in the spatial structure
		const std::map<std::string, shared_ptr<BuildingObject> >& objects_outside_spatial_structure = m_geometry_converter->getObjectsOutsideSpatialStructure();
		if (objects_outside_spatial_structure.size() > 0 && false)
		{
			osg::ref_ptr<osg::Switch> sw_objects_outside_spatial_structure = new osg::Switch();
			sw_objects_outside_spatial_structure->setName("IfcProduct objects outside spatial structure");

			converter_osg->addNodes(objects_outside_spatial_structure, sw_objects_outside_spatial_structure);
			if (sw_objects_outside_spatial_structure->getNumChildren() > 0)
			{
				model_switch->addChild(sw_objects_outside_spatial_structure);
			}
		}

		if (model_switch)
		{
			bool optimize = true;
			if (optimize)
			{
				osgUtil::Optimizer opt;
				opt.optimize(model_switch);
			}
		}

		//OC::String path = osgDB::getFilePath(file);
		//OC::String simple_filename = osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(file));

		//shared_ptr<IfcProject> project = m_ifc_model->getIfcProject();
#if 0
		if (project && false)
		{
			osg::ref_ptr<SqliteArchive> archive = new SqliteArchive;
			if (archive->open(path + "/" + simple_filename + ".db", IArchive::CREATE))
			{
				std::unordered_set<int> set_visited;
				resolveTreeItems(m_ifc_model->getIfcProject(), set_visited, archive);

				osg::ref_ptr<CMetaRecord> meta = new CMetaRecord;
				meta->setDBType(CMetaRecord::IFCBIM);
				meta->setDataFrom(ArchiveDataFrom::DATA_FROM_REFPATH);
				archive->getDatabase()->insert(meta);

				osg::ref_ptr<LayerRecord> layerrecord = new LayerRecord;
				layerrecord->setConfig(CLayerConfig::NameResident, true);
				layerrecord->setTable(simple_filename);
				layerrecord->setDisplayName(simple_filename);
				layerrecord->setVisible(true);
				archive->getDatabase()->insert(layerrecord);

				if (osg::Group* g = find(model_switch))
				{
					for (unsigned int i = 0;i<g->getNumChildren();i++)
					{
						osg::Node* node = g->getChild(i);
						String url = node->getName();
						if (url.empty())
						{
							url = StringUtil::GenGUID();
						}

						osg::ref_ptr<FeatureRecord> feature = new FeatureRecord;
						feature->setBoundString(FeatureRecord::parseBoundString(node));
						feature->setTable(simple_filename);
						feature->setFeatureUrl(osgDB::getSimpleFileName(url + ".osgb"));
						archive->writeFeature(feature);
						osgDB::writeNodeFile(*node, path + "/" + url + ".osgb");
					}
				}
				else
				{
					osg::ref_ptr<FeatureRecord> feature = new FeatureRecord;
					feature->setBoundString(FeatureRecord::parseBoundString(model_switch));
					feature->setTable(simple_filename);
					feature->setFeatureUrl(osgDB::getSimpleFileName(m_file_path));
					archive->writeFeature(feature);
					osgDB::writeNodeFile(*model_switch, path + "/" + simple_filename + ".osgb");
				}
			}
		}
#endif
	}

	return model_switch.release();
}

