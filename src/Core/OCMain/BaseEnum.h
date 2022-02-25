#ifndef _OC_ENUM_H__
#define _OC_ENUM_H__

enum ArchiveDataFrom
{
	DATA_FROM_DB,
	DATA_FROM_NET,
	DATA_FROM_REFPATH,
	DATA_FROM_RAW_DATABASE
};

enum RegionEventState
{
	STANDBY,
	INVOKED
};

enum MovableFinishState
{
	Stay_Feature_Position,
	Stay_Start_Point,
	Stay_End_Point,
	Wait_For_Next_Point,
	Disappear
};

enum EffectState
{
	STATE_EMPTY,
	STATE_HIGHLIGHT,
	STATE_TEXTURE_MOVE,
	STATE_SCALE_ANIMATION,
	STATE_TRANSPARENT
};

enum FetureMode
{
	NormalFeature,
	RoleFeature,
	EffectFeature
};

#endif