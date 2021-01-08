#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include<fstream>
#include<fbxsdk.h>
#include<vector>
#include<unordered_map>
using namespace DirectX;
using namespace std;

#include"TextureClass.h"
#include"TextureArrayClass.h"

struct BlendingIndexWeightPair
{
	unsigned int blendingIndex;
	double blendingWeight;

	BlendingIndexWeightPair():
	blendingIndex(0),
	blendingWeight(0)
	{}
};

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	std::vector<BlendingIndexWeightPair> blendingInfo;
	//unsigned int boneIndex[4];
	//XMFLOAT3 boneWeight;

	VertexType()
	{
		blendingInfo.reserve(4);
		/*for (int i = 0; i < 4; i++)
		{
			boneIndex[i] = 0;
		}
		boneWeight.x = 0.0f;
		boneWeight.y = 0.0f;
		boneWeight.z = 0.0f;*/
	}

	bool operator==(const VertexType& other) const
	{
		return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z && texture.x == other.texture.x && texture.y == other.texture.y && normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z;
	}
};

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

	Joint():
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

struct Skeleton
{
	std::vector<Joint> joints;

	~Skeleton()
	{
	    for (auto& joint : joints)
	    {
		    Keyframe* remove = joint.animation;
			while (remove)
			{
			   Keyframe* temp = remove->next;
			   delete remove;
			   remove = temp;
			}
	    }
	}
};

namespace std {

	template <>
	struct hash<VertexType>
	{
		std::size_t operator()(const VertexType& v) const
		{
			using std::size_t;
			using std::hash;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<float>()(v.position.x)
				^ (hash<float>()(v.position.y) << 1)) >> 1)
				^ (hash<float>()(v.position.z) << 1);
		}
	};

}

class FbxModelClass
{
private:
	
	

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	FbxModelClass();
	FbxModelClass(const FbxModelClass&);
	~FbxModelClass();
	
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fbxFilename, char* textureFilename);

	void Shutdown();
	void Render(float time,ID3D11DeviceContext* deviceContext);

	int GetIndexCount() const;
	ID3D11ShaderResourceView* GetTexture() const;

	int GetCurrentKeyframe() const { return currentPlayKeyframe; }

private:
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

	void ReleaseTexture();
	bool LoadFBX(char* fbxFilename,ID3D11Device* device);
	void ReleaseModel();

	void LoadNode(FbxNode* node);
	void InsertVertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& uv);
	void ProcessControlPoints(FbxMesh* mesh, std::vector<XMFLOAT3>& positions);
    
	XMFLOAT3 ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	XMFLOAT2 ReadUV(const FbxMesh* mesh, int controlPointIndex, int uvIndex);

	//½ºÄÌ·¹Åæ ¾Ö´Ï¸ÞÀÌ¼Ç
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessJointsAndAnmations(FbxNode* inNode);

	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);

	//void PlayAnimation(float time, ID3D11DeviceContext* deviceContext);



private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ModelType* m_model;
	bool mHasAnimation;
	Skeleton mSkeleton;
	FbxScene* mFbxScene;
	std::string mAnmationName;
	FbxLongLong mAnimationLength;
	unsigned int currentPlayKeyframe;

	vector<VertexType> vertices;
	vector<XMFLOAT3> mPositionList;
	vector<unsigned int> indices;
	unordered_map<VertexType, unsigned int> indexMapping;

};



