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
  starting_date = (char*) malloc(sizeof(char) * 8);     
  ending_date = (char*) malloc(sizeof(char) * 8); 
  road_segment_count = 0;
  route_distance = 0;
  last_alt_tti_value = 0;
  last_tti_value = 0;
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
  bus_peak_period_count = 0;
  for(int i = 0; i < strlen(current_route); i++) 
  {
    if(*(current_route + i) == 'V') road_segment_count ++;
    else if(*(current_route + i) == '-') bus_peak_period_count ++;
  }  

  InitRoadSegmentInfo();

  period_start = (int*) malloc(sizeof(int) * bus_peak_period_count+1);
  period_end = (int*) malloc(sizeof(int) * bus_peak_period_count+1);

  char* word  = (char*) malloc (sizeof(char) * 32);
  char* pos = current_route;
  for(int i = 0; i < (road_segment_count+2); i++)
  {
    pos = GetWord(word,pos,' ');
    cout << word << endl;
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

  char* hour  = (char*) malloc (sizeof(char) * 32);
  for(int i = 0; i < bus_peak_period_count; i++)
  {
    pos = GetWord(word,pos,' ');

    char* ptr = word;
    ptr = GetWord(hour,ptr,'-');
    *(period_start+i) = atoi(hour);
    cout << *(period_start+i) << endl;

    ptr = GetWord(hour,ptr,'-');
    *(period_end+i) = atoi(hour);
    cout << *(period_end+i) << endl;
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

  string output_file_directory_path(TIME_TRAVEL_INDEX_DATA_DIRECTORY_DEF_PATH);
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
  ofile << "Date,TTI Value" << "\n";

  if(bus_peak_period_count > 0)
  {
    string output_file_name(first_road_segment + "-" + last_road_segment + "_(" + starting_date_string + "~" + ending_date_string + ")_alt");
    string alternate_file_directory_path(TIME_TRAVEL_INDEX_DATA_DIRECTORY_ALT_PATH);
    altfile.open(alternate_file_directory_path + "/" + output_file_name + ".csv");
    altfile << "Route Name:," << "(" << first_road_segment << ")" << first_road_segment_name 
    << " ~ ("<< last_road_segment << ")" << last_road_segment_name <<"\n";

    altfile << "Route Information:" << "\n";
    altfile << "Road Segment ID,Road Segment Name" << "\n";

    string road_segment_name;
    for(int i = 0; i < road_segment_count; i++)
    {
    road_segment_name = GetRoadSegmentName((*(road_segment_info + i))->road_segment_id);
    altfile << (*(road_segment_info + i))->road_segment_id << "," << road_segment_name << "\n";
    }
    altfile << "\n" << "Daily Travel Time Index:" << "\n";
    altfile << "Date,TTI Value" << "\n";   
  }
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
  if(bus_peak_period_count > 0) altfile.close();
}

void Calculator::MonthIntervalCalculation(int current_year,int current_month,int number_of_days)
{
  string current_date;
  float tti_value = 0;
  float alt_tti_value = 0;
  bool reuse_last_tii = false;
  int corrupt_data_count = 0;
  for(int current_day = 0; current_day < number_of_days; current_day++)
  {
    current_date = GetCurrentDateEntry(current_year, current_month, current_day+1);

    for(int current_segment_id = 0; current_segment_id < road_segment_count; current_segment_id++)
    {
      corrupt_data_count = RetrieveRoadSegmentData(current_segment_id,current_year,current_month,current_day);

      if(corrupt_data_count >= 6) reuse_last_tii = true;
    }


    if(reuse_last_tii == false)
    {
      tti_value = CalculateDailyTTI(0);
      ofile << current_date << "," << tti_value << endl;
      last_tti_value = tti_value;

      if(bus_peak_period_count > 0)
      {
        alt_tti_value = CalculateDailyTTI(1);
        altfile << current_date << "," << alt_tti_value << endl; 
        last_alt_tti_value = alt_tti_value;     
      }
    }
    else
    {
      ofile << current_date << "," << last_tti_value << endl; 
      altfile << current_date << "," << last_alt_tti_value << endl; 
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

/*float Calculator::GetRoadSegmentLength(string road_segment_id)
{
  ifstream info_file;
  info_file.open(ROAD_SEGMENT_LENGTH_FILE_PATH);

  // Begin Parsing the file
  string line;
  float road_segment_length;
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
      road_segment_length = stof(column_entry);
    }
  }
  info_file.close();

  return road_segment_length;
}*/

float Calculator::CalculateDailyTTI(int mode)
{
  float tti_value = 0;
  float route_peak_travel_time = 0;
  float route_free_flow_travel_time = 0;
  float route_peak_travel_speed = 0;
  float route_free_flow_travel_speed = 0;

  for(int i = 0; i < road_segment_count; i++)
  {
    route_free_flow_travel_time = route_free_flow_travel_time + GetFreeFlowTravelTIme(i);
    if(mode == 0) route_peak_travel_time = route_free_flow_travel_time + GetPeakTravelTIme(i);
    else route_peak_travel_time = route_free_flow_travel_time + AltGetPeakTravelTIme(i);
  }

  if(route_free_flow_travel_time == 0 || route_peak_travel_time == 0) return 0;

  route_free_flow_travel_speed = (float) route_distance / route_free_flow_travel_time;
  route_peak_travel_speed = (float) route_distance  / route_peak_travel_time;

  cout << route_free_flow_travel_speed << endl;
  cout << route_peak_travel_speed << endl;

  tti_value = (float) route_free_flow_travel_speed / route_peak_travel_speed;

  return tti_value;
}

float Calculator::GetFreeFlowTravelTIme(int segment_id)
{
  int freeflow_travel_speed = 0;
  
  for(int i = 0; i < HOURS_IN_A_DAY; i++)
  {
    if((*(road_segment_info + segment_id))->average_speed[i] > freeflow_travel_speed) freeflow_travel_speed = (*(road_segment_info + segment_id))->average_speed[i];
  }
  

  if(freeflow_travel_speed == 0) return 0;
  else return (float) ((*(road_segment_info + segment_id))->road_segment_length / freeflow_travel_speed);

}

float Calculator::GetPeakTravelTIme(int segment_id)
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

float Calculator::AltGetPeakTravelTIme(int segment_id)
{
  int index = 0;
  int count = 0;
  int peak_travel_speed = 0;
  float avg_peak_travel_speed = 0;

  for(int i = 0; i < HOURS_IN_A_DAY; i++)
  {
    if(i >= *(period_start+index) && i <= *(period_end+index))
    {
      peak_travel_speed = peak_travel_speed + (*(road_segment_info + segment_id))->average_speed[i];
      count++;
    }

    if(i == *(period_end+index)) index++;
  }

  if(peak_travel_speed == 0) return 0;
  else
  {
    avg_peak_travel_speed = (float) peak_travel_speed / count;
    return (float) ((*(road_segment_info + segment_id))->road_segment_length / avg_peak_travel_speed);
  }
}

