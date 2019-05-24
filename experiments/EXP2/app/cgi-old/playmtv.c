#include <sys/mount.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <date.h>
#include "myhdr.h"
#include "cgihead.h"
#include <resolv.h>
#include <netinet/in.h>
//#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "myhdr.h"

#include <unistd.h>
//#include <syslog.h>
int main()
{

	int devnull = open("/dev/null",0);
	dup2(devnull, STDOUT_FILENO);
	dup2(devnull, STDERR_FILENO);

	if(vfork() == 0){
		execl("/jffs2/sbin/ffplay","/jffs2/sbin/ffplay","-x","240","-y","320","/tmp/v120x160.avi",NULL);
		exit(255);

	}
	return 0;
}
