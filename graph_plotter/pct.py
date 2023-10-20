import pandas as pd
import matplotlib.pyplot as plt

def plot_percentage_change(data):
    # use pct_change to calculate the percentage change (relative to previous one)
    data['Percentage_Change'] = data['TTI'].pct_change()

    plt.figure(figsize=(10, 6))
    plt.plot(data['datetime'], data['Percentage_Change'], marker='o', linestyle='-')
    plt.xlabel('Date')
    plt.ylabel('Percentage Change')
    plt.title('TTI Percentage Change')
    plt.grid()
    plt.show()

if __name__ == "__main__":
    filename = 'TTI_test_data.csv'
    data = pd.read_csv(filename)

    # convert date time column into Pandas datetime format (year/month)
    data['datetime'] = pd.to_datetime(data['datetime'], format='%Y/%m')

    plot_percentage_change(data)
