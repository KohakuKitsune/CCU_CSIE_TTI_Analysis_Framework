#include <iostream>
#include <dirent.h>
#include "./Preprocessor/prep.h"

using namespace std;

int main()
{
  DIR *open_source = NULL;
  dirent *pdir = NULL;
  Preprocessor processor;

  open_source=opendir(OPEN_SOURCE_DATA_DIRECTORY_PATH);

  while((pdir = readdir(open_source)) != NULL)
  {
    if(pdir->d_type == DT_REG) 
    {
      cout << pdir->d_name << endl;
      processor.Preprocess(pdir->d_name);
    }
  }

  closedir(open_source);

  return 0;
}