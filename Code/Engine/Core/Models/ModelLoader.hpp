#pragma once

#include "Engine/Core/Models/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"

struct Model;
class Texture;

/*! \brief A structure for the configuration to be used for this ModelLoader
*
* Currently contains nothing but still must be passed in to the ModelLoader constructor
*
*/
struct ModelLoaderConfig
{
public:
	Renderer* m_renderer = nullptr;
};

/*! \brief Create, cache and manage 3D models
* 
* The ModelLoader system loads 3D models from files on disk and manages these models, including caching them so that only one mesh per file is created for optimization.
* \sa Model
* 
*/
class ModelLoader
{
public:
	~ModelLoader() = default;
	ModelLoader(ModelLoaderConfig const& config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	//Model* CreateOrGetModelFromXml(std::string xmlPath);
	Model* CreateOrGetModelFromXml(XmlElement const* element);
	Model* CreateOrGetModelFromObj(char const* filenameWithNoExtension, Mat44 const& transform = Mat44::IDENTITY);
	Model* GetModelFromName(char const* name);
	Model* CreateModelFromObj(char const* filenameWithNoExtension, Mat44 const& transform = Mat44::IDENTITY);
	Model* CreateModel(char const* name, char const* filename, Mat44 const& transform = Mat44::IDENTITY);
	Model* CreateOrGetModelFromVertexes(char const* name, std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes);

	void LoadMaterialFile(std::map<std::string, Rgba8>& out_materialColorMap, char const* mtlFilename);
	Material CreateMaterialFromXml(XmlElement const* element);

public:
	ModelLoaderConfig m_config;
	std::vector<Model*> m_models;
};
