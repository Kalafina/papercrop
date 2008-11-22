
device_width=600
device_height=800
scroll_overlap_pixels=40
output_format=".gif"
output_to_pdf=true -- output to a pdf file, instead of multiple image files when possible.
max_vspace=6 -- pixels


outpdf=PDFWriter()

function initializeOutput(outdir)
	if output_to_pdf then
		outpdf:init()
	else
		win:deleteAllFiles()
	end
end

function outputImage(image, outdir, pageNo, rectNo)
	if output_to_pdf and outpdf:isValid() then
		outpdf:addPage(image)
	else
		image:Save(string.format("%s/%05d_%03d%s",outdir,pageNo,rectNo,output_format))
	end
end

function finalizeOutput(outdir)
	if output_to_pdf and outpdf:isValid() then
		outpdf:save(outdir.."_output.pdf")
	end
end

function postprocessImage(image)
    -- sharpen(amount in [1, 2.5], iterations), see ilu manual for more details.
	--image:sharpen(1.5, 1)
	--image:contrast(1.5)
    image:gamma(0.5)
    image:dither(16)
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

