import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

filename = 'V040660-V040660_(2022-1~2022-5).csv'
save_path = 'C:\\Users\\bsbro\\workspace\\project\\graph'

# read csv file
df = pd.read_csv(filename , skiprows=6)

# transfer Date column into standard time format
df['Date'] = pd.to_datetime(df['Date'])

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
    plt.savefig(f'{save_path}\\{name}.png')
    # clear the graph for next one
    plt.clf()
