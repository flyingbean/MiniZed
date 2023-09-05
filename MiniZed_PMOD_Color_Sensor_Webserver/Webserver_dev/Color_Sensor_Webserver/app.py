# Importing required functions
from flask import Flask, render_template, request
from TCS34725 import TCS34725
import http.server
import socketserver
import logging
import cgi
import sys
import re
import subprocess
import time, struct
# Flask constructor
app = Flask(__name__)
 
sys.path.insert(1, '/usr/local/bin/gpio')
from gpio_common import gpio_map

PsButtonPortnumber  = gpio_map['PS_BUTTON'].gpio
PS_RedPortnumber    = gpio_map['PS_R'].gpio
PS_GreenPortnumber  = gpio_map['PS_G'].gpio
PL_RedPortnumber    = gpio_map['PL_R'].gpio
PL_GreenPortnumber  = gpio_map['PL_G'].gpio
EnableMicPortnumber = gpio_map['PL_MIC1'].gpio


Light=TCS34725(0X29, debug=False)
if(Light.TCS34725_init() == 1):
    print("TCS34725 initialization error!!")
else:
   print("TCS34725 initialization success!!")
       
# Root endpoint
@app.route('/', methods=['GET','POST'])

def homepage():
    
    # Define Plot Data
    labels = [
        'Color R',
        'Color G',
        'Color B',
        'Light Illumination',       
    ]
 
    
    
    
    if(Light.TCS34725_init() == 1):
         print("TCS34725 initialization error!!")
    else:
         print("TCS34725 initialization success!!")
      
    
    ButtonPath = '/sys/class/gpio/gpio' + PsButtonPortnumber + '/value'
    ButtonFile= open (ButtonPath,'r')
    ButtonStatus = ButtonFile.read()
    ButtonFile.close()
    logging.warning("Button is %s", ButtonStatus)
    
   
  
   
   #time.sleep(2) 
    ID=Light.Read_ID() 
    Light.Get_RGBData()   
    color_r = Light.R
    color_g = Light.G
    color_b = Light.B
   
   # RGB 888 format
    Light.GetRGB888()  
   #flyingbean
    reg_RGB888_R= Light.RGB888_R,
    rgb888_r= int(reg_RGB888_R[0]),
    reg_RGB888_G= Light.RGB888_G,
    rgb888_g= int(reg_RGB888_G[0]),
    reg_RGB888_B= Light.RGB888_B,
    rgb888_b= int(reg_RGB888_B[0]),
   
    color_rgb888_num= rgb888_r[0]<<16 | rgb888_g[0]<<8 | rgb888_b[0]
    color_rgb888_num= hex(color_rgb888_num)
   
    #Get light illuminance
    color_lux=Light.Get_Lux() 
    
    #Get color temperature
    color_temp=Light.Get_ColorTemp() 
    
    data = [rgb888_r, rgb888_g, rgb888_b, color_lux]
    
    print("RGB888B: %d "%rgb888_b),     
    templateData = {
      'title' : 'GPIO input Status!',
      'ID'  : hex(ID),
      'color_r'  : int(reg_RGB888_R[0]),
      'color_g'  : int(reg_RGB888_G[0]),
      'color_b'  : int(reg_RGB888_B[0]),
      'color_lux'  : int(color_lux),
      'color_temp'  : int(color_temp),
 
    }
    if request.method=='POST':
            
        # Return the components to the HTML template
        return render_template(
            template_name_or_list='graph.html',
            data=data,
            labels=labels,
            **templateData,
        )
    else:
        return render_template(
            template_name_or_list='graph.html',
            data=data,
            labels=labels,
            **templateData,
        )
    
    
 
# Main Driver Function
if __name__ == '__main__':
    # Run the application on the local development server ##
    app.run(host='0.0.0.0', port=8080, debug=True)
