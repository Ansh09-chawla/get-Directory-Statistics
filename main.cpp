

#include "getDirStats.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <sys/time.h>
#include <sys/resource.h>

void usage(const std::string & pname, int exit_code)
{
  printf("Usage: %s N directory_name\n", pname.c_str());
  exit(exit_code);
}

int main(int argc, char ** argv)
{
  {
    struct rlimit rlim;
    rlim.rlim_cur = 256;
    rlim.rlim_max = 256;
    int res = setrlimit(RLIMIT_NOFILE, & rlim);
    assert( res == 0);
  }
  
  if (argc != 3) usage(argv[0], -1);

  Results res = getDirStats(argv[2], std::stoi(argv[1]));
  printf("--------------------------------------------------------------\n");
  printf("Largest file:      \"%s\"\n", res.largest_file_path.c_str());
  printf("Largest file size: %ld\n", res.largest_file_size);
  printf("Number of files:   %ld\n", res.n_files);
  printf("Number of dirs:    %ld\n", res.n_dirs);
  printf("Total file size:   %ld\n", res.all_files_size);
  printf("Most common file types:\n");
  for (auto & type : res.most_common_types)
    printf("  - \"%s\" x %d\n", type.first.c_str(), type.second);
  printf("Most common words:\n");
  for (auto & type : res.most_common_words)
    printf("  - \"%s\" x %d\n", type.first.c_str(), type.second);
  int gcount = 1;
  for (auto & group : res.duplicate_files) {
    printf("Duplicate files - group %d:\n", gcount++);
    for (auto & f : group) printf("  - \"%s\"\n", f.c_str());
  }
  printf("--------------------------------------------------------------\n");
  return 0;
}
