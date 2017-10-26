#!/usr/bin/python
import subprocess
import shlex
import sys
var = 1
shell_cmd = "sudo ./run2"
cmd = shlex.split(shell_cmd)
p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
while var == 1:
	num = raw_input("Enter your cmd :")
	print "you input: ", num
	if num == "tips":
	    str = "apagentui.ThinviewErrorTips####{\"code\":\"0001\",\"message\":\"xxxxxxx\"}"
	    output, err = p.communicate(str)
	    print(output.decode('utf-8'))
	    print('Exit code: ', p.returncode)
	if num == "netchange":
		str = "apagentui.ThinviewNetworkTypeChange####{\"current\":\"1\",\"pre\":\"2\"}"
		output, err = p.communicate(str)
	    print(output.decode('utf-8'))
	    print('Exit code: ', p.returncode)