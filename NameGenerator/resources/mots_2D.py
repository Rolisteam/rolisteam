#!/usr/bin/python3
import json
import os
import sys
#import numpy as np
import re
import codecs
import argparse


#filepath = "data_Name_Japanese_Male.txt"
#filepath = "data_Name_Japanese_Female.txt"
#filepath = "data_Name_Elve_Female.txt"
#filepath = "data_Name_Elve_Male.txt"
#filepath = "testdata.txt"
FilepathTab = ["_Name_Elve_Male", "_Name_Elve_Female", "_Name_Japanese_Female","_Name_Japanese_Male","_Name_Dwarf_Male","_Name_Dwarf_Female","_Name_human_star_wars_Male","_Name_human_star_wars_Female"]

probFirstLetter={}
probLetter={}
probLastLetter={}

res = []
lineCount=0
for nameFile in FilepathTab:
    filepath= "data"+nameFile+".txt"
    with codecs.open(filepath, "r", "UTF-8") as lines:
        for l in  lines:
            lineCount+=1
            # Split on white space or open parenthesis and keep the first string
            l2 = re.split("[ ,\(]",l)[0]
            lineClean = l2
            l2 = l2+"\n"



            firstLetter = l2[:1]
            if l2[:1] not in probFirstLetter:
                probFirstLetter[firstLetter]=0
            probFirstLetter[firstLetter]+=1

            lineClean=lineClean[:-1]
            lastTrio = lineClean[-3:]
            lastLetter= lastTrio[-1:]
            lastduo = lastTrio[:2]
            lastduo =lastduo.lower()
            if lastduo not in probLastLetter:
                probLastLetter[lastduo]={}
            endTuppe=probLastLetter[lastduo]
            if lastLetter not in endTuppe:
                endTuppe[lastLetter] = 0
            endTuppe[lastLetter]+=1

            firstTuppe='0'
            key=""

            for c in range(len(l2)-2):# mela\n len(l2)-2 = 2 ; C = 0 ; c=1
                if l2[c:c+1] == '\n':
                     key=firstTuppe+'0'
                else:
                    key=firstTuppe+l2[c:c+1] #key = 0m ; key = me
                key=key.lower()
                firstTuppe=l2[c:c+1] # firstTuppe: m ; e
               
                if key not in probLetter:
                    probLetter[key]={}

                
                
                dico = probLetter[key] #; dico = Dico[0m]; Dico[me]
                char =""
                if c+2>=len(l2)-1:  # 2 >= 3 not; 3=3
                    char = '0' # char = 0
                else:
                    char = l2[c+1:c+2] # char = e ; char = l

                if char not in dico:
                    dico[char] = 0
                dico[char]+=1       # Dico[0m][e]=1 ; Dico[me][l]=1



    #for l in  probLetter.keys():
    #    print(l+":"+str(probLetter[l]))

    for l in  probFirstLetter.keys():
        probFirstLetter[l]=float(float(probFirstLetter[l])/float(lineCount))



    Result = {"Description": "Description"+nameFile, "firstLetter":probFirstLetter , "data":probLetter, "lastLetter":probLastLetter}
     
    print(json.dumps(Result, sort_keys=True,indent=4,ensure_ascii=False))
