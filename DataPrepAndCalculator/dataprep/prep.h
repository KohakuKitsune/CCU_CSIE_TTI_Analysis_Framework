#ifndef PREP_H
#define PREP_H

#include <fstream>
#include <vector>

#define RAW_DATA "C:/CSIE_Project/Program/CSV/Raw_data"
#define VD_DATA "C:/CSIE_Project/Program/CSV/VD_data"
#define VALID_LOG "C:/CSIE_Project/Program/CSV/Validate_Log"

#define RAW_COlNUM 12

#define SMALL_PCU 0.5
#define NORM_PCU 1
#define HEAVY_PCU 1.5

#define HOURS 24

#define MISSING 1
#define ABNORMAL 2
#define ZERO_SERIES 4

using namespace std;

//Preprocessor Class
class Preprocessor {
  private:
    ifstream ifile;
    ofstream ofile;
    ofstream mfile;   //file to record any missing data 
    vector<string> road_id;
    vector<string> date;
    vector<int> avg_speed;
    vector<int> reg_vd;
    vector<float> pcu_vd;
    vector<int> corrupt_val;
    int idx;

    //Private Member Function

    void DataClean(string *row_data);  //Clean unwanted data and temporary save required data

    void UpdateDay(int &day, int &hour, int entry_no);
    string GetCurrentTime(int year, int month, int day, int time);
    bool CheckAbnormal(int idx);
    void Validate(string filename);    //Validate wholeness of daily data

    void WriteData();                  //Output Processed Data into File

    
  public: // public static class constants, for direction indicators
    //Constructors
    Preprocessor();    //Default Constructor                                 
   
    //Destructor
    ~Preprocessor();  //Destructs an object

    //Public Member Function
    void RunPrep(string filename); //Begins Data Preprocessing (main func of object Preprocewssor)
    //void Display();

};
#endif