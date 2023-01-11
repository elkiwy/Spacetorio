#!/usr/bin/env python3
import math as m
from PIL import Image

Ro = 100.0
Ri = 50.0

#Init a 2D matrix of pixels
cir = [[0 for x in range(int(Ro * 2))] for y in range(int(Ro * 2))]



#Load Test image 512 x 32
image = Image.open('this-is-a-test.png')
pixels = image.load()
srcWidth, srcHeight = image.size
print("width:",srcWidth)
print("height:",srcHeight)

#Utility
def show_im(img, name):  # for showing data as image
    list_image = [item for sublist in img for item in sublist]
    new_image = Image.new("RGB", (len(img[0]), len(img)))
    new_image.putdata(list_image)
    new_image.save(name,"PNG")


def dstToSrc(dstX, dstY, Ro, Ri, srcHeight, srcHeight):
    angle = m.atan2(dstY-Ro,dstX-Ro)/2
    distance = m.sqrt((dstY-Ro)*(dstY-Ro) + (dstX-Ro)*(dstX-Ro))
    distance = m.floor((distance-Ri+1)*(srcHeight-1)/(Ro-Ri))
    return int(srcHeight*angle/m.pi) % srcHeight, srcHeight-distance-1


#Cycle all the pixels in the destination image
for i in range(int(Ro)): # 0-100
    outer_radius = m.sqrt(Ro*Ro - i*i)
    #show_im(cir, "result"+str(i)+"_"+str(int(outer_radius))+".png")
    for j in range(-int(outer_radius), int(outer_radius)):
        inner_radius = m.sqrt(Ri*Ri - i*i) if i < Ri else -1
        if j < -inner_radius or j > inner_radius:
            # Upper part of the image
            dstX = Ro+j
            dstY = Ro-i
            srcX, srcY = dstToSrc(dstX, dstY, Ro, Ri, srcWidth, srcHeight)
            cir[int(dstY)][int(dstX)] = pixels[srcX, srcY]

            # Bottom part of the image
            dstX = Ro+j
            dstY = Ro+i
            srcX, srcY = dstToSrc(dstX, dstY, Ro, Ri, srcWidth, srcHeight)
            cir[int(dstY)][int(dstX)] = pixels[srcX, srcY]




show_im(cir, "result.png")
