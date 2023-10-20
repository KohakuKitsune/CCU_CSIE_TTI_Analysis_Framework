#include <iostream>
#include <sstream>
#include <string>
#include "prep.h"

using namespace std;

// Constuctors
Preprocessor::Preprocessor() // Default Constructor
{
  idx = 0;
}

// Destructor
Preprocessor::~Preprocessor() // Destructs an object
{
  cout << endl;
  cout << "successfully destroyed" << endl;
}

void Preprocessor::RunPrep(string filename) // Main function of object Preprocewssor
{
  cout << "Input file: " << filename << endl;
  string fileloc(RAW_DATA);
  cout << "File Pathname: " << fileloc << "/" << filename << endl;

  ifile.open(fileloc + "/" + filename);

  // Begin Parsing the file
  string line;
  getline(ifile, line); // skip header
  while (getline(ifile, line))
  {
    istringstream templine;
    templine.str(line);
    string temp;
    string data[RAW_COlNUM];
    int cnt = 0, col = 0;
    // Due to file format , it can only be seperated normally into words using '"' as delimiter
    while (getline(templine, temp, '\"'))
    {
      // All even words parsed are just commas
      if (cnt % 2 != 0)
      {
        data[col] = temp;
        // cout << data[col] << endl;
        col++;
      }
      cnt++;
    }

    // Do data cleaning on each row
    DataClean(data);
  }
  ifile.close();

  // Output cleaned data into output file
  /*
  cout << "road_id,datetime,avgSpeed,trafficVolume" << endl;
  for (int i = 0; i < idx; i++)
  {
    cout << road_id.at(i) << "," << date.at(i) << ","
         << avg_speed.at(i) << "," << reg_vd.at(i) << "," << pcu_vd.at(i) << endl;
  }
  */
  Validate(filename);
  WriteData();

  //Clear temporary stored data in vector
  road_id.clear();
  date.clear();
  avg_speed.clear();
  reg_vd.clear();
  pcu_vd.clear();
  corrupt_val.clear();
  idx = 0;

}

void Preprocessor::DataClean(string *row_data)
{
  road_id.push_back(row_data[0]);
  date.push_back(row_data[1]);
  avg_speed.push_back(stoi(row_data[9]));
  reg_vd.push_back(stoi(row_data[11]));

  // Calculate Traffic Volume using pcu standard
  float temp_pcu = 0;
  temp_pcu = (stof(row_data[3]) * HEAVY_PCU) + (stof(row_data[5]) * NORM_PCU) + (stof(row_data[7]) * SMALL_PCU);
  pcu_vd.push_back(temp_pcu);

  corrupt_val.push_back(0);   //preset data entry corrupt value to 0

  idx++;
}

void Preprocessor::UpdateDay(int &day, int &hour, int entry_no)
{
  //Update day if the entries of all hours in the current day have been validated
  if(entry_no % 24 == 0) day++;

  //Reset time if the last entry of the day has been validated 
  if(hour > 23) hour = 0;
}

string Preprocessor::GetCurrentTime(int year, int month, int day, int time)
{
  string temp;
  if(month < 10) temp = string( to_string(year) + "/0"  + to_string(month) + "/");
  else  temp = string( to_string(year) + "/" + to_string(month) + "/");  

  if(day < 10) temp.append("0" + to_string(day));
  else temp.append(to_string(day));

  temp.append(" ");

  if(time < 10) temp.append("0" + to_string(time) + "00");
  else temp.append(to_string(time) + "00");

  return temp;

}

bool Preprocessor::CheckAbnormal(int idx)
{
  if(reg_vd.at(idx) == 0 && avg_speed.at(idx) == 0) return false;
  else if (reg_vd.at(idx) != 0 && avg_speed.at(idx) != 0) return false;
  else return true;
}

void Preprocessor::Validate(string filename)
{
  //Show the number of entries in the current file
  //cout << idx << endl;

  //Validate for any missing data (except zero value data)
  string fileloc(VALID_LOG);

  //Extract year month from date as components to determine validate range
  string s (date.at(0));
  string delimiter = "/";
  size_t pos = 0;
  int year ,month;

  pos = s.find(delimiter);
  year = stoi(s.substr(0, pos));

  s = s.substr(pos + delimiter.length());
  pos = s.find(delimiter);
  month = stoi(s.substr(0, pos));

  //Calculate validate range
  int max = 0;
  int days = 0;

  //Determine the suppose ammount of entries in the file
  if(month == 2)
  {
    if(year % 4 == 0)
    {
      days = 29;
      max = days * HOURS; //Leap Year February
    }
    else 
    {
      days = 28;
      max = days * HOURS;
    }
  }
  else if(month == 4 || month == 6 || month == 9 || month == 11)
  {
    days = 30;
    max = days * HOURS;
  }
  else
  {
    days = 31;
    max = days * HOURS;
  }

  mfile.open(fileloc + "/" + "validation_entry.txt", _S_app);

  mfile << "Validating file \""<<filename << "\" ....\n";
  mfile << max-idx << " missing entries found.\n";
  if(max-idx != 0) mfile << "\nMissing entries: \n------------------\n";

  //Search for missing entries and input with default values.
  int today = 0;
  int hour = 0;
  int x = 0;       //Here represents the suppose index of the current entry
  int err = 1;
  string tar_date;

  for(int i = 0; i < max; i++)
  {
    //Update day if the entries of all hours in the current day have been validated
    //if(i % 24 == 0) today +=1;

    //Reset time if the last entry of the day has been validated 
    //if(time > 23) time = 0;

    UpdateDay(today, hour, i);

    //Generate current target time and checkif the entry at that time exists
    if(x < idx)
    {
      tar_date = GetCurrentTime(year,month,today,hour);
      if (tar_date.compare(date.at(x)) == 0) x++;
      else
      { 
        mfile << err <<".Entry on " << tar_date << " missing." << endl;
        vector<string>::iterator word;
        vector<int>::iterator val;
        vector<float>::iterator f_val;

        word = road_id.begin();
        road_id.insert(word+x,road_id.at(x));

        word = date.begin();
        date.insert(word+x,tar_date);

        val = avg_speed.begin();
        avg_speed.insert(val+x,0);

        val = reg_vd.begin();
        reg_vd.insert(val+x,0);

        f_val = pcu_vd.begin();
        pcu_vd.insert(f_val+x,0);

        val = corrupt_val.begin();
        corrupt_val.insert(val+x,MISSING);

        x++;
        idx++;
        err++;
      }
    }
    else
    {
      mfile << err <<".Entry on " << tar_date << " missing." << endl;
      road_id.push_back(road_id.at(0));
      date.push_back(tar_date);
      avg_speed.push_back(0);
      reg_vd.push_back(0);
      pcu_vd.push_back(0);
      corrupt_val.push_back(MISSING);
      idx++;
    }

    hour += 1;
  }

  //Abnormaly Detection
  today = 0;
  hour = 0;
  err = 1;
  x = 0;                               //Here it represents the number of abnormal entries
  vector<string> abnorm_entry;
  vector<int> abnorm_idx;
  for(int i = 0; i < max; i++)
  {
    UpdateDay(today, hour, i);

    if (CheckAbnormal(i))   //if abnormal data is found
    {
      tar_date = GetCurrentTime(year,month,today,hour);    //Retrieve date of the abnormal data entry
      abnorm_entry.push_back(tar_date);
      abnorm_idx.push_back(i);
      corrupt_val[i] += ABNORMAL;
      //Show corrupt value of the data in terminal
      //cout << corrupt_val.at(i) << endl;
      x += 1;
    }

    hour += 1;
  }

  mfile << endl << x << " abnormal entries found.\n";
  if(x > 0)
  {
    mfile << "\nAbnormal entries: \n------------------\n";
    for(int i = 0; i < x; i++)
    {
      mfile << err <<".At entry" << abnorm_entry.at(i) 
      << ", average speed: " << avg_speed.at(abnorm_idx.at(i)) 
      <<" ; traffic volume:"<< reg_vd.at(abnorm_idx.at(i)) << endl;

      err++;
    }
  }

  abnorm_entry.clear();
  abnorm_idx.clear();


  //Multiple Zero Records Detection
  today = 0;
  hour = 0;
  err = 1;
  x = 0;                        //Here it represents the number of ALL zero entries
  bool flag = false;            //Determine if entered a series of all zero entries
  int series = 0;
  vector<string> start_date;
  vector<string> end_date;
  vector<int> length;
  string start_tmp, end_tmp;
  int begin = 0;
  int last = 0;

  for(int i = 0; i < max; i++)
  {
    UpdateDay(today, hour, i);
    //tar_date = GetCurrentTime(year,month,*today,*hour);
    if (avg_speed.at(i) == 0 && reg_vd.at(i) == 0 && pcu_vd.at(i) == 0)   // Detects if the entry is a zero record.
    {
      if(flag == false)
      {
        start_tmp = GetCurrentTime(year,month,today,hour);
        x += 1;
        flag = true;
        begin = i;
      }
      else
      {
        end_tmp = GetCurrentTime(year,month,today,hour);
        x += 1;
        last = i;
      }
    }
    else
    {
      flag = false;
      if(x > 1)
      {
        start_date.push_back(start_tmp);
        end_date.push_back(end_tmp);
        length.push_back(x);


        for(int j = begin; j <= last; j++)
        {
          corrupt_val[j] += ZERO_SERIES;
        }
        series += 1;
        x = 0;
        begin = 0;
        last = 0;
      }
    }

    hour += 1;
  }

  mfile << endl << series << " series of continous all zero entries found.\n";
  if(series > 0)
  {
    mfile << "\nSeries of continual  all zero entries: \n---------------------------------\n";
    for(int i = 0; i < series; i++)
    {
      mfile << err <<".Start entry:" << start_date.at(i) 
      << " ; End entry:" << end_date.at(i) 
      <<" ; Total number of all zero entries:"<< length.at(i) << endl;

      err++;
    }
  }

  start_date.clear();
  end_date.clear();
  length.clear();

  //Show valiadated data in terminal
  /*
  cout << idx << endl;
  for (int i = 0; i < idx; i++)
  {
    cout << road_id.at(i) << "," << date.at(i) << ","
         << avg_speed.at(i) << "," << reg_vd.at(i) << "," 
         << pcu_vd.at(i) << "," << corrupt_val.at(i) << endl;
  } 
  */

  mfile << "\nFile \""<<filename 
  << "\" validation completed\n\n----------------------------------------------------------------------\n";

  mfile << endl;
  mfile.close();
}

void Preprocessor::WriteData()
{
  //Extract year month from date as components for constructing a filename
  string s (date.at(0));
  string delimiter = "/";
  size_t pos = 0;
  string year ,month;

  pos = s.find(delimiter);
  year = s.substr(0, pos);
  s = s.substr(pos + delimiter.length());
  pos = s.find(delimiter);
  month = s.substr(0, pos);

  //Contruct a new filename then create the file with it as the filename.

  string filename(road_id.at(0) + "_" + year + "_" + month);
  cout << "Current file: " << filename << endl;
  string fileloc(VD_DATA);
  cout << "File Pathname: " << fileloc << "/" << filename << ".csv" << endl;

  //Write data into output file.

  ofile.open(fileloc + "/" + filename + ".csv");

  ofile << "Road ID,Date and Time,Average Speed (km\\h),Traffic Volume" << "\n";
  for(int i = 0; i < idx; ++i)
  {
    ofile << road_id.at(i) << "," << date.at(i) << "," << avg_speed.at(i) << "," 
          << reg_vd.at(i) << endl;
  }

  ofile.close();

}
