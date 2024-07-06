/**
 * @file csim.c
 * @author minghui.qin (mhqin1018@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <getopt.h>
#include <stdbool.h>  // Add this line to include the bool type
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cachelab.h"

typedef struct CacheLine {
  int valid;
  int tag;
  int time_stamp;
} CacheLine;

typedef struct Cache_ {
  int S;
  int B;
  int E;
  CacheLine **lines;
} Cache;

int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;
int verbose = 0;
char t[1000];

Cache *CacheInit(int s, int E, int b) {
  int S = 1 << s;
  int B = 1 << b;
  Cache *cache = (Cache *)malloc(sizeof(Cache));
  cache->S = S;
  cache->B = B;
  cache->E = E;
  cache->lines = (CacheLine **)malloc(sizeof(CacheLine *) * S);
  for (int i = 0; i < S; i++) {
    cache->lines[i] = (CacheLine *)malloc(sizeof(CacheLine) * E);
    for (int j = 0; j < E; j++) {
      cache->lines[i][j].valid = 0;
      cache->lines[i][j].tag = 0;
      cache->lines[i][j].time_stamp = 0;
    }
  }
  return cache;
}

void CacheFree(Cache *cache) {
  for (int i = 0; i < cache->S; i++) {
    free(cache->lines[i]);
  }
  free(cache->lines);
  free(cache);
}

/**
 * @brief judge whether the cache hit or miss
 *
 * @param cache
 * @param set_index
 * @param address_tag
 * @return int if hit, return the line index, else return -1
 */
int JudgeHitOrMiss(Cache *cache, int set_index, int address_tag) {
  for (int i = 0; i < cache->E; i++) {
    if (cache->lines[set_index][i].valid &&
        cache->lines[set_index][i].tag == address_tag) {
      return i;
    }
  }
  return -1;
}

void UpdateCacheLine(Cache *cache, int set_index, int address_tag, int bias) {
  cache->lines[set_index][bias].valid = 1;
  cache->lines[set_index][bias].tag = address_tag;
  for (int k = 0; k < cache->E; k++) {
    if (k != bias && cache->lines[set_index][k].valid) {
      cache->lines[set_index][k].time_stamp++;
    }
  }
  cache->lines[set_index][bias].time_stamp = 0;
}

int FindEvictionLine(Cache *cache, int set_index) {
  int max_time_stamp = -1;
  int max_time_stamp_line = -1;
  for (int i = 0; i < cache->E; i++) {
    if (!cache->lines[set_index][i].valid) {
      return i;
    }
    if (cache->lines[set_index][i].time_stamp > max_time_stamp) {
      max_time_stamp = cache->lines[set_index][i].time_stamp;
      max_time_stamp_line = i;
    }
  }
  if (verbose) {
    printf("eviction\n");
  }
  eviction_count++;
  return max_time_stamp_line;
}

void LRU(Cache *cache, int set_index, int address_tag) {
  int hit_line = JudgeHitOrMiss(cache, set_index, address_tag);
  if (hit_line == -1) {
    miss_count++;
    if (verbose) {
      printf("miss\n");
    }
    hit_line = FindEvictionLine(cache, set_index);
    UpdateCacheLine(cache, set_index, address_tag, hit_line);

  } else {
    hit_count++;
    if (verbose) {
      printf("hit\n");
    }
    UpdateCacheLine(cache, set_index, address_tag, hit_line);
  }
}

/**
 * @brief Read the trace file and simulate the cache
 *
 * @param s set index bits
 * @param E associativity(lines per set)
 * @param b block bits
 */
void ReadTrace(Cache *cache, int s, int b) {
  FILE *fp = fopen(t, "r");
  if (fp == NULL) {
    exit(-1);
  }
  char identifier;  // L/S/M
  size_t address;   // 64 bits address
  int size;         // useless

  // miss_count++;
  while (fscanf(fp, "%c %lx,%d", &identifier, &address, &size) > 0) {
    int address_tag = address >> (s + b);
    int set_index = address >> b & ((1 << s) - 1);  // 左移补0 所以需要减1

    switch (identifier) {
      case 'I':
        break;
      case 'L':
        // hit_count++;
        LRU(cache, set_index, address_tag);
        break;
      case 'S':
        // hit_count++;
        LRU(cache, set_index, address_tag);
        break;
      case 'M':
        // hit_count++;
        LRU(cache, set_index, address_tag);
        LRU(cache, set_index, address_tag);
        break;
      default:
        break;
    }
  }
  fclose(fp);
}

/**
 * @brief print the usage of the program
 *
 */
void PrintUsage() {
  printf("** A Cache Simulator.\n");
  printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
  printf("Options:\n");
  printf("-h              Optional help flag that prints usage info.\n");
  printf("-v              Optional verbose flag that displays trace info.\n");
  printf("-s <s>          Number of set index bits.\n");
  printf("-E <E>          Number of lines per set(Associativity).\n");
  printf("-b <b>          Number of block bits.\n");
  printf("-t <tracefile>  Name of the valgrind trace to replay.\n\n\n");
  printf("Examples:\n");
  printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
  printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char *argv[]) {
  char opt;
  int s, E, b;
  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (opt) {
      case 'h':
        PrintUsage();
        break;
      case 'v':
        verbose = 1;
        break;
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        strcpy(t, optarg);
        break;
      default:
        PrintUsage();
        break;
    }
  }
  Cache *cache = CacheInit(s, E, b);
  ReadTrace(cache, s, b);
  CacheFree(cache);
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}
