from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import ElementNotInteractableException
import time
from parse import parse

# 讀參數檔並解析
word_list = parse()
if word_list == []:
    exit()
y1=word_list[0]
m1=word_list[1]
y2=word_list[2]
m2=word_list[3]

#Old settings for chrome driver (selenium_crawlerV4)
#options=Options()
#options.chrome_executable_path="./selenium_crawlerV4/chromedriver.exe"
#driver=webdriver.Chrome(options=options)
#driver.get("http://e-traffic.taichung.gov.tw/RoadGrid/Pages/VD/History2.html")

#Cutomize settings for chrome driver to set download directory
chromeOptions = webdriver.ChromeOptions()
prefs = {"download.default_directory" : "C:\\CSIE_Project\\Program\\CSV\\Open_Source_Data"}
chromeOptions.add_experimental_option("prefs", prefs)
chromedriver = "C:\\CSIE_Project\\Program\\selenium_crawlerV4\\chromedriver-win64\\chromedriver.exe"
driver = webdriver.Chrome(executable_path=chromedriver, options=chromeOptions)
driver.get("http://e-traffic.taichung.gov.tw/RoadGrid/Pages/VD/History2.html")

time.sleep(1)

for i in range(y2-y1+1):
    #同年
    if y1==y2:
        m2=word_list[3]
    #不同年
    elif y1!=y2:
        m2=12
    while m1<=m2:
        row=1
        col=1
        # 起始時間
        str_time_selection = driver.find_element(By.CSS_SELECTOR,"#starttime")
        str_time_selection.click()
        time.sleep(0.5)
        # 起始年
        str_year = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-year")
        str_year.click()
        time.sleep(0.5)
        # 選年
        year1_selection = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-year > option:nth-child(%d)" %(y1%20+1))
        year1_selection.click()
        time.sleep(0.5)
        # 起始月
        str_month = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-month")
        str_month.click()
        time.sleep(0.5)
        # 選月
        month1_selection = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-month > option:nth-child(%d)" %m1)
        month1_selection.click()
        time.sleep(0.5)
        # 選日
        # 直到找到開始日那個 column
        while True:
            try:
                day1_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > table > tbody > tr:nth-child(%d) > td:nth-child(%d) > a" %(row, col))
                day1_btn.click()
                time.sleep(1)
                break
            except NoSuchElementException:
                col+=1
                if col>7 :
                    row+=(col//7)
                    col%=7
                pass
        # 選小時 (設成00:00)
        hour1_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-timepicker-div > dl > dd.ui_tpicker_hour > div.ui_tpicker_hour_slider.ui-slider.ui-slider-horizontal.ui-widget.ui-widget-content.ui-corner-all > span")
        actions = ActionChains(driver)
        actions.click_and_hold(hour1_btn).move_by_offset(-200,0).release().perform()
        time.sleep(1)
        # 確認
        confirm_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-buttonpane.ui-widget-content > button.ui-datepicker-close.ui-state-default.ui-priority-primary.ui-corner-all")
        confirm_btn.click()
        time.sleep(0.5)
        # 結束時間
        end_time_selection = driver.find_element(By.CSS_SELECTOR,"#endtime")
        end_time_selection.click()
        time.sleep(1)
        # 終點年
        end_year = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-year")
        end_year.click()
        time.sleep(0.5)
        # 選年
        year2_selection = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-year > option:nth-child(%d)" %(y1%20+1))
        year2_selection.click()
        time.sleep(0.5)
        # 終點月
        end_month = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-month")
        end_month.click()
        time.sleep(0.5)
        # 選月
        month2_selection = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-header.ui-widget-header.ui-helper-clearfix.ui-corner-all > div > select.ui-datepicker-month > option:nth-child(%d)" %m1)
        month2_selection.click()
        time.sleep(0.5)
        # 選日
        if m1==2:
            if y1%4==0:
                if y1%100!=0 or y1%400==0:
                    row+=28//7
                    col+=28%7
            else:
                row+=27//7
                col+=27%7
        elif m1==1 or m1==3 or m1==5 or m1==7 or m1==8 or m1==10 or m1==12:
            row+=30//7
            col+=30%7
        elif m1==4 or m1==6 or m1==9 or m1==11:
            row+=29//7
            col+=29%7
        if col>7:
            row+=(col//7)
            col%=7
        try:
            day2_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > table > tbody > tr:nth-child(%d) > td:nth-child(%d) > a" %(row, col))
            day2_btn.click()
            time.sleep(1)
        except NoSuchElementException:
            pass
        # 選小時
        hour2_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-timepicker-div > dl > dd.ui_tpicker_hour > div.ui_tpicker_hour_slider.ui-slider.ui-slider-horizontal.ui-widget.ui-widget-content.ui-corner-all > span")
        actions = ActionChains(driver)
        actions.click_and_hold(hour2_btn).move_by_offset(200,0).release().perform()
        time.sleep(1)
        # 確認
        confirm_btn = driver.find_element(By.CSS_SELECTOR,"#ui-datepicker-div > div.ui-datepicker-buttonpane.ui-widget-content > button.ui-datepicker-close.ui-state-default.ui-priority-primary.ui-corner-all")
        confirm_btn.click()
        time.sleep(0.5)
        # 依次下載路段
        i=4
        length=len(word_list)
        while i<length: 
            # 按道路選單
            road_selection_btn=driver.find_element(By.ID,"device")
            road_selection_btn.click()
            time.sleep(0.5)
            # 選路
            option_btn = driver.find_element(By.CSS_SELECTOR,"#device > option:nth-child(%d)" %word_list[i])
            option_btn.click()
            time.sleep(0.5)
            # 送出
            send_btn = driver.find_element(By.CSS_SELECTOR,"body > div.container-fluid > div > div > div > div.panel-heading > div > button")
            send_btn.click()
            # 將此行的sleep函數調高，如sleep(10)應該會改善出錯率，單位是秒
            time.sleep(3)
            # 下載
            while True:
                try:
                    download_btn = driver.find_element(By.CSS_SELECTOR,"#chartdiv1 > div > div.amcharts-export-menu.amcharts-export-menu-top-right.amExportButton > ul > li > a")
                    download_btn.click()
                    break
                except (NoSuchElementException,ElementNotInteractableException):
                    time.sleep(3)
                    pass
            # 下載選項
            download_selection_btn = driver.find_element(By.CSS_SELECTOR,"#chartdiv1 > div > div.amcharts-export-menu.amcharts-export-menu-top-right.amExportButton > ul > li > ul > li:nth-child(2) > a > span")
            download_selection_btn.click()
            # 檔案類型
            csv_btn = driver.find_element(By.CSS_SELECTOR,"#chartdiv1 > div > div.amcharts-export-menu.amcharts-export-menu-top-right.amExportButton > ul > li > ul > li:nth-child(2) > ul > li:nth-child(1) > a")
            csv_btn.click()
            time.sleep(3)
            # 下個路段
            i+=1
        # 下一月
        m1+=1
    # 下一年
    m1=1
    y1+=1
# 關閉
driver.close()