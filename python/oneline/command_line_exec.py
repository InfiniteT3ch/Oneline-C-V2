## Oneline V2 
## @author Nadir Hamid
## @license MIT

import subprocess
import psutil
import json
import sys
import os


class Oneline_V2_CommandLine_Log(object):
  @staticmethod 
  def log( msg,  data):
     print "ONELINE COMMAND LINE: %s" %(msg)
     print "ONELINE COMMAND LINE: %s" %(data)
  @staticmethod
  def write(msg):
     print "Oneline: %s\n" %( msg )
  @staticmethod
  def stdout(msg):
     print "%s"%(msg)

## hold the current server and its PID
##
class Oneline_V2_CommandLine_Server(object):
   def __init__(self,ip="", port=000):
      current_pid = self.get_pid()
      if  current_pid:
         settings=self.get_settings( pid )
   	 self.ip=settings['ip']
	 self.port = int( settings['port'] )
      else:
	 self.ip =ip 
	 self.port=port
   def get_settings( self,  pid ):
     settings  = json.loads( open("/usr/local/onelinev2/oneline.json", "r").read())
     return settings
	
   def start(self, *args, **kwargs):
      os.chdir('/usr/local/onelinev2/')
      self.pid = subprocess.Popen(['oneline',self.ip, self.port])
      self.write_pid( self.pid )
      self.write_settings( self.ip,  self.port )
   def stop( self, *args, **kwargs):
      pid = self.get_pid()
      process = psutil.Process(int(pid))   
      status =process.status()
      if not status==psutil.PSUTIL_ZOMBIE:
        os.kill(pid, 0)
        os.remove("/usr/local/onelinev2/oneline.pid")
   def restart(self, *args, **kwargs):
     self.stop()
     self.start()
	   
   def write_pid( self, pid ):
      pid_file=open("/usr/local/onelinev2/oneline.pid","w+")
      pid_file.write(pid)
      pid_file.close()
   def write_settings(  self, ip,  port ):
      settings_file=open("/usr/local/onelinev2/oneline.json","w+")
      settings_file.write(json.dumps(dict(
		ip=ip,
		port=port
	  )))
      settings_file.close()
   def get_pid( self ):
      if os.path.isfile("/usr/local/onelinev2/oneline.pid"):
         pid = int( open("/usr/local/onelinev2/oneline.pid","r").read())
    	 return pid
      return None

class Oneline_V2_CommandLine_Client(object):
   def pack(self,*args,**kwargs):
      pass
   def controller(self,*args,**kwargs):
      pass
   def init(self,*args,**kwargs):
      pass


class Oneline_V2_CommandLine(object):
  def __init_(self,arguments=[]):
     self.arguments=arguments

     self.server_cmds=[]
     self.client_cmds=[]
     self.called_help=False
	 
     self.known_opts=dict(
	server=[
		dict( cmd='--start',  needs_arg=False, callable=True ),
		dict(  cmd='--stop',  needs_arg=False, callable=True),
	  	dict( cmd='--restart', needs_arg=False, callable=True ),
		dict( cmd='--ip',  needs_arg=True, callable=False ),
		dict( cmd='--port', needs_arg=True, callable=False)
		],
    	client=[
		dict( cmd='--pack', needs_arg=True, callable=True),
		dict( cmd='--controller', needs_arg=True, callable=True ),
		dict( cmd='--init', needs_arg=True, callable=True)
	   ]
       )
     self.run()
  def  run( self ):
     for i in range(0, len( self.arguments) ):
        try:
           next_argument=  self.arguments[i +  1]
	except Exception, e:
   	   next_argument = self.arguments[i]
        this_argument=self.arguments[ i ] 
        server_arg = self.type_of_cmd("server", this_argument)
        if server_arg:
           self.server_cmds.append( dict(
	 	arg=server_arg,
		key=this_argument,
		val=next_argument
		 ))
        client_arg=self.type_of_cmd("client", this_argument)
	if client_arg:
	   self.client_cmds.append( dict(
		 arg=client_arg,
		key=this_argument,
		val=next_argument
	  	))
        if this_argument=="--help":
	    self.called_help =True
     self.run_server_cmds()
     self.run_client_cmds()
     if self.called_help or (len(self.client_cmds)==0 and len(self.server_cmds) == 0) :
	  self.help()
  def help( self  ):
        help_string="""
	Welcome to Oneline C-V2 Command Line Help
 	server options:
	  --start: starts the  server
	  --stop: stops the server
	  --restart: restarts the server
         client options:
	  --pack:  pack an existing oneline module
	  --init: initialize a new oneline module
	  --controller: run controller commands
	 """

        Oneline_V2_CommandLine_Log.stdout( help_string )

  def type_of_cmd(self, check1, arg):
       for i in self.known_opts['client']:
	   if  arg==i  and check1== 'client':
	  	return  i
       for i in self.known_opts['server']:
	   if arg==i  and check1 == 'server':
 	  	return i
       return False
	  
  def run_server_cmds( self ):
       self.server=Oneline_V2_CommandLine_Server()
       for  i in self.server_cmds:
	    self.process_cmd(self.server, i) 
  def run_client_cmds(self):
       self.client=Oneline_V2_CommandLine_Client()
       for i in  self.client_cmds:
	    self.process_cmd(self.client, i)
  def process_cmd(self,instance,cmd):
       if cmd['needs_arg']:
	  setattr(instance,cmd['key'],cmd['val'])
       if cmd['callable']:
	  callable_fn = getattr( instance,cmd['key'])
	  callable_fn( cmd['val'] )
          
	  
  
if __name__ == "__main__":  		     
  Oneline_V2_CommandLine(sys.argv)
   
      
     
 

