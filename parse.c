#include <stdio.h>
#include <argp.h>

const char *argp_program_version =
"argex 1.0";

const char *argp_program_bug_address =
"<bug-gnu-utils@gnu.org>";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  char *args[2];            /* ARG1 and ARG2 */
  int verbose;              /* The -v flag */
  char *outfile;            /* Argument for -o */
  char *string1, *string2;  /* Arguments for -a and -b */
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"alpha",   'a', "STRING1", 0,
   "Do something with STRING1 related to the letter A"},
  {"bravo",   'b', "STRING2", 0,
   "Do something with STRING2 related to the letter B"},
  {"output",  'o', "OUTFILE", 0,
   "Output to OUTFILE instead of to standard output"},
  {0}
};


/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'v':
      arguments->verbose = 1;
      break;
    case 'a':
      arguments->string1 = arg;
      break;
    case 'b':
      arguments->string2 = arg;
      break;
    case 'o':
      arguments->outfile = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
	{
	  argp_usage(state);
	}
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2)
	{
	  argp_usage (state);
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
static char args_doc[] = "ARG1 ARG2";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
"argex -- A program to demonstrate how to code command-line options
and arguments.\vFrom the GNU C Tutorial.";

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
int main (int argc, char **argv)
{
  struct arguments arguments;
  FILE *outstream;

  char waters[] =
"a place to stay
enough to eat
somewhere old heroes shuffle safely down the street
where you can speak out loud
about your doubts and fears
and what's more no-one ever disappears
you never hear their standard issue kicking in your door
you can relax on both sides of the tracks
and maniacs don't blow holes in bandsmen by remote control
and everyone has recourse to the law
and no-one kills the children anymore
and no-one kills the children anymore
  --\"the gunners dream\", Roger Waters, 1983\n";

  /* Set argument defaults */
  arguments.outfile = NULL;
  arguments.string1 = "";
  arguments.string2 = "";
  arguments.verbose = 0;

  /* Where the magic happens */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  /* Where do we send output? */
  if (arguments.outfile)
    outstream = fopen (arguments.outfile, "w");
  else
    outstream = stdout;

  /* Print argument values */
  fprintf (outstream, "alpha = %s\nbravo = %s\n\n",
	   arguments.string1, arguments.string2);
  fprintf (outstream, "ARG1 = %s\nARG2 = %s\n\n",
	   arguments.args[0],
	   arguments.args[1]);

  /* If in verbose mode, print song stanza */
  if (arguments.verbose)
    fprintf (outstream, waters);

  return 0;
}