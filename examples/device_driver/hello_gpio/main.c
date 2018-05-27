#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int f = open( "/dev/gpio_module", O_RDWR );
    while(1)
    {
        write( f, "s", 1 );
        usleep(500000);
        write( f, "c", 1 );
        usleep(500000);
    }
    close(f);
}
