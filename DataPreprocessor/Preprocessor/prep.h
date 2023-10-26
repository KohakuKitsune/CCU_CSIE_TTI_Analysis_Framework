#ifndef PREP_H
#define PREP_H

#include <fstream>
#include <vector>

#define OPEN_SOURCE_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Open_Source_Data"
#define PREPROCESSED_DATA_DIRECTORY_PATH "C:/CSIE_Project/Program/CSV/Traffic_Volume_Data"
#define VALIDATION_LOG_PATHNAME "C:/CSIE_Project/Program/CSV/Validatation_Log"
#define ROAD_SEGMENT_INFORMATION_FILE_PATH "C:/CSIE_Project/Program/CSV/Road_Segment_Information/Road_Segement_Name.csv"

#define OPEN_SOURCE_DATA_COLUMN_COUNT 12
#define NUMBER_OF_HOURS_IN_A_DAY 24


#define MISSING 1
#define ABNORMAL 2
#define ZERO_SERIES 4

using namespace std;

//Preprocessor Class
class Preprocessor {
  private:
    ifstream ifile;
    ofstream ofile;
    ofstream vfile;   //validation file
    string road_id;
    vector<string> date;
    vector<int> average_speed;
    vector<int> traffic_volume;
    vector<int> data_corrupt_value;
    int processed_entry_count;

    //Private Member Function
    void ExtractDataRequired(string *data_row_entry);  //Clean unwanted data and temporary save required data
    void UpdateDay(int &day, int &hour, int entry_no);
    string GetCurrentDateEntry(int year, int month, int day, int time);
    bool IsAbnormalEntry(int index);
    void ValidateData(string open_data_file_name);    //Validate wholeness of daily data
    void WriteData();                  //Output Processed Data into File

    
  public: // public static class constants, for direction indicators
    //Constructors
    Preprocessor();    //Default Constructor                                 
   
    //Destructor
    ~Preprocessor();  //Destructs an object

    //Public Member Function
    void Preprocess(string file_name); //Begins Data Preprocessing (main func of object Preprocewssor)
    //void Display();

};
#endif