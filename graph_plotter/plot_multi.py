import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates


dir_path = 'C:\\Users\\bsbro\\workspace\\project\\test'
save_path = 'C:\\Users\\bsbro\\workspace\\project\\graph'

# check directory
if not os.path.isdir(dir_path):
    print('Please input the corret directory name')
    exit()

# plot every route
for filename in os.listdir(dir_path):
    if filename.endswith(".csv"):
        file_path = os.path.join(dir_path, filename)
        df = pd.read_csv(file_path , skiprows=6)
        # transfer Date column into standard time format
        df['Date'] = pd.to_datetime(df['Date'])
        route_name = filename[0:15]

        # Group data by month then plot
        for name, group in df.groupby(df['Date'].dt.to_period('M')):
            plt.figure(figsize=(12, 6))
            plt.plot(group['Date'], group['TTI Value'])
            # set x axis scale
            plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%m/%d'))
            plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=7))
            # set y axis scale
            plt.ylim(0,3)
            #set label
            plt.title(f'TTI value for {name.year}-{name.month}')
            plt.xlabel('Date')
            plt.ylabel('TTI Value')

            # save graph every month
            if not os.path.exists(os.path.join(save_path, route_name)):
                os.makedirs(os.path.join(save_path, route_name))
            plt.savefig(os.path.join(save_path, route_name, f'{route_name}_({name}).png'))
            # clear the graph for next one
            plt.clf()



