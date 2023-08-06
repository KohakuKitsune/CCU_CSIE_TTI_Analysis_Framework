from datetime import datetime, timedelta

earliest_year=2020

def parse(input_file_path='./selenium_crawlerV4/input.txt', road_number_file_path='./selenium_crawlerV4/road.txt'):
    # 開參數檔案
    try:
        with open(input_file_path, 'r') as file:
            data = file.read()
            words = data.split()
    except FileNotFoundError:
        print(f'file open error: {input_file_path} not found')
        return []
    # 檢查時間參數
    try:
        start_date_str = words[0]
        end_date_str = words[1]
        start_date = datetime.strptime(start_date_str, '%Y-%m').date()
        end_date = datetime.strptime(end_date_str, '%Y-%m').date()
        current_date = datetime.now().date()
        print(start_date,end_date,current_date)
        # 檢查年分早於2020
        if start_date.year < earliest_year or end_date.year < earliest_year:
            print(f'time parameter error: can not be earlier than {earliest_year}')
            return []
        # 檢查年分晚於目前
        if start_date.year > current_date.year or end_date.year > current_date.year:
            print(f'time parameter error: can not be later than {current_date.year}')
            return []
        # 檢查月份最新僅能上個月
        if start_date.year == current_date.year or end_date.year == current_date.year:
            if start_date.month >= current_date.month or end_date.month >= current_date.month:
                print(f'time parameter error: the latest time can be is last month included')
                return []
        # 檢查先後
        if start_date > end_date:
            print('time parameter error: start date must be earlier than end date')
            return []
    except ValueError:
        print('time parameter error: time input format is incorrect')
        return []
    # 開合法路段檔案並做成集合
    try:
        with open(road_number_file_path, 'r') as file:
            road_index=file.read().split()
    except FileNotFoundError:
        print(f'file open error: {road_number_file_path} not found')
        return []
    # 將所有資料處理並存到 result
    i=0
    road_cnt=0
    result = list()
    for word in words:
        word_str=str(word)
        if i<2:
            # 將時間切分並轉換為整數
            result.extend([int(word_str[2:4]), int(word_str[5:7])])
        else:
            # 檢查路段合法
            if word not in road_index:
                print(f'road number error: {word} is not a valid road number')
            else:
                # 將道路編號轉換成我們需要的index
                result.append(road_index.index(word)+2)
                #result.append(word)
                road_cnt+=1
        i+=1
    if road_cnt == 0:
        print('road cnt error: no valid road number input')
        return []
    else: return result