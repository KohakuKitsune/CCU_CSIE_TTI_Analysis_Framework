#include <iostream>
#include <dirent.h>
#include <string>
#include <cstring>
#include "./Calculator/tti.h"

using namespace std;

int main()
{
  int route_count;        
  char** routes; 
  string input;  
  Calculator calculator;
  ifstream input_file;

  string file_location(INPUT_FILE_PATH);
  input_file.open(file_location);

  getline(input_file,input);
  route_count = stoi(input);

  routes = (char**) malloc (sizeof (char*) * route_count);
  for(int i = 0; i < route_count; i++) *(routes+i) = (char*) malloc(sizeof(char)*1024);

  //Reads routes from file
  for(int i = 0; i < route_count; i ++)
  {
    getline(input_file,input);
    strcpy(*(routes+i), input.c_str());
  }

  input_file.close();

  //-Begin to do calculation on each route-//
  for(int i = 0; i < route_count; i ++) calculator.RunCalc(*(routes+i));
  
  return 0;
}