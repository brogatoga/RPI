# $language = "Python"
# $interface = "1.0"

# RPI Cisco Networking Academy
# Pod Connection Script for SecureCRT
# Created by Matthew Heffler and Joseph W. Dougherty
# Version 1.0

def main():

	# Tab captions
	captions = ["R1","R2","R3","R4","R5","R6","FRS","S1","S2","S3","S4"]
	
	# Get pod number
	pod = crt.Dialog.Prompt("Enter your pod number","Connect to pod","")
	if pod == "":
		return
	
	# Get pod password
	linepw = crt.Dialog.Prompt("Enter line password:","Device Authentication","",True)					
	if linepw == "": 
		return
	
	
	# Store offset for opening additional pods
	j=crt.GetTabCount()
	
	if crt.GetTab(j).Caption == "Script Tab" or crt.GetTab(j).Caption == "":
		j=0
	k=j
	
	# Loop through each port to open connection
	i=2001
	while(i<=2012):
		# Skip 2008
		if i==2008:
			i=i+1
		crt.Session.ConnectInTab("/TELNET 128.213.10." + str(100+int(pod)) + " " + str(i))
		i=i+1
	
	# Cycle back through and apply captions and passwords
	while(j<crt.GetTabCount()):
		objCurrentTab = crt.GetTab(j+1)
		objCurrentTab.Activate()
		objCurrentTab.Caption = "P"+str(pod)+captions[j-k]
		objCurrentTab.Screen.Send(linepw)
		objCurrentTab.Screen.Send("\n\n")
		objCurrentTab.Screen.Send("no");
		objCurrentTab.Screen.Send("\n\n");
		objCurrentTab.Screen.Send("en");
		objCurrentTab.Screen.Send("\n\n");
		j=j+1
	
	# And focus on the first tab in the new connection
	crt.GetTab(k+1).Activate()
	
main()
