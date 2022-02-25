#ifndef _CINRADHEADER_H__
#define _CINRADHEADER_H__

/*
雷达基数据SA,SB,CA
*/
struct CINRAD_Record
{
	char reserve[14];
	unsigned short radType;
	char reserve2[10];
	unsigned int  RadialCollTime;
	unsigned short JulianTime;
	unsigned short UnDimmedDistance;
	unsigned short Heading;
	unsigned short RadialDataNumber;
	unsigned short RadialDataState;//0:该仰角第一条径向数据，1:中间...，2:最后...，3:体扫开始的第一条径向数据,4:..结束..
	unsigned short Pitching; //[x/8.0] * [180/4096]
	unsigned short PitchingNo;
	unsigned short DistanceOfFirstDBZ;
	unsigned short DistanceOfFirstDoppler;
	unsigned short PerStockLengthOfDBZ;
	unsigned short PerStockLengthOfDoppler;
	unsigned short NumStockOfDBZ;
	unsigned short NumStockOfDoppler;
	unsigned short SectNo;
	unsigned int fixConstantNumber;
	unsigned short PtrOfDBZ;
	unsigned short PtrOfSpeed;
	unsigned short PtrOfSpectralWidth;
	unsigned short DopplerResolution;//2:0.5m/s 4:1.0m/s
	unsigned short VCR;//11:降水模式，16层仰角 21:降水模式，14层仰角，31：晴空模式，8层仰角，41：晴空模式7层仰角
	char reserve3[8];
	unsigned short PtrOfDBZ_RP;
	unsigned short PtrOfSpeed_RP;
	unsigned short PtrOfSpectralWidth_RP;
	unsigned short NyqueistSpeed;// x/100 = m/s
	char reserve4[38];
};

struct CINRAD_RecordSASB
{
	CINRAD_Record header;
	unsigned char bufferDBZ[460];
	unsigned char bufferDoppler[920];
	unsigned char bufferSpectralWidth[920];
	char reserve5[4];
};

struct CINRAD_RecordCA
{
	CINRAD_Record header;
	unsigned char bufferDBZ[800];
	unsigned char bufferDoppler[1600];
	unsigned char bufferSpectralWidth[1600];
	char reserve5[4];
};

/*
天擎雷达基数据SA,SB,CA的FMT格式,与X波段兼容相同
*/
struct CINRAD_RecordFMT_Generic
{
	int magicNumber;
	short majorVersion;
	short minorVersion;
	int genericType;
	int productType;
	char reserved[16];
};

struct CINRAD_RecordFMT_SiteConfig
{
	char siteCode[8];
	char siteName[32];
	float latitude;
	float longitude;
	int antennaHeight;
	int groundHeight;
	float frequency;
	float beamWidthHori;
	float beamWidthVert;
	int rdaVersion;
	short radarType;
	char reserved2[54];
};

struct CINRAD_RecordFMT_TaskConfig
{
	char taskName[32];
	char taskDescription[128];
	int polarizationType;
	int scanType;
	int pulseWidth;
	int scanStartTime;
	int cutNumber;
	float horizontalNoise;
	float verticalNoise;
	float horizontalCalibration;
	float verticalCalibration;
	float horizonalNoiseTemperature;
	float verticalNoiseTemperature;
	float dzrCalibration;
	float phidpCalibration;
	float ldrCalibration;
	char reserved3[40];
};

//扫描配置块
struct CINRAD_RecordFMT_CutConfig
{
	int processMode;
	int waveForm;
	float prf1;
	float prf2;
	int dealiasingMode;
	float azimuth;
	float elevation;
	float startAngle;
	float endAngle;
	float angularResolution;
	float scanSpeed;
	int logResolution;
	int dopplerResolution;
	int maximumRange1;
	int maximumRange2;
	int startRange;
	int sample1;
	int sample2;
	int phaseMode;
	float atmosphericLoss;
	float nyquistSpeed;
	long momentsMask;
	long momentsSizeMask;
	int miscFileterMask;
	float sqiThreshold;
	float sigThreshold;
	float csrThreshold;
	float logThreshold;
	float cpaThreshold;
	float pmiThreshold;
	float dplogThreshold;
	char thresholdsr[4];
	int dBTMask;
	int dBZMask;
	int velocityMask;
	int spectrumMask;
	int dpMask;
	char maskReserved[12];
	int scanSync;
	int direction;
	short groundClutterClassicType;
	short groundClutterFilterType;
	short groundClutterFilterNotchWidth;
	short groundClutterFilterWindow;
	char reserved3[80];
};

//径向数据块
struct CINRAD_RadialDataFMT
{
	int radialState;
	int spotBlank;
	int sequenceNumber;
	int radialNumber;
	int elevationNumber;
	float azimunth;
	float elevation;
	int seconds;
	int microseconds;
	int lengthOfdata;
	int momentNumber;
	char reserved[20];
};

struct CINRAD_MomentDataFMT
{
	int dataType;
	int scale;
	int offset;
	short binLength;
	short flags;
	int length;
	char reserved[12];
};

struct CINRAD_DataFMT
{
	CINRAD_RadialDataFMT Radial;
	CINRAD_MomentDataFMT Moment;
	std::vector<char> bufferDBZ;
	std::vector<char> bufferDoppler;
	std::vector<char> bufferSpectralWidth;
};

struct CINRAD_RecordFMT
{
	CINRAD_RecordFMT_Generic	Generic;
	CINRAD_RecordFMT_SiteConfig SiteConfig;
	CINRAD_RecordFMT_TaskConfig TaskConfig;
};

enum Type
{
	SASB,
	CA,
	SASBCAXFMT,
	UNKNOWN
};

#endif // !1
