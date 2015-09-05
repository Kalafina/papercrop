# About #

This program converts PDF documents such as articles, and academic papers into a PDF (or image sequence) so that it can be read on a small screen of e-book devices or smartphones.

![http://jupiter.kaist.ac.kr/~taesoo/projects/paperCrop/out1/0.jpg](http://jupiter.kaist.ac.kr/~taesoo/projects/paperCrop/out1/0.jpg) -> ![http://jupiter.kaist.ac.kr/~taesoo/projects/paperCrop/out2/0_0.jpg](http://jupiter.kaist.ac.kr/~taesoo/projects/paperCrop/out2/0_0.jpg)

# Usage #

1.      Open a PDF file (by clicking Load button, or drag-and-drop)

2.      Choose a preset by clicking the default preset button

3.      Click "Process all pages" button

4.      Output will be in the same folder with the input file. (~_1.pdf)_

# Usage 2 (for expert users) #

1. Open a PDF file (by clicking Load button, or drag-and-drop)

2. (Optional) choose how many columns are in the pdf file, and then click the update button.

3. if results are not satisfactory, modify MIN gap, MIN text gap and margin parameters, and then click the update button.

4. (Optional) choose a rendering option by clicking the option button

5. Process all pages.

# Usage 3 (manual segmentation) #

1. Open a PDF file (by clicking Load button, or drag-and-drop)

2. Uncheck [automatic segmentation](Use.md)

3. Select a region by clicking, delete one by hitting [DEL](DEL.md) when necessary, or create one by dragging.

4. Process all pages or Process current page

# FAQ #

1. How can I change output aspect ratio?

Open config.lua and edit "vector\_PDF={600,800,..." appropriately.

2. How can I create a preset

Copy presets/one-column papers.lua to a new file (e.g. "presets/ACM TOG.lua"), and edit it.

3. How can I change the default preset

Edit presets/default preset.lua. It points to one of the preset files.

4. The output folder is empty. Why?

The output file xxx\_1.pdf is in the same folder as the input pdf (if you chose "cancel" on the file saving dialog). Not in the [xxx](xxx.md) folder.

5. I am a Windows user and the program crash with some dll errors.

Please install visual studio 2010 redistributable first. (google download vc2010 redistributable)

6. I am a mac user.

Sorry, there is no official support for mac. I am a linux user who happens to know how to build binaries for windows. Source codes would compile with slight modifications on mac though.

- Following FAQs are irrelevant to the default "vector PDF" device.

7. How can I change output format?

Edit config.lua. Find ".gif" and replace it as .jpg, .bmp or .png.
Don't forget to set output\_to\_pdf=false if desired output format is NOT pdf.

8. How can I improve readability?

Edit config.lua. One can adjust image processing parameters such as sharpen, contrast, gamma, dithering, and color depth.

9. Output is gray. Why not color?

Color pdf is usually larger. If you need colors, then edit config.lua and edit color\_depth=8 -> color\_depth=24. (From version 0.43)


# Change logs #
version 0.51,
- finally outputs to native PDFs which preserves texts.
- requires java 7 jre to run.

version 0.4, (Feb 1st, 2010).

- pre-crop pages (cropped regions are not included in the output. thanks to murraypaul)

- book splitting (thanks to x)

- fixed occasional font problems (thanks to murraypaul)

- fixed occasional crash

version 0.3, (Oct 19th, 2008).

- reflow preset added. (thanks to caritas.)

- output to PDF (by default).

version 0.24, (Oct 16th, 2008).

- minor bug fixes (incorrrect overlapping regions, incorrect margin).

- Easier to change the output format.

version 0.23, (Oct 15th, 2008).

- minor bug fixes (incorrrect overlapping regions, occasional crash).

- font information is used when available.

version 0.22, (Oct 7th, 2008).

- a dithering bug fixed.

- preset functionality added.

version 0.2, (Oct 6th, 2008).

- batch processing added.

- default output format became gif. (small, lossless format)

- dithering added

- some bug fixes

version 0.1, (May 14th, 2008).

- initial version.