#!/usr/bin/env python3
import math as m
from PIL import Image

Ro = 100.0
Ri = 50.0

#Init a 2D matrix of pixels
cir = [[0 for x in range(int(Ro * 2))] for y in range(int(Ro * 2))]



#Load Test image 512 x 32
#image = Image.open('this-is-a-test.png')
image = Image.open('ground.jpg')
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



def dstToSrc(dstX, dstY, Ro, Ri, srcWidth, srcHeight, angleRange):
    angle = m.atan2(dstY-Ro,dstX-Ro)
    if angle < angleRange[0] or angle > angleRange[1]: return None, None
    percAngle = (angle-angleRange[0]) / (angleRange[1]-angleRange[0])
    distance = m.sqrt((dstY-Ro)*(dstY-Ro) + (dstX-Ro)*(dstX-Ro))
    percHeight = (distance-Ri+1) / (Ro-Ri) # [0.0, 1.0]
    targetSrcHeight = m.floor(percHeight * (srcHeight-1))
    percWidth = angle / (m.pi*2) # [0.0, 1.0]
    #return int(srcWidth*percWidth) % srcWidth, srcHeight-targetSrcHeight-1
    return int(srcWidth*percAngle) % srcWidth, srcHeight-targetSrcHeight-1


arcAngle = 120
arcAngleRange = [m.radians(-(arcAngle/2)-90), m.radians((arcAngle/2)-90)]
#arcAngleRange = [m.radians(-180), m.radians(180)]

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
            srcX, srcY = dstToSrc(dstX, dstY, Ro, Ri, srcWidth, srcHeight, arcAngleRange)
            if srcX is not None: cir[int(dstY)][int(dstX)] = pixels[srcX, srcY]

            # Bottom part of the image
            dstX = Ro+j
            dstY = Ro+i
            srcX, srcY = dstToSrc(dstX, dstY, Ro, Ri, srcWidth, srcHeight, arcAngleRange)
            if srcX is not None: cir[int(dstY)][int(dstX)] = pixels[srcX, srcY]




show_im(cir, "result.png")
