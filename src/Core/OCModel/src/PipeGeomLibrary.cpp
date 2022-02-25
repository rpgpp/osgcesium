#include "PipeGeomLibrary.h"
#include "ModelUtility.h"

namespace OC
{	
	#if SingletonConfig == SingletonNormal
			template<>Modeling::PipeGeomLibrary* OC::CSingleton<Modeling::PipeGeomLibrary>::msSingleton= NULL;
	#elif SingletonConfig == SingletonProcess
			template<>std::map<int,Modeling::PipeGeomLibrary*> OC::CSingleton<Modeling::PipeGeomLibrary>::msPidton;
	#endif
	namespace Modeling
	{
		PipeGeomLibrary::PipeGeomLibrary()
		{
			mNormaizeState = new osg::StateSet;
			mNormaizeState->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
		}

		PipeGeomLibrary::~PipeGeomLibrary()
		{
			mInstanceTextures.clear();
		}

		osg::Node* PipeGeomLibrary::createInstance(CRectPipeLine* pipeline)
		{
			if (!mRectPipe.valid() || !mRectPipeClose.valid())
			{
				mRectPipe = createUnitRectPipe(false);
				mRectPipeClose = createUnitRectPipe(true);
			}

			PipePoint p1 = pipeline->getStPoint();
			PipePoint p2 = pipeline->getEdPoint();

			PipeSection sect1 = p1.getSection();
			PipeSection sect2 = p2.getSection();

			CVector3 refNormal = pipeline->getRefNormal();

			CVector3 direction = p2.getPoint() - p1.getPoint();
			CVector3 center = p1.getPoint() + direction * 0.5f;
			double length = direction.length();
			direction.normalise();


			CVector3 localY = direction.crossProduct(refNormal);
			CVector3 localZ = localY.crossProduct(direction);

			CQuaternion q1 = CVector3::UNIT_X.getRotationTo(direction);
			CQuaternion q2 = (q1 * CVector3::UNIT_Z).getRotationTo(localZ);

			osg::Matrixd rotate1 = osg::Matrixd(QuaternionToQuat(q1));
			osg::Matrixd rotate2 = osg::Matrixd(QuaternionToQuat(q2));

			double scaleX = length;
			double scaleY = sect1.getWidth();	
			double scaleZ = sect1.getHeight();
			osg::Matrixd matrix = osg::Matrixd::scale(scaleX,scaleY,scaleZ) * rotate1 *  rotate2 * osg::Matrixd::translate(Vector3ToVec3d(center));

#if 0
			osg::ref_ptr<osg::Geode> transform = new osg::Geode;
			osg::ref_ptr<osg::StateSet> stateset = transform->getOrCreateStateSet();
			if (!mPipeInstanceProgram.valid())
			{
				mPipeInstanceProgram = new osg::Program;
				mPipeInstanceProgram->setName("pipe_instance");
				osg::ref_ptr<osg::Shader> vertShader = osgDB::readShaderFile(osg::Shader::VERTEX,"global://shader/pipe_instance.vert");
				mPipeInstanceProgram->addShader(vertShader);
			}
			stateset->setAttributeAndModes(mPipeInstanceProgram,osg::StateAttribute::ON |  osg::StateAttribute::PROTECTED);		
			osg::ref_ptr<osg::Uniform>	 matrix_uniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "pipe_matrix");
			stateset->addUniform( matrix_uniform.get() );
			matrix_uniform->set(mat);			
#else
			osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
			transform->setMatrix(matrix);
#endif

			if (pipeline->getClose())
			{
				transform->addChild(mRectPipeClose->getDrawable(0));
			}
			else
			{
				transform->addChild(mRectPipe->getDrawable(0));
			}

			String filename = pipeline->getTextureImage();
			bool imageInternal = pipeline->getImageInternal();
			if (!filename.empty())
			{
				osg::StateSet* stateset = mInstanceTextures.find(filename);
				if (stateset == NULL)
				{
					stateset = ModelUtility::createTextureStateset(filename,imageInternal);
					stateset->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
					osg::TexMat* texmat = new osg::TexMat;
					texmat->setScaleByTextureRectangleSize(false);
					texmat->setMatrix(osg::Matrix::scale(osg::Vec3(length,1,1)));
					stateset->setTextureAttributeAndModes(0, texmat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setName(filename);
					//mInstanceTextures.insert(stateset);
				}
				transform->setStateSet(stateset);
			}
			else
			{
				transform->setStateSet(mNormaizeState);
			}

			return transform.release();
		}

		osg::Node* PipeGeomLibrary::createInstance(CCirclePipeLine* pipeline)
		{
			if (!mCirclePipe.valid())
			{
				mCirclePipe = createUnitCirclePipe();
			}

			PipePoint p1 = pipeline->getStPoint();
			PipePoint p2 = pipeline->getEdPoint();

			PipeSection sect1 = p1.getSection();
			PipeSection sect2 = p2.getSection();

			if (sect1 != sect2)
			{
				return NULL;
			}

			CVector3 refNormal = pipeline->getRefNormal();
			CVector3 direction = p2.getPoint() - p1.getPoint();
			CVector3 center = p1.getPoint() + direction * 0.5f;
			double length = direction.length();
			direction.normalise();
			CVector3 localY = direction.crossProduct(refNormal);
			CVector3 localZ = localY.crossProduct(direction);

			CQuaternion q1 = CVector3::UNIT_X.getRotationTo(direction);
			CQuaternion q2 = (q1 * CVector3::UNIT_Z).getRotationTo(localZ);

			osg::Matrixd rotate1 = osg::Matrixd(QuaternionToQuat(q1));
			osg::Matrixd rotate2 = osg::Matrixd(QuaternionToQuat(q2));

			double scaleX =length;
			double scaleYZ = sect1.getRadius();
			osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;

			osg::Matrixd mat = osg::Matrixd::scale(scaleX,scaleYZ,scaleYZ) * rotate1 * rotate2 * osg::Matrixd::translate(Vector3ToVec3d(center));
			transform->setMatrix(mat);

			if (pipeline->getClose())
			{
				transform->addChild(mCirclePipe);
			}
			else
			{
				transform->addChild(mCirclePipe->getDrawable(0));
			}

			String filename = pipeline->getTextureImage();
			bool imageInternal = pipeline->getImageInternal();
			
			if (!filename.empty())
			{
				osg::StateSet* stateset = mInstanceTextures.find(filename);
				if (stateset == NULL)
				{
					stateset = ModelUtility::createTextureStateset(filename,imageInternal);
					stateset->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
					osg::TexMat* texmat = new osg::TexMat;
					texmat->setScaleByTextureRectangleSize(false);
					texmat->setMatrix(osg::Matrix::scale(osg::Vec3(length,1,1)));
					stateset->setTextureAttributeAndModes(0, texmat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setName(filename);
					//mInstanceTextures.insert(stateset);
				}
				transform->setStateSet(stateset);
			}
			else
			{
				transform->setStateSet(mNormaizeState);
			}

			return transform.release();
		}

		osg::Geode* PipeGeomLibrary::createUnitCirclePipe()
		{
			PipeSection circleSecttion(1.0f);			
			CCirclePipeLine pipe;
			pipe.setStPoint(PipePoint(CVector3::UNIT_X * -0.5f,circleSecttion));
			pipe.setEdPoint(PipePoint(CVector3::UNIT_X * 0.5f,circleSecttion));
			pipe.setClose(true);
			return pipe.buildGeode();
		}
				   
		osg::Geode* PipeGeomLibrary::createUnitRectPipe(bool close)
		{
			PipeSection rectSect(1.0f,1.0f);
			CRectPipeLine pipe;
			pipe.setStPoint(PipePoint(CVector3::UNIT_X * -0.5f,rectSect));
			pipe.setEdPoint(PipePoint(CVector3::UNIT_X * 0.5f,rectSect));
			pipe.setClose(close);
			return pipe.buildGeode();
		}
	}
}





