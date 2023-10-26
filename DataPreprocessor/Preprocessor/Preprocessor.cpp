#include <iostream>
#include <sstream>
#include <string>
#include "prep.h"

using namespace std;

// Constuctors
Preprocessor::Preprocessor() // Default Constructor
{
  processed_entry_count = 0;
}

// Destructor
Preprocessor::~Preprocessor() // Destructs an object
{
  cout << endl;
  cout << "An object of Preprocessor class successfully destroyed" << endl;
}

void Preprocessor::Preprocess(string file_name) // Main function of object Preprocewssor
{
  cout << "Input file: " << file_name << endl;
  string directory_path(OPEN_SOURCE_DATA_DIRECTORY_PATH);
  cout << "File Pathname: " << directory_path << "/" << file_name << endl;

  ifile.open(directory_path + "/" + file_name);

  // Begin Parsing the file
  string line;
  string current_row_entry[OPEN_SOURCE_DATA_COLUMN_COUNT];
  getline(ifile, line); // skip header
  while (getline(ifile, line))
  {
    istringstream entry_copy;
    entry_copy.str(line);
    string column_entry;
    int parse_index = 0, column_index = 0;

    // Due to file format , it can only be seperated normally into words using '"' as delimiter
    while (getline(entry_copy, column_entry, '\"'))
    {
      // All even words parsed are just commas
      if (parse_index % 2 != 0)
      {
        current_row_entry[column_index] = column_entry;
        column_index++;
      }
      parse_index++;
    }

    ExtractDataRequired(current_row_entry);
  }
  road_id.assign(current_row_entry[0]);
  ifile.close();

  ValidateData(file_name);
  WriteData();

  //Clear temporary stored data in vector
  date.clear();
  average_speed.clear();
  traffic_volume.clear();
  data_corrupt_value.clear();
  processed_entry_count = 0;

}

void Preprocessor::ExtractDataRequired(string *data_row_entry)
{
  road_id.assign(data_row_entry[0]);
  date.push_back(data_row_entry[1]);
  average_speed.push_back(stoi(data_row_entry[9]));
  traffic_volume.push_back(stoi(data_row_entry[11]));
  data_corrupt_value.push_back(0);  
  processed_entry_count++;
}

void Preprocessor::UpdateDay(int &day, int &hour, int entry_no)
{
  if(entry_no % NUMBER_OF_HOURS_IN_A_DAY == 0)
  {
    hour = 0;
    day++;

  } 

  //Reset time if the last entry of the day (entry at 23pm) has been validated 
  if(hour > 23) hour = 0;
}

string Preprocessor::GetCurrentDateEntry(int year, int month, int day, int time)
{
  string current_date_entry;
  if(month < 10) current_date_entry = string( to_string(year) + "/0"  + to_string(month) + "/");
  else  current_date_entry = string( to_string(year) + "/" + to_string(month) + "/");  

  if(day < 10) current_date_entry.append("0" + to_string(day));
  else current_date_entry.append(to_string(day));

  current_date_entry.append(" ");

  if(time < 10) current_date_entry.append("0" + to_string(time) + "00");
  else current_date_entry.append(to_string(time) + "00");

  return current_date_entry;

}

bool Preprocessor::IsAbnormalEntry(int index)
{
  if(traffic_volume.at(index) == 0 && average_speed.at(index) == 0) return false;
  else if (traffic_volume.at(index) != 0 && average_speed.at(index) != 0) return false;
  else return true;
}

void Preprocessor::ValidateData(string open_data_file_name)
{
  //Show the number of entries in the current file
  //cout << idx << endl;


  string validation_log_directory_path(VALIDATION_LOG_PATHNAME);

  //Extract year month from date as components to determine validate range
  string starting_date (date.at(0));
  string delimiter = "/";
  size_t end_of_word = 0;
  int year ,month;

  end_of_word = starting_date.find(delimiter);
  year = stoi(starting_date.substr(0, end_of_word));

  starting_date = starting_date.substr(end_of_word + delimiter.length());
  end_of_word = starting_date.find(delimiter);
  month = stoi(starting_date.substr(0, end_of_word));

  //Calculate validate range
  int expected_number_of_entries = 0;
  int number_of_days = 0;

  //Determine the suppose ammount of entries in the file
  if(month == 2)
  {
    if(year % 4 == 0)
    {
      number_of_days = 29;
      expected_number_of_entries = number_of_days * NUMBER_OF_HOURS_IN_A_DAY; //Leap Year February
    }
    else 
    {
      number_of_days = 28;
      expected_number_of_entries = number_of_days * NUMBER_OF_HOURS_IN_A_DAY;
    }
  }
  else if(month == 4 || month == 6 || month == 9 || month == 11)
  {
    number_of_days = 30;
    expected_number_of_entries = number_of_days * NUMBER_OF_HOURS_IN_A_DAY;
  }
  else
  {
    number_of_days = 31;
    expected_number_of_entries = number_of_days * NUMBER_OF_HOURS_IN_A_DAY;
  }

  vfile.open(validation_log_directory_path + "/" + "validation_entry.txt", _S_app);
  //Validate for any missing data (except zero value data)
  vfile << "Validating file \""<< open_data_file_name << "\" ....\n";
  vfile << expected_number_of_entries - processed_entry_count << " missing entries found.\n";
  if(processed_entry_count < expected_number_of_entries) vfile << "\nMissing entries: \n------------------\n";

  //Search for missing entries and input with default values.
  int day = 0;
  int hour = 0;
  int expected_index_of_current_entry = 0;       
  int error_count = 1;
  string tar_date;

  for(int i = 0; i < expected_number_of_entries; i++)
  {

    UpdateDay(day, hour, i);
  
    //Generate current target time and checkif the entry at that time exists
    if(expected_index_of_current_entry < processed_entry_count)
    {
      tar_date = GetCurrentDateEntry(year,month,day,hour);
      if (tar_date.compare(date.at(expected_index_of_current_entry)) == 0)
      {
        expected_index_of_current_entry++;
      }
      else
      { 
        vfile << error_count <<".Entry on " << tar_date << " missing." << endl;
        vector<string>::iterator word;
        vector<int>::iterator val;

        word = date.begin();
        date.insert(word+expected_index_of_current_entry,tar_date);

        val = average_speed.begin();
        average_speed.insert(val+expected_index_of_current_entry,0);

        val = traffic_volume.begin();
        traffic_volume.insert(val+expected_index_of_current_entry,0);

        val = data_corrupt_value.begin();
        data_corrupt_value.insert(val+expected_index_of_current_entry,MISSING);

        expected_index_of_current_entry++;
        processed_entry_count++;
        error_count++;
      }
    }
    else
    {
      vfile << error_count <<".Entry on " << tar_date << " missing." << endl;
      road_id.push_back(road_id.at(0));
      date.push_back(tar_date);
      average_speed.push_back(0);
      traffic_volume.push_back(0);
      data_corrupt_value.push_back(MISSING);
      processed_entry_count++;
    }

    hour += 1;
  }

  //Abnormaly Detection
  day = 0;
  hour = 0;
  error_count = 1;
  int number_of_abnormal_entries = 0;                          
  vector<string> abnorm_entry;
  vector<int> abnorm_idx;
  for(int i = 0; i < expected_number_of_entries; i++)
  {
    UpdateDay(day, hour, i);

    if (IsAbnormalEntry(i))   //if abnormal data is found
    {
      tar_date = GetCurrentDateEntry(year,month,day,hour);    //Retrieve date of the abnormal data entry
      abnorm_entry.push_back(tar_date);
      abnorm_idx.push_back(i);
      data_corrupt_value[i] += ABNORMAL;
      //Show corrupt value of the data in terminal
      //cout << corrupt_val.at(i) << endl;
      number_of_abnormal_entries += 1;
    }

    hour += 1;
  }

  vfile << endl << number_of_abnormal_entries << " abnormal entries found.\n";
  if(number_of_abnormal_entries > 0)
  {
    vfile << "\nAbnormal entries: \n------------------\n";
    for(int i = 0; i < number_of_abnormal_entries; i++)
    {
      vfile << error_count <<".At entry" << abnorm_entry.at(i) 
      << ", average speed: " << average_speed.at(abnorm_idx.at(i)) 
      <<" ; traffic volume:"<< traffic_volume.at(abnorm_idx.at(i)) << endl;

      error_count++;
    }
  }

  abnorm_entry.clear();
  abnorm_idx.clear();


  //Multiple Zero Records Detection
  day = 0;
  hour = 0;
  error_count = 1;
  int zero_entry_count = 0;                        
  bool in_zero_entry_series = false; 
  int number_of_zero_entry_series = 0;
  vector<string> start_date;
  vector<string> end_date;
  vector<int> length;
  string start_tmp, end_tmp;
  int begin = 0;
  int last = 0;

  for(int i = 0; i < expected_number_of_entries; i++)
  {
    UpdateDay(day, hour, i);
    //tar_date = GetCurrentTime(year,month,*today,*hour);
    if (average_speed.at(i) == 0 && traffic_volume.at(i) == 0)   // Detects if the entry is a zero record.
    {
      if(in_zero_entry_series == false)
      {
        start_tmp = GetCurrentDateEntry(year,month,day,hour);
        zero_entry_count += 1;
        in_zero_entry_series = true;
        begin = i;
      }
      else
      {
        end_tmp = GetCurrentDateEntry(year,month,day,hour);
        zero_entry_count += 1;
        last = i;
      }
    }
    else
    {
      in_zero_entry_series = false;
      if(zero_entry_count > 1)
      {
        start_date.push_back(start_tmp);
        end_date.push_back(end_tmp);
        length.push_back(zero_entry_count);


        for(int j = begin; j <= last; j++)
        {
          data_corrupt_value[j] += ZERO_SERIES;
        }
        number_of_zero_entry_series += 1;
        zero_entry_count = 0;
        begin = 0;
        last = 0;
      }
    }

    hour += 1;
  }

  vfile << endl << number_of_zero_entry_series << " series of continous all zero entries found.\n";
  if(number_of_zero_entry_series > 0)
  {
    vfile << "\nSeries of continual  all zero entries: \n---------------------------------\n";
    for(int i = 0; i < number_of_zero_entry_series; i++)
    {
      vfile << error_count <<".Start entry:" << start_date.at(i) 
      << " ; End entry:" << end_date.at(i) 
      <<" ; Total number of all zero entries:"<< length.at(i) << endl;

      error_count++;
    }
  }

  start_date.clear();
  end_date.clear();
  length.clear();

  vfile << "\nFile \""<< open_data_file_name 
  << "\" validation completed\n\n----------------------------------------------------------------------\n";

  vfile << endl;
  vfile.close();
}

void Preprocessor::WriteData()
{
  ifstream info_file;
  info_file.open(ROAD_SEGMENT_INFORMATION_FILE_PATH);

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
    if(road_id.compare(column_entry) == 0)
    {
      getline(entry_copy, column_entry, ',');
      road_segment_name.assign(column_entry);
    }
  }
  info_file.close();


  string date_for_current_file (date.at(0));
  string delimiter = "/";
  size_t pos = 0;
  string year ,month;

  pos = date_for_current_file.find(delimiter);
  year = date_for_current_file.substr(0, pos);
  date_for_current_file = date_for_current_file.substr(pos + delimiter.length());
  pos = date_for_current_file.find(delimiter);
  month = date_for_current_file.substr(0, pos);
  
  string output_file_name(road_id + "_" + year + "_" + month);
  cout << "Current file: " << output_file_name << endl;
  string output_file_directory_path(PREPROCESSED_DATA_DIRECTORY_PATH);
  cout << "File Pathname: " << output_file_directory_path << "/" << output_file_name << ".csv" << endl;


  ofile.open(output_file_directory_path + "/" + output_file_name + ".csv");

  ofile << "Road Segment ID" << "," << road_id << ","  << "Road Segment Name" << "," << road_segment_name <<"\n";

  ofile << "Date and Time,Average Speed (km\\h),Traffic Volume" << "\n";
  for(int i = 0; i < processed_entry_count; ++i)
  {
    ofile << date.at(i) << "," << average_speed.at(i) << "," << traffic_volume.at(i) <<endl;
  }

  ofile.close();

}
