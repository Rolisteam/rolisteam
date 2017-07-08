# Simple python script to generate all rolisteam translations at once
import os, sys

def printUsage() :
	print ("usage: python generatetranslations.py [--help] [-h] lreleasePath")
	print ("	--help : display help")
	print ("	-h : display help")
	print ("	lreleasePath : Path to the lrelease.exe of your Qt distribution")

def main() :
	if ((len(sys.argv) != 2) or (sys.argv[1] == "--help") or (sys.argv[1] == "-h")) :
		printUsage()
		return
	
	print("   Generating translations...")
	target = "translations/rolisteam"
	os.system(sys.argv[1] + " " + target +".ts")
	os.system(sys.argv[1] + " " + target +"_de.ts")
	os.system(sys.argv[1] + " " + target +"_es.ts")
	os.system(sys.argv[1] + " " + target +"_fr.ts")
	os.system(sys.argv[1] + " " + target +"_hu_HU.ts")
	os.system(sys.argv[1] + " " + target +"_nl_NL.ts")
	os.system(sys.argv[1] + " " + target +"_pt_BR.ts")
	os.system(sys.argv[1] + " " + target +"_ro_RO.ts")
	os.system(sys.argv[1] + " " + target +"_tr.ts")
	print("   Translations generated")
	
if __name__ == "__main__":
    main()