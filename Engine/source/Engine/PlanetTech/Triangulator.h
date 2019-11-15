#pragma once
#include "Patch.h"
#include <Engine/Graphics/Frustum.h>


namespace render {


class Planet;


enum TriNext
{
	CULL,
	LEAF,
	SPLIT,
	SPLITCULL
};

struct Tri
{
	Tri(vec3 A, vec3 B, vec3 C, Tri* Parent, int16 Level)
		:a(A), b(B), c(C), parent(Parent), level(Level)
	{
	}

	Tri* parent = nullptr;

	Tri* c1 = nullptr;
	Tri* c2 = nullptr;
	Tri* c3 = nullptr;
	Tri* c4 = nullptr;

	TriNext state;

	int16 level;

	vec3 a;
	vec3 b;
	vec3 c;
};

class Triangulator final
{
public:
	Triangulator() = default;
	~Triangulator() = default;

	//Member functions
	void Init(Planet* const planet);
	bool Update(Scene const* const renderScene);
	void GenerateGeometry();

	bool IsFrustumLocked() { return m_LockFrustum; }
	Frustum& GetFrustum() { return m_Frustum; }
	int32 GetVertexCount() { return (int32)m_Positions.size(); }

private:
	friend class Planet;

	void Precalculate();
	TriNext SplitHeuristic(vec3 &a, vec3 &b, vec3 &c, int16 level, bool frustumCull);
	void RecursiveTriangle(vec3 a, vec3 b, vec3 c, int16 level, bool frustumCull);

	//Triangulation paramenters
	float m_AllowedTriPx = 300.f;
	int32 m_MaxLevel = 15;

	std::vector<Tri> m_Icosahedron;
	std::vector<float> m_DistanceLUT;
	std::vector<float> m_TriLevelDotLUT;
	std::vector<float> m_HeightMultLUT;

	std::vector<Tri*> m_Leafs;

	Planet* m_Planet = nullptr;
	Frustum m_Frustum;
	bool m_LockFrustum = false;

	std::vector<PatchInstance> m_Positions;
};


} // namespace render
