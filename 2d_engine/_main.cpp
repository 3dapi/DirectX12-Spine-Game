
// using console for debugging
#if defined(DEBUG) || defined(_DEBUG)
#define CONSOLE_DEBUG 1
#endif

#if defined(CONSOLE_DEBUG) && (CONSOLE_DEBUG==1)
#pragma comment(linker, "/subsystem:console")
#endif

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mf.lib")

#if defined(CONSOLE_DEBUG) && (CONSOLE_DEBUG==1)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include "MainApp.h"


#if defined(CONSOLE_DEBUG) && (CONSOLE_DEBUG==1)
int main(int, char**)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(544);
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(nullptr); 
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
#endif
	try
	{
		auto appMain = G2::IG2AppFrameWin::instance();
		auto hr = appMain->init(hInstance);
		if (FAILED(hr))
			return 0;
		hr = appMain->Run();
		delete appMain;
	}
	catch (DXException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	//char buf[1024]{};
	//while(1)
	//{
	//	std::cin >> buf;
	//}
}

