#include "pch.h"
#include "App.h"

#include <ppltasks.h>

using namespace Open_Glider_Simulator;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// Die Hauptfunktion wird nur zum Initialisieren unserer IFrameworkView-Klasse verwendet.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// Die erste Methode, die aufgerufen wird, wenn IFrameworkView erstellt wird.
void App::Initialize(CoreApplicationView^ applicationView)
{
	// Ereignishandler für App-Lebenszyklus registrieren. Dieses Beispiel enthält Activated, damit wir
	// können das CoreWindow aktivieren und mit dem Rendering für das Fenster beginnen.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

	// Zu diesem Zeitpunkt haben wir Zugriff auf das Gerät. 
	// Wir können die geräteabhängigen Ressourcen erstellen.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
}

// Aufruf erfolgt, wenn das CoreWindow-Objekt erstellt (oder neu erstellt) wird.
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

	m_deviceResources->SetWindow(window);
}

// Initialisiert Szeneressourcen oder lädt einen zuvor gespeicherten App-Zustand.
void App::Load(Platform::String^ entryPoint)
{
	if (m_main == nullptr)
	{
		m_main = std::unique_ptr<Open_Glider_SimulatorMain>(new Open_Glider_SimulatorMain(m_deviceResources));
	}
}

// Diese Methode wird aufgerufen, nachdem das Fenster aktiv ist.
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			m_main->Update();

			if (m_main->Render())
			{
				m_deviceResources->Present();
			}
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Für IFrameworkView erforderlich.
// Terminate-Ereignisse führen nicht zu einem Aufruf von Uninitialize. Es wird aufgerufen, wenn Ihr IFrameworkView
// wird gelöscht, während die App im Vordergrund ist.
void App::Uninitialize()
{
}

// Ereignishandler für den Anwendungslebenszyklus

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() startet erst, wenn CoreWindow aktiviert wird.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Anwendungszustand nach dem Anfordern einer Verzögerung asynchron speichern. Das Aufrechterhalten einer Verzögerung
	// weist darauf hin, dass die Anwendung ausgelastet ist und Vorgänge angehalten werden. Vorsicht
	// die Verzögerung kann nicht unbegrenzt aufrechterhalten werden. Nach etwa fünf Sekunden
	// wird die App beendet.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
        m_deviceResources->Trim();

		// Code hier eingeben.

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Beliebige Daten oder einen beliebigen Zustand wiederherstellen, der beim Anhalten entladen wurde. Daten und Zustand werden
	// beim Wiederaufnehmen nach dem Anhalten standardmäßig erhalten. Diese Ereignisklasse
	// tritt nicht auf, wenn die App zuvor beendet wurde.

	// Code hier eingeben.
}

// Ereignishandler für Fenster.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	m_deviceResources->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
	m_main->CreateWindowSizeDependentResources();
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

// DisplayInformation-Ereignishandler.

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	// Hinweis: Der hier abgerufene Wert für "LogicalDpi" stimmt ggf. nicht mit dem effektiven DPI-Wert der App überein,
	// wenn die Skalierung für Geräte mit hoher Auflösung erfolgt. Sobald der DPI-Wert für "DeviceResources" festgelegt wurde,
	// sollten Sie ihn immer mithilfe der Methode "GetDpi" abrufen.
	// Weitere Informationen finden Sie unter "DeviceResources.cpp".
	m_deviceResources->SetDpi(sender->LogicalDpi);
	m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->ValidateDevice();
}