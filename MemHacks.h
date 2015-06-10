#ifndef MEMHACKS_H
#define MEMHACKS_H
#include "ProcessModder.h"

void PatchMenus(ProcessModder &process);
#define PatchMenusIf(condition, process) if ( condition ) PatchMenus(process)

void ZeroScTimer(ProcessModder &process);
#define ZeroScTimerIf(condition, process) if ( condition ) ZeroScTimer(process)

void NoLat(ProcessModder &process);
#define NoLatIf(condition, process) if ( condition ) NoLat(process)

void RunMap(ProcessModder &process, const char* mapPath);
#define RunMapIf(condition, process, mapPath) if ( condition ) RunMap(process, mapPath)

#endif