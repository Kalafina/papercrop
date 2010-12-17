Installation:
You have to have STKMaker6.0 and AutoIt3 installed. (Both freeware)
Find "desktop_folder=c:..." in config.lua and correct it (according to your windows version and username). 
This really have to be the desktop folder of your MS operating system.
Also, find "screenWidth" and "screenHeight" in stkmaker.au3 and correct it (according to your monitor's resolution).

Usage 1 (using PaperCrop):
When you press the "convert all pages" button on the papercrop program, the stkmaker.au3 script tries to 
1. move the output jpeg images into [desktop folder]/_stk_temporary folder,
2. run the STKMaker, 
3. load images in the [desktop folder]/_stk_temporary folder, 
4. type in author and booktitle,
5. save [desktop folder]/booktitle.stk,
6. delete the temporary images and folder
7. close the STKMaker
all automatically. (Make sure not moving the mouse while the stkmaker.au3 script is working.)
So all you need to do is to copy the resulting STK file into your ebook. 
(Even this can be automated by editing config.lua move_to_folder variable.)

Usage 2: console mode conversion from jpeg sequence to STK format without using PaperCrop

1. Copy images into [desktop folder]/_stk_temporary folder
2. in a command line window, type below:
cmd /c stkmaker.au3 booktitle author


