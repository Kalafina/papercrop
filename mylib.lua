-- collection of utility functions that depends only on standard LUA. (no dependency on baseLib or mainLib)
-- all functions are platform independent

function os.capture(cmd, raw)
  local f = assert(io.popen(cmd, 'r'))
  local s = assert(f:read('*a'))
  f:close()
  if raw then return s end
  s = string.gsub(s, '^%s+', '') --trim left spaces
  s = string.gsub(s, '%s+$', '') --trim right spaces
  s = string.gsub(s, '[\n\r]+', ' ') 
  return s
end

function os.createDir(path)

   if os.isUnix() then
      os.execute('mkdir "'..path..'"')
   else
      os.execute("md "..string.gsub(path, '/', '\\'))
   end
end

function os.rename(name1, name2)

   if os.isUnix() then
      os.execute("mv "..name1.." "..name2)
   else
      local cmd=string.gsub("move "..name1.." "..name2, '/', '\\')
      print(cmd)
      os.execute(cmd)
   end
end

function os.deleteFiles(mask)

   if os.isUnix() then
      os.execute("rm "..mask)
   else
      os.execute("del "..string.gsub(mask, '/', '\\'))
   end
end

function os.parentDir(currDir)
   return os.rightTokenize(string.gsub(currDir, "\\","/"), "/")
end

function os.relativeToAbsolutePath(folder,currDir)

	if string.sub(folder, 1,1)~="/" then
		currDir=currDir or os.currentDirectory()
		while(string.sub(folder,1,2)=="..") do
			currDir=os.parentDir(currDir)
			folder=string.sub(folder,4)
		end
		while(string.sub(folder,1,1)==".") do
			folder=string.sub(folder,3)
		end
		if folder=="" then
			folder=currDir
		else
			folder=currDir.."/"..folder
		end
	end
	return folder
end
function os.absoluteToRelativePath(folder, currDir) -- param1: folder or file name
	assert(string.sub(folder,1,1)=="/")
	currDir=currDir or os.currentDirectory()
	local n_ddot=0
	while string.sub(folder,1,#currDir)~=currDir or currDir=="" do
		currDir=os.parentDir(currDir)
		n_ddot=n_ddot+1
	end
	local str=""
	for i=1,n_ddot do
		str=str.."../"
	end
	print(n_ddot, currDir)
	str=str..string.sub(folder,#currDir+2)
	return str
end

function os.currentDirectory()
	if os.isUnix() then
		return os.capture('pwd')
	else
		return os.capture('cd')
	end
end
function os.copyFile(mask)

   if os.isUnix() then
      os.execute("cp "..mask)
   else
      print("copy "..      string.gsub(mask, '/', '\\'))
      os.execute("copy "..      string.gsub(mask, '/', '\\'))
   end
end

-- copy folder a to folder b (assuming folder b doesn't exists yet.)
-- otherwise, behaviors are undefined.
-- os.copyResource("../a", "../b", {'%.txt$', '%.lua$'}) 
function os.copyRecursive(srcFolder, destFolder, acceptedExt)
	if string.sub(destFolder, -1,-1)=="/" then
		destFolder=string.sub(destFolder, 1, -2)
	end

	acceptedExt=acceptedExt or os.globParam.acceptedExt
	local backup=os.globParam.acceptedExt

	os.createDir(destFolder)
	-- first copy files directly in the source folder
	if true then
		os.globParam.acceptedExt=acceptedExt
		local files=os.glob(srcFolder.."/*")
		os.globParam.acceptedExt=backup

		for i,f in ipairs(files) do
			os.copyFiles(f, destFolder.."/"..string.sub(f, #srcFolder+2))
		end
	end

	-- recursive copy subfolders
	local folders=os.globFolders(srcFolder)
	
	for i, f in ipairs(folders) do
		os.copyRecursive(srcFolder.."/"..f, destFolder.."/"..f,  acceptedExt)
	end
end
function os.copyFiles(src, dest, ext) -- copy source files to destination folder and optinally change file extensions.
   if os.isUnix() then
      os.execute('cp "'..src..'" "'..dest..'"')
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

function os._globWin32(attr, mask, ignorepattern)
	local tbl=string.tokenize(os.capture("dir /b/a:"..attr.." "..string.gsub(mask, "/", "\\"), true), "\n")
	tbl[table.getn(tbl)]=nil
	local files={}
	local c=1
	local prefix=""
	--if string.find(mask, "/") then
		--prefix=os.parentDir(mask).."/"
	--else
		--prefix=""
	--end
	for i,fn in ipairs(tbl) do
		if string.sub(fn, string.len(fn))~="~" then
			if not (ignorepattern and string.isMatched(fn, ignorepattern)) then
				files[c]=prefix..fn 
				c=c+1
			end
		end
	end
	return files 
end
os.globParam={}
os.globParam.ignorePath={'^%.', '^CVS'}
os.globParam.acceptedExt={'%.txt$','%.pdf$', '%.lua$', '%.cpp$', '%.c$', '%.cxx$', '%.h$', '%.hpp$', '%.py$','%.cc$'}

function os.globFolders(path) -- os.globFolders('..') list all folders in the .. folder
	if path==nil or path=="." then
		path=""
	end
	
	if os.isUnix() then
		local path='"'..path..'"'	
		if path=='""' then path="" end
		local tbl=string.tokenize(os.capture('ls -1 -p '..path.." 2> /dev/null",true), "\n")
		local tbl2=array:new()
		local ignorepath=os.globParam.ignorePath
		for i,v in ipairs(tbl) do
			if string.sub(v,-1)=="/" then
				local fdn=string.sub(v, 1,-2)
				if not string.isMatched(fdn, ignorepath) then
					tbl2:pushBack(fdn) 
				end
			end
		end
		return tbl2
	else
		return os._globWin32('D', path, os.globParam.ignorePath)
	end
end

function os._processMask(mask)
	mask=string.gsub(mask, "#", "*")
	local folder, lmask=os.rightTokenize(mask, '/',true)
	local wildcardCheckPass=true
	if not string.find(lmask, '[?*]') then -- wild card
		folder=folder..lmask
		lmask="*"
	end
	if string.sub(folder, -1)~="/" and folder~="" then
		folder=folder.."/"
	end
	return folder,lmask 
end

function os.findgrep(mask, bRecurse, pattern)
	local printFunc={ 
		iterate=function (self, v)
			util.grepFile(v, pattern)
		end
	}	
	os.find(mask, bRecurse, true, printFunc)
end
function os.find(mask, bRecurse, nomessage, printFunc) 
	local printFunc=printFunc or { iterate=function (self, v) print(v) end}
	--mask=string.gsub(mask, "#", "*")
	--mask=string.gsub(mask, "%%", "*")
	if string.find(mask, "[#%%?%*]")==nil then
		-- check if this is a file not a folder
		if os.isFileExist(mask) then
			printFunc:iterate(mask)
			return 
		else
			fn,path=os.processFileName(mask)-- fullpath
			mask=path.."/*"..fn -- try full search
		end
	end
	if not nomessage then
		io.write('globbing '..string.sub(mask,-30)..'                               \r')
	end
	if bRecurse==nil then bRecurse=false end

	if os.isUnix() then 
		local folder, lmask=os.rightTokenize(mask, '/')
		if lmask=="*" then
			mask=folder
			folder, lmask=os.rightTokenize(mask, '/')
		end
		local containsRelPath=false
		if string.find(lmask, '[?*]') then -- wild card
			containsRelPath=true
		end
		local cmd='ls -1 -p '..mask..' 2>/dev/null'
		local tbl=string.tokenize(os.capture(cmd,true), "\n")
		local lenfolder=string.len(folder)
		--print(cmd,mask,#tbl,lenfolder)
		if lenfolder==0 then lenfolder=-1 end
		local acceptedExt=deepCopyTable(os.globParam.acceptedExt)

		if string.find(mask,"%*%.") then
			local idx=string.find(mask,"%*%.")+2
			acceptedExt[#acceptedExt+1]="%."..string.sub(mask,idx)..'$'
			--print(acceptedExt[#acceptedExt])
		end

		for i=1, table.getn(tbl)-1 do
			local v=tbl[i]
			if string.sub(v,-1)~="/" and string.isMatched(v, acceptedExt) then
				if containsRelPath then
					printFunc:iterate(v)
				else
					if string.sub(mask,-1)=="/" then
						printFunc:iterate(mask..v)
					else
						printFunc:iterate(mask.."/"..v)
					end
				end
			end
		end
	else
		local folder, lmask=os._processMask(mask)
		local out=os._globWin32("-d", folder..lmask)
		local acceptedExt=os.globParam.acceptedExt
		for i=1, table.getn(out) do
			if string.isMatched(out[i], acceptedExt) then
				printFunc:iterate(folder..out[i])
			end
		end
	end
	local verbose=false
	if bRecurse then
		local folder, lmask=os._processMask(mask)
		if verbose then print(folder, lmask) end
		local subfolders=os.globFolders(folder)
		if verbose then printTable(subfolders) end
		for i=1, table.getn(subfolders) do
			local v=subfolders[i]
			os.find(folder..v..'/'..lmask, true, nomessage, printFunc)
		end
	end
	if not nomessage then
		io.write('                                                                   \r')
	end
end
function os.glob(mask, bRecurse, nomessage) -- you can use # or % instead of *. e.g. os.glob('#.jpg')

	local tbl2=array:new()
	function tbl2:iterate(v)
		--print(v)
		self:pushBack(v)
	end
	os.find(mask, bRecurse, nomessage, tbl2)
	return tbl2
end

function os.rightTokenize(str, sep, includeSep)
	local len=string.len(str)
	for i=len,1,-1 do
		if string.sub(str, i,i)==sep then
			if includeSep then
				return string.sub(str, 1, i-1)..sep, string.sub(str, i+1)
			end
			return string.sub(str, 1, i-1), string.sub(str, i+1)
		end
	end
	return "", str
end

function os.sleep(aa)
   local a=os.clock()
   while os.difftime(os.clock(),a)<aa do -- actually busy waits rather then sleeps
   end
end

function os.VI_path()
	if os.isUnix() then
--		return "vim" 
		return "gvim"
	else
		return "gvim"
	end
end	

function os.vi_check(fn)
	local L = require "functional.list"
	local otherVim='vim'
	local servers=string.tokenize(os.capture(otherVim..' --serverlist 2>&1',true), "\n")
	local out=L.filter(function (x) return string.upper(fn)==x end,servers)
	local out_error=L.filter(function (x) return string.find(x,"Unknown option argument")~=nil end,servers)
	if #out_error>=1 then return nil end
	return #out>=1
end

function os.vi_console_close_all()
	local L = require "functional.list"
	local servers=string.tokenize(os.capture('vim --serverlist',true), "\n")
	local out=L.filter(function (x) return fn~="GVIM" end,servers)
	for i,v in ipairs(out) do
		os.execute('vim --servername "'..v..'" --remote-send ":q<CR>"')
	end
end

function os.vi_console_cmd(fn, line)
	local cc
	if line then
		cc=' +'..line..' "'..fn..'"'
	else
		cc=' "'..fn..'"'
	end

	local check_res=os.vi_check(fn)
	if check_res then
		print('already open')
		-- open the already open window.. (not very useful)
		--return 'vim --servername "'..fn..'" --remote '..cc
		-- open viewer mode
		return os.vi_readonly_console_cmd(fn, line)
	elseif check_res==nil then
		-- doesn't support servername
		return 'vim '..cc
	else
		return 'vim --servername "'..fn..'" '..cc
	end
end

function os.vi_readonly_console_cmd(fn, line)
	local cc
	if line then
		cc=' +'..line..' "'..fn..'"'
	else
		cc=' "'..fn..'"'
	end

	return 'vim -R -M -c ":set nomodifiable" '..cc
end
function os.vi_line(fn, line)
	if os.vi_check(fn) then
		execute(os.vi_console_cmd(fn,line))
		return
	end
	os.launch_vi_server()
	local VI=os.VI_path()..' --remote-silent'
	local cmd=VI..' +'..line..' "'..fn..'"'
	--print(cmd)
	execute(cmd)
end
function os.launch_vi_server()
	local lenvipath=string.len(os.VI_path())
	if os.vi_check(string.upper(os.VI_path())) then
		print("VI server GVIM open")
		return
	end
	print("launching GVIM server...")
	if os.isUnix() then
		if os.VI_path()=="vim" then
			execute('cd ../..', 'gnome-terminal -e "vim --servername vim"') -- this line is unused by default. (assumed gnome dependency)
		else
			execute('cd ../..', os.VI_path())
		end
	else
		if os.isFileExist(os.capture('echo %WINDIR%').."\\vim.bat") then
			execute('cd ..\\..', os.VI_path())
		else
			execute('cd ..\\..', "start "..os.VI_path())
		end
	end
	
	for i=1,10 do
		if os.vi_check(string.upper(os.VI_path())) then
			print("VI server GVIM open")
			break
		else
			print('.')
			--os.sleep(1)
		end
	end
end
function os.home_path()
	return os.capture("echo %HOMEDRIVE%")..os.capture("echo %HOMEPATH%")
end

function string.isMatched(str, patterns)
	local matched=false
	for k,ip in ipairs(patterns) do
		local idx=string.find(str, ip)
		if idx~=nil then
			matched=true
		end
	end
	return matched
end
function string.findLastOf(str, pattern)
	local lastS=nil
	local idx=0
	while idx+1<#str do
		idx=string.find(str, pattern, idx+1)
		if idx then
			lastS=idx
		else
			break
		end
	end
	return lastS
end
function os.processFileName(target)-- fullpath
	local target=string.gsub(target, '\\', '/')
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

	return filename, path
end

function os.vi(...)
	os._vi(os.VI_path(), ...)
end

function os._vi(servername, ...)
	local VI=os.VI_path() ..' --servername '..servername..' --remote-silent'
	local VI2=os.VI_path() ..' --servername '..servername..' --remote-send ":n '
	local VI3='<CR>"'

	local targets={...}

	local otherVim='vim'

	local vicwd=os.capture(otherVim..' --servername '..servername..' --remote-expr "getcwd()"')
	if vicwd=="" then
		os.launch_vi_server()
		-- try one more time
		vicwd=os.capture(otherVim ..' --servername '..servername..' --remote-expr "getcwd()"')
	end
	print('vicwd=',vicwd)
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
			elseif string.sub(target, 1,6)=="../../" and string.sub(vicwd, -10)=="taesoo_cmu" then -- fastest method 
				local cmd=VI2..string.sub(target,7)..VI3
				print(cmd)
				if os.isUnix() then
					os.execute(cmd.."&")
				else
					os.execute(cmd)
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
				if os.isUnix() then
					execute("cd "..path, "rm -f *.lua~", VI.." "..filename.."&")
				else
					execute("cd "..path, "rm -f *.lua~", "rm -f #*#", VI.." "..filename)
				end
				--	    end

			end

		else
			local cmd=VI..' "'..target..'"'
			print(cmd)
			if os.isUnix() then
				os.execute(cmd.."&")
			else
				os.execute(cmd)
			end
		end
	end
end
function os.emacs(...)
	local targets={...}

	local EMACS
	if os.isUnix() then
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

function os.isUnix() 
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

function array:pushBack(...)
	for i, x in ipairs({...}) do
		table.insert(self, x)
	end
end

function array:concat(tbl)
	for i, v in ipairs(tbl) do 
		table.insert(self, v)
	end
end

function array:removeAt(i)
	table.remove(self, i)
end


function array:assign(tbl)
   for i=1,table.getn(tbl) do
      self[i]=tbl[i]
   end
end
function array:remove(...)
   local candi={...}
   if type(candi[1])=='table' then
      candi=candi[1]
   end

   local backup=array:new()
   for i=1,table.getn(self) do
      backup[i]={self[i], true}
   end
   
   for i, v in ipairs(candi) do
      backup[v][2]=false
   end

   local count=1
   for i=1, table.getn(backup) do
      if backup[i][2] then
	 self[count]=backup[i][1]
	 count=count+1
      end
   end

   for i=count, table.getn(backup) do
      self[i]=nil
   end

end

function array:back()
	return self[table.getn(self)]
end

function string.join(tbl, sep)
	return table.concat(tbl, sep)
end

function string.isOneOf(str, ...)
	local tbl={...}
	for i,v in ipairs(tbl) do
		if str==v then
			return true
		end
	end
	return false
end

-- similar to string.sub
function table.isubset(tbl, first, last)

	if last==nil then last=table.getn(tbl) end

	local out={}
	for i=first,last do
		out[i-first+1]=tbl[i]
	end
	return out
end

function table.find(tbl, x)
	for k, v in pairs(tbl) do
		if v==x then 
			return k
		end
	end
	return nil
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

function pairsByKeys (t, f)
   local a = {}
   for n in pairs(t) do table.insert(a, n) end
   if f==nil then
	   f=function (a,b) -- default key comparison function
		   if type(a)==type(b) then
			   return a<b
		   end
		   return type(a)<type(b)
	   end
   end
   table.sort(a, f)
   local i = 0      -- iterator variable
   local iter = function ()   -- iterator function
		   i = i + 1
		   if a[i] == nil then return nil
		   else return a[i], t[a[i]]
		   end
		end
   return iter
end

function printTable(t, bPrintUserData)
	local out="{"
	for k,v in pairsByKeys(t) do
		local tv=type(v)
		if tv=="string" or tv=="number" or tv=="boolean" then
			out=out..'['..k..']='..tostring(v)..', '
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


function table.fromstring(t_str)
	local fn=loadstring("return "..t_str)
	if fn then
		local succ,msg=pcall(fn)
		if succ then
			return msg
		else
			print('pcall failed! '..t_str..","..msg)
		end
	else
		print('compile error')
	end
	return nil
end

function table.tostring2(t)
	return table.tostring(util.convertToLuaNativeTable(t))
end
function table.fromstring2(t)
	return util.convertFromLuaNativeTable(table.fromstring(t))
end
function table.tostring(t)
	-- does not check reference. so infinite loop can occur.  to prevent
	-- such cases, use pickle() or util.saveTable() But compared to pickle,
	-- the output of table.tostring is much more human readable.  if the
	-- table contains userdata, use table.tostring2, fromstring2 though it's
	-- slower.  (it preprocess the input using
	-- util.convertToLuaNativeTable 
	-- a=table.tostring(util.convertToLuaNativeTable(t)) convert to
	-- string t=util.convertFromLuaNativeTable(table.fromstring(a)) 
	-- convert back from the string)

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

	if not os.isUnix() then
		if echoOff then
			fout:write("@echo off\necho off\n")
		end
		fout:write("setlocal\n")
	end
	for i,c in ipairs(list) do
		fout:write(c.."\n")
	end
	fout:close()
end

function os.execute2(...) -- excute multiple serial operations
	execute(...)
end

function os.pexecute(...) -- excute multiple serial operations
	if os.isUnix() then
		execute(...)
	else
		local list={...}
		createBatchFile("_temp.bat",list)      
		os.execute("start _temp.bat")	
	end      
end

-- escape so that it can be used in double quotes
function os.shellEscape(str)
	if os.isUnix() then
		str=string.gsub(str, '\\', '\\\\')
		str=string.gsub(str, '"', '\\"')
		str=string.gsub(str, '%%', '\\%%')
	else
		str=string.gsub(str, '\\', '\\\\')
		str=string.gsub(str, '"', '\\"')
		str=string.gsub(str, '%$', '\$')
	end
	return str
end

function os.luaExecute(str, printCmd)
	local luaExecute
	local gotoRoot
	local endMark
	local packagepath=os.shellEscape('package.path="./OgreFltk/Resource/scripts/ui/?.lua;./OgreFltk/work/?.lua"')
	if os.isUnix() then
		luaExecute="lua -e \""..packagepath.."dofile('OgreFltk/work/mylib.lua');"
		gotoRoot="cd ../.."
		endMark="\""
	else
		luaExecute="OgreFltk\\work\\lua -e dofile('OgreFltk/work/mylib.lua');"
		gotoRoot="cd ..\\.."
		endMark=""
	end
	str=os.shellEscape(str)
	if printCmd then print(luaExecute..str..endMark) end
	execute(gotoRoot, luaExecute..str..endMark)
end
-- deprecated (the same as os.execute2)
function execute(...)
	local list={...}
	if not math.seeded then
		math.randomseed(os.time())
		math.seeded=true
	end
	if os.isUnix() then
		if #list<3 then
			local cmd=""
			for i,c in ipairs(list) do
				cmd=cmd..";"..c
			end
			--print(string.sub(cmd,2))
			os.execute(string.sub(cmd, 2))
		else
			local fn='temp/_temp'..tostring(math.random(1,10000))
			createBatchFile(fn, list)
			os.execute("sh "..fn)
			os.deleteFiles(fn)
		end
	else
		createBatchFile("_temp.bat",list,true)
		--      os.execute("cat _temp.bat")
		os.execute("_temp.bat")	
	end
end

util=util or {}

function util.chooseFirstNonNil(a,b,c)
	if a~=nil then return a end
	if b~=nil then return b end
	return c
end
function util.convertToLuaNativeTable(t)
	local result={}
	if type(t)=="userdata" then
		result=t:toTable() -- {"__userdata", typeName, type_specific_information...}
	elseif type(t)=="table" then
		for k, v in pairs(t) do
			if type(v)=="table" or type(v)=="userdata" then
				result[k]=util.convertToLuaNativeTable(v)
			else
				result[k]=v
			end
		end
	else
		result=t
	end
	return result
end

function util.convertFromLuaNativeTable(t)
	local result
	if type(t)=="table" then
		if t[1]=="__userdata" then
			result=_G[t[2]].fromTable(t)
		else
			result={}
			for k, v in pairs(t) do
				result[k]=util.convertFromLuaNativeTable(v)
			end
		end
	else
		result=t
	end

	return result
end

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

function util.iterateFile(fn, printFunc)
	printFunc=printFunc or
	{
		iterate=function (self,lineno, line) 
			print(lineno, line)
		end
	}
	local fin, msg=io.open(fn, "r")
	if fin==nil then
		print(msg)
		return
	end
	local ln=1
	--local c=0
	--local lastFn, lastLn
	for line in fin:lines() do
		printFunc:iterate(ln,line)
		ln=ln+1
	end
	fin:close()
end

function util.grepFile(fn, pattern, prefix,raw, printFunc)
	printFunc=printFunc or 
	{ 
		iterate=function(self,fn,ln,idx,line)
			print(fn..":"..ln..":"..string.trimLeft(line))
		end
	}
	prefix=prefix or ""
	pattern=string.lower(pattern)
	local fin, msg=io.open(fn, "r")
	if fin==nil then
		print(msg)
		return
	end
	local ln=1
	--local c=0
	--local lastFn, lastLn
	for line in fin:lines() do
		local lline=string.lower(line)
		local res, idx
		if raw then
			res,idx=pcall(string.find, lline, pattern)
		else
			res,idx=pcall(string.find, lline, pattern, nil,true)
		end
		if res and idx then 
			--				print(prefix..fn..":"..ln..":"..idx..":"..string.trimLeft(line))
			printFunc:iterate(prefix..fn,ln,idx,line)
			--c=c+1
			--lastFn=prefix..fn
			--lastLn=ln
		end
		ln=ln+1
	end
	fin:close()
	--if c==1 then
		--os.vi_line(lastFn, lastLn)
	--end
end
function util.grep(mask, pattern, prefix, bRecurse,raw) -- deprecated. use util.findgrep
	local list=os.glob(mask, bRecurse, true)
	for i, fn in ipairs(list) do
		util.grepFile(fn,  pattern, prefix,raw)
	end
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
function string.trimLeft(str)
	a=string.find(str, '[^%s]')
	if a==nil then
		return ""
	end
	return string.sub(str, a)
end
function string.trimRight(str)
	--a=string.find(str, '[%s$]')-- doesn't work
	a=string.find(str, '[%s]',#str-1)
	if a==nil then return str end
	return string.trimRight(string.sub(str,1,a-1))
end

function deepCopyTable(t)
	assert(type(t)=="table", "You must specify a table to copy")

	local result={}
	for k, v in pairs(t) do
		if type(v)=="table" then
			result[k]=deepCopyTable(v)
		elseif type(v)=="userdata" then
			if v.copy then
				result[k]=v:copy()
			else
				print('Cannot copy '..k)
			end
		else
			result[k]=v
		end
	end

	-- copy the metatable, if there is one
	return setmetatable(result, getmetatable(t))
end

function util.copy(b) -- deep copy
	if type(b)=='table' then
		local result={}
		for k,v in pairs(b) do
			result[k]=util.copy(v)
		end
		-- copy the metatable, if there is one
		return setmetatable(result, getmetatable(b))
	elseif type(b)=='userdata' then
		return b:copy()
	else
		return b
	end
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

function table.count(a)
	local count=0
	for k,v in pairs(a) do
		count=count+1
	end
	return count
end

-- t1 and t2 will be shallow copied. you can deepCopy using deepCopy(table.merge(...))
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

-- note that t2 will be deep copied because it seems to be more useful (and safe)
function table.mergeInPlace(t1,t2, overwrite) -- t1=merge(t1,t2)
	for k,v in pairs(t2) do
		if overwrite or t1[k]==nil then
			t1[k]=util.copy(v)
		end
	end	
end

dbg={defaultLinecolor="solidred", linecolor="solidred", _count=1}

function os.open(t)
	if os.isUnix() then
		os.execute('gnome-open '..t)
	else
		os.execute('start cmd/c '..t)
	end
end

function os.print(t)
   if type(t)=="table" then
      printTable(t)
   else
      dbg.print(t)
   end
end

function dbg.print(...)
	local arr={...}
	for k,v in ipairs(arr) do
		if type(v)=='userdata' then
			local info=class_info(v)
			if info.methods.__tostring then
				print(v)
			else
				print('userdata which has no __tostring implemented:')
				util.printInfo(v)
			end
		else
			print(v)
		end
	end
end
function dbg.showCode(fn,ln)
	util.iterateFile(fn,
	{ 
		iterate=function (self, lineno, c) 
			if lineno>ln-5 and lineno<ln+5 then
				if lineno==ln then
					print(lineno.."* "..c)
				else
					print(lineno.."  "..c)
				end
			end
		end
	}
	)
end

function dbg.console(msg, stackoffset)

	stackoffset=stackoffset or 0
	if(msg) then print (msg) end
	if dbg._consoleLevel==nil then
		dbg._consoleLevel=0
	else
		dbg._consoleLevel=dbg._consoleLevel+1
	end
	local function at(line, index)
		return string.sub(line, index, index)
	end

	local function handleStatement(statement)

		local output
		if string.find(statement, "=") and not string.find(statement, "==") then -- assignment statement
			output={pcall(loadstring(statement))}
		else -- function calls or print variables: get results
			output={pcall(loadstring("return ("..statement..")"))}
			if output[1]==false and output[2]=="attempt to call a nil value" then
				-- statement
				output={pcall(loadstring(statement))}
			end
		end

		if output[1]==false then 
			print("Error! ", output[2]) 
		else
			if type(output[1])~='boolean' then
				output[2]=output[1] -- sometimes error code is not returned for unknown reasons.
			end
			if type(output[2])=='table' then printTable(output[2]) 
			elseif output[2] then
				dbg.print(unpack(table.isubset(output, 2)))
			elseif type(output[2])=='boolean' then
				print('false')
			end
		end
	end

	local event
	while true do
		io.write("[DEBUG"..dbg._consoleLevel.."] > ")
		line=io.read('*line')
		local cmd=at(line,1)
		local cmd_arg=tonumber(string.sub(line,2))
		if not (string.sub(line,2)=="" or cmd_arg) then
			if not ( cmd=="r" and at(line,2)==" ") then
				if not string.isOneOf(cmd, ":", ";") then
					cmd=nil
				end
			end
		end

		if cmd=="h" or string.sub(line,1,4)=="help" then --help
			print('cs[level=3]      : print callstack')
			print(';(lua statement) : eval lua statements. e.g.) ;print(a)')
			print(':(lua statement) : eval lua statements and exit debug console. e.g.) ;dbg.startCount(10)')
			print('s[number=1]      : proceed n steps')
			print('r filename [lineno]  : run until execution of a line. filename can be a postfix substring. e.g.) r syn.lua 32')
			print('e[level=2]       : show current line (at callstack level 2) in emacs editor')
			print('v[level=2]       : show current line (at callstack level 2) in vi editor')
			print('c[level=2]       : show nearby lines (at callstack level 2) here')
			print('l[level=0]       : print local variables. Results are saved into \'l variable.')
			print("                   e.g) DEBUG]>print('l.self.mVec)")
			print('cont             : exit debug mode')
			print('lua variable     : print contents; Simply typing "a" print the content of the variable "a".')
			print('lua statement    : run it')
		elseif line=="cont" then break
		elseif string.sub(line,1,2)=="cs" then dbg.callstack(tonumber(string.sub(line,3)) or 3)
		elseif cmd=="v" then
			local info=debug.getinfo((cmd_arg or 1)+1+stackoffset)
			if info and info.source=="=(tail call)" then
				info=debug.getinfo((cmd_arg or 1)+2+stackoffset)
			end
			if info then
				--os.vi_line(string.sub(info.source,2), info.currentline)
				local fn=string.sub(info.source,2)
				fn=os.relativeToAbsolutePath(fn)
				fn=os.absoluteToRelativePath(fn, os.relativeToAbsolutePath("../.."))
				os.luaExecute([[os.vi_line("]]..fn..[[",]]..info.currentline..[[)]])
			end
		elseif cmd=="c" then
			local level=(cmd_arg or 1)+1+stackoffset
			local info=debug.getinfo(level)
			if info and info.source=="=(tail call)" then
				level=level+1
				info=debug.getinfo(level)
			end
			if info then
				local ln=info.currentline
				print(string.sub(info.source,2))
				dbg.showCode(string.sub(info.source,2),ln)
				dbg._saveLocals=dbg.locals(level+1,true)
			else
				print('no such level')
			end
		elseif cmd=="e" then
			local info=debug.getinfo((cmd_arg or 1)+1+stackoffset)
			local cmd="emacsclient -n +"..info.currentline..' "'..string.sub(info.source,2)..' "'
			print(cmd)
			os.execute(cmd)
		elseif cmd==";" then
			handleStatement(string.sub(line,2))
		elseif cmd==":" then
			handleStatement(string.sub(line,2))
			break
		elseif cmd=="s" or cmd=="'" then
			local count=cmd_arg or 1
			event={"s", count}
			break
		elseif cmd=="r" then
			event={"r", string.sub(line, 3)}
			break
		elseif cmd=="l" then
			local level=(cmd_arg or 1)+2
			dbg._saveLocals=dbg.locals(level)
		else 
			statement=string.gsub(line, '``', 'dbg._saveLocals')
			statement=string.gsub(line, '`', 'dbg._saveLocals.')
			handleStatement(statement)
		end
	end

	dbg._consoleLevel=dbg._consoleLevel-1
	if event then
		if event[1]=="s" then 
			return dbg.step(event[2]) 
		elseif event[1]=="r" then
			return dbg.run(event[2])
		end
	end

end

function dbg.callstack(level)
	if level==nil then
		level=4
	end
	while true do
		local info=debug.getinfo(level)
		local k=info.name
		if k==nil then
			break
		else
			print(info.short_src..":"..info.currentline..":"..k)
			level=level+1
		end
	end
end


function dbg.locals(level, noprint)
	local output={}
	if level==nil then level=4 end
	cur=1
	while true do
		if debug.getinfo(level, 'n')==nil then return output end
		k,v=debug.getlocal(level, cur)
		if k~=nil then
			output[k]=v or "(nil)"
			cur=cur+1
		else
			break
		end
	end
	if not noprint then
		os.print(output)
	end
	return output
end
