#include <iostream>
#include <dirent.h>
#include "./Calculator/tti.h"

using namespace std;

int main()
{
  DIR *preproccessed_data = NULL;
  dirent *pdir = NULL;

  cout << "Listing all vd data files:" << endl;
  preproccessed_data=opendir(PREPROCESSED_DATA_DIRECTORY_PATH);

  while((pdir = readdir(preproccessed_data)) != NULL)
  {
    if(pdir->d_type == DT_REG) 
    {
      cout << pdir->d_name << endl;
    }
  }
  closedir(preproccessed_data);

  
  return 0;
}