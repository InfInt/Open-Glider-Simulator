#include "pch.h"
#include "Open_Glider_SimulatorMain.h"
#include "Common\DirectXHelper.h"

using namespace Open_Glider_Simulator;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Lädt und initialisiert die Anwendungsobjekte, wenn die Anwendung geladen wird.
Open_Glider_SimulatorMain::Open_Glider_SimulatorMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// Registrieren, um über Geräteverlust oder Neuerstellung benachrichtigt zu werden
	m_deviceResources->RegisterDeviceNotify(this);

	// TODO: Dies mit Ihrer App-Inhaltsinitialisierung ersetzen.
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));

	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

	// TODO: Timereinstellungen ändern, wenn Sie etwas Anderes möchten als den standardmäßigen variablen Zeitschrittmodus.
	// z. B. für eine Aktualisierungslogik mit festen 60 FPS-Zeitschritten Folgendes aufrufen:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

Open_Glider_SimulatorMain::~Open_Glider_SimulatorMain()
{
	// Registrierung der Gerätebenachrichtigung aufheben
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Aktualisiert den Anwendungszustand, wenn sich die Fenstergröße ändert (z. B. Änderung der Geräteausrichtung)
void Open_Glider_SimulatorMain::CreateWindowSizeDependentResources() 
{
	// TODO: Dies mit Ihrer größenabhängigen Initialisierung Ihres App-Inhalts ersetzen.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Aktualisiert den Anwendungszustand ein Mal pro Frame.
void Open_Glider_SimulatorMain::Update() 
{
	// Die Szeneobjekte aktualisieren.
	m_timer.Tick([&]()
	{
		// TODO: Dies mit Ihren App-Inhaltsupdatefunktionen ersetzen.
		m_sceneRenderer->Update(m_timer);
		m_fpsTextRenderer->Update(m_timer);
	});
}

// Rendert den aktuellen Frame dem aktuellen Anwendungszustand entsprechend.
// Gibt True zurück, wenn der Frame gerendert wurde und angezeigt werden kann.
bool Open_Glider_SimulatorMain::Render() 
{
	// Nicht versuchen, etwas vor dem ersten Update zu rendern.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Den Viewport zurücksetzen, damit der gesamte Bildschirm das Ziel ist.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Renderziele zum Bildschirm zurücksetzen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Den Hintergrundpuffer und die Ansicht der Tiefenschablone bereinigen.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Die Szeneobjekte rendern.
	// TODO: Dies mit den Inhaltsrenderingfunktionen Ihrer App ersetzen.
	m_sceneRenderer->Render();
	m_fpsTextRenderer->Render();

	return true;
}

// Weist Renderer darauf hin, dass die Geräteressourcen freigegeben werden müssen.
void Open_Glider_SimulatorMain::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Weist Renderer darauf hin, dass die Geräteressourcen jetzt erstellt werden können.
void Open_Glider_SimulatorMain::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
