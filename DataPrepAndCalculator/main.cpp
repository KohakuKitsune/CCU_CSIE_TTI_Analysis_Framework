#include <iostream>
#include <dirent.h>
#include "./dataprep/prep.h"
#include "./tti/tti.h"

using namespace std;

int main()
{
  DIR *raw_dir = NULL, *vd_dir = NULL;
  dirent *pdir = NULL;
  Preprocessor d_prep;
  CalculateTTI d_calc;

  /*-------------Begin Data Preprocessing-------------*/
  //Raw Data
  
  //cout << "Listing all raw data files:" << endl;
  raw_dir=opendir(RAW_DATA);

  while((pdir = readdir(raw_dir)) != NULL)
  {
    if(pdir->d_type == DT_REG) 
    {
      cout << pdir->d_name << endl;
      d_prep.RunPrep(pdir->d_name);
    }
  }

  closedir(raw_dir);
  
  
  /*--------------------------------------------------*/
  /*------------Begin TII KPI calculation-------------*/
  //VD Data
  cout << "Listing all vd data files:" << endl;
  vd_dir=opendir(VD_DATA);

  while((pdir = readdir(vd_dir)) != NULL)
  {
    if(pdir->d_type == DT_REG) 
    {
      cout << pdir->d_name << endl;
      //d_calc.RunCalc(pdir->d_name);
    }
  }

  closedir(vd_dir);

  //Begin Calculate TTI
  /*--------------------------------------------------*/
  return 0;
}