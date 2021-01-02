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

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;

	bool operator==(const VertexType& other) const
	{
		return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z && texture.x == other.texture.x && texture.y == other.texture.y && normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z;
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
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount() const;
	ID3D11ShaderResourceView* GetTexture() const;

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


private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ModelType* m_model;

	vector<VertexType> vertices;
	vector<unsigned int> indices;
	unordered_map<VertexType, unsigned int> indexMapping;

};



