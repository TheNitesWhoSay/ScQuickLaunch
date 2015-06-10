#include "Launching.h"
#include "ProcessModder.h"
#include "Config.h"
#include <AclAPI.h>
#include <iostream>
#include <fstream>
#include <string>

bool RunCustomStart()
{
	std::ifstream file("CustomStarter.bat");
	file.seekg (0, file.end);
    int length = (int)file.tellg();
	if ( length < 1 )
	{
		std::cout << "No code found in CustomStart.bat" << std::endl;
		return false;
	}

    file.seekg (0, file.beg);
	char* string = new char[length+1];
	string[length] = '\0';
	file.read(string, length);
	system(string);
	return true;
}

bool RunPostLoad()
{
	std::ifstream file("PostLoad.bat");
	file.seekg (0, file.end);
    int length = (int)file.tellg();
	if ( length < 1 )
	{
		std::cout << "No code found in PostLoad.bat" << std::endl;
		return false;
	}

    file.seekg (0, file.beg);
	char* string = new char[length+1];
	string[length] = '\0';
	file.read(string, length);
	system(string);
	return true;
}

void StartWindowsExplorer()
{
	system("Taskkill /f /im explorer.exe");
	system(("Start " + std::string(getenv("systemroot")) + "\\explorer.exe").c_str());
}

void KillWindowsExplorer()
{
	system("Taskkill /f /im explorer.exe");
	system("Start %SystemRoot%\\explorer.exe \"%userprofile%\\Desktop\"");
}

bool StartSc(bool ddEmulate)
{
	char scExePath[MAX_PATH+12];
	PROCESS_INFORMATION PI = { };
	STARTUPINFO SUI = { };
	SUI.cb = sizeof(STARTUPINFO);

	if ( GetScExePath(scExePath, MAX_PATH) )
	{
		if ( ddEmulate )
		{
			std::string launchPath;
			launchPath = "\"" + std::string(scExePath) + "\" ddemulate";
			if ( CreateProcess(NULL, (LPSTR)launchPath.c_str(), NULL, NULL, false, NULL, NULL, NULL, &SUI, &PI) == TRUE )
				return true;
		}
		return CreateProcess(scExePath, NULL, NULL, NULL, false, NULL, NULL, NULL, &SUI, &PI) == TRUE;
	}
	return false;
}

bool StartScWindowed(bool &windowed)
{
	PROCESS_INFORMATION PI = { };
	STARTUPINFO SUI = { };
	SUI.cb = sizeof(STARTUPINFO);

	KillSc(false);
	KillChaos();
	if ( BackupChaosRegistry() && LoadQuickLaunchChaosRegistry() )
	{
		if ( CreateProcess("wmode\\Chaoslauncher.exe", NULL, NULL, NULL, false, NULL, NULL, NULL, &SUI, &PI) != 0 )
		{
			windowed = true;
			return true;
		}
		else
			std::cout << "Failed to launch Chaoslauncher, cannot enter w-mode." << std::endl;
	}
	else
		std::cout << "Failed to setup required Chaoslauncher registry entries, cannot enter w-mode." << std::endl;

	windowed = false;
	return StartSc(false);
}

void KillSc(bool immediateReturn)
{
	HWND hChaos = FindWindow(NULL, "Brood War");
	if ( immediateReturn )
		PostMessage(hChaos, WM_CLOSE, NULL, NULL);
	else
	{
		DWORD PID;
		if ( hChaos != NULL )
		{
			SendMessage(hChaos, WM_CLOSE, NULL, NULL);
			Sleep(10);
		}
		if ( FindProcess("Starcraft.exe", PID) )
			system("Taskkill /f /im Starcraft.exe");
	}
}

bool ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable)
{
	bool success = false;
	HANDLE hToken = NULL;
	if ( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken) )
	{
		LUID luid;
		if ( LookupPrivilegeValue(NULL, szPrivilege, &luid) )
		{
			TOKEN_PRIVILEGES NewState;
			NewState.PrivilegeCount = 1;
			NewState.Privileges[0].Luid = luid;
			NewState.Privileges[0].Attributes = (fEnable ? SE_PRIVILEGE_ENABLED : 0);
			if ( AdjustTokenPrivileges(hToken, FALSE, &NewState, 0, NULL, NULL) )
				success = true;
		}
		CloseHandle(hToken);
	}
	return success;
}

bool BackupChaosRegistry()
{
	bool success = false;
	ModifyPrivilege(SE_BACKUP_NAME, TRUE);

	HKEY key;
	if ( RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Chaoslauncher", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS )
	{
		DeleteFile("wmode\\ChaosBackup.reg");
		if ( RegSaveKey(key, "wmode\\ChaosBackup.reg", NULL) == ERROR_SUCCESS )
			success = true;
		else
			std::cout << "Failed to save key" << std::endl;

		RegCloseKey(key);
	}
	else
		success = true;

	return success;
}

void LoadBackupChaosRegistry()
{
	ModifyPrivilege(SE_BACKUP_NAME, TRUE);
	ModifyPrivilege(SE_RESTORE_NAME, TRUE);
	
	HKEY key;
	if ( RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Chaoslauncher", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS )
	{
		RegRestoreKey(key, "wmode\\ChaosBackup.reg", REG_FORCE_RESTORE);
		RegCloseKey(key);
	}
}

bool LoadQuickLaunchChaosRegistry()
{
	if ( ModifyPrivilege(SE_BACKUP_NAME, TRUE) == false || ModifyPrivilege(SE_RESTORE_NAME, TRUE) == false )
		std::cout << "Privilege trouble..." << std::endl;
	
	bool success = false;
	HKEY key;
	if ( RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Chaoslauncher", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS )
	{
		if ( RegRestoreKey(key, "wmode\\Chaoslauncher.reg", REG_FORCE_RESTORE) == ERROR_SUCCESS )
			success = true;
		else
			std::cout << "Failed to load quick launch registry values." << std::endl;

		RegCloseKey(key);
	}
	else
		std::cout << "Failed to open HKLM/Software." << std::endl;

	return success;
}

void KillChaos()
{
	HWND hChaos = FindWindow(NULL, "Chaoslauncher");
	DWORD PID;
	if ( hChaos != NULL )
	{
		SendMessage(hChaos, WM_CLOSE, NULL, NULL);
		Sleep(10);
	}
	if ( FindProcess("Chaoslauncher.exe", PID) )
		system("Taskkill /f /im Chaoslauncher.exe");
}

void CleanupChaos()
{
	KillChaos();
	LoadBackupChaosRegistry();
}
