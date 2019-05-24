#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
        int devnull = open("/dev/null",0);
        dup2(devnull,STDOUT_FILENO);
        dup2(devnull,STDERR_FILENO);

        if(vfork() == 0) {
                execl("/bin/sh","/bin/sh","/jffs2/guidemo/guitest",NULL);
                exit(255);
        }

        return 0;
}

