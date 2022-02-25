#pragma once
#include "OCUtility/StringUtil.h"
#include "OCMain/JsonPackage.h"
#include "OCesium/glTFConverter.h"
#include "OCesium/ObliqueConverter.h"
#include "OCModel/ModelDataManager.h"
#include "OCZip/zip.h"
#include "TextureVisitor.h"
#include "OCesium/CesiumTool.h"

using namespace OC;
using namespace OC::Cesium;
using namespace OC::Modeling;

void initEnviroment();

void conv2obj(OC::Json::Value& root);
void offset2origin(OC::Json::Value& root);
void npy2txt(OC::Json::Value& root);
void Packmetadata(OC::Json::Value& root);
void PackJson(OC::Json::Value& root);
void PackGroup(OC::Json::Value& root);
void OCZip(OC::Json::Value& root);
void computeTexture(OC::Json::Value& root);

/*
{\"function\":\"obj23dtiles\",\"filename\":\"D:/Work/Data/1.IVE\",\"offset\":\"116.39123 39.90691 0 \",\"localOffset\":true,\"writeGltf\":true,\"AtlasImageSize\":2048,\"MaxTextureSize\":256}
*/
void obj23dtiles(OC::Json::Value& root);
void oblique23dtiles(OC::Json::Value& root);
void shp23dtiles(OC::Json::Value& root);

/*
{\"function\":\"csv23dtiles\",\"filename\":\"I:/Dev/Assets/pointcloud/14.csv\"}
*/
void csv23dtiles(OC::Json::Value& root);

/*
{\"function\":\"obj2gltf\",\"filename\":\"I:/Data/管井模型1128/ive/ysb.ive\",\"glb\":false}
*/
void obj2gltf(OC::Json::Value& root);
/*
{\"function\":\"pipe23dtiles\",\"filename\":\"I:/Data/pipe/WS\"}
*/
void pipe23dtiles(OC::Json::Value& root);
void nc2raw(OC::Json::Value& root);
void toVolume(OC::Json::Value& root);

/*
{\"function\":\"toi3dm\",\"filename\":\"I:/Dev/Cesium/objTo3d-tiles-master/bin/barrel/barrel.obj\"}
*/
void toi3dm(OC::Json::Value& root);

/*
{\"function\":\"decodekey\",\"key\":\"C0F2D04666C8639F3D859A9EF59D30BFF6CBF3D60985FF52D435712621D1A0C92A97363FE12254A1FE0DC7288E51C50A9DED1C349725692EB735E89021AA0836\"}
*/
void decodekey(String jsonStr);

/*
{\"function\":\"encodekey\",\"code\":\"3BC94E4\",\"uuid\":\"03C00218-044D-052E-FA06-4D0700080009\",\"year\":2021,\"month\":5,\"day\":19}
*/
void encodekey(String jsonStr);

/*
{\"function\":\"biosuuid\"}
*/
void biosuuid(String jsonStr);

/*
{\"function\":\"regiter\",\"code\":\"3BC94E4\",\"uuid\":\"03C00218-044D-052E-FA06-4D0700080009\",\"year\":2022,\"month\":5,\"day\":19}
*/
void regiter(String jsonStr);

/*
{\"function\":\"tokml\",\"filename\":\"I:/Project/GuiZhou/开发数据资料/自动站站点信息/安顺市考核自动站.csv.radarstation\"}
*/
void tokml(OC::Json::Value& root);

void json2ShpTable(OC::Json::Value& root);
void json2Mxd(OC::Json::Value& root);

void mapcaching(OC::Json::Value& root);

void test();


