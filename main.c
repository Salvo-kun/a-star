#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <argp.h>

const char *argp_program_version = "a-star_1.0";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  char *args[1];  /* ARG1 */
  int verbose;    /* The -v flag */
  char *outfile;  /* Argument for -o */
  int numThreads; /* Argument for -t */
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"threads", 't', "NUMTHREADS", 0, "Number of threads to be used"},
  {"output", 'o', "OUTFILE", 0, "Output to OUTFILE instead of to standard output"},
  {0}
};

/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
  {
  case 'v':
    arguments->verbose = 1;
    break;
  case 't':
    arguments->numThreads = atoi(arg);
    break;
  case 'o':
    arguments->outfile = arg;
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1)
    {
      argp_usage(state);
    }
    arguments->args[state->arg_num] = arg;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1)
    {
      argp_usage(state);
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "INPUTFILE";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] = "a-star -- A program to exploit A* algorithm to find an optimal path. It allows using both sequential and parallel versions.";

/*
   The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};

/*
   The main function.
   Notice how now the only function call needed to process
   all command-line options and arguments nicely
   is argp_parse.
*/
int main(int argc, char **argv)
{
  struct arguments arguments;
  // FILE *instream;
  FILE *outstream;

  char waters[] = "a place to stay enough to eat somewhere old heroes shuffle safely down the street --\"the gunners dream\", Roger Waters, 1983\n";

  /* Set argument defaults */
  arguments.outfile = NULL;
  arguments.numThreads = 1;
  arguments.verbose = 0;

  /* Where the magic happens */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  /* Where do we send output? */
  if (arguments.outfile)
    outstream = fopen(arguments.outfile, "w");
  else
    outstream = stdout;

  /* Print argument values */
  fprintf(outstream, "numThreads = %d\n\n", arguments.numThreads);
  fprintf (outstream, "INPUTFILE = %s\n\n", arguments.args[0]);
  
  /* If in verbose mode, print song stanza */
  if (arguments.verbose)
    fprintf(outstream, "%s", waters);

  return 0;
}