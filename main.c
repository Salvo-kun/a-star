#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <argp.h>
#include <libgen.h>
#include "graph/domain.h"
#include "graph/heuristic.h"
#include "graph/hash.h"
#include "pathfinding/pathfinding.h"
#include "utils/util.h"

const char *argp_program_version = "a-star_1.0";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  char *args[1];  /* ARGS */
  int verbose;    /* The -v flag */
  char *outfile;  /* Argument for -o */
  int numThreads; /* Argument for -t */
  int algorithm;  /* Argument for -a */
  int domain;     /* Argument for -d */
  int heuristic;  /* Argument for -e */
  int hash;       /* Argument for -h */
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
        {"heuristic", 'e', "HEURISTIC", 0, "Chosen heuristic, according to domain, if required. For 2D grids, 0 is Manhattan distance."},
        {"hash", 'h', "HASH", 0, "Chosen hash function, if required. 0 is Module Hash, 1 is Multiplicative Hash"},
        {"output", 'o', "OUTFILE", 0, "Output the path found to OUTFILE instead of standard output."},
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
  case 'e':
    arguments->heuristic = atoi(arg);
    break;
  case 'f':
    arguments->hash = atoi(arg);
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
static char args_doc[] = "IN_FILE";

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
  FILE *outfile, *outstream;

  /* Set argument defaults */
  arguments.outfile = NULL;
  arguments.numThreads = 1;
  arguments.algorithm = 1;
  arguments.domain = 0;
  arguments.heuristic = 0;
  arguments.hash = 1;
  arguments.verbose = 0;
  outstream = stdout;

  /* Where the magic happens */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  /* Where do we send output? */
  if (arguments.outfile)
    outfile = util_fopen(arguments.outfile, "w");

  /* Print argument values */
  if (arguments.verbose)
  {
    fprintf(outstream, "---------------------- PARAMETERS ---------------------- \n");
    fprintf(outstream, "verbose = %d\n", arguments.verbose);
    fprintf(outstream, "outfile = %s\n", arguments.outfile);
    fprintf(outstream, "hash = %d\n", arguments.hash);
    fprintf(outstream, "heuristic = %d\n", arguments.heuristic);
    fprintf(outstream, "domain = %d\n", arguments.domain);
    fprintf(outstream, "algorithm = %d\n", arguments.algorithm);
    fprintf(outstream, "numThreads = %d\n", arguments.numThreads);
    fprintf(outstream, "IN_FILE = %s\n", arguments.args[0]);
    fprintf(outstream, "---------------------------------------------------- \n");
  }

  // Choose domain's data reading strategy
  void *(*readData)(char *, int *) = choose_domain_reader(arguments.domain);
  util_check_r(readData != NULL, "Choose a valid domain!", 1);

  // Choose heuristic if not Djikstra
  int (*heuristic)(vertex_t *, vertex_t *) = NULL;
  if (arguments.algorithm != 0)
  {
    heuristic = choose_heuristic(arguments.heuristic, arguments.domain);
    util_check_r(heuristic != NULL, "Choose a valid heuristic!", 2);
  }

  // Choose hash function if parallel A*
  hash_t *hash_data = NULL;
  if (arguments.algorithm == 2)
  {
    hash_data = choose_hash(arguments.hash, arguments.numThreads);
    util_check_r(hash_data != NULL, "Choose a valid hash function!", 3);
  }

  graph_t *g;
  vertex_t **src, **dst;
  path_t **path;

  src = (vertex_t **)util_malloc(sizeof(vertex_t *));
  util_check_r(src != NULL, "Could not allocate src.", 4);

  dst = (vertex_t **)util_malloc(sizeof(vertex_t *));
  util_check_r(dst != NULL, "Could not allocate dst.", 5);

  path = (path_t **)util_malloc(sizeof(path_t *));
  util_check_r(path != NULL, "Could not allocate path.", 6);

  uint64_t tg = nano_count();
  g = graph_create(arguments.args[0], readData);
  util_check_r(g != NULL, "Could not create graph!", 7);
  tg = nano_count() - tg;

  if (arguments.verbose)
    fprintf(outstream, "Graph created in %f seconds.\n", NS_TO_S(tg));

  int srcId, dstId;

  fprintf(stdout, "Enter source node id: ");
  fscanf(stdin, "%d", &srcId);
  graph_find(g, srcId, src);
  util_check_r(*src != NULL, "Source node not found!", 8);

  fprintf(stdout, "Enter destination node id: ");
  fscanf(stdin, "%d", &dstId);
  graph_find(g, dstId, dst);
  util_check_r(*dst != NULL, "Destination node not found!", 9);

  uint64_t t = nano_count();

  // Choose algorithm
  switch (arguments.algorithm)
  {
  case 0:
    seq_djikstra_path(g, *src, *dst, path);
    break;
  case 1:
    seq_a_star_path(g, *src, *dst, heuristic, path);
    break;
  case 2:
    par_a_star_path(g, *src, *dst, heuristic, path, arguments.numThreads, hash_data);
    break;
  }

  t = nano_count() - t;

  if (*path != NULL)
  {
    if (arguments.verbose)
      fprintf(outstream, "\nFound path in %f seconds.\tCost = %d.\tLength = %d.\tVisited nodes = %d.\tRevisited nodes = %d.\n", NS_TO_S(t), (*path)->cost, stack_count((*path)->nodes) - 1, (*path)->visited_nodes, (*path)->revisited_nodes);

    if (arguments.outfile)
    {
      while (!stack_empty_m((*path)->nodes))
      {
        int *id;

        stack_pop((*path)->nodes, (void **)&id);
        fprintf(outfile, "%d\n", *id);
      }
    }

    stack_destroy((*path)->nodes, NULL);
    util_free(*path);
  }
  else
  {
    fprintf(outstream, "Path not found\n");
  }

  graph_destroy(g, free);
  util_free(path);
  util_free(src);
  util_free(dst);

  if (arguments.algorithm == 2)
    hash_destroy(hash_data);

  if (arguments.outfile)
    fclose(outfile);

  return 0;
}