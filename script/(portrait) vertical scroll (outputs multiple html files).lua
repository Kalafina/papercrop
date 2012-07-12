width=device_width
height=device_height

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.
function processPage(outdir, pageNo, numRects)
	
	OPF_Table = {} 
	table.insert(OPF_Table,"<html>\n")
	table.insert(OPF_Table,"<body>\n")
	
	--print("my outdir is" ,string.format("%s/%05d",outdir,pageNo)) 
    --processHTMLPageSubRoutine(string.format("%s/%05d",outdir,pageNo), pageNo, width, numRects,OPF_Table)

	processHTMLPageSubRoutine(outdir, pageNo, width, numRects,OPF_Table)

	table.insert(OPF_Table,"</body>\n")
	table.insert(OPF_Table,"</html>\n")

	str = table.concat(OPF_Table,'')
	print (str)
	util.writeFile(string.format("%s/FrontPage.html",outdir), str) 
    
	return 1
end


function processAllPages(outdir)
	initializeOutput(outdir)
	local pageNo=0
	
	OPF_Table = {} 
	table.insert(OPF_Table,"<html>\n")
	table.insert(OPF_Table,"<body>\n")
	
	while pageNo<win:getNumPages() do
		win:setCurPage(pageNo)
		if processHTMLPageSubRoutine(outdir, pageNo, width, win:getNumRects(),OPF_Table)==0 then
			return 0
		end
		pageNo=pageNo+1
	end
	
	table.insert(OPF_Table,"</body>\n")
	table.insert(OPF_Table,"</html>\n")

	str = table.concat(OPF_Table,'')
	print (str)
	util.writeFile(string.format("%s/FrontPage.html",outdir), str) 
	
	
	--finalizeOutput(outdir)
    return 1
end 
