#include "Launching.h"
#include "MemHacks.h"
#include "Config.h"
#include <iostream>
using namespace std;

#define SC_WINDOW_NAME "Brood War"

void ScQuickLaunch(const char* mapPath, bool killExplorer, bool ddEmulate, bool fastMenus, bool zeroTimer, bool noLat,
				   bool windowed, bool customStart, bool postLoad, bool runMap)
{
	/*zeroTimer = true;
	fastMenus = true;
	windowed = true;*/

	ProcessModder process;
	KillWindowsExplorerIf(killExplorer);

	if ( customStart )
		windowed = false;

	if ( ( customStart && RunCustomStart() ) || ( windowed && StartScWindowed(windowed) ) || StartSc(ddEmulate) )
	{
		cout << "Waiting for StarCraft..." << endl;

		while ( !process.isOpen() ) // While process isn't open
		{
			if ( process.openWithWindowName(SC_WINDOW_NAME) ) // Try to open the process
			{
				PatchMenusIf(fastMenus, process);
				RunPostLoadIf(postLoad);
				cout << "StarCraft opened successfully." << endl;
			}
			else
				Sleep(500); // Sleep a fair amount of time upon failure to prevent notable CPU usage.
		}

		// Do any 1-off actions
		RunMapIf(runMap, process, mapPath);

		while ( process.isOpen() ) // While process is open do w/e you want to it
		{
			ZeroScTimerIf(zeroTimer, process);
			NoLatIf(noLat, process);
			Sleep(50);
		}

		cout << "StarCraft closed." << endl;
	}
	else
		cout << "Failed to start StarCraft." << endl;

	CleanupChaosIf(windowed);
	StartWindowsExplorerIf(killExplorer);
}

int main(int argc, const char* argv[])
{
	const char* mapPath;
	bool killExplorer(false), ddEmulate(false), fastMenus(false), zeroTimer(false), noLat(false),
		 windowed(false), customStart(false), postLoad(false), runMap(false);

	displayOptionalFlags();
	parseFlags(argc, argv, mapPath, killExplorer, ddEmulate, fastMenus, zeroTimer, noLat, windowed, customStart, postLoad, runMap);
	ScQuickLaunch(mapPath, killExplorer, ddEmulate, fastMenus, zeroTimer, noLat, windowed, customStart, postLoad, runMap);
	return 0;
}
