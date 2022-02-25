#ifndef _OCVOLUMCONVERTER_TOOL_H__
#define _OCVOLUMCONVERTER_TOOL_H__

#include "TaskWorkQueue.h"
using namespace OC;

int convertVolume(String filename,String outfilename,String varname);
int handleFile(OC::String filename,OC::TaskWorkQueue::RequestType type);
int clearCacher(time_t today);
void test();

time_t tickTheDayBefore(time_t daybefore = 0);
String parseTime(time_t t);

#endif