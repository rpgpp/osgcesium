#include "CinRadC.h"

using namespace OC;
using namespace OC::Volume;

#if 0

#define	NUMBEROFBIN						4096  //ÿ����������

//--------------------------------------------------------------------------------------------------
//	�ļ���ʶ
typedef struct tagFILEIDENTIFIERS
{
	char	sFileID[4];						//�״����ݱ�ʶ(ԭʼ���ݱ�ʶ��'RD'Ϊ�״�ԭʼ����,'GD'Ϊ�״��������ݵ�...)
	float	fVersionNo;						//��ʾ���ݸ�ʽ�İ汾��
	long	lFileHeaderLength;	  //��ʾ�ļ�ͷ�ĳ���
}FILEIDENTIFIERS;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	վַ���
typedef struct tagRADARSITE
{
	char		sCountry[30];						//������
	char		sProvince[20];					//ʡ��
	char		sStation[40];						//վ��
	char		sStationNumber[10];	    //��վ��
	char		sRadarType[20];					//�״��ͺ�
	char		sLongitude[16];					//���ȣ��ı�������E������W��
	char		sLatitude[16];					//γ�ȣ��ı�����γN����γS��
	long		lLongitudeValue;				//���ȣ���λ1/1000�㣬����������������
	long		lLatitudeValue;					//γ�ȣ���λ1/1000�㣬��γ������γ����
	long		lHeight;								//���θ߶ȣ�mm��
	short		shMaxAngle;							//��������ڵ����ǣ���λ1/100�㣩
	short		shOptiAngle;						//��ѹ۲����ǣ���λ1/100�㣩
}RADARSITE;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	���ܲ���
typedef struct tagRADARPERFORMANCEPARAM
{
	long 			  lAntennaG;		//��������(0.001dB)
	USHORT 			usBeamH;			//��ֱ��������(1/100��)
	USHORT			usBeamL;			//ˮƽ��������(1/100��)
	UCHAR			  ucPolarization;	//����״̬
															//	 0-ˮƽ
															//	 1-��ֱ
															//	 2-˫ƫ��
															// 	 3-Բƫ��
															//	 4-����
	USHORT		usSidelobe;			//��һ�԰�(0.01dB)
	long		lPower;				//��ֵ����(��)
	long		lWavelength;		//����(΢��)
	USHORT		usLogA;				//�������ջ���̬��Χ(0.01dB)
	USHORT		usLineA;			//���Խ��ջ���̬��Χ(0.01dB)
	USHORT		usAGCP;				//AGC�ӳ���(΢��)
	USHORT	    usLogMinPower;		//�������ջ���С�ɲ⹦��(0.01dBm)
	USHORT	    usLineMinPower;		//���Խ��ջ���С�ɲ⹦��(0.01dBm)
	UCHAR		ucClutterT;			//�Ӳ�������ֵ(0.01dB)
	UCHAR		ucVelocityP;		//�����մ���ģʽ(ԭ���ٶȴ�����ʽ 0-�� 1-PPP 2-FFT)
														//	0 ~ PPP����
														//	1 ~ APRF
														//	2 ~ DPRF
														//  3 ~ ������
														//  4 ~ ȫ��FFT
														//  5 ~ ����FFT
														//  6 ~ ��λ����
	UCHAR	ucFilterP;				//�����Ӳ�������ʽ
														//	0-��
														//	1-�˲���1��ԭ���Ӳ�ͼ��
														//	2-�˲���2��ԭ���Ӳ�ͼ���˲�����
														//	3-�˲���3��ԭ���˲�����
														//	4-�˲���4��ԭ���׷�����
														//	8-���˲���
	UCHAR	ucNoiseT;				//����������ֵ(0~255)
	UCHAR	ucSQIT;					//SQI��ֵ(0.01)
	UCHAR	ucIntensityC;			//RVPǿ��ֵ������õ�ͨ��
														//	1-����ͨ��
														//	2-����ͨ��
	UCHAR	ucIntensityR;			//ǿ��ֵ�����Ƿ�����˾��붩��
														//	0-û��
														//	1-�ѽ���
}RADARPERFORMANCEPARAM;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	�ⳤ�仯�ṹ
typedef struct tagBINPARAM
{
	short	sCode;			//�仯�ṹ����
	short	Begin;			//��ʼ��λ��(10��)
	short	End;				//������λ��(10��)
	short	BinLength;	//�ⳤ(1/10��)
}BINPARAM;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	�����
typedef struct tagLAYERPARAM
{
	UCHAR	ucDataType;		//����۲�Ҫ��
										//	1-��ǿ��
										//	2-��Ҫ��	(��PRF)
										//	3-��Ҫ��	(˫PRF)
										//	4-˫��ƫ��
										//	5-˫��ƫ�������
										//	6-˫����(��������)
										//	7-˫����(��������)
	UCHAR	ucAmbiguousP;	//��ģ��״̬
										//	0-����ģ��
										//	1-������ģ��
										//	2-˫T��ģ��
										//	3-��ʽ��ģ��
										//	4-˫T��������ģ��
										//	5-��ʽ��������ģ��
										//	6-˫PPI��ģ��
										//	9-������ʽ
	USHORT	usSpeed;		//����ת��(0.01��/��)
	USHORT	usPRF1;			//��һ���ظ�Ƶ��(0.1Hz)
	USHORT	usPRF2;			//�ڶ����ظ�Ƶ��(0.1Hz)
	USHORT	usPulseWidth;	//����(΢��) //(��λ��0.1΢��) //20180129
	USHORT	usMaxV;			//���ɲ��ٶ�(����/��)
	USHORT	usMaxL;			//���ɲ����(10��)
	USHORT	usZBinWidth;	//ǿ�����ݿⳤ(1/10��)
	USHORT	usVBinWidth;	//�ٶ����ݿⳤ(1/10��)
	USHORT	usWBinWidth;	//�׿����ݿⳤ(1/10��)
	USHORT	usZBinNumber;	//ǿ�����ݿ���
	USHORT	usVBinNumber;	//�ٶ����ݿ���
	USHORT	usWBinNumber;	//�׿����ݿ���
	USHORT	usRecordNumber;	//ɨ�辶�����
	SHORT		sSwpAngle;		//����
												//	PPI-��д��һ��(1/100��)��������FFFF
												//	RHI-����
												//	VOL-��д����(1/100��)
	char		cDataForm;		//�������з�ʽ
										//	11-CorZ
										//	12-UnZ
										//	13-V
										//	14-W
										//	21-CorZ+UnZ
										//	22-CorZ+V+W
										//	23-UnZ+V+W
										//	24-CorZ+UnZ+V+W
										//	4X-˫ƫ��Ҫ������ģʽ
										//	6X-˫ƫ�������Ҫ������ģʽ
										//	8X-˫����Ҫ������ģʽ
	ULONG	ulDataBegin;	//�������ݿ�ʼλ��(�ֽڼ���)
}LAYERPARAM;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	�۲����
typedef struct tagRADAROBSERVATIONPARAM
{
	UCHAR		ucType;		//ɨ�跽ʽ
											//	1-RHI
											//	10-PPI
											//	1XX-XX���VOL
											//	200-����FFTɨ��
	USHORT	usSYear;	//�۲⿪ʼʱ�����(2000-)
	UCHAR		ucSMonth;	//�۲⿪ʼʱ�����(1-12)
	UCHAR		ucSDay;		//�۲⿪ʼʱ�����(1-31)
	UCHAR		ucSHour;	//�۲⿪ʼʱ���ʱ(0-23)
	UCHAR		ucSMinute;	//�۲⿪ʼʱ��ķ�(0-59)
	UCHAR		ucSSecond;	//�۲⿪ʼʱ�����(0-59)
	UCHAR		ucTimeP;	//ʱ����Դ
											//	0-�����ʱ��(1����δ��ʱ)
											//  1-�����ʱ��(1�����Ѷ�ʱ)
											//	2-GPS
											//	3-����
	ULONG		ulSMillisecond;	//�۲⿪ʼʱ������С��λ(΢��)
	UCHAR		ucCalibration;  //��У״̬
														//  0-��
														//	1-�Զ�
														// 	2-1�������˹�
														//  3-1�����˹�
														//	�����벻��
	UCHAR		ucIntensityI;	//ǿ�Ȼ��ִ���
	UCHAR		ucVelocityP;	//�ٶȴ�������
	USHORT		usZStartBin;	//ǿ����Ч���ݿ�ʼ����
	USHORT		usVStartBin;	//�ٶ���Ч���ݿ�ʼ����
	USHORT		usWStartBin;	//�׿���Ч���ݿ�ʼ����
	LAYERPARAM	LayerInfo[32];  //��������ݽṹ
	USHORT		usRHIA;			//RHI���ڵķ�λ��(0.01��Ϊ��λ)
								//PPI��VOLʱΪFFFF
								//
								//����FFTʱΪͨ����(128��256)
	short 		sRHIL;			//RHI���ڵ��������(0.01��Ϊ��λ) 
								//PPI��VOL�͵���FFTʱΪFFFF
								//��ɨʱ��λ����
								//����FFT��THIʱΪ��λ(0.01��Ϊ��λ)
	short 		sRHIH;			//RHI���ڵ��������(0.01��Ϊ��λ) 
								//PPI��VOL�͵���FFTʱΪFFFF
								//��ɨʱ��λ����
								//����FFT��THIʱΪ����(0.01��Ϊ��λ)
	USHORT		usEYear;		//�۲����ʱ�����(2000-)
	UCHAR			ucEMonth;		//�۲����ʱ�����(1-12)
	UCHAR			ucEDay;			//�۲����ʱ�����(1-31)
	UCHAR			ucEHour;		//�۲����ʱ���ʱ(0-23)
	UCHAR			ucEMinute;		//�۲����ʱ��ķ�(0-59)
	UCHAR			ucESecond;		//�۲����ʱ�����(0-59)
	UCHAR			ucETenth;		//�۲����ʱ���1/100��(0-99)
	USHORT		usZBinByte;		//ǿ�����ݿⳤ�仯���
														//	������д0
														//	������дռ���ֽ���
	BINPARAM	BinRange1[5];	//5��8�ֽ�(ǿ���ޱ仯������ֽ�)
	USHORT		usVBinByte;		//�ٶ����ݿⳤ�仯���
														//	������д0
														//	������дռ���ֽ���
	BINPARAM	BinRange2[5];	//5��8�ֽ�(�ٶ��ޱ仯������ֽ�)
	USHORT		usWBinByte;		//�׿����ݿⳤ�仯���
														//	������д0
														//	������дռ���ֽ���
	BINPARAM	BinRange3[5];	//5��8�ֽ�(�׿��ޱ仯������ֽ�)
}RADAROBSERVATIONPARAM;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	������Ϣ
typedef struct tagRADAROTHERINFORMATION
{
	char	sStationID[2];		//̨վ����
	char	sScanName[10];		//ɨ������
	char	sSpare[550];			//�����ֽ�
}RADAROTHERINFORMATION;
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	ԭʼ���ݿ�ṹ
typedef struct tagRADARDATABLOCK
{
	short	  sElevation;					//����(1/100��)
	USHORT	usAzimuth;					//��λ(1/100��)
	UCHAR		ucHour;								//ʱ
	UCHAR		ucMinute;							//��
	UCHAR		ucSecond;							//��
	ULONG		ulMillisecond;			//���С��(0.1����)
	UCHAR		ucCorZ[NUMBEROFBIN];		//���������Ӳ�������dBZֵ=(�洢ֵ-64)/2  0��ʾ��Ч
	UCHAR		ucUnZ[NUMBEROFBIN];		//δ���������Ӳ�������dBZֵ=(�洢ֵ-64)/2  0��ʾ��Ч						
	char		cV[NUMBEROFBIN];		//�ٶ�ֵ= �洢ֵ*70/256.  -128 ��ʾ��Чֵ -127��ʾ������
							//��ֵ����Զ���״�վ
							//��ֵ���������״�վ

	UCHAR	       ucW[NUMBEROFBIN];	  //�׿�ֵ=�洢ֵ*16/256.  0��ʾ��Ч
	SHORT		siZDR[NUMBEROFBIN];	 //��ַ�����ֵ����λΪ0.01dB   -0x8000 ��ʾ��Чֵ
	SHORT		siPHDP[NUMBEROFBIN];	  //�������ֵ����λΪ0.01    -0x8000 ��ʾ��Чֵ
	SHORT		siROHV[NUMBEROFBIN];	  //���ϵ��ֵ(��λΪ0.001)     0 ��ʾ��Чֵ
	SHORT		uKDP[NUMBEROFBIN];   		//KDP,��λ��0.01        -0x8000 ��ʾ��Чֵ
}RADARDATABLOCK;
//--------------------------------------------------------------------------------------------------


CinRadC::CinRadC()
{

}

CinRadC::~CinRadC()
{

}


void f()
{
	String filename = "E:\\volume\\data\\QGXGLBVT080723171208.009";

	std::ifstream ifs(filename.c_str(), std::ios::binary);
	if (ifs)
	{
		tagFILEIDENTIFIERS fileIdendityiers;
		ifs.read((char*)&fileIdendityiers, sizeof(tagFILEIDENTIFIERS));

		tagRADARSITE radarSite;
		ifs.read((char*)&radarSite, sizeof(tagRADARSITE));

		tagRADARPERFORMANCEPARAM radarPerformanceParam;
		ifs.read((char*)&radarPerformanceParam, sizeof(tagRADARPERFORMANCEPARAM));

		BINPARAM binpParam;
		ifs.read((char*)&binpParam, sizeof(BINPARAM));

		RADAROBSERVATIONPARAM radarObservationParam;
		ifs.read((char*)&radarObservationParam, sizeof(RADAROBSERVATIONPARAM));

		RADAROTHERINFORMATION radarOtherInfomation;
		ifs.read((char*)&radarOtherInfomation, sizeof(RADAROTHERINFORMATION));

		typedef std::map<int, std::vector< RADARDATABLOCK>> RadarDataBlockMap;

		RadarDataBlockMap radarDataBlockMap;

		RADARDATABLOCK radarDataBlock;
		while (ifs.read((char*)&radarDataBlock, sizeof(RADARDATABLOCK)))
		{
			int pitch = radarDataBlock.sElevation;
			radarDataBlockMap[pitch].push_back(radarDataBlock);
			memset(&radarDataBlock, 0, sizeof(RADARDATABLOCK));
		}

		ifs.close();
	}
}

#endif
