---------------------------------------------------------------------
-- user settings
---------------------------------------------------------------------

device_width=600
device_height=800
scroll_overlap_pixels=40
output_format=".jpg"
output_to_pdf=true -- output to a pdf file, instead of multiple image files when possible.
nr_of_pages_per_pdf_book = 100;
max_vspace=16 -- pixels
landscapeRotate="rotateLeft"

---------------------------------------------------------------------
-- utility functions
---------------------------------------------------------------------

if landscapeRotate=="rotateLeft" then
   landscapeRotate=function (img) img:rotateLeft() end
else
   landscapeRotate=function (img) img:rotateRight() end
end


---------------------------------------------------------------------
-- split books                                                     --
---------------------------------------------------------------------


book_pages = {
  book_part_nr,
  nr_of_pages
};

function book_pages:init(part_nr)
  self.book_part_nr = part_nr;
  self.nr_of_pages = 0;
  self.outpdf=PDFWriter()
  self.outpdf:init()
end

function book_pages:init_for_next_part()
  self:init(self.book_part_nr + 1);
end

function book_pages:add_page (image, outdir)
  self.nr_of_pages = self.nr_of_pages + 1;
  self.outpdf:addPage(image)
  collectgarbage();
end

function book_pages:writeToFile(outdir)
  if self.nr_of_pages > 0 then
    local outpdf = PDFWriter();
    self.outpdf:save(outdir .. "_" .. self.book_part_nr .. ".pdf");
    collectgarbage();
  end;
end
--[[

function book_pages:init(part_nr)
  self.book_part_nr = part_nr;
  self.nr_of_pages = 0;
end

function book_pages:init_for_next_part()
  self:init(self.book_part_nr + 1);
end

function book_pages:add_page (image, outdir)
  self.nr_of_pages = self.nr_of_pages + 1;
  image:Save(string.format("%s/%05d_%05d%s",outdir,self.book_part_nr,self.nr_of_pages,output_format));
  collectgarbage();
end

function book_pages:writeToFile(outdir)
  if self.nr_of_pages > 0 then
    local outpdf = PDFWriter();
    outpdf:init();
    images = {};
    for j = 1, self.nr_of_pages do
      local image = CImage();
      image:Load(string.format("%s/%05d_%05d%s",outdir,self.book_part_nr,j,output_format));
      --- strange, but the loaded image seems to be mirrored (a bug in CImage:Load ?) ---
      --- as a quick fix, we save and load it again to undo the mirroring             ---
      image:Save(string.format("%s/%05d_%05d_ttt%s",outdir,self.book_part_nr,j,output_format));
      image = CImage();
      collectgarbage();
      image:Load(string.format("%s/%05d_%05d_ttt%s",outdir,self.book_part_nr,j,output_format));
      images[j] = image; -- prevent image from being garbage collected
                         -- (I guess that 'outpdf' does not keep a reference to it, but still needs it?)
      ---
      outpdf:addPage(image);
    end
    outpdf:save(outdir .. "_" .. self.book_part_nr .. ".pdf");
    win:deleteAllFilesWithoutConfirm()
    images = {};
    collectgarbage();
  end;
end

]]--
---------------------------------------------------------------------


--outpdf=PDFWriter()

function initializeOutput(outdir)
	if output_to_pdf then
		--vv--outpdf:init();
    book_pages:init(1);
    --^^--
  else
		win:deleteAllFiles()
	end
end

function outputImage(image, outdir, pageNo, rectNo)

  if output_to_pdf then ----if output_to_pdf and outpdf:isValid() then
		--vv--outpdf:addPage(image)
    if (book_pages.nr_of_pages < nr_of_pages_per_pdf_book) then
      book_pages:add_page(image, outdir);
    else
      book_pages:writeToFile(outdir);
      book_pages:init_for_next_part();
    end
    --^^--
	else
		image:Save(string.format("%s/%05d_%03d%s",outdir,pageNo,rectNo,output_format))
	end
end

function finalizeOutput(outdir)
--vv--if output_to_pdf and outpdf:isValid() then
--vv--  outpdf:save(outdir.."_output.pdf")
  if output_to_pdf then
    book_pages:writeToFile(outdir);
    book_pages:init(0);
	end
--^^--
end

function postprocessImage(image)
    -- sharpen(amount in [1, 2.5], iterations), see ilu manual for more details.
	--image:sharpen(1.5, 1)
	--image:contrast(1.5)
    image:gamma(0.5)
--    image:dither(16)
end

function processPageSubRoutine(imageM, pageNo, width, numRects)
	
	for rectNo=0, numRects-1 do
		win:setStatus("processing"..pageNo.."_"..rectNo)
		local image=CImage()
		win:getRectImage_width(pageNo, rectNo, width, image)
		if imageM:GetHeight()==0 then
			imageM:CopyFrom(image)
		else
			imageM:concatVertical(imageM, image)

		end
	end
	trimVertSpaces(imageM, 2, max_vspace, 255)
end

function splitImage_old(imageM, height, outdir, pageNo, rotateRight)

	if imageM:GetHeight()>height then
		-- split into multiple subpages 
		numSubPage=math.ceil((imageM:GetHeight()-scroll_overlap_pixels)/height)
		win:setStatus("num"..numSubPage)
		local imageS=CImage()
		startPos=vectorn()
		startPos:linspace(0, imageM:GetHeight()-height, numSubPage)
		for subPage=0, numSubPage-1 do
			start=math.floor(startPos:value(subPage))
			imageS:crop(imageM, 0, start, imageM:GetWidth(), start+height)
			if rotateRight then imageS:rotateRight() end
			outputImage(imageS,outdir,pageNo,subPage)
			win:setStatus("saving "..pageNo.."_"..subPage)
		end
	else
	    local imageS=CImage()
        imageS:crop(imageM, 0, 0, imageM:GetWidth(), imageM:GetHeight())
		if rotateRight then imageS:rotateRight() end
		outputImage(imageS,outdir,pageNo,0)
	end
end

function splitImage(imageM, height, outdir, pageNo, rotateRight)
	-- split into multiple subpages 
	local imageS=CImage()
    local subPage=0
    
	while true 
    do
        curY=subPage*(height-scroll_overlap_pixels)
        if curY+height <= imageM:GetHeight() then
            imageS:crop(imageM, 0, curY, imageM:GetWidth(), curY+height)
            if rotateRight then imageS:rotateRight() end
            outputImage(imageS,outdir,pageNo,subPage)
            win:setStatus("saving "..pageNo.."_"..subPage)
        else
            imageS:crop(imageM, 0, curY, imageM:GetWidth(), imageM:GetHeight())
            if rotateRight then imageS:rotateRight() end
            outputImage(imageS,outdir,pageNo,subPage)
            win:setStatus("saving "..pageNo.."_"..subPage)
            break
        end
        subPage=subPage+1
	end
end

function splitImagePart(imageM, height, outdir, pageNo, rotateRight)
	-- split into multiple subpages 
	local imageS=CImage()
    local subPage=0
    
	while true 
    do
        curY=subPage*(height-scroll_overlap_pixels)
        if curY+height <= imageM:GetHeight() then
            imageS:crop(imageM, 0, curY, imageM:GetWidth(), curY+height)
            if rotateRight then imageS:rotateRight() end
            postprocessImage(imageS)
            outputImage(imageS,outdir,pageNo,subPage)
            win:setStatus("saving "..pageNo.."_"..subPage)
       else
            imageM:crop(imageM, 0, curY, imageM:GetWidth(), imageM:GetHeight())
            break
        end
        subPage=subPage+1
	end
end

