Dim $screenWidth=1280
Dim $screenHeight=800

MouseMove(-1*$screenWidth, -1*$screenHeight)
MouseMove(0.5*$screenWidth+180, 0.5*$screenHeight)

; Run stkmaker
Run("c:\Program Files\STAReTEK\STK Maker6.0\STKMaker.exe")

; Wait for the Notepad become active - it is titled "Untitled - Notepad" on English systems
WinWaitActive("[class:TFrmMain]")
Sleep(500)

; manga
MouseClick("primary", 0.5*$screenWidth+180, 0.5*$screenHeight, 1)

; import pictures (1083,124)
Dim $cx=0.5*$screenWidth-640
Dim $cy=0.5*$screenHeight-400

MouseClick("primary", $cx+1083, $cy+124,1)

; choose Desktop/_stk
Sleep(500)
Send("+{LEFT 3}")
Send("+{RIGHT 1}")
Send("_stk")
Send("{ENTER}")
Sleep(500)

;Next
MouseClick("primary", $cx+1107, $cy+753,1)
Send("{TAB}{TAB}")
Send($CmdLine[1])
Send("{TAB}")
Send($CmdLine[2])
MouseClick("primary", $cx+428, $cy+521,1)
Send("!s!y") ; save

WinWaitActive("[class:TFrm_MessageBox]")
MouseClick("primary", $cx+706, $cy+460,1)
Sleep(500)
; Close STKMaker
MouseClick("primary", $cx+1132, $cy+47,1)
Sleep(500)
Send("{TAB}{ENTER}")

WinWaitClose("[class:TFrmMain]")
