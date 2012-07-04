#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
#  Add quote in lyx using lyxclient

import os,sys

HOME = os.environ.get("HOME")
PAPERCROP_PATH= os.path.join(HOME, "bin/papercrop/paperCrop_source")
CWD=os.getcwd()

if not os.path.exists(PAPERCROP_PATH):
	print('PaperCrop Error! Cannot find paperCrop', HOME, PAPERCROP_PATH)

def do_papercrop (fn):
	print("Do papercrop")
	# print(document.get_title())
	# print(document.get_authors())
	print('cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+CWD+'/'+fn+'\'"&')
	os.system('cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+CWD+'/'+fn+'\'"&')

	return True

# main
if len(sys.argv)>1:
	do_papercrop(sys.argv[1])
else:
	print('usage: paperCrop filename')
