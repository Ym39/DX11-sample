#include "FbxModelClass.h"

FbxModelClass::FbxModelClass():
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_Texture(nullptr),
	m_vertexCount(0),
	m_indexCount(0),
	currentPlayKeyframe(0)
{
}

FbxModelClass::FbxModelClass(const FbxModelClass&)
{
}

FbxModelClass::~FbxModelClass()
{
}

bool FbxModelClass::Initialize(ID3D11Device* device,HWND hwnd, ID3D11DeviceContext* deviceContext, char* fbxFilename, char* textureFilename)
{
	bool result;

	result = LoadFBX(fbxFilename,device);
	if (result == false)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, textureFilename);
	if (result == false)
	{
		return false;
	}

	mShader = new SkinnedMeshShaderClass;
	result=mShader->Initialize(device, hwnd);
	if (result == false)
	{
		return false;
	}

	return true;
}

void FbxModelClass::Shutdown()
{
	ShutdownBuffers();

	ReleaseTexture();

	ReleaseModel();

	mShader->Shutdown();

	return;
}

void FbxModelClass::Render(float time, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 lightPosition, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
    //PlayAnimation(time,deviceContext);

	UpdateBoneTransform(time);

	RenderBuffers(deviceContext);

	mShader->Render(deviceContext, GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, GetTexture(), lightPosition, diffuseColor, ambientColor, cameraPosition, specularColor, specularPower, mUpdateBoneTransfroms);

	return;
}

int FbxModelClass::GetIndexCount() const
{
	return indices.size();
}

ID3D11ShaderResourceView* FbxModelClass::GetTexture() const
{
	return m_Texture->GetTexture();
}

void FbxModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

void FbxModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(InputVertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool FbxModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
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

void FbxModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

}

bool FbxModelClass::LoadFBX(char* fbxFilename, ID3D11Device* device)
{
	FbxManager* manager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(manager, "");

	bool status = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
	if (status == false)
	{
		return false;
	}

	mFbxScene = FbxScene::Create(manager, "scene");
	importer->Import(mFbxScene);
	importer->Destroy();

	FbxNode* rootNode = mFbxScene->GetRootNode();

	FbxAxisSystem sceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::DirectX.ConvertScene(mFbxScene);

	FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(mFbxScene, true);

	ProcessSkeletonHierarchy(rootNode);
	LoadNode(rootNode);
	SetInputVertices();

	mUpdateBoneTransfroms.resize(mSkeleton.joints.size());

	for (int i = 0; i < mSkeleton.joints.size(); i++)
	{
		const Keyframe* curKey = mSkeleton.joints[i].animation;
		
		XMMATRIX boneOffset = XMLoadFloat4x4(&mSkeleton.joints[i].globalBindposeInverse);
		XMMATRIX toParent = XMLoadFloat4x4(&curKey->globalTransfrom);
		XMMATRIX finalTransform = XMMatrixMultiply(boneOffset, toParent);
		finalTransform *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

		mUpdateBoneTransfroms[i] = XMMatrixTranspose(finalTransform);
	}


	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(InputVertex) * inputVertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &(inputVertices.front());
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	/*vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &(vertices.front());
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;*/

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = &(indices.front());
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void FbxModelClass::ReleaseModel()
{
}

void FbxModelClass::LoadNode(FbxNode* node)
{
    //ProcessSkeletonHierarchy(node);
	if (mSkeleton.joints.empty())
	{
		mHasAnimation = false;
	}
	else
	{
		mHasAnimation = true;
	}

	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

	if (nodeAttribute != nullptr)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* mesh = node->GetMesh();

			std::vector<XMFLOAT3> positions;
			ProcessControlPoints(mesh, positions);
			if (mHasAnimation)
			{
				ProcessJointsAndAnmations(node);
			}
			ProcessMesh(mesh);

			/*unsigned int triCount = mesh->GetPolygonCount();
			unsigned int vertexCount = 0;

			for (unsigned int i = 0; i < triCount; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					int controlPointIndex = mesh->GetPolygonVertex(i, j);

					XMFLOAT3 position = positions[controlPointIndex];
					XMFLOAT3 normal = ReadNormal(mesh, controlPointIndex, vertexCount);
					XMFLOAT2 uv = ReadUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j));

					InsertVertex(position, normal, uv);

					vertexCount++;
				}
			}

			if (mHasAnimation == true)
	        {
	         	ProcessJointsAndAnmations(node);
	        }*/
		}
	}

	const int childCount = node->GetChildCount();
	for (unsigned i = 0; i < childCount; ++i)
	{
		LoadNode(node->GetChild(i));
	}
}

void FbxModelClass::InsertVertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& uv)
{
	VertexType vertex;
	vertex.position = position;
	vertex.normal = normal;
	vertex.texture = uv;

	mPositionList.push_back(position);
 
	auto lookup = indexMapping.find(vertex);

	if (lookup != indexMapping.end())
	{
		indices.push_back(lookup->second);
	}
	else
	{
		unsigned int index = vertices.size();
		indexMapping[vertex] = index;
		indices.push_back(index);
		vertices.push_back(vertex);
	}
}

void FbxModelClass::ProcessControlPoints(FbxMesh* mesh, std::vector<XMFLOAT3>& positions)
{
	/*unsigned int count = mesh->GetControlPointsCount();
	positions.resize(count);

	for (unsigned int i = 0; i < positions.size(); i++)
	{
		positions[i] = XMFLOAT3(mesh->GetControlPointAt(i).mData[0], mesh->GetControlPointAt(i).mData[1], mesh->GetControlPointAt(i).mData[2]);
	}*/
	unsigned int ctrlPointCount = mesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint;
		XMFLOAT3 currPosition = XMFLOAT3(mesh->GetControlPointAt(i).mData[0], mesh->GetControlPointAt(i).mData[1], mesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->position = currPosition;
		mCtrlPoint[i] = currCtrlPoint;
	}
}

void FbxModelClass::ProcessMesh(FbxMesh* mesh)
{
	mTriangleCount = mesh->GetPolygonCount();
	int vertexCount = 0;

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			int controlPointIndex = mesh->GetPolygonVertex(i, j);
			CtrlPoint* currCtrlPoint = mCtrlPoint[controlPointIndex];

			VertexType temp;
			temp.position = currCtrlPoint->position;
			temp.normal = ReadNormal(mesh, controlPointIndex, vertexCount);
			temp.texture = ReadUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j));

			for (unsigned int i = 0; i < currCtrlPoint->blendingInfo.size(); ++i)
			{
				BlendingIndexWeightPair currBlendingInfo;
				currBlendingInfo.blendingIndex = currCtrlPoint->blendingInfo[i].blendingIndex;
				currBlendingInfo.blendingWeight = currCtrlPoint->blendingInfo[i].blendingWeight;
				temp.blendingInfo.push_back(currBlendingInfo);
			}

			auto lookup = indexMapping.find(temp);

			if (lookup != indexMapping.end())
			{
				indices.push_back(lookup->second);
			}
			else
			{
				unsigned int index = vertices.size();
				indexMapping[temp] = index;
				indices.push_back(index);
				vertices.push_back(temp);
			}
		}
	}

	for (auto itr = mCtrlPoint.begin(); itr != mCtrlPoint.end(); ++itr)
	{
		delete itr->second;
	}
	mCtrlPoint.clear();

	mPositionList.resize(vertices.size());
	for (int i = 0; i < mPositionList.size(); i++)
	{
		mPositionList[i] = vertices[i].position;
	}
}

XMFLOAT3 FbxModelClass::ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	if (mesh->GetElementNormalCount() < 1)
		return XMFLOAT3();

	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	XMFLOAT3 result;

	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	    {
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
 	    }
		break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
			result.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
		break;
	}

	return result;
}

XMFLOAT2 FbxModelClass::ReadUV(const FbxMesh* mesh, int controlPointIndex, int uvIndex)
{
	if (mesh->GetElementUVCount() < 1)
		return XMFLOAT2();

	const FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
	XMFLOAT2 result;

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(uvIndex).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(uvIndex).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(uvIndex);
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;
		}
	}
	break;
	}

	return result;
}

void FbxModelClass::ProcessSkeletonHierarchy(FbxNode* inRootNode)
{

	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode,0,0,-1);
	}
}

void FbxModelClass::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.parentIndex = inParentIndex;
		currJoint.name = inNode->GetName();
		mSkeleton.joints.push_back(currJoint);
	}
	for (int i = 0; i<inNode->GetChildCount();i++)
	{
	    ProcessSkeletonHierarchyRecursively(inNode->GetChild(i),inDepth+1,mSkeleton.joints.size(),myIndex);
	}
}

void FbxModelClass::ProcessJointsAndAnmations(FbxNode* inNode)
{
	if (inNode == nullptr)
	{
		return;
	}

    FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	
	FbxAMatrix gemotryTransform = GetGeometryTransformation(inNode);

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex,FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}

		unsigned int numOfCluster = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfCluster; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * gemotryTransform;

			/*XMMATRIX xmGlobalBindposeInverseMatrix=XMMatrixIdentity();
			FbxVector4 translation = globalBindposeInverseMatrix.GetT();
			FbxVector4 rotation = globalBindposeInverseMatrix.GetR();

			xmGlobalBindposeInverseMatrix *= XMMatrixRotationX(-XMConvertToRadians(rotation.mData[0]));
			xmGlobalBindposeInverseMatrix *= XMMatrixRotationY(-XMConvertToRadians(rotation.mData[1]));
			xmGlobalBindposeInverseMatrix *= XMMatrixRotationZ(XMConvertToRadians(rotation.mData[2]));
			xmGlobalBindposeInverseMatrix *= XMMatrixTranslation(translation.mData[0], translation.mData[1], -translation.mData[2]);*/

			XMFLOAT4X4 boneOffset;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					boneOffset.m[i][j] = globalBindposeInverseMatrix.Get(i,j);
				}
			}

			mSkeleton.joints[currJointIndex].globalBindposeInverse = boneOffset;
			mSkeleton.joints[currJointIndex].node = currCluster->GetLink();

			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
		     	BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.blendingIndex = currJointIndex;
				currBlendingIndexWeightPair.blendingWeight = currCluster->GetControlPointWeights()[i];
				//vertices[currCluster->GetControlPointIndices()[i]].blendingInfo.push_back(currBlendingIndexWeightPair);
				mCtrlPoint[currCluster->GetControlPointIndices()[i]]->blendingInfo.push_back(currBlendingIndexWeightPair);
				/*vertices[currCluster->GetControlPointIndices()[i]].boneIndex[i] = currJointIndex;
				switch (i)
				{
				case 0:
					vertices[currCluster->GetControlPointIndices()[i]].boneWeight.x = currCluster->GetControlPointWeights()[i];
					break;
				case 1:
					vertices[currCluster->GetControlPointIndices()[i]].boneWeight.y = currCluster->GetControlPointWeights()[i];
					break;
				case 2:
					vertices[currCluster->GetControlPointIndices()[i]].boneWeight.z = currCluster->GetControlPointWeights()[i];
					break;
				}*/
			}
			
			FbxAnimStack* currAnimStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			mAnmationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = mFbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30) +1;
			Keyframe** currAnim = &mSkeleton.joints[currJointIndex].animation;

			for(FbxLongLong i = start.GetFrameCount(FbxTime::eFrames30); i<end.GetFrameCount(FbxTime::eFrames30); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i,FbxTime::eFrames30);
				*currAnim = new Keyframe();
				(*currAnim)->frameName = i;
				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * gemotryTransform;
				FbxAMatrix fbxGlobalTransform= currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);

				/*XMMATRIX xmGlobalTr = XMMatrixIdentity();
				FbxVector4 translation = fbxGlobalTransform.GetT();
				FbxVector4 rotation = fbxGlobalTransform.GetR();

				xmGlobalTr *= XMMatrixRotationX(-XMConvertToRadians(rotation.mData[0]));
				xmGlobalTr *= XMMatrixRotationY(-XMConvertToRadians(rotation.mData[1]));
				xmGlobalTr *= XMMatrixRotationZ(XMConvertToRadians(rotation.mData[2]));
				xmGlobalTr *= XMMatrixTranslation(translation.mData[0], translation.mData[1], -translation.mData[2]);*/

				XMFLOAT4X4 key;
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						key.m[i][j] = fbxGlobalTransform.Get(i, j);
					}
				}

				(*currAnim)->globalTransfrom = key;
				//(*currAnim)->globalTransfrom = XMMatrixTranspose(xmGlobalTr);
				currAnim = &((*currAnim)->next);
			}
		}
	}

	BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.blendingIndex = 0;
	currBlendingIndexWeightPair.blendingWeight = 0;
	for (auto itr = mCtrlPoint.begin(); itr != mCtrlPoint.end(); ++itr)
	{
		for (unsigned int i = itr->second->blendingInfo.size(); i < 4; ++i)
		{
			itr->second->blendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

FbxAMatrix FbxModelClass::GetGeometryTransformation(FbxNode* inNode)
{
    const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT,lR,lS);
}

unsigned int FbxModelClass::FindJointIndexUsingName(const std::string& inJointName)
{
	for (unsigned int i = 0; i < mSkeleton.joints.size(); ++i)
	{
		if (mSkeleton.joints[i].name == inJointName)
		{
			return i;
		}
	}

	return 0;
}

void FbxModelClass::SetInputVertices()
{
	inputVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		inputVertices[i].position = vertices[i].position;
		inputVertices[i].normal = vertices[i].normal;
		inputVertices[i].texture = vertices[i].texture;

		for (int j = 0; j < vertices[i].blendingInfo.size(); ++j)
		{
			inputVertices[i].boneIndices[j] = vertices[i].blendingInfo[j].blendingIndex;
			switch (j)
			{
			case 0:
				inputVertices[i].weight.x = vertices[i].blendingInfo[j].blendingWeight;
				break;
			case 1:
				inputVertices[i].weight.y = vertices[i].blendingInfo[j].blendingWeight;
				break;
			case 2:
				inputVertices[i].weight.z = vertices[i].blendingInfo[j].blendingWeight;
				break;
			}
		}
	}

}

void FbxModelClass::UpdateBoneTransform(float time)
{
	static float lateTime = 0.0f;
	lateTime += time;

	if (lateTime >= 1000.0f / 30.0f)
	{
		for (int i = 0; i < mSkeleton.joints.size(); i++)
		{
			const Keyframe* curKey = mSkeleton.joints[i].animation;
			for (int keyCount = 0; keyCount < currentPlayKeyframe; keyCount++)
			{
				curKey = curKey->next;
			}

			XMMATRIX boneOffset = XMLoadFloat4x4(&mSkeleton.joints[i].globalBindposeInverse);
			XMMATRIX toParent = XMLoadFloat4x4(&curKey->globalTransfrom);
			XMMATRIX finalTransform = XMMatrixMultiply(boneOffset, toParent);
		    finalTransform *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

			mUpdateBoneTransfroms[i] = XMMatrixTranspose(finalTransform);
		}

		currentPlayKeyframe++;
		lateTime = 0.0f;

		if (currentPlayKeyframe == mAnimationLength - 1)
		{
			currentPlayKeyframe = 0;
			lateTime = 0.0f;
		}

		/*if (currentPlayKeyframe == 7)
		{
			int ff = currentPlayKeyframe;
		}*/


	}
}

//void FbxModelClass::PlayAnimation(float time, ID3D11DeviceContext* deviceContext)
//{
//    static float lateTime = 0.0f;
//	lateTime += time;
//
//	if (lateTime >= 1000.0f / 30)
//	{
//		for (int i = 0; i<vertices.size();i++)
//		{
//		    XMVECTOR vertexPos = XMVectorZero();
//			XMVECTOR originVertexPosition = XMLoadFloat3(&mPositionList[i]);
//			float lastWeight = 1;
//			float weight;
//		    for (auto& v = vertices[i].blendingInfo.begin(); v != vertices[i].blendingInfo.end()-2; v++)
//		    {
//			    const Joint& curJoint = mSkeleton.joints[v->blendingIndex];
//				const Keyframe* curKey = curJoint.animation;
//				for (int keyCount = 0; keyCount < currentPlayKeyframe; keyCount++)
//				{
//				    curKey = curKey->next;
//				}
//
//
//				XMMATRIX offset = XMLoadFloat4x4(&curJoint.globalBindposeInverse);
//				XMMATRIX toRoot = XMLoadFloat4x4(&curKey->globalTransfrom);
//
//				XMMATRIX finalTransfrom = XMMatrixMultiply(offset,toRoot);
//				XMVECTOR localPosition = XMVector3TransformCoord(originVertexPosition,offset);
//
//				
//				XMVECTOR skinnedLocalPos = XMVector3TransformCoord(localPosition,finalTransfrom);
//				XMVECTOR det = XMMatrixDeterminant(offset);
//				XMVECTOR skinnedWorldPos = XMVector3TransformCoord(skinnedLocalPos,XMMatrixInverse(&det, offset));
//
//			    weight = v->blendingWeight;
//				//vertexPos += skinnedWorldPos * weight;
//				vertexPos += XMVector3TransformCoord(originVertexPosition, finalTransfrom) * weight;
//				lastWeight -= weight;
//		    }
//
//			const Joint& curJoint = mSkeleton.joints[(vertices[i].blendingInfo.end()-1)->blendingIndex];
//			const Keyframe* curKey = curJoint.animation;
//			for (int keyCount = 0; keyCount < currentPlayKeyframe; keyCount++)
//			{
//				curKey = curKey->next;
//			}
//
//			XMMATRIX offset = XMLoadFloat4x4(&curJoint.globalBindposeInverse);
//			XMMATRIX toRoot = XMLoadFloat4x4(&curKey->globalTransfrom);
//
//			XMMATRIX finalTransfrom = XMMatrixMultiply(offset, toRoot);
//			XMVECTOR localPosition = XMVector3TransformCoord(originVertexPosition, offset);
//
//			XMVECTOR skinnedLocalPos = XMVector3TransformCoord(localPosition, finalTransfrom);
//			XMVECTOR det = XMMatrixDeterminant(offset);
//			XMVECTOR skinnedWorldPos = XMVector3TransformCoord(skinnedLocalPos, XMMatrixInverse(&det, offset));
//
//			//vertexPos += XMVector3TransformCoord(skinnedWorldPos, XMMatrixInverse(&det, curJoint.globalBindposeInverse)) * lastWeight;
//			vertexPos += XMVector3TransformCoord(originVertexPosition, finalTransfrom) * lastWeight;
//
//
//			XMStoreFloat3(&vertices[i].position,vertexPos);
//	    }
//
//		currentPlayKeyframe++;
//		if (currentPlayKeyframe == mAnimationLength - 1)
//		{
//			currentPlayKeyframe = 0;
//			lateTime = 0.0f;
//		}
//
//		if (currentPlayKeyframe == 7)
//		{
//			int ff = currentPlayKeyframe;
//		}
//
//		D3D11_MAPPED_SUBRESOURCE resource;
//		deviceContext->Map(m_vertexBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&resource);
//		memcpy(resource.pData,&(vertices.front()),vertices.size());
//		deviceContext->Unmap(m_vertexBuffer,0);
//
//	}
//}
