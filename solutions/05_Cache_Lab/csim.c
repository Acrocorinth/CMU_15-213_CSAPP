#include "cachelab.h"

typedef struct cache_line {
  int valid;      //有效位
  int tag;        //标记位
  int time_tamp;  //时间戳
} Cache_line;

int main()
{
    printSummary(0, 0, 0);
    return 0;
}
