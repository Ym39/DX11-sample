#pragma once
#include <directxmath.h>
#include<fbxsdk.h>
using namespace DirectX;


struct Keyframe
{
	FbxLongLong frameName;
	XMFLOAT4X4 globalTransfrom;
	Keyframe* next;

	Keyframe() :
		next(nullptr)
	{}

};

struct Joint
{
	std::string name;
	int parentIndex;
	XMFLOAT4X4 globalBindposeInverse;
	Keyframe* animation;
	FbxNode* node;

	Joint() :
		node(nullptr),
		animation(nullptr)
	{
		parentIndex = -1;
	}

	//~Joint()
	//{
	//	while (animation)
	//	{
	//		Keyframe* temp = animation->next;
	//		delete animation;
	//		animation = temp;
	//	}
	//}
};