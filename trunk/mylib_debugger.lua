
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

	if false then
		-- use open buffer checking. doesn't work correctly if you use "l vf..." command.
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
	else
		return 'vim '..cc
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
