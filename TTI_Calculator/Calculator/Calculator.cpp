#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <locale>
#include "tti.h"

using namespace std;


// Constuctors
Calculator::Calculator() // Default Constructor
{
  current_route = (char*) malloc(sizeof(char) * 1024); 
  route = NULL;   
  route_cnt  = 0;     
}

// Destructor
Calculator::~Calculator() // Destructs an object
{
  cout << endl;
  cout << "An Calculator Object successfully destroyed" << endl;
}

void Calculator::RunCalc() // Main function of object Calculator
{
  //--------Recieve input from file-----------//
  string file_location(INPUT_FILE_PATH);
  ifile.open(file_location);

  getline(ifile,input);
  route_cnt = stoi(input);

  InitRoute(route_cnt);

  //Reads input from file
  for(int i = 0; i < route_cnt; i ++)
  {
    getline(ifile,input);
    strcpy(*(routes+i), input.c_str());
  }

  //-Begin to do calculation on each route-//
  for(int i = 0; i < route_cnt; i ++)
  {
    current_route = *(routes+i);

    route_info.road_segment_cnt = 0;
    for(int j = 0; j < strlen(current_route); j++)
    {
      if(*(current_route + j) == 'V') route_info.road_segment_cnt ++;
    }
    //
    //Extract road id of each road segment, starting date and ending date.//
    InitDChunk();
    char* temp  = (char*) malloc (sizeof(char) * 32);
    char* pos = current_route;
    for(int j = 0; j < (route_info.seg_cnt+2); j++)
    {
      pos = GetWord(temp,pos,' ');

      if(j < route_info.seg_cnt) strcpy(*(route_info.segments+j), temp);

      else if(j < route_info.seg_cnt+1) strcpy(route_info.start, temp);

      else strcpy(route_info.end, temp);
      
    }
    //
    /*
    //Output all contents in dchunk//
    for(int j = 0; j < vd_daily.seg_cnt; j++) cout << *(vd_daily.segments+j) << endl;
    cout << "starting date" <<vd_daily.start << endl;
    cout << "ending date" <<vd_daily.end << endl;    
    */
    //
    //Begin to extract data with given route and time interval//
    RetrieveData();

  }  
  //InitDChunk();

}

char* Calculator::GetWord(char* dest,char* src, char delim)  
{
	
  char* ptr=src;
  char* qtr=dest;
  while(*ptr&&isspace(*ptr)) ptr++;

  while(*ptr&& (*(ptr) != delim))
  {
    if(qtr-dest>=8) break;
    *qtr++=*ptr++;
  }

  *qtr='\0';

  if(qtr-dest==0){
    return NULL;
  }

  return ptr;
}

void Calculator::InitRoute(int cnt)
{
  routes = (char**) malloc (sizeof (char*) * cnt);
  for(int i = 0; i < cnt; i++)
    *(routes+i) = (char*) malloc(sizeof(char)*1024);
}

void Calculator::InitDChunk()
{
  route_info.segments = (char**) malloc(sizeof(char*) * (route_info.seg_cnt)); 
  route_info.filename = (char**) malloc(sizeof(char*) * (route_info.seg_cnt));

  for(int i = 0; i < route_info.seg_cnt; i++)
  {
    *(route_info.segments+i) = (char*) malloc(sizeof(char) * 8);
    *(route_info.filename+i) = (char*) malloc(sizeof(char) * 8);
  }

  route_info.fpost = (long*) malloc(sizeof(long) * (vd_daily.seg_cnt));    
  vd_daily.start = (char*) malloc(sizeof(char) * 8);     
  vd_daily.end = (char*) malloc(sizeof(char) * 8);       

  vd_daily.d_entry = (dblock**) malloc(sizeof(dblock*) * (24 *vd_daily.seg_cnt));
  for(int i = 0; i < (24 * vd_daily.seg_cnt); i++)
  {
    *(vd_daily.d_entry+i) = (dblock*) malloc(sizeof(dblock));
  }  

}

void Calculator::RetrieveData()
{
    int start_y = 0;
    int start_m = 0;
    int end_y = 0;
    int end_m = 0;
    int loop_cnt = 0;

    GetYearNMonth(&start_y, &start_m, route_info.start);
    GetYearNMonth(&end_y, &end_m, route_info.end);

    if(end_y > start_y) end_m = end_m + (12 * (end_y - start_y));

    loop_cnt = end_m - start_m + 1;

    int curr_y = start_y;
    int curr_m = start_m;
    int days = 0;
    for(int i = 0; i < loop_cnt; i++)
    {
      if(curr_m == 2 )
      {
        if(curr_y % 4 == 0) days = 29;
        else days = 28;
    
      }
      else if(curr_m < 8 )
      {
        if(curr_m % 2 ==0) days = 30;
        else days = 31;

      }
      else
      {
        if(curr_m % 2 ==0) days = 31;
        else days = 30;
      }

      CalMonthTTI(curr_y, curr_m, days);

      curr_m++;

      if(curr_m > 12)
      {
        curr_m = 1;
        curr_y++;
      }
      days = 0;
    }
  

}

void Calculator::GetYearNMonth(int* year, int* month, char* date)
{
  char* d_pos = date;
  char* s_year = (char*) malloc(sizeof(char) * 5);
  char* s_month = (char*) malloc(sizeof(char) * 3);

  d_pos = date;
  d_pos = GetWord(s_year,d_pos,'/');
  d_pos++;
  d_pos = GetWord(s_month,d_pos,' ');

  *year = atoi(s_year);
  *month = atoi(s_month);

  free(s_year);
  free(s_month);
}

void Calculator::CalMonthTTI(int year, int month, int days)
{
  return;
}