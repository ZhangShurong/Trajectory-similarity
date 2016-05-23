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
            #if "re" in filename:
             #   continue
            img = Image.open(filename)
            print(img.size)
            img = img.resize((15,15),Image.ANTIALIAS)
            print(img.size)
            newfilename=filename[:-4]
            img.save(newfilename + "re" + '.png')
