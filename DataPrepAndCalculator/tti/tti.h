#ifndef TTI_H
#define TTI_H

#include <fstream>
#include <vector>

#define VD_DATA "C:/Users/user/OneDrive/Desktop/CSIE_Project/Program/CSV/VD_data"
#define TTI_DATA "C:/Users/user/OneDrive/Desktop/CSIE_Project/Program/CSV/TTI" 

#define ONE_HOUR        1
#define TWO_HOUR        2
#define THREE_HOUR      3
#define MANY_HOUR       4

#define HOURS_PER_DAY 24
#define DAY_PER_MONTH 30
#define MORNING     12
#define EVENING     24

using namespace std;



class CalculateTTI {
    private:
        ifstream ifile;             //input file
        ofstream ofile;             //output file
        vector<string> road_id;     //Road ID
        vector<string> date;        //Date of the Day
        vector<float> avg_speed;    //Average Speed of the current time(h)
        vector<int> TV;             //Traffic Volume of the current time(h)
        vector<float> tti;          //Store the tti of the hour
        int idx;                    //Store current time(h). Ex: idx = 1 means the first hour of the day       
        int mode;                   //Define the peak period (1/2/3/more hour)
        int dayCnt;                 //Store the total day of the current month
        int ttiCnt;                 //Store the calculated tti count

        //Private function
        FindPeak_1();                               //Find the peak 1 hour
        FindPeak_2();                               //Find the peak 2 hour
        FindPeak_3();                               //Find the peak 3 hour
        FindTTI(float speed, int tmp_f);            //Calculate TTI
        WriteData();                                //Write Data into new file
        PushData(string *data);

    public:

        RunCalc(string filename);   //Main Function of Calculate TTI
};
#endif