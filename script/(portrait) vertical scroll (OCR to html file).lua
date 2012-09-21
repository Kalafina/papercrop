width=device_width
height=device_height

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.

function processPage(outdir, pageNo, numRects)
	
	OPF_Table = {} 

    
	processImageForOCR(outdir, pageNo,numRects,OPF_Table)
	
	

	str = table.concat(OPF_Table,'')
	util.writeFile(string.format("%s/List.text",outdir), str)
	Join_HTML_Files(string.format("%s/List.text",outdir),string.format("%s/%s.html",outdir,basename(outdir))) 
	return 1
end




function processAllPages(outdir)
	initializeOutput(outdir)
	local pageNo=0
	
	OPF_Table = {} 
	
	while pageNo<win:getNumPages() do
		win:setCurPage(pageNo)
		
		if processImageForOCR(outdir, pageNo,win:getNumRects(),OPF_Table)==0 then
			return 0
		end
		pageNo=pageNo+1
	end

	str = table.concat(OPF_Table,'')
	util.writeFile(string.format("%s/List.text",outdir), str) 
	--util.iterateFile(string.format("%s/List.text",outdir))
	Join_HTML_Files(string.format("%s/List.text",outdir),string.format("%s/%s.html",outdir,basename(outdir)))
	
	--finalizeOutput(outdir)
    return 1
end 
