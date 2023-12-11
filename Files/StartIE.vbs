Set objIE = CreateObject("InternetExplorer.Application")
objIE.Navigate WScript.Arguments(0)
objIE.Visible = 1