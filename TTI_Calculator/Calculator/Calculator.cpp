#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <locale>
#include "tti.h"

using namespace std;


// Constuctors
Calculator::Calculator() // Default Constructor
{
  starting_date = (char*) malloc(sizeof(char) * 8);     
  ending_date = (char*) malloc(sizeof(char) * 8); 
  road_segment_count = 0;
  route_distance = 0;
  for(int i = 0; i < 6; i++)
  {
    tti_value[i] = 0;
    last_tti_value[i] = 1;
  }
}

// Destructor
Calculator::~Calculator() // Destructs an object
{
  cout << endl;
  cout << "An Calculator Object successfully destroyed" << endl;
}

void Calculator::RunCalc(char* current_route) // Main function of object Calculator
{
  road_segment_count = 0;
  route_distance = 0;
  for(int i = 0; i < strlen(current_route); i++) 
  {
    if(*(current_route + i) == 'V') road_segment_count ++;
  }  

  InitRoadSegmentInfo();

  char* word  = (char*) malloc (sizeof(char) * 32);
  char* pos = current_route;
  for(int i = 0; i < (road_segment_count+2); i++)
  {
    pos = GetWord(word,pos,' ');
    //cout << word << endl;
    if(i < road_segment_count)
    {
      (*(road_segment_info + i))->road_segment_id.assign(word);
      pos = GetWord(word,pos,' ');
      (*(road_segment_info + i))->road_segment_length = atof(word);
      route_distance = route_distance + (*(road_segment_info + i))->road_segment_length;
    }

    else if(i == road_segment_count) strcpy(starting_date, word);

    else strcpy(ending_date, word); 
  }

  CalculateRouteTTI();
}


void Calculator::InitRoadSegmentInfo()
{
  road_segment_info = (ROAD_SEGMENT**) malloc(sizeof(ROAD_SEGMENT*) * road_segment_count);

  for(int i = 0; i < road_segment_count; i++)
  {
    *(road_segment_info + i) = new ROAD_SEGMENT;
    (*(road_segment_info + i))->road_segment_length = 0;

    for(int j  = 0 ; j < HOURS_IN_A_DAY; j++)
    {
      (*(road_segment_info + i))->average_speed[j] = 0;
      (*(road_segment_info + i))->traffic_volume[j] = 0;
    }
  }
}

void Calculator::CreateOutputFile(int starting_year,int starting_month,int ending_year,int ending_month)
{
  string first_road_segment = (*(road_segment_info + 0))->road_segment_id;
  string last_road_segment = (*(road_segment_info + (road_segment_count-1)))->road_segment_id;

  string starting_date_string(to_string(starting_year) + "-" + to_string(starting_month));
  string ending_date_string(to_string(ending_year) + "-" + to_string(ending_month));


  string output_file_name(first_road_segment + "-" + last_road_segment + "_(" + starting_date_string + "~" + ending_date_string + ")");

  string output_file_directory_path(TIME_TRAVEL_INDEX_DATA_DIRECTORY_PATH);
  ofile.open(output_file_directory_path + "/" + output_file_name + ".csv");

  string first_road_segment_name = GetRoadSegmentName(first_road_segment);
  string last_road_segment_name = GetRoadSegmentName(last_road_segment);

  ofile << "Route Name:," << "(" << first_road_segment << ")" << first_road_segment_name 
  << " ~ ("<< last_road_segment << ")" << last_road_segment_name <<"\n";

  ofile << "Route Information:" << "\n";
  ofile << "Road Segment ID,Road Segment Name" << "\n";

  string road_segment_name;
  for(int i = 0; i < road_segment_count; i++)
  {
    road_segment_name = GetRoadSegmentName((*(road_segment_info + i))->road_segment_id);
    ofile << (*(road_segment_info + i))->road_segment_id << "," << road_segment_name << "\n";
  }
  ofile << "\n" << "Daily Travel Time Index:" << "\n";
  ofile << "Date,TTI Value Type 1,TTI Value Type 2,TTI Value Type 3(AM),TTI Value Type 3(PM),TTI Value Type 4(AM),TTI Value Type 4(PM)" << "\n";
  
}


void Calculator::CalculateRouteTTI()
{
  int starting_year = 0;
  int starting_month = 0;
  int ending_year = 0;
  int ending_month = 0;
  int loop_cnt = 0;

  GetYearNMonth(&starting_year, &starting_month, starting_date);
  GetYearNMonth(&ending_year, &ending_month, ending_date);

  CreateOutputFile(starting_year,starting_month,ending_year,ending_month);

  if(ending_year > starting_year) ending_month = ending_month + (12 * (ending_year - starting_year));

  loop_cnt = ending_month - starting_month + 1;

  int current_year = starting_year;
  int current_month = starting_month;
  int number_of_days = 0;
  for(int i = 0; i < loop_cnt; i++)
  {
    number_of_days = GetTotalDaysInCurrentMonth(current_year,current_month);

    MonthIntervalCalculation(current_year,current_month,number_of_days);
    
    current_month++;

    if(current_month > 12)
    {
      current_month = 1;
      current_year++;
    }
    number_of_days = 0;
  }
  ofile.close();
}

void Calculator::MonthIntervalCalculation(int current_year,int current_month,int number_of_days)
{
  string current_date;
  bool reuse_last_tii = false;
  int corrupt_data_count = 0;
  for(int current_day = 0; current_day < number_of_days; current_day++)
  {
    current_date = GetCurrentDateEntry(current_year, current_month, current_day+1);

    for(int current_segment_id = 0; current_segment_id < road_segment_count; current_segment_id++)
    {
      corrupt_data_count = RetrieveRoadSegmentData(current_segment_id,current_year,current_month,current_day);

      if(corrupt_data_count >= 8) reuse_last_tii = true;
    }


    if(reuse_last_tii == false)
    {
      CalculateDailyTTI();
      ofile << current_date;
      for(int i  = 0; i < 6; i++)
      {
        //if(tti_value[i] >= 1)
        //{
          ofile  << "," << tti_value[i];
          last_tti_value[i] = tti_value[i];
          tti_value[i] = 0;
        //}
        //else
        /*{
          ofile  << "," << 1;
          last_tti_value[i] = 1;
          tti_value[i] = 0; 
        }*/
      }
      ofile  << endl;
    }
    else
    {
      ofile << current_date;
      for(int i  = 0; i < 6; i++)
      {
        ofile << "," << last_tti_value[i];
      }
      ofile  << endl;
    }

    reuse_last_tii = false;
  }
}

int Calculator::RetrieveRoadSegmentData(int segment_id,int year,int month,int day)
{
  int error_count = 0;
  string segment_name = (*(road_segment_info + segment_id))->road_segment_id;

  string date;
  if(month < 10) date = string( to_string(year) + "_0"  + to_string(month));
  else  date = string( to_string(year) + "_" + to_string(month));  

  string traffic_volume_file_name(segment_name+ "_" + date);
  string segment_data_directory_path(TRAFFIC_VOLUME_DATA_DIRECTORY_PATH);

  ifile.open(segment_data_directory_path + "/" + traffic_volume_file_name + ".csv");

  int skip_entries = NUMBER_OF_FILE_HEADER_ROWS + (HOURS_IN_A_DAY * day);

  string line;
  string current_row_entry[TRAFFIC_VOLUME_DATA_COLUMN_COUNT];

  // skip to the entry of the starting hour of the day
  for(int i = 0; i < skip_entries; i++) getline(ifile, line);

  for(int i = 0; i < HOURS_IN_A_DAY; i++)
  {
    getline(ifile, line);
    istringstream entry_copy;
    entry_copy.str(line);
    string column_entry;
    int parse_index = 0, column_index = 0;

    getline(entry_copy, column_entry, ',');

    getline(entry_copy, column_entry, ',');
    (*(road_segment_info + segment_id))->average_speed[i] = stoi(column_entry);

    getline(entry_copy, column_entry, ',');
    (*(road_segment_info + segment_id))->traffic_volume[i] = stoi(column_entry);

    getline(entry_copy, column_entry, ',');
    if(stoi(column_entry) > 0) error_count ++;
  }
  ifile.close();

  return error_count;
}

char* Calculator::GetWord(char* dest,char* src, char delim)  
{
	
  char* ptr=src;
  char* qtr=dest;
  while(*ptr&&(*(ptr) == delim)) ptr++;

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

void Calculator::GetYearNMonth(int* year, int* month, char* date)
{
  char* ptr = date;
  char* year_string = (char*) malloc(sizeof(char) * 5);
  char* month_string = (char*) malloc(sizeof(char) * 3);

  ptr = date;
  ptr = GetWord(year_string,ptr,'/');
  ptr++;
  ptr = GetWord(month_string,ptr,' ');

  *year = atoi(year_string);
  *month = atoi(month_string);

  free(year_string);
  free(month_string);
}

int Calculator::GetTotalDaysInCurrentMonth(int year, int month)
{
  int days = 0;
  if(month == 2 )
  {
    if(year % 4 == 0) days = 29;
    else days = 28;
  }
  else if(month < 8 )
  {
    if(month % 2 ==0) days = 30;
    else days = 31;

  }
  else
  {
    if(month % 2 ==0) days = 31;
    else days = 30;
  }

  return days;
}

string Calculator::GetRoadSegmentName(string road_segment_id)
{
  ifstream info_file;
  info_file.open(ROAD_SEGMENT_NAME_FILE_PATH);

  // Begin Parsing the file
  string line;
  string road_segment_name;
  getline(info_file, line); // skip header
  while (getline(info_file, line))
  {
    istringstream entry_copy;
    entry_copy.str(line);
    string column_entry;

    getline(entry_copy, column_entry, ',');
    if(road_segment_id.compare(column_entry) == 0)
    {
      getline(entry_copy, column_entry, ',');
      road_segment_name.assign(column_entry);
    }
  }
  info_file.close();

  return road_segment_name;
}

string Calculator::GetCurrentDateEntry(int year, int month, int day)
{
  string current_date_entry;
  if(month < 10) current_date_entry = string( to_string(year) + "/0"  + to_string(month) + "/");
  else  current_date_entry = string( to_string(year) + "/" + to_string(month) + "/");  

  if(day < 10) current_date_entry.append("0" + to_string(day));
  else current_date_entry.append(to_string(day));

  return current_date_entry;
}

void Calculator::CalculateDailyTTI()
{
  float route_peak_travel_time[3];
  float route_free_flow_travel_time[2];
  float route_free_flow_travel_speed;
  float route_peak_travel_speed;

  for(int i = 0; i < 3 ; i++)
  {
    route_peak_travel_time[i] = 0;
    route_free_flow_travel_time[i] = 0;   
  }

  for(int i = 0; i < road_segment_count; i++)
  {
    route_free_flow_travel_time[0] = route_free_flow_travel_time[0] + GetFreeFlowTravelTime(i);
    route_free_flow_travel_time[1] = route_free_flow_travel_time[1] + AltGetFreeFlowTravelTime(i);
    route_peak_travel_time[0] = route_peak_travel_time[0] + GetPeakTravelTime(i);
    AltGetPeakTravelTime(i ,route_peak_travel_time);

    cout << GetFreeFlowTravelTime(i) << endl;
    cout << AltGetFreeFlowTravelTime(i) << endl;
    cout << GetPeakTravelTime(i) << endl;
  }

  for(int i = 0; i < 2; i++)
  {
    if(route_peak_travel_time[0] == 0 || route_free_flow_travel_time[i] == 0)
    { 
      tti_value[i] = 1;
    }
    else
    {
      route_free_flow_travel_speed = (float) route_distance / route_free_flow_travel_time[i];
      route_peak_travel_speed = (float) route_distance  / route_peak_travel_time[0];
      tti_value[i] = (float) route_free_flow_travel_speed / route_peak_travel_speed;
    }

    if(route_peak_travel_time[1] == 0 || route_free_flow_travel_time[i] == 0)
    {
      tti_value[2+(i*2)] = 1;
    }
    else
    {
      route_free_flow_travel_speed = (float) route_distance / route_free_flow_travel_time[i];
      route_peak_travel_speed = (float) route_distance  / route_peak_travel_time[1];
      tti_value[2+(i*2)] = (float) route_free_flow_travel_speed / route_peak_travel_speed;
      
    }

    if(route_peak_travel_time[2] == 0 || route_free_flow_travel_time[i] == 0)
    {
      tti_value[3+(i*2)] = 1;
    }
    else
    {     
      route_free_flow_travel_speed = (float) route_distance / route_free_flow_travel_time[i];
      route_peak_travel_speed = (float) route_distance  / route_peak_travel_time[2];
      tti_value[3+(i*2)] = (float) route_free_flow_travel_speed / route_peak_travel_speed;
    }
  }

}

float Calculator::GetFreeFlowTravelTime(int segment_id)
{
  int freeflow_travel_speed = 0;
  
  for(int i = 0; i < HOURS_IN_A_DAY; i++)
  {
    if((*(road_segment_info + segment_id))->average_speed[i] > freeflow_travel_speed) freeflow_travel_speed = (*(road_segment_info + segment_id))->average_speed[i];
  }
  
  return (float) ((*(road_segment_info + segment_id))->road_segment_length / freeflow_travel_speed);

}

float Calculator::AltGetFreeFlowTravelTime(int segment_id)
{
  return (float) ((*(road_segment_info + segment_id))->road_segment_length / SPEED_LIMIT);
}

float Calculator::GetPeakTravelTime(int segment_id)
{
  int peak_traffic_volume = 0;
  int peak_travel_speed = 0;
  int index = 0;

  for(int i = 0; i < HOURS_IN_A_DAY; i++)
  {
    if((*(road_segment_info + segment_id))->traffic_volume[i] > peak_traffic_volume)
    { 
      peak_traffic_volume = (*(road_segment_info + segment_id))->traffic_volume[i];
      index = i;
    }
  }

  if(peak_traffic_volume == 0) return 0;
  else
  {
    peak_travel_speed = (*(road_segment_info + segment_id))->average_speed[index];

    if(peak_travel_speed == 0) return 0;
    else return (float) ((*(road_segment_info + segment_id))->road_segment_length / peak_travel_speed);
  }
}

void Calculator::AltGetPeakTravelTime(int segment_id, float* travel_time)
{
  float threshold = 0;
  float sum_sharpness = 0;
  float am_peak_speed = 0;
  float pm_peak_speed = 0;
  float sharpness[18];
  int am_candidate[4];
  int pm_candidate[5];
  int non_zero_count = 0;
  int am_candidate_count = 0;
  int pm_candidate_count = 0;

  for(int i = 0; i < 18 ; i++) sharpness[i] = 0;
  for(int i = 0; i < 4 ; i++) am_candidate[i] = 0;
  for(int i = 0; i < 5 ; i++) pm_candidate[i] = 0;

  for(int i = 0; i < 18; i++)
  {
    sharpness[i] = CalculateSharpness(segment_id,i+3);
    if(sharpness != 0)
    {
      sum_sharpness = (float) sum_sharpness + sharpness[i];
      non_zero_count++;
    }
  }

  if(non_zero_count == 0)
  {
    *(travel_time+1) = (float) *(travel_time+1) + 0;
    *(travel_time+2) = (float) *(travel_time+2) + 0;
    return;
  }

  threshold = (float) sum_sharpness / non_zero_count;

  for(int i = 4; i < 8; i++)
  {
    if(sharpness[i] > threshold && (*(road_segment_info + segment_id))->average_speed[i+3] != 0)
    {
      am_candidate[am_candidate_count] = (*(road_segment_info + segment_id))->average_speed[i+3];
      am_candidate_count++;
    }
  }

  am_peak_speed = am_candidate[0];
  for(int i = 0; i < am_candidate_count; i++)
  {
    if(am_candidate[i] < am_peak_speed) am_peak_speed = am_candidate[i];
  }
  
  if(am_peak_speed == 0) *(travel_time+1) = (float) *(travel_time+1) + 0;
  else *(travel_time+1) = (float) *(travel_time+1) + ((*(road_segment_info + segment_id))->road_segment_length / am_peak_speed);


  for(int i = 13; i < 18; i++)
  {
    if(sharpness[i] > threshold && (*(road_segment_info + segment_id))->average_speed[i+3] != 0)
    {
      pm_candidate[pm_candidate_count] = (*(road_segment_info + segment_id))->average_speed[i+3];
      pm_candidate_count++;
    }
  }

  pm_peak_speed = pm_candidate[0];
  for(int i = 0; i < pm_candidate_count; i++)
  {
    if(pm_candidate[i] < pm_peak_speed) pm_peak_speed = pm_candidate[i];

  }
  
  if(pm_peak_speed == 0) *(travel_time+2) = (float) *(travel_time+2) + 0;
  else *(travel_time+2) = (float) *(travel_time+2) + ((*(road_segment_info + segment_id))->road_segment_length / pm_peak_speed);

}

float Calculator::CalculateSharpness(int segment_id, int index)
{
  float distance_a = CalculateDistance(segment_id, index - 3, index + 3);
  float distance_b = CalculateDistance(segment_id, index - 3, index);
  float distance_c = CalculateDistance(segment_id, index, index + 3);

  return (float) 1 - (fabs(distance_a) / ( fabs(distance_b) + fabs(distance_c) ) );
}

float Calculator::CalculateDistance (int segment_id, int start, int end)
{
  int time_difference = end - start;
  int speed_difference = (*(road_segment_info + segment_id))->average_speed[end] - (*(road_segment_info + segment_id))->average_speed[start];
  float distance_square = pow(time_difference, 2) + pow(speed_difference, 2);

  return sqrtf(distance_square); 
}
