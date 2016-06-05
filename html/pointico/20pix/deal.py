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
            if "num" in filename:
                continue
            for i in range(0,10):
                img = Image.open(filename)
                draw = ImageDraw.Draw(img)
                if "red" in filename:
                    draw.text((7,5), str(i), fill = 'green')
                else:
                    draw.text((7,5), str(i), fill = 'red')
                newfilename=filename[:-4]
                img.save(newfilename + "num" + str(i) + '.png')
                i = i+1
    
