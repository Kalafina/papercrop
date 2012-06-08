
require('mylib')

CBZwriter=LUAclass()


function CBZwriter:__init(fn)
	self.filename=fn
	self.numPages=0

	if os.isFileExist(fn) then
		os.deleteFiles(fn)
	end
	if os.isFileExist("temp.zip") then
		os.deleteFiles("temp.zip")
	end
end
function CBZwriter:addPage(image, color_depth)
	local fn=string.format("page%06d.png",self.numPages+1)
	image:save(fn, color_depth)
	os.execute("zip -m -1 temp.zip "..fn)
	self.numPages=self.numPages+1
end
function CBZwriter:save()
	-- compatibility function. does nothing
	os.copyFile('temp.zip "'..self.filename..'"')
	os.deleteFiles("temp.zip")
end
function CBZwriter:init()
	-- compatibility function. does nothing
end
function CBZwriter:addPageColor(image)
	-- compatibility function
	self:addPage(image,24)
end

do
	XMLwriter=LUAclass()

	function XMLwriter:__init(fn, book_pages, outdir)
		self.outdir=outdir
		self.filename=fn
		self.book_pages=book_pages

		--if os.isFileExist(fn) then
		--	os.deleteFiles(fn)
		--end
		self.pages={}
	end
	function XMLwriter:addPage(image, color_depth)
		for i=1, self.book_pages.cache.pages:size() do
			local pdfPage=self.book_pages.cache.pages[i]
			local tbl2={}
			local tbl=self.book_pages.cache[pdfPage]
			for i=1,#tbl do
				tbl2[i]={tbl[i]:left(), tbl[i]:top(), tbl[i]:right(), tbl[i]:bottom()}
			end
			self.pages[#self.pages+1]=tbl2
			self.pages[#self.pages].pdfPage=pdfPage+1
		end
		self.book_pages:clearCache()
	end
	function XMLwriter:save()
		local str=table.tostring(self.pages)
		str=string.gsub(str, "%['","")
		str=string.gsub(str, "%']","")
		util.writeFile(self.filename, str)
		self.pages={}
		local src=self.outdir..".pdf"
		local tgt=string.sub(self.filename,1,-4).."pdf"
		local xml=self.filename
		print(src, tgt, xml)
		local cmd

		if os.isUnix() then
			cmd="java -cp bin:lib/iText.jar:lib/iText-xtra.jar PdfCrop '"..src.."' '"..tgt.."' '"..xml.."'"
		else
			cmd='java -cp bin;lib/iText.jar;lib/iText-xtra.jar PdfCrop "'..src..'" "'..tgt..'" "'..xml..'"'
 		end
		execute("cd pdfCrop", cmd);
	end
	function XMLwriter:init()
		-- compatibility function. does nothing
	end
	function XMLwriter:addPageColor(image)
		assert(false)
	end
end
