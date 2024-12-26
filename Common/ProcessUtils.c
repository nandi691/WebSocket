#include "ProcessUtils.h"

void SetProcessName(const char* procName)
{
	prctl(PR_SET_NAME, (unsigned long)procName, 0, 0, 0);
}

void SpawnOtherProcess()
{
	SpawnWebSocketServer();
}

void SpawnWebSocketServer()
{
	const char * procName  = "WebSocketServer";
	PROCESS_CREATION(procName);
	while(1){
		//printf("WebSocket Server Running...\n");
	}; //Start implementing Websockets from here
	return;
}
