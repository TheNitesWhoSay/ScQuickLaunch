#ifndef LAUNCHING_H
#define LAUNCHING_H
#include "WMode.h"

bool RunCustomStart();

bool RunPostLoad();
#define RunPostLoadIf(condition) if ( condition ) RunPostLoad()

void StartWindowsExplorer();
#define StartWindowsExplorerIf(condition) if ( condition ) StartWindowsExplorer()

void KillWindowsExplorer();
#define KillWindowsExplorerIf(condition) if ( condition ) KillWindowsExplorer()

bool StartSc(bool ddEmulate);

bool StartScWindowed(bool &windowed);

void KillSc(bool immediateReturn);

//HRESULT ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable);

bool BackupChaosRegistry();

void LoadBackupChaosRegistry();

bool LoadQuickLaunchChaosRegistry();

void KillChaos();

void CleanupChaos();
#define CleanupChaosIf(condition) if ( condition) CleanupChaos()

#endif