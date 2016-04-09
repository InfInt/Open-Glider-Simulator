#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"

// Rendert Direct2D- und 3D-Inhalt auf dem Bildschirm.
namespace Open_Glider_Simulator
{
	class Open_Glider_SimulatorMain : public DX::IDeviceNotify
	{
	public:
		Open_Glider_SimulatorMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Open_Glider_SimulatorMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Zeiger in den Geräteressourcen zwischengespeichert.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Mit Ihren eigenen Inhaltsrenderern ersetzen.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Schleifentimer wird gerendert.
		DX::StepTimer m_timer;
	};
}