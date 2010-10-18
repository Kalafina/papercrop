-- collection of utility functions that depends only on standard LUA. (no dependency on baseLib or mainLib)
-- some of the functions works only when gnu tools "find" and "grep" are installed.

function os.capture(cmd, raw)
  local f = assert(io.popen(cmd, 'r'))
  local s = assert(f:read('*a'))
  f:close()
  if raw then return s end
  s = string.gsub(s, '^%s+', '')
  s = string.gsub(s, '%s+$', '')
  s = string.gsub(s, '[\n\r]+', ' ')
  return s
end


function os.createDir(path)

   if os.isLinux() then
      os.execute("mkdir "..path)
   else
      os.execute("md "..string.gsub(path, '/', '\\'))
   end
end

function os.rename(name1, name2)

   if os.isLinux() then
      os.execute("mv "..name1.." "..name2)
   else
      local cmd=string.gsub("move "..name1.." "..name2, '/', '\\')
      print(cmd)
      os.execute(cmd)
   end
end

function os.deleteFiles(mask)

   if os.isLinux() then
      os.execute("rm "..mask)
   else
      os.execute("del "..string.gsub(mask, '/', '\\'))
   end
end

function os.parentDir(currDir)
   return os.rightTokenize(string.gsub(currDir, "\\","/"), "/")
end

function os.copyFile(mask)

   if os.isLinux() then
      os.execute("cp "..mask)
   else
      print("copy "..      string.gsub(mask, '/', '\\'))
      os.execute("copy "..      string.gsub(mask, '/', '\\'))
   end
end
function os.copyFiles(src, dest, ext)
   if os.isLinux() then
      os.execute("cp "..src.." "..dest)
   else
      local cmd="copy "..string.gsub(src, '/', '\\').." "..string.gsub(dest, '/', '\\')
      print(cmd)
      os.execute(cmd)
   end

   if ext then
      local files=os.glob(dest.."/*"..ext[1])

      for i,file in ipairs(files) do
	 if string.find(file, ext[1])~=string.len(file) then
	    os.rename(file,  string.gsub(file, ext[1], ext[2]))
	 end
      end
   end
end


function os.glob(mask)
   local files=os.capture("find "..mask,true)
   local start=1
   local len=string.len(files)
   local szout=array:new()
   while start and start<len do
      local n=string.find(files, '\n', start)
      local str=string.sub(files,start, n-1)
      szout:pushBack(str)
      start=n+1
   end

   return szout
end

function os.rightTokenize(str, sep)
   local len=string.len(str)
   for i=len,1,-1 do
      if string.sub(str, i,i)==sep then
	 return string.sub(str, 1, i-1), string.sub(str, i+1)
      end
   end
   return str, nil
end

function os.sleep(aa)
   local a=os.clock()
   while os.difftime(os.clock(),a)<aa do -- actually busy waits rather then sleeps
   end
end

function os.VI_path()
	return '"c:\\Program Files\\Vim\\vim72\\gvim.exe"'
end	
function os.vi_line(fn, line)

	local VI=os.VI_path()..' --remote-silent'
	local cmd=VI..' +'..line..' "'..fn..'"'
	print(cmd)
	execute(cmd)
end
function os.vi(...)
	local VI=os.VI_path() ..' --remote-silent'

	local targets={...}


	local itgt, target
	for itgt,target2 in ipairs(targets) do
		--      local target=string.sub(target2,4)
		local target=target2

		if string.find(target, '*') ~=nil or string.find(target, '?')~=nil then

			if false then
				-- open each file. too slow
				local subtgts=os.glob(target)

				local istgt,subtgt
				for istgt,subtgt in ipairs(subtgts) do
					local cmd=VI..' "'..subtgt..'"'
					if string.find(cmd,'~')==nil then
						os.execute(cmd) 
					end
				end
			else
				local lastSep
				local newSep=0
				local count=0
				repeat lastSep=newSep
					newSep=string.find(target, "/", lastSep+1) 	    
					count=count+1
				until newSep==nil 

				local path=string.sub(target, 0, lastSep-1)

				local filename
				if lastSep==0 then filename=string.sub(target,lastSep) else filename=string.sub(target, lastSep+1) end

				print(filename, path, count)

				--	    execute("cd "..path, 'find . -maxdepth 1 -name "'..filename..'"| xargs '..EMACS)


				-- if string.find(filename, "*") then
				--    execute("cd "..path, 'find '..filename..' -maxdepth 1 | xargs '..EMACS)
				-- else

				print("cd "..path, VI.." "..filename)
				execute("cd "..path, "rm -f *.lua~", "rm -f #*#", VI.." "..filename)
				--	    end

			end

		else
			print(VI..' "'..target..'"')
			execute(VI..' "'..target..'"')
		end
	end


end
function os.emacs(...)
	local targets={...}

	local EMACS
	if os.isLinux() then
		EMACS="emacs"
	else      
		EMACS="emacsclient -n"
	end

	local itgt, target
	print("emacs")
	for itgt,target2 in ipairs(targets) do
		--      local target=string.sub(target2,4)
		local target=target2

		if string.find(target, '*') ~=nil or string.find(target, '?')~=nil then

			if false then
				-- open each file. too slow
				local subtgts=os.glob(target)

				local istgt,subtgt
				for istgt,subtgt in ipairs(subtgts) do
					local cmd=EMACS..' "'..subtgt..'"'
					if string.find(cmd,'~')==nil then
						os.execute(cmd) 
					end
				end
			else
				local lastSep
				local newSep=0
				local count=0
				repeat lastSep=newSep
					newSep=string.find(target, "/", lastSep+1) 	    
					count=count+1
				until newSep==nil 

				local path=string.sub(target, 0, lastSep-1)

				local filename
				if lastSep==0 then filename=string.sub(target,lastSep) else filename=string.sub(target, lastSep+1) end

				print(filename, path, count)

				--	    execute("cd "..path, 'find . -maxdepth 1 -name "'..filename..'"| xargs '..EMACS)


				-- if string.find(filename, "*") then
				--    execute("cd "..path, 'find '..filename..' -maxdepth 1 | xargs '..EMACS)
				-- else
				execute("cd "..path, "rm -f *.lua~", "rm -f #*#", EMACS.." "..filename)
				--	    end

			end

		else
			os.execute(EMACS..' "'..target..'"')
		end
	end
end   

function os.emacs2(target)
	os.execute('find . -name "'..target..'"| xargs emacsclient -n')      
end

function os.isLinux() 
	local isWin=string.find(string.lower(os.getenv('OS') or 'nil'),'windows')~=nil
	return not isWin
end

function os.isFileExist(fn)
	local f=io.open(fn,'r')
	if f==nil then return false end
	f:close()
	return true
end



-- LUAclass method is for avoiding so many bugs in luabind's "class" method (especially garbage collection).

-- usage: MotionLoader=LUAclass()
--   ...  MotionLoader:__init(a,b,c)

--        VRMLloader=LUAclass(MotionLoader)
--   ...  VRMLloader:__init(a,b,c)
--              MotionLoader.__init(self,a,b,c)
--        end

--  loader=VRMLloader:create({a,b,c}) -- Note that parameters are enclosed by {}

function LUAclass(baseClass)


	local classobj={}
	if __classMTs==nil then
		__classMTs={}
		__classMTs.N=0
	end

	__classMTs.N=__classMTs.N+1
	local classId=__classMTs.N
	__classMTs[classId]={__index=classobj}
	classobj.__classId=classId

	classobj.new=function (classobj, ...)
		local new_inst={}
		setmetatable(new_inst, __classMTs[classobj.__classId])
		new_inst:__init(unpack({...}))
		return new_inst
	end
	if baseClass~=nil then
		setmetatable(classobj, {__index=baseClass})
	end

	return classobj	 
end

-- one indexing. 100% compatible with original lua table. (an instance is an empty table having a metatable.)
array=LUAclass()

function array:__init()
end

function array:size()
	return table.getn(self)
end

function array:pushBack(x)
	self[table.getn(self)+1]=x
end

function array:back()
	return self[table.getn(self)]
end

function string.join(table, sep)
	local out=""

	for i,v in ipairs(table) do
		out=out..sep..v
	end
	return string.sub(out, string.len(sep)+1)
end


function table.isubset(tbl, first, last)

	if last==nil then last=table.getn(tbl) end

	local out={}
	for i=first,last do
		out[i-first+1]=tbl[i]
	end
	return out
end

function table._ijoin(tbl1, tbl2)
	local out={}
	local n1=table.getn(tbl1)
	local n2=table.getn(tbl2)
	for i=1,n1 do
		out[i]=tbl1[i]
	end

	for i=1,n2 do
		out[i+n1]=tbl2[i]
	end
	return out
end

function table.ijoin(...)
	local input={...}
	local out={}
	for itbl, tbl in ipairs(input) do
		out=table._ijoin(out, tbl)
	end
	return out
end

function table.join(...)
	local input={...}
	local out={}
	for itbl, tbl in ipairs(input) do
		for k,v in pairs(tbl) do
			out[k]=v
		end
	end
	return out
end

function table.mult(tbl, b)

	local out={}
	for k,v in pairs(tbl) do
		out[k]=v*b
	end
	setmetatable(out,table)
	return out
end


function table.add(tbl1, tbl2)

	local out={}

	for k,v in pairs(tbl1) do
		if tbl2[k] then
			out[k]=tbl1[k]+tbl2[k]
		end
	end
	for k,v in pairs(tbl2) do
		if tbl1[k] then
			out[k]=tbl1[k]+tbl2[k]
		end
	end
	setmetatable(out,table)

	return out
end
table.__mul=table.mult
table.__add=table.add

function printTable(t, bPrintUserData)
	local out="{"
	for k,v in pairs(t) do
		local tv=type(v)
		if tv=="string" or tv=="number" or tv=="boolean" then
			out=out..'['..k..']='..v..', '
		elseif tv=="userdata" then
			if bPrintUserData==true then
				out=out..'\n['..k..']=\n'..v..', '
			else
				out=out..'['..k..']='..tv..', '
			end
		else
			out=out..'['..k..']='..tv..', '
		end
	end
	print(out..'}')
end

function out(...)
	local list={...}
	if table.getn(list)==1 then
		local t=list[1]
		if type(t)=="table" then
			printTable(t)
		else
			print(t)
		end
	else
		printTable(list)
	end
end

function table.tostring(t)
	-- does not check reference. so infinite loop can occur. 
	-- to prevent such cases, use pickle()!!!

	local out="{"

	local N=table.getn(t)
	local function packValue(v)
		local tv=type(v)
		if tv=="number" or tv=="boolean" then
			return tostring(v)
		elseif tv=="string" then
			return '"'..tostring(v)..'"'
		elseif tv=="table" then
			return table.tostring(v)
		end
	end

	for i,v in ipairs(t) do
		out=out..packValue(v)..", "
	end

	for k,v in pairs(t) do

		local tk=type(k)
		local str_k
		if tk=="string" then
			str_k="['"..k.."']="
			out=out..str_k..packValue(v)..', '
		elseif tk~="number" or k>N then	 
			str_k='['..k..']='
			out=out..str_k..packValue(v)..', '
		end
	end
	return out..'}'
end
function table.remove_if(table, func)
	for k,v in pairs(table) do
		if func(k,v) then
			table[k]=nil
		end
	end
end
function createBatchFile(fn, list, echoOff)
	local fout, msg=io.open(fn, "w")
	if fout==nil then print(msg) end

	if echoOff then
		fout:write("@echo off\necho off\n")
	end
	fout:write("setlocal\n")
	for i,c in ipairs(list) do
		fout:write(c.."\n")
	end
	fout:close()
end

function os.execute2(...) -- excute multiple serial operations
	execute(...)
end

function os.pexecute(...) -- excute multiple serial operations
	if os.isLinux() then
		execute(...)
	else
		local list={...}
		createBatchFile("_temp.bat",list)      
		os.execute("start _temp.bat")	
	end      
end

-- deprecated (the same as os.execute2)
function execute(...)
	local list={...}
	if os.isLinux() then
		local cmd=""
		for i,c in ipairs(list) do
			cmd=cmd..";"..c
		end
		print(string.sub(cmd,2))
		os.execute(string.sub(cmd, 2))
	else
		createBatchFile("_temp.bat",list,true)
		--      os.execute("cat _temp.bat")
		os.execute("_temp.bat")	
	end
end

util=util or {}

function util.readFile(fn)
	local fout, msg=io.open(fn, "r")
	if fout==nil then
		print(msg)
		return
	end

	contents=fout:read("*a")
	fout:close()
	return contents
end

function util.writeFile(fn, contents)
	local fout, msg=io.open(fn, "w")
	if fout==nil then
		print(msg)
		return
	end

	fout:write(contents)
	fout:close()
end
function util.appendFile(fn, arg)
	local fout, msg=io.open(fn, "a")
	if fout==nil then
		print(msg)
		return
	end
	fout:write(arg)
	fout:close()   
end

util.outputToFileShort=util.appendFile
function util.mergeString(arg)
	local out=""
	for i,v in ipairs(arg) do
		if type(v)~="string" then
			out=out.."\t"..tostring(v)
		else
			out=out.."\t"..v
		end
	end
	return out
end

function os.findDotEmacsFolder()
	local candidates=
	{"C:\\Documents and Settings\\Administrator\\Application Data",
	"C:\\Documents and Settings\\sonah\\Application Data",
	"C:\\usr\\texlive\\HOME",
}
for i, candidate in ipairs(candidates) do
	if os.isFileExist(candidate.."\\.emacs") then
		return candidate
	end
end
return "c:\\DotEmacsNotFound"
end

function string.lines(str)
	local t = {}
	local function helper(line) table.insert(t, line) return "" end
	helper((str:gsub("(.-)\r?\n", helper)))
	return t
end

function string.tokenize(str, pattern)
	local t = {}
	local function helper(line) table.insert(t, line) return "" end
	helper((str:gsub("(.-)"..pattern, helper)))
	return t
end

function deepCopyTable(t)
	assert(type(t)=="table", "You must specify a table to copy")

	local result={}
	for k, v in pairs(t) do
		if type(v)=="table" then
			result[k]=deepCopyTable(v)
		elseif type(v)=="userdata" then
			result[k]=v:copy()
		else
			result[k]=v
		end
	end

	-- copy the metatable, if there is one
	return setmetatable(result, getmetatable(t))
end


function shallowCopyTable(t)
	assert(type(t)=="table", "You must specify a table to copy")

	local result={}

	for k, v in pairs(t) do
		result[k]=v
	end

	-- copy the metatable, if there is one
	return setmetatable(result, getmetatable(t))
end

function table.merge(t1, t2)
	local result={}
	for k,v in pairs(t1) do
		result[k]=v
	end
	for k,v in pairs(t2) do
		result[k]=v
	end
	return result
end
