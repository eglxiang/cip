#include "arg.h"

/* print usage for this program */
static void usage( char* name )
{
  cerr<<name<<": track a single object using particle filtering"<<endl<<endl;
  cerr<<"Usage: "<<name<<" [options] <video_file>"<<endl<<endl;
  cerr<<"Arguments:"<<endl;
  cerr<<"  <vid_file>          A clip of video in which to track an object"<<endl;
  cerr<<"\nOptions:"<<endl;
  cerr<<"  -h                  Display this message and exit"<<endl;
  cerr<<"  -a                  Display all particles, not just the most likely"<<endl;
  cerr<<"  -o                  Output tracking sequence frames as "<< EXPORT_BASE << EXPORT_EXTN <<endl;
  cerr<<"  -p <particles>      Number of particles (default "<<PARTICLES<<")"<<endl;
}



/*
  arg_parse() parses the command line arguments, setting appropriate globals.
  
  argc and argv should be passed directly from the command line
*/
char* arg_parse( int argc, char** argv )
{
  int i = 0;
  int end=0;
  char* pname;                      /* program name */
  char* vid_file;                   /* input video file name */
  int num_particles = PARTICLES;    /* number of particles */

  int show_all = FALSE;             /* TRUE to display all particles */
  int exported = FALSE;               /* TRUE to exported tracking sequence */
  /*extract program name from command line (remove path, if present) */
  pname = remove_path( argv[0] );

  /*parse commandline options */
  while( !end )
    {
      char* arg_check;
      int arg = getopt( argc, argv, OPTIONS );
      if( arg == -1 )
	break;

      switch( arg )
	{
	  /* user asked for help */
	case 'h':
	  usage( pname );
	  exit(0);
	  break;
	  
	case 'a':
	  show_all = TRUE;
	  break;

	  /* user wants to output tracking sequence */
	case 'o':
	  exported = TRUE;
	  break;

	  /* user wants to set number of particles */
	case 'p':
	  if( ! optarg )
	    fatal_error( "error parsing arguments at -%c\n"	\
			 "Try '%s -h' for help.", arg, pname );
	  num_particles = strtol( optarg, &arg_check, 10 );
	  if( arg_check == optarg  ||  *arg_check != '\0' )
	    fatal_error( "-%c option requires an integer argument\n"	\
			 "Try '%s -h' for help.", arg, pname );
	  break;
	case '?': optind--; end=1; break;	//argument begin without '-', is the input out put name, by Waks
	  
	  /* catch invalid arguments */
	default:
	  fatal_error( "-%c: invalid option\nTry '%s -h' for help.",
		       arg/*optopt*/, pname );
	}
    }
  
  /* make sure input and output files are specified */
  if( argc - optind < 1 )
    fatal_error( "no input image specified.\nTry '%s -h' for help.", pname );
  if( argc - optind > 2 )
    fatal_error( "too many arguments.\nTry '%s -h' for help.", pname );
 
  /* record video file name */
  vid_file = argv[optind];

  return vid_file;
}

static int getopt(int Argc, char **Argv, char *Str)
{
    int Optchar;
    char *Option;

    if ( optind >= Argc ) return EOF;

    Option = Argv[optind++];

    if ( *Option++ != '-' ) return '?';

    Optchar = *Option++;

    while ( *Str && *Str != Optchar ) Str++;
    if ( ! *Str ) return '?';

    if ( *++Str == ':' )
    {
	if ( *Option ) optarg = Option;
	else
	if ( optind < Argc ) optarg = Argv[optind++];
	else
	Optchar = '?';
    }

    return Optchar;
}