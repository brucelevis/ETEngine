#include "stdafx.hpp"

#include "SkyboxTestScene.hpp"

#include "../../SceneGraph/Entity.hpp"

#include "../Materials/TexPBRMaterial.hpp"
#include "../Materials/EmissiveMaterial.hpp"

#include "../../Graphics\FrameBuffer.hpp"
#include "../../Framebuffers\Gbuffer.hpp"
#include "../../Framebuffers\HDRframeBuffer.hpp"

#include "../../Components/ModelComponent.hpp"
#include "../../Components/LightComponent.hpp"

#include "../../Prefabs/Skybox.hpp"

#include <random>

SkyboxTestScene::SkyboxTestScene() : AbstractScene("SkyboxTestScene")
{
}
SkyboxTestScene::~SkyboxTestScene()
{
	SafeDelete(m_pMat);
}

void SkyboxTestScene::Initialize()
{
	//Materials
	//**************************
	m_pMat = new TexPBRMaterial(
		"Resources/Textures/BaseColor.png",
		"Resources/Textures/Roughness.png",
		"Resources/Textures/Metalness.png",
		"Resources/Textures/AOMap.png",
		"Resources/Textures/NormalMap.png");
	m_pMat->SetSpecular(0.5f);

	//Skybox
	//**************************
	SetSkybox("Resources/Textures/skybox/sb.jpg");

	//Models
	//*************************
	auto pModelComp = new ModelComponent("Resources/Models/helmet.dae");
	pModelComp->SetMaterial(m_pMat);
	auto pHelmet = new Entity();
	pHelmet->AddComponent(pModelComp);
	pHelmet->GetTransform()->Translate(vec3(0, 0, 0));
	AddEntity(pHelmet);

	//Lights
	//**************************
	m_pLigEntity = new Entity();
	m_pLigEntity->AddComponent(new LightComponent(
		new DirectionalLight(vec3(1, 1, 1), 0.99f)));
	m_pLigEntity->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	AddEntity(m_pLigEntity);

	SETTINGS->Window.VSync(false);
}

void SkyboxTestScene::Update()
{
	//LOGGER::Log("FPS: " + to_string(TIME->FPS()));
	//cout << "FPS: " + to_string(TIME->FPS()) << endl;

	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_2))
	{
		m_pLigEntity->GetTransform()->RotateEuler(0.1f, 0, 0);
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_8))
	{
		m_pLigEntity->GetTransform()->RotateEuler(-0.1f, 0, 0);
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_4))
	{
		m_pLigEntity->GetTransform()->RotateEuler(0, -0.1f, 0);
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_6))
	{
		m_pLigEntity->GetTransform()->RotateEuler(0, 0.1f, 0);
	}
}

void SkyboxTestScene::Draw()
{
}

void SkyboxTestScene::DrawForward()
{
}

void SkyboxTestScene::PostDraw()
{
}