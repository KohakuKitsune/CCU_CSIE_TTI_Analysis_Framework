import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

plt.rcParams['font.sans-serif'] = ['SimHei']
def line_cnt_before_blank(file_path): # count the line numbers before blank
    with open(file_path, 'r', encoding='utf-8') as f: # read all line into lines
        lines = f.readlines()

    line_cnt = 0
    for line in lines:
        if line.strip() == '' or line.strip() == ',': # if blank
            break
        line_cnt = line_cnt + 1
    return line_cnt + 2

dir_path = 'C:\\CSIE_Project\\Program\\CSV\\Time_Travel_Index_Data'
save_path = 'C:\\CSIE_Project\\Program\\GRAPH\\Type35'


# check directory
if not os.path.isdir(dir_path):
    print('Please input the corret directory name')
    exit()

# plot every route
for filename in os.listdir(dir_path):
    if filename.endswith(".csv"):
        file_path = os.path.join(dir_path, filename)
        df = pd.read_csv(file_path , skiprows=line_cnt_before_blank(file_path))
        # transfer Date column into standard time format
        df['Date'] = pd.to_datetime(df['Date'])
        route_name = filename[0:15]
        
        print(f'TTI Graph of {route_name} is plotting ...')
        # Group data by month then plot
        for name, group in df.groupby(df['Date'].dt.to_period('M')):
            plt.figure(figsize=(12, 6))
            plt.plot(group['Date'], group['TTI Value Type 3(AM)'], 'r', label="TTI Value Type 3(AM)\nPeak :sharpness\nFreeflow :當日最高平均時速")
            plt.plot(group['Date'], group['TTI Value Type 4(AM)'], 'm', label="TTI Value Type 4(AM)\nPeak :sharpness\nFreeflow :交通部規定之路段的速限(50km/h)")
            # set x axis scale
            plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
            plt.gcf().autofmt_xdate(rotation=90)
            plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=1))
            # set y axis scale
            plt.ylim(0,3)
            #set label
            plt.title(f'TTI value for {name.year}-{name.month}')
            plt.xlabel('Date')
            plt.ylabel('TTI Value')
            plt.legend(fontsize="14",loc="upper right")

            # save graph every month
            if not os.path.exists(os.path.join(save_path, route_name)):
                os.makedirs(os.path.join(save_path, route_name))
            plt.savefig(os.path.join(save_path, route_name, f'{route_name}_({name}).png'))
            # clear the graph for next one
            plt.close()



