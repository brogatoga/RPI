# $language = "Python"
# $interface = "1.0"

# RPI Cisco Networking Academy
# Pod Connection Script for SecureCRT
# Created by Matthew Heffler and Joseph W. Dougherty
# Version 1.0

import os

def main():

	ldir = crt.Dialog.Prompt("Enter your log directory (e.g. C:\\configs or /home/user/configs) or leave empty for default:","Save Configuration","")
	if ldir == "":
		ldir = os.getcwd()
	
	j=0
	k=j
	
	while(j<crt.GetTabCount()):
		objCurrentTab = crt.GetTab(j+1)
		if objCurrentTab.Session.Logging:
			objCurrentTab.Session.Log(False)
		objCurrentTab.Activate()
		objCurrentTab.Screen.Synchronous=True
		objCurrentTab.Screen.Send(chr(21)) # Send ^U to erase the line so the command doesn't get executed
		objCurrentTab.Screen.Send(chr(26))
		objCurrentTab.Screen.Send("\n\n\n")
		objCurrentTab.Screen.Send("en\nterminal length 0\nshow run\n\n") # Disable the pager
		objCurrentTab.Session.LogFileName = os.path.join(ldir, (objCurrentTab.Caption + ".txt"))
		objCurrentTab.Screen.WaitForString("Building configuration...")
		objCurrentTab.Session.Log(True)
		objCurrentTab.Screen.WaitForString("#")
		objCurrentTab.Session.Log(False)
		objCurrentTab.Screen.Synchronous=False
		objCurrentTab.Screen.Send("terminal length 24\n") # Restore the pager to the default
		j=j+1
	
	crt.GetTab(k+1).Activate()
	
main()

