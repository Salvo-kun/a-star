#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <argp.h>
#include "graph/domain.h"
#include "graph/heuristic.h"
#include "pathfinding/pathfinding.h"
#include "utils/util.h"

const char *argp_program_version = "a-star_1.0";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  char *args[1];  /* ARG1 */
  int verbose;    /* The -v flag */
  char *outfile;  /* Argument for -o */
  int numThreads; /* Argument for -t */
  int algorithm;  /* Argument for -a */
  int domain;     /* Argument for -d */
  int heuristic;  /* Argument for -h */
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
    {
        {"verbose", 'v', 0, 0, "Produce verbose output."},
        {"threads", 't', "NUMTHREADS", 0, "Number of threads to be used."},
        {"algorithm", 'a', "ALGORITHM", 0, "Chosen algorithm, 0 for Djikstra, 1 for sequential A*, 2 for parallel A*."},
        {"domain", 'd', "DOMAIN", 0, "Chosen domain, 0 for 2D grids."},
        {"heuristic", 'h', "HEURISTIC", 0, "Chosen heuristic, according to domain. For 2D grids, 0 is Manhattan distance."},
        {"output", 'o', "OUTFILE", 0, "Output to OUTFILE instead of to standard output."},
        {0}};

/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t parse_opt(int key, char *arg, struct argp_state *state)
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
  case 'a':
    arguments->algorithm = atoi(arg);
    break;
  case 'd':
    arguments->domain = atoi(arg);
    break;
  case 'h':
    arguments->heuristic = atoi(arg);
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
  arguments.algorithm = 1;
  arguments.domain = 0;
  arguments.heuristic = 0;
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
  fprintf(outstream, "INPUTFILE = %s\n\n", arguments.args[0]);

  /* If in verbose mode, print song stanza */
  if (arguments.verbose)
    fprintf(outstream, "%s", waters);

  // Choose domain's data reading strategy
  void *(*readData)(char *, int *) = choose_domain_reader(arguments.domain);
  util_check_r(readData != NULL, "Choose a valid domain!", 1);

  // Choose heuristic
  int (*heuristic)(vertex_t *, vertex_t *) = choose_heuristic(arguments.heuristic, arguments.domain);
  util_check_r(heuristic != NULL, "Choose a valid heuristic!", 2);

  graph_t *g;
  vertex_t *src, *dst;
  path_t *path;

  src = (vertex_t *)util_malloc(sizeof(vertex_t));
  util_check_no_r(src != NULL, "Could not allocate src.");

  dst = (vertex_t *)util_malloc(sizeof(vertex_t));
  util_check_no_r(dst != NULL, "Could not allocate dst.");

  path = (path_t *)util_malloc(sizeof(path_t));
  util_check_no_r(path != NULL, "Could not allocate path.");

  uint64_t tg = nano_count();
  g = graph_create(arguments.args[0], readData);
  tg = nano_count() - tg;
  fprintf(outstream, "Graph created in %f seconds\n", NS_TO_S(tg));

  // graph_find(g, srcId, &src);
  // graph_find(g, dstId, &dst);

  uint64_t t = nano_count();

  // Choose algorithm
  switch (arguments.algorithm)
  {
    case 0:
      seq_djikstra_path(g, src, dst, &path);
      break;
    case 1:
      seq_a_star_path(g, src, dst, heuristic, &path);
      break;
    case 2:
      par_a_star_path(g, src, dst, heuristic, &path, arguments.numThreads);
      break;
  }

  t = nano_count() - t;

  if (path != NULL)
  {
    fprintf(outstream, "\nFound path in %f seconds with cost = %d and length = %d. Visited nodes = %d\tRevisited nodes = %d\n", NS_TO_S(t), path->cost, stack_count(path->nodes) - 1, path->visited_nodes, path->revisited_nodes);

    while (!stack_empty_m(path->nodes))
    {
      int *id;

      stack_pop(path->nodes, (void **)&id);
      fprintf(outstream, "%d%s", *id, stack_empty_m(path->nodes) ? "" : ", ");
    }

    fprintf(outstream, "\n");
  }
  else
  {
    fprintf(outstream, "Path not found\n");
  }

  graph_destroy(g, free);
  free(path);

  fprintf(outstream, "\n\n");

  return 0;
}