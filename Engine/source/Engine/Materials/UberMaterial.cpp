#include "stdafx.h"
#include "UberMaterial.h"

#include <EtCore/Content/ResourceManager.h>

#include <Engine/Graphics/TextureData.h>
#include <Engine/Graphics/Shader.h>


UberMaterial::UberMaterial() 
	: Material("Shaders/DefUberShader.glsl")
{ }

void UberMaterial::SetBaseColorTexture(T_Hash const id)
{
	m_TexBaseColorAsset = id;
	m_OutdatedTextureData = true;
}

void UberMaterial::SetNormalTexture(T_Hash const id)
{
	m_TexNormalAsset = id;
	m_OutdatedTextureData = true;
}

void UberMaterial::SetMetallicRoughnessTexture(T_Hash const id)
{
	m_TexMetallicRoughnessAsset = id;
	m_OutdatedTextureData = true;
}

void UberMaterial::SetOcclusionTexture(T_Hash const id)
{
	m_TexOcclusionAsset = id;
	m_OutdatedTextureData = true;
}

void UberMaterial::SetEmissiveTexture(T_Hash const id)
{
	m_TexEmissiveAsset = id;
	m_OutdatedTextureData = true;
}

void UberMaterial::LoadTextures()
{
	Viewport::GetCurrentApiContext()->SetShader(m_Shader.get());

	if (m_TexBaseColorAsset != 0u)
	{
		m_TexBaseColor = ResourceManager::Instance()->GetAssetData<TextureData>(m_TexBaseColorAsset);
	}
	else
	{
		m_TexBaseColor = nullptr;
	}

	if (m_TexMetallicRoughnessAsset != 0u)
	{
		m_TexMetallicRoughness = ResourceManager::Instance()->GetAssetData<TextureData>(m_TexMetallicRoughnessAsset);
	}
	else
	{
		m_TexMetallicRoughness = nullptr;
	}

	if (m_TexNormalAsset != 0u)
	{
		m_TexNormal = ResourceManager::Instance()->GetAssetData<TextureData>(m_TexNormalAsset);
	}
	else
	{
		m_TexNormal = nullptr;
	}

	if (m_TexOcclusionAsset != 0u)
	{
		m_TexOcclusion = ResourceManager::Instance()->GetAssetData<TextureData>(m_TexOcclusionAsset);
	}
	else
	{
		m_TexOcclusion = nullptr;
	}

	if (m_TexEmissiveAsset != 0u)
	{
		m_TexEmissive = ResourceManager::Instance()->GetAssetData<TextureData>(m_TexEmissiveAsset);
	}
	else
	{
		m_TexEmissive = nullptr;
	}

	m_OutdatedTextureData = false;
}

void UberMaterial::UploadDerivedVariables()
{
	I_GraphicsApiContext* const api = Viewport::GetCurrentApiContext();

	//Bind active textures
	if (m_OutdatedTextureData)
	{
		LoadTextures();
	}

	m_Shader->Upload("uUseBaseColTex"_hash, (m_TexBaseColorAsset != 0u));
	m_Shader->Upload("uUseNormalTex"_hash, (m_TexNormalAsset != 0u));
	m_Shader->Upload("uUseMetallicRoughnessTex"_hash, (m_TexMetallicRoughnessAsset != 0u));
	m_Shader->Upload("uUseOcclusionTex"_hash, (m_TexOcclusionAsset != 0u));
	m_Shader->Upload("uUseEmissiveTex"_hash, (m_TexEmissiveAsset != 0u));

	if (m_TexBaseColor != nullptr)
	{
		m_Shader->Upload("uTexBaseColor"_hash, 0);
		api->LazyBindTexture(0, E_TextureType::Texture2D, m_TexBaseColor->GetHandle());
	}

	if (m_TexNormal != nullptr)
	{
		m_Shader->Upload("uTexNormal"_hash, 1);
		api->LazyBindTexture(1, E_TextureType::Texture2D, m_TexNormal->GetHandle());
	}

	if (m_TexMetallicRoughness != nullptr)
	{
		m_Shader->Upload("uTexMetallicRoughness"_hash, 2);
		api->LazyBindTexture(2, E_TextureType::Texture2D, m_TexMetallicRoughness->GetHandle());
	}

	if (m_TexNormal != nullptr)
	{
		m_Shader->Upload("uTexOcclusion"_hash, 3);
		api->LazyBindTexture(3, E_TextureType::Texture2D, m_TexOcclusion->GetHandle());
	}

	if (m_TexEmissive != nullptr)
	{
		m_Shader->Upload("uTexEmissive"_hash, 4);
		api->LazyBindTexture(4, E_TextureType::Texture2D, m_TexEmissive->GetHandle());
	}

	//Upload uniforms
	m_Shader->Upload("uBaseColor"_hash, m_BaseColor);
	m_Shader->Upload("uEmissiveFactor"_hash, m_EmissiveFactor);
	m_Shader->Upload("uRoughness"_hash, m_Roughness);
	m_Shader->Upload("uMetallic"_hash, m_Metallic);
}