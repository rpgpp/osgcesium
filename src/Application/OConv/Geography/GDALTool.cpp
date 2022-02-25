#include "GDALTool.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "OCMain/DataStream.h"

namespace OC
{
	GDALTool::GDALTool()
	{
        OGRRegisterAll();
        GDALAllRegister();
        CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	}

	GDALTool::~GDALTool()
	{

	}

    void GDALTool::testIDW(String filename)
    {
        std::ifstream ifs(filename.c_str());
        FileStreamDataStream dataStream(&ifs,false);
    }

	void GDALTool::testContour(String inputDEM, String outputSHP, double dfContourInterval)
	{
        GDALDataset* pInDataset = (GDALDataset*)GDALOpen(inputDEM.c_str(), GA_ReadOnly);
        if (pInDataset == NULL)
        {
            printf("��ȡͼ��ʧ�ܣ�");
            return;
        }

        int nDemWidth = pInDataset->GetRasterXSize(); //��ȡͼ���  
        int nDemHeight = pInDataset->GetRasterYSize(); //��ȡͼ���  
        int Count = pInDataset->GetRasterCount(); //������  


        //��ȡͼ�����ݲ���
        GDALRasterBand* pInRasterBand = pInDataset->GetRasterBand(1);
        float* pData = new float[nDemWidth * nDemHeight]();

        CPLErr err = pInRasterBand->RasterIO(GF_Read, 0, 0, nDemWidth, nDemHeight, pData, nDemWidth, nDemHeight, GDT_Float32, 0, 0);
        if (err == CE_Failure)
        {
            cout << "��ȡDEMͼ������ʱ����" << endl;
            GDALDestroyDriverManager();
            return;
        }

        //�ж�ͼ�����Ƿ����쳣ֵ������ȡ�쳣ֵʵ��ֵ
        float fNoData = 0;
        int nIdx;
        for (int i = 0; i < nDemHeight; i++)
        {
            for (int j = 0; j < nDemWidth; j++)
            {
                nIdx = i * nDemWidth + j;
                if (pData[nIdx] <= -9999)
                {
                    fNoData = pData[nIdx];
                }
            }
        }

        //����ʸ��ͼ
        const char* pszDriverName = "ESRI Shapefile";
        GDALDriver* poDriver;
        GDALAllRegister();
        poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (poDriver == NULL)
        {
            printf("%s driver not available.\n", pszDriverName);
            exit(1);
        }
        GDALDataset* poDS;
        poDS = poDriver->Create(outputSHP.c_str(), 0, 0, 0, GDT_Unknown, NULL); //��������Դ
        if (poDS == NULL)
        {
            printf("Creation of output file failed.\n");
            exit(1);
        }
        OGRLayer* poLayer;
        OGRSpatialReference* poSpatialRef = new OGRSpatialReference(pInDataset->GetProjectionRef());
        poLayer = poDS->CreateLayer("Elevation", poSpatialRef, wkbLineString, NULL); //����ͼ��
        if (poLayer == NULL)
        {
            printf("Layer creation failed.\n");
            exit(1);
        }


        OGRFieldDefn ofieldDef1("Elevation", OFTInteger);    //��ʸ��ͼ�д����߳�ֵ�ֶ�
        if (poLayer->CreateField(&ofieldDef1) != OGRERR_NONE)
        {
            cout << "����ʸ��ͼ�����Ա�ʧ�ܣ�" << endl;
            GDALClose((GDALDatasetH)poDS);
            GDALClose(pInDataset);
            return;
        }

        //����ͼ�񲨶�����ʸ��ͼ�ȸ���
        if (fNoData == 0)
            GDALContourGenerate(pInRasterBand, dfContourInterval, 0, 0, NULL, false, 0, (OGRLayerH)poLayer, -1, 0, NULL, NULL);
        else //���쳣ֵʱ�������쳣ֵ���д���
            GDALContourGenerate(pInRasterBand, dfContourInterval, 0, 0, NULL, true, fNoData, (OGRLayerH)poLayer, -1, 0, NULL, NULL);

        GDALClose(poDS);
        GDALClose(pInDataset);


        if (pData != NULL)
        {
            delete[] pData;
            pData = NULL;
        }
	}
}