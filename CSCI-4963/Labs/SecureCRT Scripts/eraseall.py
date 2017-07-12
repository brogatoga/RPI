# $language = "Python"
# $interface = "1.0"

# RPI Cisco Networking Academy
# Pod Connection Script for SecureCRT
# Created by Matthew Heffler and Joseph W. Dougherty
# Version 1.0

def main():

	j=0
	k=j
	
	while(j<crt.GetTabCount()):
		objCurrentTab = crt.GetTab(j+1)
		objCurrentTab.Activate()
		objCurrentTab.Screen.Synchronous=True
		objCurrentTab.Screen.Send(chr(26))
		objCurrentTab.Screen.Send("en\n\n")
		objCurrentTab.Screen.Send("delete vlan.dat\n\n\n")
		objCurrentTab.Screen.Send("copy running-config startup-config\n\n")
		objCurrentTab.Screen.Send("erase startup-config\n\n")
		objCurrentTab.Screen.Send("reload\n\n")
		objCurrentTab.Screen.Synchronous=False
		j=j+1
	
	crt.GetTab(k+1).Activate()
	
main()
