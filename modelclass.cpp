////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass():m_Texture(nullptr),m_model(nullptr)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, char* textureFilename,char* modelFilename)
{
	bool result;

	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	//�� ���� �ؽ��ĸ� �ҷ���
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, char* textureFilename1, char* textureFilename2)
{
	bool result;

	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//�� ���� �ؽ��ĸ� �ҷ���
	result = LoadTexture(device, textureFilename1,textureFilename2);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, char* textureFilename1, char* textureFilename2, char* textureFilename3)
{
	bool result;

	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//�� ���� �ؽ��ĸ� �ҷ���
	result = LoadTexture(device, textureFilename1, textureFilename2,textureFilename3);
	if (!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	//���ؽ��ĸ� ������ �մϴ�.
	ReleaseTexture();

	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	//// Set the number of vertices in the vertex array.
	//m_vertexCount = 3;

	//// Set the number of indices in the index array.
	//m_indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���ؽ� �迭�� �ε��� �迭�� �����͸� �ҷ���
	for (int i = 0; i < m_vertexCount; ++i)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		indices[i] = i;
	}

	//// Load the vertex array with data.
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top left.
	//vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	//vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Top right.
	//vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	////vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	////vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	//// Load the index array with data.
	//indices[0] = 0;  // Bottom left.
	//indices[1] = 1;
	//indices[2] = 2;  // Top middle.
	////indices[3] = 0;  // Bottom right.
	////indices[4] = 2;
	////indices[5] = 3;

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	//�ؽ��� ��ü�� ����ϴ�.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	//�ؽ��� ��ü�� �ʱ�ȭ�մϴ�.
	bool result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, char* filename1, char* filename2)
{
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray)
	{
		return false;
	}

	//�ؽ��� ��ü�� �ʱ�ȭ�մϴ�.
	bool result = m_TextureArray->Initialize(device, filename1, filename2);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, char* filename1, char* filename2, char* filename3)
{
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray)
	{
		return false;
	}

	//�ؽ��� ��ü�� �ʱ�ȭ�մϴ�.
	bool result = m_TextureArray->Initialize(device, filename1, filename2,filename3);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	//�ؽ��� ��ü�� �������մϴ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = nullptr;
	}
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;

	//�������� ���ϴ�
	fin.open(filename);

	//���� �������� ���� ���ϸ� �Լ��� �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	//���ؽ� ī��Ʈ�� �н��ϴ�.
	char input;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> m_vertexCount;

	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for (int i = 0; i < m_vertexCount; ++i)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = nullptr;
	}

	return;
}
