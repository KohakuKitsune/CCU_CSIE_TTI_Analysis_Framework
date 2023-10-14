#ifndef TTI_H
#define TTI_H

#include <fstream>
#include <vector>

#define INPUT "C:/CSIE_Project/Program/DataPrepAndCalculator/tti/input.txt"
#define VD_DATA "C:/CSIE_Project/Program/CSV/VD_data"
#define TTI_DATA "C:/CSIE_Project/Program/CSV/TTI" 

//Below is a data structure that holds the a hour's long vd data of a road segment.
typedef struct hour_road_vd_block {
    char* road_id;
    int avg_speed;
    int reg_vd;
    float pcu_vd;   
} dblock;

//Below is a data structure that holds the a day's vd data of all road segments.
typedef struct day_time_interval_route_vd_datablock {
    char** segments; //stores all road segments in the current route.
    char** filename; //storess the current vd_file of each road segment.
    long* fpost;    //stores the last position read in each vd file of each road segment
    char* start;     //starting date
    char* end;       //ending date
    int seg_cnt;
    dblock** d_entry;
} dchunk;

using namespace std;

class CalculateTTI {
  private:
    ifstream ifile;
    ofstream ofile;
    string input;
    char* curr_route;   
    char** route;   //stores all input routes
    dchunk vd_daily;
    int r_cnt;      //route count

    //Private function
    char* GetWord(char* dest,char* src, char delim);  //Extract word from string
    void InitRoute(int cnt);   //Initialize space to store route input
    void InitDChunk();         //Initialize space to store data for TTI calculation of the current route.
    void RetrieveData();       //Extract daily vd data of the route with given time interval.
    void GetYearNMonth(int* year, int* month, char* date);  //Get year and month from given date.
    void CalMonthTTI(int year, int month, int days);  //Calculation of TTI of the route in a given month.


  public:
    //Constructors
    CalculateTTI();    //Default Constructor  

    //Destructor
    ~CalculateTTI();  //Destructs an object

    //Public Member Function
    void RunCalc();   //Main Function of Calculate TTI
};
#endif