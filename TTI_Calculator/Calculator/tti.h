#ifndef TTI_H
#define TTI_H

#include <fstream>
#include <vector>

#define INPUT_FILE_PATH "C:/CSIE_Project/Program/TTI_Calculator/input.txt"
#define TRAFFIC_VOLUME_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Traffic_Volume_Data"
#define TIME_TRAVEL_INDEX_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Time_Travel_Index_Data" 
#define ROAD_SEGMENT_NAME_FILE_PATH "C:/CSIE_Project/Program/CSV/Road_Segment_Information/Road_Segement_Name.csv"

#define HOURS_IN_A_DAY 24
#define NUMBER_OF_FILE_HEADER_ROWS 2
#define TRAFFIC_VOLUME_DATA_COLUMN_COUNT 3
#define SPEED_LIMIT 50
/*#define AM_START 7
#define AM_END 10
#define PM_START 16
#define PM_END 20*/
#define K_VALUE 3

using namespace std;

//Below is a data structure that holds the a hour's long vd data of a road segment.
typedef struct hour_road_vd_block {
  string road_segment_id;
  string traffic_volume_file_name;
  float road_segment_length;
  int average_speed[HOURS_IN_A_DAY];
  int traffic_volume[HOURS_IN_A_DAY];
} ROAD_SEGMENT;

using namespace std;

class Calculator{
  private:
    ifstream ifile;
    ofstream ofile;
    char* starting_date;
    char* ending_date;
    int road_segment_count;
    float route_distance;
    float tti_value[6];
    float last_tti_value[6];
    ROAD_SEGMENT** road_segment_info;

    //Private function
    char* GetWord(char* dest,char* src, char delim);  //Extract word from string
    void InitRoadSegmentInfo();          //Initialize space to store information of the road segment
    void CreateOutputFile(int starting_year,int starting_month,int ending_year,int ending_month);
    void CalculateRouteTTI();
    void MonthIntervalCalculation(int current_year,int current_month,int number_of_days);
    int RetrieveRoadSegmentData(int segment_id,int year,int month,int day);
    void GetYearNMonth(int* year, int* month, char* date);  //Get year and month from given date.
    int GetTotalDaysInCurrentMonth(int year, int month);
    string GetRoadSegmentName(string road_segment_id);
    string GetCurrentDateEntry(int year, int month, int day);
    //float GetRoadSegmentLength(string road_segment_id);
    void CalculateDailyTTI();
    float GetFreeFlowTravelTime(int segment_id);
    float AltGetFreeFlowTravelTime(int segment_id);
    float GetPeakTravelTime(int segment_id);
    void AltGetPeakTravelTime(int segment_id, float* travel_time);
    float CalculateSharpness(int segment_id, int index);
    float CalculateDistance (int segment_id, int start, int end);


  public:
    //Constructors
    Calculator();    //Default Constructor  

    //Destructor
    ~Calculator();  //Destructs an object

    //Public Member Function
    void RunCalc(char* current_route);   //Main Function of Calculate TTI
};
#endif