#include "ProcessUtils.h"

int main(int argc, char *argv[]) /* Main Program for TLS Websocket*/
{
	SetProcessName("WebSocketMain");
	SpawnOtherProcess();
	while(1){};
	return 0;
}
