
require('mylib')

CBZwriter=LUAclass()

function CBZwriter:__init(fn)
	self.filename=fn
	self.numPages=0
	os.deleteFiles(fn)
	os.deleteFiles("temp.zip")
end
function CBZwriter:addPage(image, color_depth)
	local fn=string.format("page%06d.png",self.numPages+1)
	image:save(fn, color_depth)
	os.execute("zip -m -1 temp.zip "..fn)
	self.numPages=self.numPages+1
end
function CBZwriter:save()
	-- compatibility function. does nothing
	os.deleteFiles("page*.png")
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
