// CRO-MAG RALLY ENTRY POINT
// (C) 2022 Iliyas Jorio
// This file is part of Cro-MagRally. https://github.com/jorio/cromagrally

// 2024 Modded CMR Game by rk8 (codename). this is a derivative of an original work under the CC A-NC-SA 4.0 IPL.
// Internal info about code: Some C files have been either changed a lot or a little. Look around to see the new stuff and new additional data that makes it possible! There are some C+ files here and there that were already included, and not added by this mod. Some commented out code is original while others are unused tests or features from the official version of the port which are unused, like Internet features or special params for rendering and such.

#include <SDL.h>
#include "Pomme.h"
#include "PommeInit.h"
#include "PommeFiles.h"

#include <iostream>
#include <cstring>

extern "C"
{
	#include "game.h"
	#include "version.h"

	SDL_Window* gSDLWindow = nullptr;
	FSSpec gDataSpec;
	CommandLineOptions gCommandLine;
	int gCurrentAntialiasingLevel;

/*
#if _WIN32
	// Tell Windows graphics driver that we prefer running on a dedicated GPU if available
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
#endif
*/

	void GameMain(void);
}

static fs::path FindGameData(const char* executablePath)
{
	fs::path dataPath;

	int attemptNum = 0;

#if !(__APPLE__)
	attemptNum++;		// skip macOS special case #0
#endif
    
    printf(" Finding startup data... \n");

	if (!executablePath)
		attemptNum = 2;

tryAgain:
	switch (attemptNum)
	{
		case 0:			// special case for macOS app bundles
			dataPath = executablePath;
			dataPath = dataPath.parent_path().parent_path() / "Resources";
			break;

		case 1:
			dataPath = executablePath;
			dataPath = dataPath.parent_path() / "Data";
			break;

		case 2:
			dataPath = "Data";
			break;

		default:
			throw std::runtime_error("Couldn't find the Data folder.");
	}

	attemptNum++;

	dataPath = dataPath.lexically_normal();

	// Set data spec -- Lets the game know where to find its asset files
	gDataSpec = Pomme::Files::HostPathToFSSpec(dataPath / "Skeletons");

	FSSpec someDataFileSpec;
	OSErr iErr = FSMakeFSSpec(gDataSpec.vRefNum, gDataSpec.parID, ":Skeletons:Brog.bg3d", &someDataFileSpec);
	if (iErr)
	{
		goto tryAgain;
	}

	return dataPath;
}

static void ParseCommandLine(int argc, char** argv)
{
	memset(&gCommandLine, 0, sizeof(gCommandLine));
	gCommandLine.vsync = 1;

	for (int i = 1; i < argc; i++)
	{
		std::string argument = argv[i];

		if (argument == "--track")
		{
			GAME_ASSERT_MESSAGE(i + 1 < argc, "practice track # unspecified");
			gCommandLine.bootToTrack = atoi(argv[i + 1]);
			i += 1;
		}
		else if (argument == "--car")
		{
			GAME_ASSERT_MESSAGE(i + 1 < argc, "car # unspecified");
			gCommandLine.car = atoi(argv[i + 1]);
			i += 1;
		}
		else if (argument == "--stats")
			gDebugMode = 1;
		else if (argument == "--no-vsync")
			gCommandLine.vsync = 0;
		else if (argument == "--vsync")
			gCommandLine.vsync = 1;
		else if (argument == "--adaptive-vsync")
			gCommandLine.vsync = -1;
#if 0
		else if (argument == "--fullscreen-resolution")
		{
			GAME_ASSERT_MESSAGE(i + 2 < argc, "fullscreen width & height unspecified");
			gCommandLine.fullscreenWidth = atoi(argv[i + 1]);
			gCommandLine.fullscreenHeight = atoi(argv[i + 2]);
			i += 2;
		}
		else if (argument == "--fullscreen-refresh-rate")
		{
			GAME_ASSERT_MESSAGE(i + 1 < argc, "fullscreen refresh rate unspecified");
			gCommandLine.fullscreenRefreshRate = atoi(argv[i + 1]);
			i += 1;
		}
#endif
	}
}

static void GetInitialWindowSize(int display, int& width, int& height)
{
	const float aspectRatio = 16.0f / 9.0f;
	const float screenCoverage = 2.0f / 3.0f;

	SDL_Rect displayBounds = { .x = 0, .y = 0, .w = 640, .h = 480 };
	SDL_GetDisplayUsableBounds(display, &displayBounds);

	if (displayBounds.w > displayBounds.h)
	{
		width	= displayBounds.h * screenCoverage * aspectRatio;
		height	= displayBounds.h * screenCoverage;
	}
	else
	{
		width	= displayBounds.w * screenCoverage;
		height	= displayBounds.w * screenCoverage / aspectRatio;
	}
}

static void Boot(int argc, char** argv)
{
    printf("Starting up... \n");
    
	const char* executablePath = argc > 0 ? argv[0] : NULL;

	ParseCommandLine(argc, argv);

	// Start our "machine"
	Pomme::Init();

	// Load game prefs before starting
	InitDefaultPrefs();
	LoadPrefs();

retryVideo:
	// Initialize SDL video subsystem
	if (0 != SDL_Init(SDL_INIT_VIDEO))
	{
		throw std::runtime_error("Couldn't initialize SDL video subsystem.");
	}

	// Create window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	gCurrentAntialiasingLevel = gGamePrefs.antialiasingLevel;
	if (gCurrentAntialiasingLevel != 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1 << gCurrentAntialiasingLevel);
	}

	int display = gGamePrefs.monitorNum;
	if (display >= SDL_GetNumVideoDisplays())
	{
		display = 0;
	}

	int initialWidth = 640;
	int initialHeight = 480;
	GetInitialWindowSize(display, initialWidth, initialHeight);

	gSDLWindow = SDL_CreateWindow(
			"Cro-Mag Rally " PROJECT_VERSION,
			SDL_WINDOWPOS_UNDEFINED_DISPLAY(display),
			SDL_WINDOWPOS_UNDEFINED_DISPLAY(display),
			initialWidth,
			initialHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

	if (!gSDLWindow)
	{
		if (gCurrentAntialiasingLevel != 0)
		{
			printf("Couldn't create SDL window with the requested MSAA level. Retrying without MSAA...\n");

			// retry without MSAA
			gGamePrefs.antialiasingLevel = 0;
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			goto retryVideo;
		}
		else
		{
			throw std::runtime_error("Couldn't create SDL window.");
		}
	}

	// Find path to game data folder
	fs::path dataPath = FindGameData(executablePath);

	// Init joystick subsystem
	{
		SDL_Init(SDL_INIT_GAMECONTROLLER);
		auto gamecontrollerdbPath8 = (dataPath / "System" / "gamecontrollerdb.txt").u8string();
		if (-1 == SDL_GameControllerAddMappingsFromFile((const char*)gamecontrollerdbPath8.c_str()))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Cro-Mag Rally", "Couldn't load gamecontrollerdb.txt!", gSDLWindow);
		}
	}

	// Set fullscreen mode from prefs
	SetFullscreenMode(true);
}

static void Shutdown()
{
	Pomme::Shutdown();

	if (gSDLWindow)
	{
		SDL_DestroyWindow(gSDLWindow);
		gSDLWindow = NULL;
	}

	SDL_Quit();
}

int main(int argc, char** argv)
{
	int				returnCode				= 0;
	std::string		finalErrorMessage		= "";
	bool			showFinalErrorMessage	= false;

	try
	{
		Boot(argc, argv);
		GameMain();
	}
	catch (Pomme::QuitRequest&)
	{
		// no-op, the game may throw this exception to shut us down cleanly
	}
#if !(_DEBUG)
	// In release builds, catch anything that might be thrown by GameMain
	// so we can show an error dialog to the user.
	catch (std::exception& ex)		// Last-resort catch
	{
		returnCode = 1;
		finalErrorMessage = ex.what();
		showFinalErrorMessage = true;
	}
	catch (...)						// Last-resort catch
	{
		returnCode = 1;
		finalErrorMessage = "unknown";
		showFinalErrorMessage = true;
	}
#endif

	Shutdown();

	if (showFinalErrorMessage)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Uncaught exception: %s\n", finalErrorMessage.c_str());
		SDL_ShowSimpleMessageBox(0, "Cro-Mag Rally: Uncaught exception", finalErrorMessage.c_str(), nullptr);
	}

	return returnCode;
}

