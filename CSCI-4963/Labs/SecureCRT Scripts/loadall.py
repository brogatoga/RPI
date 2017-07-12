# $language = "Python"
# $interface = "1.0"

# RPI Cisco Networking Academy
# Pod Connection Script for SecureCRT
# Created by Matthew Heffler and Joseph W. Dougherty
# Modified by Marc Aldorasi to be a loading script
# Bugs: IOS will beep on ^Z until conf t mode is entered, then it always works fine from then on.
# Version 1.1

import os

def main():

	ldir = crt.Dialog.Prompt("Enter your log directory (e.g. C:\\configs or /home/user/configs) or leave empty for default:","Load Configuration","")
	if ldir == "":
		ldir = os.getcwd()
	
	j=0
	k=j

	
	while(j<crt.GetTabCount()):
		objCurrentTab = crt.GetTab(j+1)
		objCurrentTab.Activate()
		objCurrentTab.Screen.Send(chr(21)) # Send ^U to erase the line so the command doesn't get executed
		objCurrentTab.Screen.Send(chr(26)) # This beeps in a laggy fashion on first start.
		objCurrentTab.Screen.Send("\n\n\n")
		objCurrentTab.Screen.Send("en\nterminal length 0\nconf t\n\n") # Disable the pager
		objCurrentTab.Screen.Synchronous=True
		fname = os.path.join(ldir, (objCurrentTab.Caption + ".txt"))
		for line in open(fname, 'r'): # The file will automatically be closed at the end of this block or if there is an error
			sline = line.strip(" ")
			if sline and not sline.startswith(("!","B","C","*")): # Check that sline is nonempty and doesn't start with !, B, C, or *.  Every useless string we send is a round trip to the server, which is slow
				# B starts Building configuration, C starts Current configuration, and * starts status messages that might have crept into the config (Configured from console by console is likely to do so if you were in conf t when you ran saveall)
				objCurrentTab.Screen.Send(sline) # sline includes a trailing newline
				objCurrentTab.Screen.WaitForString("#")
				# Be clever and only do this on routers, not switches
				if sline.startswith("interface") and objCurrentTab.Caption[-2:-1]=="R":
					objCurrentTab.Screen.Send("no shutdown\n")
					objCurrentTab.Screen.WaitForString("#")
		objCurrentTab.Screen.Synchronous=False
		objCurrentTab.Screen.Send(chr(21)) # Send ^U to erase the line incase something failed and there's still text in the buffer
		objCurrentTab.Screen.Send("terminal length 24\n") # Restore the pager to the default
		j=j+1
	
	crt.GetTab(k+1).Activate()
	
main()

