width=device_width
height=device_height

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.
function processPage(outdir, pageNo, numRects)
	
	print("my outdir is" ,string.format("%s/%05d",outdir,pageNo)) 
    processHTMLPageSubRoutine(string.format("%s/%05d",outdir,pageNo), pageNo, width, numRects)

    
	return 1
end


function processAllPages(outdir)
	initializeOutput(outdir)
	local pageNo=0
	while pageNo<win:getNumPages() do
		win:setCurPage(pageNo)
		if processPageSubRoutine(outdir, pageNo, width, win:getNumRects())==0 then
			return 0
		end
		pageNo=pageNo+1
	end
	--finalizeOutput(outdir)
    return 1
end
