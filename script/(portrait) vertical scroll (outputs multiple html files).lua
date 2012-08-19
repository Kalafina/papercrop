width=device_width
height=device_height

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.

function aarons_fun(lineno, line)
	print("aaron",lineno, line)
end



function processPage(outdir, pageNo, numRects)
	
	OPF_Table = {} 
	--table.insert(OPF_Table,"<html>\n")
	--table.insert(OPF_Table,"<body>\n")
 
	processHTMLPageSubRoutine(outdir, pageNo, width,false, numRects,OPF_Table)

	--table.insert(OPF_Table,"</body>\n")
	--table.insert(OPF_Table,"</html>\n")

	str = table.concat(OPF_Table,'')
	--util.iterateFile(string.format("%s/List.text",outdir),aarons_fun)
	util.writeFile(string.format("%s/List.text",outdir), str)
	Join_HTML_Files(string.format("%s/List.text",outdir),string.format("%s/%s.html",outdir,basename(outdir))) 
    print("got here")
	return 1
end


function processAllPages(outdir)
	initializeOutput(outdir)
	local pageNo=0
	
	OPF_Table = {} 
	--table.insert(OPF_Table,"<html>\n")
	--table.insert(OPF_Table,"<body>\n")
	
	while pageNo<win:getNumPages() do
		win:setCurPage(pageNo)
		if processHTMLPageSubRoutine(outdir, pageNo, width,false, win:getNumRects(),OPF_Table)==0 then
			return 0
		end
		pageNo=pageNo+1
	end
	
	--table.insert(OPF_Table,"</body>\n")
	--table.insert(OPF_Table,"</html>\n")

	str = table.concat(OPF_Table,'')
	util.writeFile(string.format("%s/List.text",outdir), str) 
	--util.iterateFile(string.format("%s/List.text",outdir))
	Join_HTML_Files(string.format("%s/List.text",outdir),string.format("%s/%s.html",outdir,basename(outdir))) 
    print("got here")
	
	--finalizeOutput(outdir)
    return 1
end 
