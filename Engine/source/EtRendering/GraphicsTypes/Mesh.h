#pragma once
#include "VertexInfo.h"

#include <EtCore/Content/Asset.h>
#include <EtCore/Util/LinkerUtils.h>


// forward declarations
namespace et {
	REGISTRATION_NS(render);
namespace render {
	class MeshData;
	class Material;
} namespace pl {
	class EditableMeshAsset;
} }


namespace et {
namespace render {


//---------------------------------
// MeshSurface
//
// Render packet / renderable view into a subset of a mesh's vertex information
//
class MeshSurface final
{
public:
	// c-tor d-tor
	//-------------
	MeshSurface(MeshData const* const mesh, render::Material const* const material);
	~MeshSurface();

	// accessors
	//-----------
	render::Material const* GetMaterial() const { return m_Material; }
	T_ArrayLoc GetVertexArray() const { return m_VertexArray; }

	// Data
	///////
private:
	render::Material const* m_Material = nullptr;
	T_ArrayLoc m_VertexArray = 0u;
};


//---------------------------------
// SurfaceContainer
//
// List of surfaces per mesh
//
class SurfaceContainer final
{
public:
	// c-tor d-tor
	//-------------
	SurfaceContainer() = default;
	~SurfaceContainer();

	// functionality
	//-------------
	MeshSurface const* GetSurface(MeshData const* const mesh, render::Material const* const material);

	// Data
	///////
private:
	std::vector<MeshSurface*> m_Surfaces;
};


//---------------------------------
// MeshData
//
// Info about GPU mesh data and metadata
//
class MeshData final
{
	// definitions
	//-------------
	REGISTRATION_FRIEND_NS(render)
	friend class MeshAsset;
	friend class pl::EditableMeshAsset;

	// c-tor d-tor
	//-------------
	MeshData();
public:
	~MeshData();

	// accessors
	//-----------
	T_VertexFlags GetSupportedFlags() const { return m_SupportedFlags; }
	math::Sphere const& GetBoundingSphere() const { return m_BoundingSphere; }
	size_t GetIndexCount() const { return m_IndexCount; }
	E_DataType GetIndexDataType() const { return m_IndexDataType; }
	T_BufferLoc GetVertexBuffer() const { return m_VertexBuffer; }
	T_BufferLoc GetIndexBuffer() const { return m_IndexBuffer; }
	MeshSurface const* GetSurface(render::Material const* const material) const;

	// Data
	///////
private:

	T_VertexFlags m_SupportedFlags = 0u;
	E_DataType m_IndexDataType = E_DataType::UInt;

	math::Sphere m_BoundingSphere;

	size_t m_VertexCount = 0u;
	size_t m_IndexCount = 0u;

	T_BufferLoc m_VertexBuffer = 0u;
	T_BufferLoc m_IndexBuffer = 0u;

	SurfaceContainer* m_Surfaces = nullptr; // pointer in order to enable const access
};


//---------------------------------
// MeshAsset
//
// Loadable Mesh Data
//
class MeshAsset final : public core::Asset<MeshData, false>
{
	RTTR_ENABLE(core::Asset<MeshData, false>)
	DECLARE_FORCED_LINKING()
public:

	static std::string const s_Header;

	static bool ReadEtMesh(MeshData* const meshData, std::vector<uint8> const& loadData);

	// Construct destruct
	//---------------------
	MeshAsset() : core::Asset<MeshData, false>() {}
	virtual ~MeshAsset() = default;

	// Asset overrides
	//---------------------
	bool LoadFromMemory(std::vector<uint8> const& data) override;
};


} // namespace render
} // namespace et
