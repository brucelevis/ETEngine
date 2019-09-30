#pragma once
#include <EtCore/Helper/Singleton.h>


class Directory;

class ScreenshotCapture : public Singleton<ScreenshotCapture>
{
public:
	void Take() { m_Take = true; }

private:
	friend class Singleton<ScreenshotCapture>;
	friend class SceneRenderer;
	friend class AbstractFramework;

	ScreenshotCapture() = default;
	virtual ~ScreenshotCapture();

	void Initialize(std::string basePath);

	std::string GetFileName();
	void HandleCapture();//To be called by Render pipeline

	Directory* m_BaseDir = nullptr;
	bool m_Take = false;
};