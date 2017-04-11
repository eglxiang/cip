#ifndef _ARG_H_
#define _ARG_H_

#include "defs.h"
#include "utils.h"

/******************************** Definitions ********************************/
/* command line options */
#define OPTIONS ":p:oah"

/* default basename and extension of exported frames */
#define EXPORT_BASE "./frames/frame_"
#define EXPORT_EXTN ".png"
#define EXPORT_NAME_LENGTH	23	//strlen(EXPORT_BASE) + strlen(EXPORT_EXTN) + 4

/* default number of particles */
#define PARTICLES 100

/*************************** global variables ******************************/
static int optind = 1;
static char *optarg;

/***************************** Function Prototypes ***************************/
static void usage( char* );
char* arg_parse( int, char** );
static int getopt(int Argc, char **Argv, char *Str);

#endif
