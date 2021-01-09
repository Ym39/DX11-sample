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
#include"MathHelper.h"
#include "Utility.h"
#include "SkinnedMeshShaderClass.h"

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

	bool operator==(const VertexType& rhs) const
	{
		bool sameBlendingInfo = true;

		if(!(blendingInfo.empty() && rhs.blendingInfo.empty()))
		{
			// Each vertex should only have 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (blendingInfo[i].blendingIndex != rhs.blendingInfo[i].blendingIndex ||
					abs(blendingInfo[i].blendingWeight - rhs.blendingInfo[i].blendingWeight) > 0.001)
				{
					sameBlendingInfo = false;
					break;
				}
			}
		}

		bool result1 = MathHelper::CompareVector3WithEpsilon(position, rhs.position);
		bool result2 = MathHelper::CompareVector3WithEpsilon(normal, rhs.normal);
		bool result3 = MathHelper::CompareVector2WithEpsilon(texture, rhs.texture);

		return result1 && result2 && result3 && sameBlendingInfo;
	}
};

struct InputVertex
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 weight;
	unsigned int boneIndices[4];
};

struct CtrlPoint
{
	XMFLOAT3 position;
	std::vector<BlendingIndexWeightPair> blendingInfo;

	CtrlPoint()
	{
		blendingInfo.reserve(4);
	}
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
	
	bool Initialize(ID3D11Device* device,HWND hwnd, ID3D11DeviceContext* deviceContext, char* fbxFilename, char* textureFilename);

	void Shutdown();
	void Render(float time,ID3D11DeviceContext* deviceContext,XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 lightPosition, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);

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
	void ProcessMesh(FbxMesh* mesh);
    
	XMFLOAT3 ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	XMFLOAT2 ReadUV(const FbxMesh* mesh, int controlPointIndex, int uvIndex);

	//½ºÄÌ·¹Åæ ¾Ö´Ï¸ÞÀÌ¼Ç
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessJointsAndAnmations(FbxNode* inNode);

	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);

	//void PlayAnimation(float time, ID3D11DeviceContext* deviceContext);
	void SetInputVertices();

	void UpdateBoneTransform(float time);



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

	unsigned int mTriangleCount;
	unordered_map<unsigned int, CtrlPoint*> mCtrlPoint;

	vector<VertexType> vertices;
	vector<InputVertex> inputVertices;
	vector<XMFLOAT3> mPositionList;
	vector<unsigned int> indices;
	unordered_map<VertexType, unsigned int> indexMapping;

	SkinnedMeshShaderClass* mShader;
	vector<XMMATRIX> mUpdateBoneTransfroms;

};



