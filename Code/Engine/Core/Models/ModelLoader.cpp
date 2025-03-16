#include "Engine/Core/Models/ModelLoader.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Models/CPUMesh.hpp"
#include "Engine/Core/Models/Model.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/GPUMesh.hpp"


ModelLoader::ModelLoader(ModelLoaderConfig const& config)
	: m_config(config)
{
}


void ModelLoader::Startup()
{
}

void ModelLoader::BeginFrame()
{
}

void ModelLoader::EndFrame()
{
}

void ModelLoader::Shutdown()
{
	for (int modelIndex = 0; modelIndex < (int)m_models.size(); modelIndex++)
	{
		delete m_models[modelIndex];
		m_models[modelIndex] = nullptr;
	}
}

//Model* ModelLoader::CreateOrGetModelFromXml(std::string xmlPath)
//{
//	XmlDocument doc(xmlPath.c_str());
//	XmlResult fileLoadResult = doc.LoadFile(xmlPath.c_str());
//	if (fileLoadResult != XmlResult::XML_SUCCESS)
//	{
//		ERROR_AND_DIE(Stringf("Could not find or open file \"%s\"", xmlPath.c_str()));
//	}
//
//	XmlElement const* element = doc.RootElement();
//	return CreateOrGetModelFromXml(element);
//}

Model* ModelLoader::CreateOrGetModelFromXml(XmlElement const* element)
{
	std::string name = ParseXmlAttribute(*element, "name", "");
	std::string path = ParseXmlAttribute(*element, "path", "");
	XmlElement const* transformXmlElement = element->FirstChildElement("Transform");
	Mat44 transform(transformXmlElement);
	
	Model* existingModel = GetModelFromName(name.c_str());
	if (existingModel)
	{
		return existingModel;
	}

	return CreateModel(name.c_str(), path.c_str(), transform);
}

Model* ModelLoader::CreateOrGetModelFromObj(char const* filenameWithNoExtension, Mat44 const& transform)
{
	Model* existingModel = GetModelFromName(filenameWithNoExtension);
	if (existingModel)
	{
		return existingModel;
	}

	return CreateModelFromObj(filenameWithNoExtension, transform);
}

Model* ModelLoader::GetModelFromName(char const* name)
{
	for (int modelIndex = 0; modelIndex < (int)m_models.size(); modelIndex++)
	{
		if (!strcmp(m_models[modelIndex]->m_name.c_str(), name))
		{
			return m_models[modelIndex];
		}
	}

	return nullptr;
}

Model* ModelLoader::CreateModelFromObj(char const* filenameWithNoExtension, Mat44 const& transform)
{
	std::string filenameStr = filenameWithNoExtension;
	std::string objFilename = filenameStr + ".obj";

	return CreateModel(filenameWithNoExtension, objFilename.c_str(), transform);
}

Model* ModelLoader::CreateModel(char const* name, char const* filename, Mat44 const& transform)
{
	char* objFileDrive = new char[_MAX_DRIVE];
	char* objFileDir = new char[_MAX_DIR];
	char* objFileName = new char[_MAX_FNAME];

	SplitPath(filename, objFileDrive, objFileDir, objFileName, nullptr);

	//CPUMesh* newCpuMesh = new CPUMesh(name);
	std::vector<Vertex_PCUTBN> vertexes;
	std::vector<unsigned int> indexes;
	std::map<std::string, Rgba8> materialColorMap;

	std::string objFileContents;
	FileReadToString(objFileContents, filename);
	if (objFileContents.empty())
	{
		ERROR_AND_DIE(Stringf("Could not open or read file \"%s\"", filename));
	}
	Strings objFileLines;
	int numLinesInObjFile = SplitStringOnDelimiter(objFileLines, objFileContents, '\n', true);

	// Load all materials from mtl files referenced in this obj file
	for (int lineIndex = 0; lineIndex < numLinesInObjFile; lineIndex++)
	{
		Strings lineComponents;
		SplitStringOnDelimiter(lineComponents, objFileLines[lineIndex], ' ', true);

		if (!strcmp(lineComponents[0].c_str(), "mtllib"))
		{
			char const* mtlFilename = MakePath(objFileDrive, objFileDir, lineComponents[1].c_str(), nullptr);
			std::string mtlFilenameStr = mtlFilename;
			TrimString(mtlFilenameStr);
			LoadMaterialFile(materialColorMap, mtlFilenameStr.c_str());
		}
	}
	//--------------------------------------------------------------------------------------------------------------

	// Load all vertexes, normals and textureCoords from the obj file
	// Store these in temporary arrays to construct faces from indexes later
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<Vec2> textureCoords;
	for (int lineIndex = 0; lineIndex < numLinesInObjFile; lineIndex++)
	{
		Strings lineComponents;
		SplitStringOnDelimiter(lineComponents, objFileLines[lineIndex], ' ', true);

		if (!strcmp(lineComponents[0].c_str(), "v"))
		{
			Vec3 vertexPosition = Vec3((float)atof(lineComponents[1].c_str()), (float)atof(lineComponents[2].c_str()), (float)atof(lineComponents[3].c_str()));
			vertexPosition = transform.TransformPosition3D(vertexPosition);
			positions.push_back(vertexPosition);
		}
		else if (!strcmp(lineComponents[0].c_str(), "vn"))
		{
			Vec3 vertexNormal = Vec3((float)atof(lineComponents[1].c_str()), (float)atof(lineComponents[2].c_str()), (float)atof(lineComponents[3].c_str()));
			vertexNormal = transform.TransformVectorQuantity3D(vertexNormal);
			normals.push_back(vertexNormal);
		}
		else if (!strcmp(lineComponents[0].c_str(), "vt"))
		{
			Vec2 uvCoords = Vec2((float)atof(lineComponents[1].c_str()), (float)atof(lineComponents[2].c_str()));
			textureCoords.push_back(uvCoords);
		}
	}
	//--------------------------------------------------------------------------------------------------------------

	int numFaces = 0;
	Rgba8 currentColor = Rgba8::WHITE;
	
	std::vector<ModelGroup> groups;
	ModelGroup currentModelGroup;

	for (int lineIndex = 0; lineIndex < numLinesInObjFile; lineIndex++)
	{
		TrimString(objFileLines[lineIndex]);
		Strings lineComponents;
		int numVertexes = SplitStringOnDelimiter(lineComponents, objFileLines[lineIndex], ' ', true);

		// Update vertex color based on the material using materials in materialColorMap
		if (!strcmp(lineComponents[0].c_str(), "usemtl"))
		{
			TrimString(lineComponents[1]);
			auto materialMapEntry = materialColorMap.find(lineComponents[1]);
			if (materialMapEntry == materialColorMap.end())
			{
				ERROR_AND_DIE(Stringf("Mesh attempting to use undefined material \"%s\"", lineComponents[1].c_str()));
			}

			currentColor = materialMapEntry->second;
		}

		// Set model group if it should be set
		if (!strcmp(lineComponents[0].c_str(), "g"))
		{
			if (!currentModelGroup.m_name.empty())
			{
				std::string cpuMeshName = name;
				cpuMeshName += "_" + currentModelGroup.m_name;
				currentModelGroup.m_cpuMesh = new CPUMesh(cpuMeshName, vertexes, indexes);
				currentModelGroup.m_cpuMesh->CalculateTangentBasis(normals.empty(), true);
				currentModelGroup.m_gpuMesh = new GPUMesh(currentModelGroup.m_cpuMesh, m_config.m_renderer);
				vertexes.clear();
				indexes.clear();

				groups.push_back(currentModelGroup);
			}
			currentModelGroup = ModelGroup(lineComponents[1]);
		}

		// Construct Vertex_PCUTBN objects from face lines in obj and add to CPUMesh
		if (!strcmp(lineComponents[0].c_str(), "f"))
		{
			numFaces++;
		
			unsigned int faceStartVertexIndex = 0;

			// Always create and add the first three vertexes with indexes to the face
			{
				Strings vertexInfo;

				SplitStringOnDelimiter(vertexInfo, lineComponents[1], '/');
				Vec3 vertexPos = Vec3::ZERO;
				Vec2 vertexTextureCoords = Vec2::ZERO;
				Vec3 vertexNormal = Vec3::ZERO;
				if (vertexInfo.size())
				{
					size_t vertexPosIndex = atoi(vertexInfo[0].c_str()) - 1;
					if (vertexPosIndex < positions.size())
					{
						vertexPos = positions[vertexPosIndex];
					}
				}
				if (vertexInfo.size() > 1 && !vertexInfo[1].empty())
				{
					size_t texCoordsIndex = atoi(vertexInfo[1].c_str()) - 1;
					if (texCoordsIndex < textureCoords.size())
					{
						vertexTextureCoords = textureCoords[texCoordsIndex];
					}
				}
				// Vertex normal
				if (vertexInfo.size() > 2 && !vertexInfo[2].empty())
				{
					size_t vertexNormalIndex = atoi(vertexInfo[2].c_str()) - 1;
					if (vertexNormalIndex < normals.size())
					{
						vertexNormal = normals[vertexNormalIndex].GetNormalized();
					}
				}
				faceStartVertexIndex = (unsigned int)vertexes.size();
				vertexes.push_back(Vertex_PCUTBN(vertexPos, currentColor, vertexTextureCoords, Vec3::ZERO, Vec3::ZERO, vertexNormal));
				indexes.push_back(faceStartVertexIndex);


				vertexInfo.clear();
				SplitStringOnDelimiter(vertexInfo, lineComponents[2], '/');
				vertexPos = Vec3::ZERO;
				vertexTextureCoords = Vec2::ZERO;
				vertexNormal = Vec3::ZERO;
				if (vertexInfo.size())
				{
					size_t vertexPosIndex = atoi(vertexInfo[0].c_str()) - 1;
					if (vertexPosIndex < positions.size())
					{
						vertexPos = positions[vertexPosIndex];
					}
				}
				if (vertexInfo.size() > 1 && !vertexInfo[1].empty())
				{
					size_t texCoordsIndex = atoi(vertexInfo[1].c_str()) - 1;
					if (texCoordsIndex < textureCoords.size())
					{
						vertexTextureCoords = textureCoords[texCoordsIndex];
					}
				}
				// Vertex normal
				if (vertexInfo.size() > 2 && !vertexInfo[2].empty())
				{
					size_t vertexNormalIndex = atoi(vertexInfo[2].c_str()) - 1;
					if (vertexNormalIndex < normals.size())
					{
						vertexNormal = normals[vertexNormalIndex].GetNormalized();
					}
				}
				indexes.push_back((unsigned int)vertexes.size());
				vertexes.push_back(Vertex_PCUTBN(vertexPos, currentColor, vertexTextureCoords, Vec3::ZERO, Vec3::ZERO, vertexNormal));


				vertexInfo.clear();
				SplitStringOnDelimiter(vertexInfo, lineComponents[3], '/');
				vertexPos = Vec3::ZERO;
				vertexTextureCoords = Vec2::ZERO;
				vertexNormal = Vec3::ZERO;
				if (vertexInfo.size())
				{
					size_t vertexPosIndex = atoi(vertexInfo[0].c_str()) - 1;
					if (vertexPosIndex < positions.size())
					{
						vertexPos = positions[vertexPosIndex];
					}
				}
				if (vertexInfo.size() > 1 && !vertexInfo[1].empty())
				{
					size_t texCoordsIndex = atoi(vertexInfo[1].c_str()) - 1;
					if (texCoordsIndex < textureCoords.size())
					{
						vertexTextureCoords = textureCoords[texCoordsIndex];
					}
				}
				// Vertex normal
				if (vertexInfo.size() > 2 && !vertexInfo[2].empty())
				{
					size_t vertexNormalIndex = atoi(vertexInfo[2].c_str()) - 1;
					if (vertexNormalIndex < normals.size())
					{
						vertexNormal = normals[vertexNormalIndex].GetNormalized();
					}
				}
				indexes.push_back((unsigned int)vertexes.size());
				vertexes.push_back(Vertex_PCUTBN(vertexPos, currentColor, vertexTextureCoords, Vec3::ZERO, Vec3::ZERO, vertexNormal));
			}
		
			// Then for each new vertex, only add the new vertex and add 0, Idx - 1 and Idx as the indexes
			for (int vertexIndex = 3; vertexIndex < numVertexes - 1; vertexIndex++)
			{
				Strings vertexInfo;

				SplitStringOnDelimiter(vertexInfo, lineComponents[vertexIndex + 1], '/');
				Vec3 vertexPos = Vec3::ZERO;
				Vec2 vertexTextureCoords = Vec2::ZERO;
				Vec3 vertexNormal = Vec3::ZERO;
				if (vertexInfo.size())
				{
					size_t vertexPosIndex = atoi(vertexInfo[0].c_str()) - 1;
					if (vertexPosIndex < positions.size())
					{
						vertexPos = positions[vertexPosIndex];
					}
				}
				if (vertexInfo.size() > 1 && !vertexInfo[1].empty())
				{
					size_t texCoordsIndex = atoi(vertexInfo[1].c_str()) - 1;
					if (texCoordsIndex < textureCoords.size())
					{
						vertexTextureCoords = textureCoords[texCoordsIndex];
					}
				}
				// Vertex normal
				if (vertexInfo.size() > 2 && !vertexInfo[2].empty())
				{
					size_t vertexNormalIndex = atoi(vertexInfo[2].c_str()) - 1;
					if (vertexNormalIndex < normals.size())
					{
						vertexNormal = normals[vertexNormalIndex].GetNormalized();
					}
				}
				vertexes.push_back(Vertex_PCUTBN(vertexPos, currentColor, vertexTextureCoords, Vec3::ZERO, Vec3::ZERO, vertexNormal));

				indexes.push_back(faceStartVertexIndex);
				indexes.push_back(faceStartVertexIndex + vertexIndex - 1);
				indexes.push_back(faceStartVertexIndex + vertexIndex);
			}
		}
	}

	std::string cpuMeshName = name;
	cpuMeshName += "_" + currentModelGroup.m_name;
	currentModelGroup.m_cpuMesh = new CPUMesh(cpuMeshName, vertexes, indexes);
	currentModelGroup.m_cpuMesh->CalculateTangentBasis(normals.empty(), true);
	currentModelGroup.m_gpuMesh = new GPUMesh(currentModelGroup.m_cpuMesh, m_config.m_renderer);
	groups.push_back(currentModelGroup);

	Model* newModel = new Model(name, groups, m_config.m_renderer);
	m_models.push_back(newModel);

	return newModel;
}

Model* ModelLoader::CreateOrGetModelFromVertexes(char const* name, std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes)
{
	Model* existingModel = GetModelFromName(name);
	if (existingModel)
	{
		return existingModel;
	}

	ModelGroup group;
	group.m_name = name;
	group.m_cpuMesh = new CPUMesh(name, vertexes, indexes);
	group.m_cpuMesh->CalculateTangentBasis(false, true);
	Model* newModel = new Model(name, {group}, m_config.m_renderer);
	m_models.push_back(newModel);
	return newModel;
}

void ModelLoader::LoadMaterialFile(std::map<std::string, Rgba8>& out_materialColorMap, char const* mtlFilename)
{
	std::string mtlFileContents;
	FileReadToString(mtlFileContents, mtlFilename);

	if (mtlFileContents.empty())
	{
		ERROR_AND_DIE(Stringf("Could not open or read file \"%s\"", mtlFilename));
	}

	Strings mtlFileLines;
	int numLinesInMtlFile = SplitStringOnDelimiter(mtlFileLines, mtlFileContents, '\n');

	std::string currentMtlName;

	for (int lineIndex = 0; lineIndex < numLinesInMtlFile; lineIndex++)
	{
		Strings lineComponents;
		SplitStringOnDelimiter(lineComponents, mtlFileLines[lineIndex], ' ');

		if (!strcmp(lineComponents[0].c_str(), "newmtl"))
		{
			if (out_materialColorMap.find(currentMtlName) == out_materialColorMap.end())
			{
				out_materialColorMap[currentMtlName] = Rgba8::WHITE;
			}

			currentMtlName = lineComponents[1];
			TrimString(currentMtlName);
		}
		else if (!strcmp(lineComponents[0].c_str(), "Kd"))
		{
			std::string redFloatStr = lineComponents[1];
			std::string greenFloatStr = lineComponents[2];
			std::string blueFloatStr = lineComponents[3];
			TrimString(redFloatStr);
			TrimString(greenFloatStr);
			TrimString(blueFloatStr);

			Rgba8 materialColor;
			materialColor.r = DenormalizeByte((float)atof(redFloatStr.c_str()));
			materialColor.g = DenormalizeByte((float)atof(greenFloatStr.c_str()));
			materialColor.b = DenormalizeByte((float)atof(blueFloatStr.c_str()));

			out_materialColorMap[currentMtlName] = materialColor;
		}
	}
	if (out_materialColorMap.find(currentMtlName) == out_materialColorMap.end())
	{
		out_materialColorMap[currentMtlName] = Rgba8::WHITE;
	}
}

Material ModelLoader::CreateMaterialFromXml(XmlElement const* element)
{
	Material material;

	std::string shaderName = ParseXmlAttribute(*element, "shader", "");
	std::string vertexTypeStr = ParseXmlAttribute(*element, "vertexType", "VertexType_PCU");
	VertexType vertexType = GetVertexTypeFromString(vertexTypeStr);
	if (!shaderName.empty())
	{
		material.m_shader = m_config.m_renderer->CreateOrGetShader(shaderName.c_str(), vertexType);
	}
	std::string diffuseTextureName = ParseXmlAttribute(*element, "diffuseTexture", "");
	if (!diffuseTextureName.empty())
	{
		material.m_diffuseTexture = m_config.m_renderer->CreateOrGetTextureFromFile(diffuseTextureName.c_str());
	}
	std::string normalTextureName = ParseXmlAttribute(*element, "normalTexture", "");
	if (!normalTextureName.empty())
	{
		material.m_normalTexture = m_config.m_renderer->CreateOrGetTextureFromFile(normalTextureName.c_str());
	}
	std::string specGlosEmitTextureName = ParseXmlAttribute(*element, "specGlossEmitTexture", "");
	if (!specGlosEmitTextureName.empty())
	{
		material.m_specGlosEmitTexture = m_config.m_renderer->CreateOrGetTextureFromFile(specGlosEmitTextureName.c_str());
	}

	return material;
}
