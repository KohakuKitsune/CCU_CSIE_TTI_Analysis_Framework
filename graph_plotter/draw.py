import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def DrawGraph(df):
  index = list(df.columns)
  print(index)

  road_id = df.loc[0][0]
  xlabel = index[1]
  ylabel = index[2]

  print(road_id)
  print(xlabel)
  print(ylabel)

  df.plot(x = xlabel,y = ylabel)

  plt.title("TTI Data graph")
  plt.xlabel(xlabel)
  plt.ylabel(ylabel)
  plt.legend([road_id])
  plt.savefig('./GRAPH/testplot.png')
  plt.show()
  plt.close()

def DrawTrend(df):
  index = list(df.columns)
  print(index)

  road_id = df.loc[0][0]
  xlabel = index[1]
  ylabel = index[2]

  print(road_id)
  print(xlabel)
  print(ylabel)

  growth = np.round( df[ylabel].pct_change()*100,2)
  print(growth)