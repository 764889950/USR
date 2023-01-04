#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <unistd.h> 	//usleep
#include <string.h>     //strlen
#include "parameter.h"
#include "Framebuffer.h"

#include <ctype.h>
#include <stdarg.h>

void  Handler(int signo)
{
    //System Exit
    exit(0);
}

void logAlways(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    #pragma omp critical
    {
        vfprintf(stderr, fmt, args);
        fflush(stderr);
    }
    va_end(args);
}


void printHelp()
{
    logAlways("Show this help message\n");
	logAlways("command: \"inf\"，读取显示分辨率\n");
	logAlways("command: \"bmp255\"，显示全白色\n");
	logAlways("command: \"bmp0\"，显示全黑色\n");
	logAlways("command: \"demo -d DIR\"，DIR为将要投显的位图路径,e.g. \"./FrameBuffer demo -d home/share/dst.bmp\"\n");
	logAlways("command: \"help\"，显示帮助\n");
	// logAlways("r表示(rows)行，c表示(cols)列,M、N代表对应的分辨率\n");

    logAlways("\n");
}

static int stringcasecompare(const char* a, const char* b)
{
    while(*a && *b)
    {
        if (tolower(*a) != tolower(*b))
            return tolower(*a) - tolower(*b);
        a++;
        b++;
    }
    return *a - *b;
}

int main(int argc, char **argv)
{
	
	// for (int i=0;i<argc;i++){
		// printf("程序参数%d:%s\n",i,argv[i]);
	// }

	if (argc < 2)
    {
        printHelp();
        exit(1);
    }
	else if (stringcasecompare(argv[1], "inf") == 0)
    {
        inf();
    }
	else if (stringcasecompare(argv[1], "curr") == 0)
    {
		curr();
		// logAlways("demo\n");
    }
	else if (stringcasecompare(argv[1], "bmp255") == 0)
    {
		bmp_255();
		// logAlways("bmp255\n");
    }
    else if (stringcasecompare(argv[1], "bmp0") == 0)
    {
		bmp_0();
		// logAlways("bmp0\n");
    }

    else if (stringcasecompare(argv[1], "help") == 0)
    {
        printHelp();
    }
	else if (stringcasecompare(argv[1], "demo") == 0)
    {
		if(demo(argc,argv)==-1)
		printHelp();
		// logAlways("demo\n");
    }
    else
    {
        logAlways("Unknown command: %s\n", argv[1]);
        exit(1);
    }

    return 0;
}


    // pinMode(0,OUTPUT);