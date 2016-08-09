# -*- coding: utf-8 -*- #文件也为UTF-8
import os
import os.path
from PIL import Image, ImageDraw, ImageFont
rootdir='.'
for parent,dirnames,filenames in os.walk(rootdir):
    for dirname in  dirnames:                       #输出文件夹信息
        print  ("dirname is" + dirname)
    for filename in filenames:
        if "png" in filename:
            if "start" in filename:
                continue
            if "end" in filename:
                continue
            for i in range(0,2):
                img = Image.open(filename)
                draw = ImageDraw.Draw(img)
                
                label = 's'
                fname = 'start'
                if (i != 0):
                    label = 'e'
                    fname = 'end'
                if "red" in filename:
                    draw.text((3,-1), label, fill = 'green')
                else:
                    draw.text((3,-1), label, fill = 'red')
                newfilename=filename[:-4]
                img.save(newfilename + fname + '.png')
                i = i+1
    
