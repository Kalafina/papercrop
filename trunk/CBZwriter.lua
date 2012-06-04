
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

	function XMLwriter:__init(fn)
		self.filename=fn
		self.numPages=0

		if os.isFileExist(fn) then
			os.deleteFiles(fn)
		end
	end
	function XMLwriter:addPage(image, color_depth)
		self.numPages=self.numPages+1
	end
	function XMLwriter:save()
	end
	function XMLwriter:init()
		-- compatibility function. does nothing
	end
	function XMLwriter:addPageColor(image)
		-- compatibility function
	end
end
