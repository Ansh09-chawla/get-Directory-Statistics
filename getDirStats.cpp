/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// You need to edit this file.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, as long as you implement the getDirStats() function as
/// defined in "getDirStats.h".

#include "getDirStats.h"
#include "digester.h"
#include <stack>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>


static bool
is_dir(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), & buff)) return false;
  return S_ISDIR(buff.st_mode);
}

// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// getDirStats() computes stats about directory a directory
//   dir_name = name of the directory to examine
//   n = how many top words/filet types/groups to report
//
// if successful, results.valid = true
// on failure, results.valid = false
//
Results 
getDirStats(const std::string & dir_name, int n)
{
  // The results below are all hard-coded, to show you all the fields
  // you need to calculate. You should delete all code below and
  // replace it with your own code.
  Results res;
  int flag_for_main_directory =0;
  // prepare a fake results
  res.largest_file_path = "";
  res.largest_file_size = -1;
  res.n_files = 0;
  res.n_dirs = 0;
  res.all_files_size = 0;
 
  // initialization of hashmaps
  std::unordered_map<std::string,int> file_types;
  std::unordered_map<std::string,int> hist;
  std::unordered_map<std::string,std::vector<std::string>> duplicate_files;
  
  if (! is_dir(dir_name)){
    std::cout<<"Could not get dir stats, returning defualt values.\n";
    return res;
  } 
  // code adapted from hints provided by Dr. Pavol 
  //https://gitlab.com/cpsc457/public/find-empty-directories
  std::vector<std::string> stack;
  stack.push_back(dir_name);
 
  while( ! stack.empty()) {
    auto dirname = stack.back();
    stack.pop_back();

  
    struct stat sstruct;
    stat(dirname.c_str(), &sstruct);
    if (is_dir(dirname.c_str())){
      if(flag_for_main_directory){
        res.n_dirs++;
      }  
      flag_for_main_directory =1;
    }
    else{
      res.n_files++;
      res.all_files_size+=sstruct.st_size;
      if(sstruct.st_size>res.largest_file_size){
        res.largest_file_size = sstruct.st_size;
        res.largest_file_path = dirname.c_str();
      }
      std::string unique_file_string = sha256_from_file( dirname.c_str() );
      duplicate_files[unique_file_string].insert(duplicate_files[unique_file_string].begin(),dirname.c_str());

    // code adapted from hints provided by Dr. Pavol 
    // https://gitlab.com/cpsc457/public/word-histogram/-/blob/master/main.cpp
      FILE *fp;
      fp = fopen(dirname.c_str(),"r");
      if(fp==nullptr){
        std::cout<<"Failed to open one of the files, quitting.\n";
        exit(-1);
      }
      while(1) {
        std::string result;
        while(1) {
          int c = fgetc(fp);
          if(c == EOF) break;
          c = tolower(c);
          if(! isalpha(c)) {
            if(result.size() == 0)
              continue;
            else{
              // same code as prof, just a small change to make words contain three characters
              if(result.size()<3){ 
                result.clear();
                continue;
              }
              break;
            }  
          }
          else {
              result.push_back(c);       
          }

        }
     
          
        if(result.size() == 0) break;
        hist[result] ++;

      }

      fclose(fp);
      // code adapted from hints provided by Dr. Pavol 
      // https://gitlab.com/cpsc457/public/popen-example/-/blob/master/main.cpp
      std::string cmd = "file -b " + dirname;
      // run the command in popen()
      fp = popen( cmd.c_str(), "r");
      if( fp == nullptr) {
        printf("popen() failed, quitting\n");
        exit(-1);
      }
    // get the output of popen() using fgets(3) into raw C-style char buffer
      std::string ftype;
      char buff[4096];
      char * ress = fgets(buff, sizeof(buff), fp);
      pclose(fp);
      if( ress != nullptr) {
      // find the end of the first field ('\0', or '\n' or ',')
        int eol = 0;
        while(buff[eol] != ',' && buff[eol] != '\n' && buff[eol] != 0) eol ++;
      // terminate the string
        buff[eol] = 0;
      // remember the type
         ftype = buff;
      } else {
      // file(1) did not produce any output... no idea why, so let's
      // just skip the file, but make the file type something bizare
         ftype = "file(1) failed, not sure why";
      }
      file_types[ftype]++;
    }


  // code adapted from hints provided by Dr. Pavol 
  //https://gitlab.com/cpsc457/public/find-empty-directories

    DIR * dir = opendir( dirname.c_str());
    if( dir) {
      while(1) {
      	dirent * de = readdir( dir);
       	if( ! de) break;
	      std::string name = de-> d_name;
	      if( name == "." || name == "..") continue;
	      std::string path = dirname + "/" + de-> d_name;
	      stack.push_back( path);
      }
      closedir( dir);
    }
  
  }

  // code implemented by Anshdeep Singh to sort using number of entries for comparison, common comparator for 
  // most_common_words and most_common_types
  auto compare = [](auto& a, auto& b) {
        return a.second > b.second;
  };


  // code adapted from hints provided by Dr. Pavol 
  // https://gitlab.com/cpsc457/public/word-histogram/-/blob/master/main.cpp
  for (auto& h : hist) {
      res.most_common_words.emplace_back(h.first, h.second);
  }
  // if we have more than N entries, we'll sort partially, since
  // we only need the first N to be sorted

  if (res.most_common_words.size() > size_t(n)) {
      std::partial_sort(res.most_common_words.begin(), res.most_common_words.begin() + n, res.most_common_words.end(), compare);
      // drop all entries after the first n
      res.most_common_words.resize(n);
  } else {
      std::sort(res.most_common_words.begin(), res.most_common_words.end(), compare);
  }
 

  for (auto& file_type : file_types) {
      res.most_common_types.emplace_back(file_type.first, file_type.second);
  }

  // if we have more than N entries, we'll sort partially, since
  // we only need the first N to be sorted
  if (res.most_common_types.size() > size_t(n)) {
      std::partial_sort(res.most_common_types.begin(), res.most_common_types.begin() + n, res.most_common_types.end(), compare);
      // drop all entries after the first n
      res.most_common_types.resize(n);
  }else {
      std::sort(res.most_common_types.begin(), res.most_common_types.end(), compare);
  }
  for (auto& dups : duplicate_files) {
    if(dups.second.size()>1)
        res.duplicate_files.emplace_back(dups.second);
  }
  // code implemented by Anshdeep Singh to sort using sizefor comparison,
  // comparator for sorting duplicate files
  auto compare_dups = [](auto& a, auto& b) {
        return a.size() > b.size();
  };
  if (res.duplicate_files.size() > size_t(n)) {
      std::partial_sort(res.duplicate_files.begin(), res.duplicate_files.begin() + n, res.duplicate_files.end(),compare_dups);
      // drop all entries after the first n
      res.duplicate_files.resize(n);
  } else {
      std::sort(res.duplicate_files.begin(), res.duplicate_files.end(), compare_dups);
  }
  return res;
}
