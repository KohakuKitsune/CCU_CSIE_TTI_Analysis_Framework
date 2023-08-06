#include <iostream>
#include <sstream>
#include <string>
#include "tti.h"

using namespace std;

CalculateTTI::RunCalc(string filename)
{
    /*=======File path checker=======*/
    cout << "Input file: " << filename << endl;
    string fileloc(VD_DATA);
    cout << "File Pathname: " << fileloc << "/" << filename << endl;

    ifile.open(fileloc + "/" + filename);

    /*=====Start Parsing the file=====*/
    int lineCnt = 0;            //Calculate the total line cnt of the csv file (used to calculate the dayCnt of the month)
    string line;
    getline(ifile, line);       //skip the description header line
    while(getline(ifile, line)){
        istringstream templine;
        templine.str(line);
        string temp;
        string data[HOURS_PER_DAY];
        int col = 0;

        /*The Cleaned Data uses "," to seperate the data*/
        while(getline(templine, temp, ',')){

            data[col] = temp;
            col++;
        }

        PushData(data);
        lineCnt++;
    }
    ifile.close();



    /*=====Identify the peak period(1/2/3/more hour)=====*/
    cout << "Enter the mode you want for peak period" << endl;
    cout << "1: ONE_HOUR\t2: TWO_HOUR\t3: THREE_HOUR\t4: MANY_HOUR" << endl;
    cin >> mode;

    //dayCnt = (lineCnt / 24) + 1;       //Calculate the number of day of that month
    dayCnt = 10;

    switch(mode){
        idx = 0;
        ttiCnt = 0;
        /*=======Find the peak of the day (1 hour)=======*/
        case ONE_HOUR:
            FindPeak_1();
            break;

        /*=======Find the peak of the day (2 hour)=======*/
        case TWO_HOUR:
            FindPeak_2();
            break;

        /*=======Find the peak of the day (3 hour)=======*/
        case THREE_HOUR:
            FindPeak_3();
            break;

        /*=======Find the peak of the day (more hour)=======*/
        case MANY_HOUR:
            //FindPeak(filename, MANY_HOUR);
            break;

        default:
            cout << "Invalid mode! Please insert the following number:" << endl;
            cout << "1: ONE_HOUR\t2: TWO_HOUR\t3: THREE_HOUR\t4: MANY_HOUR" << endl;
            cout << "Run again!" << endl;
            exit(1);
    }

    //Print out all TII values. (Remember add **date or other descriptions)
    for(int i = 0; i < ttiCnt; i++) cout << date.at(0 + 24*i) << "\t" <<tti.at(i) << endl;
    
    /*=======Write Data into a new file=======*/
    //WriteData();
}

CalculateTTI::FindPeak_1()
{
    int tmp_p = 0, tmp_f = 0;       //tmp_p is to store the index of peak hour || tmp_f is to store the index of freeflow hour
    int peak = TV.at(0);            //initial as the first traffic volume
    int freeflow = TV.at(0);        //initial as the first traffic volume

    cout << "Mode 1 initiated" << endl;
    
    for(int j = 0; j < dayCnt; j++){
        /*=====Find max traffic volume & it's average speed=====*/
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i)){
                peak = TV.at(i);
                tmp_p = i;
                //cout << "tmp_p: " <<tmp_p << endl;
            }

        }

        /*=====Find min traffic volume & it's average speed=====*/
        freeflow = peak;
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(freeflow > TV.at(i) && avg_speed.at(i) != 0){
                freeflow = TV.at(i);
                tmp_f = i;
                //cout << "tmp_f: " <<tmp_f << endl;
            }
        }

        FindTTI(avg_speed.at(tmp_p), tmp_f);
        idx = idx + HOURS_PER_DAY;      //Adjust the pointer to the next day 
    }
}

CalculateTTI::FindPeak_2()
{
    int tmp_p1 = 0, tmp_p2 = 0, tmp_f = 0;             
    int peak = TV.at(0);
    int freeflow = 1;        //not TV.at(0) because it might counter max/min = max/0 MATH ERROR
    float average_speed;

    for(int j = 0; j < dayCnt; j++){

        /*=====Find the 1st big traffic volume (TV)=====*/
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i)){
                peak = TV.at(i);
                tmp_p1 = i;
            }
        }

        freeflow = peak;
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(freeflow > TV.at(i) && avg_speed.at(i) != 0){
                freeflow = TV.at(i);
                tmp_f = i;
                cout << "tmp_f: " <<tmp_f << endl;
            }
        }

        /*=====Find the 2nd big traffic volume (TV)=====*/
        peak = TV.at(0);        //refreshes the peak to find the second big traffiv volume (TV)
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i) && peak != TV.at(tmp_p1)){
                peak = TV.at(i);
                tmp_p2 = i;
            }
        }

        average_speed = (avg_speed.at(tmp_p1) + avg_speed.at(tmp_p2)) / TWO_HOUR;
        //cout << "check check1" << endl;
        FindTTI(average_speed, tmp_f);
        //cout << "check check2" << endl;
        idx = idx + HOURS_PER_DAY;      //current row
    }
}

CalculateTTI::FindPeak_3()
{
    int tmp_p1 = 0, tmp_p2 = 0, tmp_p3 = 0, tmp_f = 0;             
    int peak = TV.at(0);
    int freeflow = 1;        //not TV.at(0) because it might counter max/min = max/0 MATH ERROR
    float average_speed;

    for(int j = 0; j < dayCnt; j++){
        /*=====Find the 1st big traffic volume (TV)=====*/
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i)){
                peak = TV.at(i);
                tmp_p1 = i;
            }
        }

        freeflow = peak;
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(freeflow > TV.at(i) && avg_speed.at(i) != 0){
                freeflow = TV.at(i);
                tmp_f = i;
                cout << "tmp_f: " <<tmp_f << endl;
            }
        }

        /*=====Find the 2nd big traffic volume (TV)=====*/
        peak = TV.at(0);        //refreshes the peak to find the second big traffiv volume (TV)
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i) && peak != TV.at(tmp_p1)){
                peak = TV.at(i);
                tmp_p2 = i;
            }
        }

        /*=====Find the 3rd big traffic volume (TV)=====*/
        peak = TV.at(0);        //refreshes the peak to find the second big traffiv volume (TV)
        for(int i = idx; i < HOURS_PER_DAY + idx; i++){
            if(peak < TV.at(i) && peak != TV.at(tmp_p1) && peak != TV.at(tmp_p2)){
                peak = TV.at(i);
                tmp_p3 = i;
            }
        }

        average_speed = (avg_speed.at(tmp_p1) + avg_speed.at(tmp_p2) + avg_speed.at(tmp_p3)) / THREE_HOUR;

        FindTTI(average_speed, tmp_f);
        idx = idx + HOURS_PER_DAY;      //current row
    }
}

CalculateTTI::FindTTI(float average_speed, int tmp_f)
{
    tti.push_back((avg_speed.at(tmp_f)/average_speed));         //TTI = FreeFlow speed / Current average speed
    ttiCnt++;
}

CalculateTTI::WriteData()
{
    /*Create a file and write the calculated TTI into it*/
}

CalculateTTI::PushData(string *data)
{
    road_id.push_back(data[0]);
    date.push_back(data[1]);
    avg_speed.push_back(stof(data[2]));
    TV.push_back(stoi(data[3]));
}