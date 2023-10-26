#ifndef TTI_H
#define TTI_H

#include <fstream>
#include <vector>

#define INPUT_FILE_PATH "C:/CSIE_Project/Program/DataPrepAndCalculator/tti/input.txt"
#define PREPROCESSED_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Traffic_Volume_Data"
#define TIME_TRAVEL_INDEX_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Time_Travel_Index_Data" 

//Below is a data structure that holds the a hour's long vd data of a road segment.
typedef struct hour_road_vd_block {
    string road_segment_id;
    string filename;
    int average_speed;
    int traffic_volume;
} ROAD_SEGMENT;

//Below is a data structure that holds the a day's vd data of all road segments.
typedef struct route_information {
    char* starting_date;
    char* ending_date;  
    int road_segment_cnt;
    ROAD_SEGMENT** road_segment_info;
} ROUTE;

using namespace std;

class Calculator{
  private:
    ifstream ifile;
    ofstream ofile;
    string input;
    char* current_route;   
    char** routes;   //stores all input routes
    ROUTE route_info;
    int route_cnt;      //route count

    //Private function
    char* GetWord(char* dest,char* src, char delim);  //Extract word from string
    void InitRoute(int cnt);   //Initialize space to store route input
    void InitDChunk();         //Initialize space to store data for TTI calculation of the current route.
    void RetrieveData();       //Extract daily vd data of the route with given time interval.
    void GetYearNMonth(int* year, int* month, char* date);  //Get year and month from given date.
    void CalMonthTTI(int year, int month, int days);  //Calculation of TTI of the route in a given month.


  public:
    //Constructors
    Calculator();    //Default Constructor  

    //Destructor
    ~Calculator();  //Destructs an object

    //Public Member Function
    void RunCalc();   //Main Function of Calculate TTI
};
#endif