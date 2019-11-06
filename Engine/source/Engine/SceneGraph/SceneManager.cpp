#include "stdafx.h"

#include "SceneManager.h"
#include "AbstractScene.h"

#include <Engine/SceneRendering/SceneRenderer.h>

#include <algorithm>


SceneManager::~SceneManager()
{
	for (AbstractScene* scene : m_pSceneVec)
	{
		delete scene;
		scene = nullptr;
	}
}

void SceneManager::AddGameScene(AbstractScene* scene)
{
	auto it = find(m_pSceneVec.begin(), m_pSceneVec.end(), scene);

	if (it == m_pSceneVec.end())
	{
		m_pSceneVec.push_back(scene);

		if (m_ActiveScene == nullptr && m_NewActiveScene == nullptr)
			m_NewActiveScene = scene;
	}
}

void SceneManager::RemoveGameScene(AbstractScene* scene)
{
	auto it = find(m_pSceneVec.begin(), m_pSceneVec.end(), scene);

	if (it != m_pSceneVec.end())
	{
		m_pSceneVec.erase(it);
	}
}

void SceneManager::SetActiveGameScene(std::string sceneName)
{
	auto it = find_if(m_pSceneVec.begin(), m_pSceneVec.end(), [sceneName](AbstractScene* scene)
	{
		return (strcmp(scene->m_Name.c_str(), sceneName.c_str()) == 0);
	});

	if (it != m_pSceneVec.end())
	{
		m_NewActiveScene = *it;
	}
}

void SceneManager::NextScene()
{
	for (uint32 i = 0; i < m_pSceneVec.size(); ++i)
	{
		if (m_pSceneVec[i] == m_ActiveScene)
		{
			auto nextScene = (++i) % m_pSceneVec.size();
			m_NewActiveScene = m_pSceneVec[nextScene];
			break;
		}
	}
}

void SceneManager::PreviousScene()
{
	for (size_t i = 0; i < m_pSceneVec.size(); ++i)
	{
		if (m_pSceneVec[i] == m_ActiveScene)
		{
			if (i == 0) i = m_pSceneVec.size();
			--i;
			m_NewActiveScene = m_pSceneVec[i];
			break;
		}
	}
}

void SceneManager::OnTick()
{
	if (m_NewActiveScene != nullptr)
	{
		for (SceneRenderer* sceneRenderer : SceneRenderer::GetAll())
		{
			sceneRenderer->ShowSplashScreen();
		}
		m_SplashFrame = true;

		//Deactivate the current active scene
		if (m_ActiveScene != nullptr)
			m_ActiveScene->RootOnDeactivated();

		//Set New Scene
		m_ActiveScene = m_NewActiveScene;
		m_NewActiveScene = nullptr;
	}
	else if (m_SplashFrame)
	{
		m_SplashFrame = false;
		LOG(std::string("Switching to scene: ") + m_ActiveScene->m_Name);
		//Active the new scene and reset SceneTimer
		m_ActiveScene->RootOnActivated();
	}
	else
	{
		for (SceneRenderer* sceneRenderer : SceneRenderer::GetAll())
		{
			sceneRenderer->HideSplashScreen();
		}
	}

	if ((m_ActiveScene != nullptr) && (!m_SplashFrame))
	{
		m_ActiveScene->RootUpdate();
	}
}