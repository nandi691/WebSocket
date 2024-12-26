#include<iostream>
#include<unistd.h>
#include <sys/prctl.h>

int main(int argc, char *argv[]) /* Main Program for TLS Websocker*/
{
	//using namespace std;

	prctl(PR_SET_NAME, (unsigned long)"WSMain", 0, 0, 0);
	while(1){
		//sleep(10);
	}
	return 0;
}
