#! /usr/bin/python
#**************************************************************************#
#*     Copyright (C) 2009 by Renaud Guezennec                              #
#*   http://renaudguezennec.homelinux.org/accueil,3.html                   #
#*                                                                         #
#*   Canal+ Daily Downloader is free software;                             #
#*   you can redistribute it and/or modify                                 #
#*   it under the terms of the GNU General Public License as published by  #
#*   the Free Software Foundation; either version 2 of the License, or     #
#*   (at your option) any later version.                                   #
#*                                                                         #
#*   This program is distributed in the hope that it will be useful,       #
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#*   GNU General Public License for more details.                          #
#*                                                                         #
#*   You should have received a copy of the GNU General Public License     #
#*   along with this program; if not, write to the                         #
#*   Free Software Foundation, Inc.,                                       #
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
#**************************************************************************#

from getopt import getopt, GetoptError
from os import system, remove, makedirs , listdir
from os.path import join, expanduser, exists, isdir, isfile
import re
from sys import exit, argv, stdout

VALID=1
ERROR=0
WARNING=2
def checkRule1_1(chaine,newfile):
    motif = re.compile('class ([A-Za-z]+)')
    objet_corresp = motif.match(chaine)
    if objet_corresp:
        classname = objet_corresp.group(1)
        motif = re.compile('([A-Z][a-z]+)')
        if(classname.startswith('Q')): #remove Qt class of the rules
            return VALID
        objet_corresp = motif.match(classname)
        if objet_corresp:
            return VALID
        else:
            return ERROR
    else:
        return VALID


def checkRule1_6(chaine,newfile):
    if len(chaine)>120 :
        return ERROR
    else:
        return VALID

def checkRule1_8(chaine,newfile):
    if(chaine.startswith('\t')):
        return ERROR
    else:
        return VALID


def checkRule1_8(chaine,newfile):
    if(chaine.startswith('\t')):
        return ERROR
    else:
        return VALID

def checkRule1_():
    
def checkFile(filepath,output):
    #function to call and its error message.
    functionList=(('checkRule1_1',"Rule 1.1: class"),('checkRule1_6',"Rule 1.6 : line length"),('checkRule1_8',"Rule 1.8 : line starts with tabulation"))
    if isfile(filepath):
        
        filehandler = open(filepath)
        filecontent = filehandler.readlines()
        linecount = 1
        firsterror = True
        newfile = True
        for line in filecontent:
            for func in functionList:
                result=eval(func[0])(line,newfile)
                if (result==0):
                    if(firsterror):
                        output.write('<h2>{0}</h2>\n'.format(filepath))
                        firsterror=False
                    output.write('Line {0} : Error {1}<br/>\n'.format(linecount,func[1]))
            linecount+=1
            newfile = False
               # else:
                #    print 'Return code : {0}'.format(result)
        

def isSupportedFile(f):
	checked_filetype = {'header': '.h', 'source': '.cpp'}
	countValid=0
	for b in checked_filetype.values():
		if( f.endswith(b)):
			return True
	return False
		

def readDirectory(src_directory,output):
	if isdir(src_directory):
		for f in listdir(src_directory):
			if isSupportedFile(f):
				checkFile(join(src_directory, f),output)
			elif isdir(join(src_directory, f)):
				readDirectory(join(src_directory, f),output)

	else:
		print src_directory+" is not a directory"




def usage():
    print "-h: display help"
    print "-v: display the version information"
    print "-o: output file"
    print "-D: [directory] : path to where you store your tv show"

def version():
    print "Rolisteam Coding Rules Checker V0.1"
    print "Author: Reaud Guezennec"
    print "This program is under a GPLv2 licence"

def main():
    src_directory=""
    output = ""
    #Get option and arguments
    try:
        opts, args = getopt(argv[1:], "hvo:D:",
        ["help", "version","output", "Directory"])
    except GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        exit(2)
	
	
    #Process options and arguments
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            exit(0)
        if o in ("-o"):
            output = a
        if o in ("-D", "--Directory"):
            src_directory = a
        if o in ("-V", "--Version"):
            version()
            exit(0)
    if len(output) > 0:
        outputHandler = open(output,'a')
    else:
        outputHandler = stdout
    readDirectory(src_directory,outputHandler)


if __name__ == "__main__":
    main()
