#!/usr/bin/python3
import json
import os
import sys
#import numpy as np
import re
import codecs
import argparse


filepath = "data_Name_Japanese_Male.txt"
outputFile = "result_Name_Japanese_Male.txt"

probFirstLetter={}
probLetter={}


res = []
lineCount=0
with codecs.open(filepath, "r", "UTF-8") as lines:
    for l in  lines:
        lineCount+=1
        # Split on white space or open parenthesis and keep the first string
        l2 = re.split("[ ,\(]",l)[0]
        l2 = l2+"\n"



        firstLetter = l2[:1]
        if l2[:1] not in probFirstLetter:
            probFirstLetter[firstLetter]=0
        probFirstLetter[firstLetter]+=1
        firstTuppe='0'
        key=""
        for c in range(len(l2)-2):
            if l2[c:c+1] == '\n':
                 key=firstTuppe+'0'
            else:
                key=firstTuppe+l2[c:c+1]
            key=key.lower()
            firstTuppe=l2[c:c+1]
           
            if key not in probLetter:
                probLetter[key]={}

            
            
            dico = probLetter[key]
            char =""
            if c+2>=len(l2)-1:
                char = '0'
            else:
                char = l2[c+1:c+2]


            
            if char not in dico:
                dico[char] = 0
            dico[char]+=1


















#for l in  probLetter.keys():
#    print(l+":"+str(probLetter[l]))

for l in  probFirstLetter.keys():
    probFirstLetter[l]=float(float(probFirstLetter[l])/float(lineCount))



Result = {"Description": "data_Name_Japanese_Male", "firstLetter":probFirstLetter , "data":probLetter}
 
print(json.dumps(Result, sort_keys=True,indent=4,ensure_ascii=False))
