#! /usr/bin/python

## Oneline V2 
## @author Nadir Hamid
## @license MIT

import subprocess
import psutil
import json
import sys
import os
import re
from cookiecutter.main import cookiecutter


class Oneline_V2_CommandLine_Errors(object):
  CLIENT_NAME_NOT_USABLE="Name must consist of alphanumerics, numbers and underscores"
  ARGS_TOO_LESS="Not enough arguments were given"
  CONTROLLER_MISSING="Controller was not found"
  CONTROLLER_FN_MISSING="Controller function was not found"
  CONTROLLER_FN_UNCALLABLE="Controller function is not callable"
class Oneline_V2_CommandLine_Messages(object):
  PACKING_MODULE="Packing module.."
  CREATING_MODULE="Creating module.." 
  REMOVING_MODULE="Removing module.."
  USING_CONTROLLER="Using controller.."
class Oneline_V2_CommandLine_Defines(object):
  COOKIECUTTER_REPO = "https://github.com/InfiniteT3ch/Oneline-C-V2-Template.git"
  
   

class Oneline_V2_CommandLine_Log(object):
  @staticmethod 
  def log( msg,  data):
     print "ONELINE COMMAND LINE: %s" %(msg)
     print "ONELINE COMMAND LINE: %s" %(data)
  @staticmethod
  def write(msg):
     print "Oneline: %s\n" %( msg )
  @staticmethod
  def error(msg):
     print "Oneline: ERROR %s \n"% (msg)
  @staticmethod
  def stdout(msg):
     print "%s"%(msg)

def Oneline_V2_CommandLine_Not_Implemented():
   Oneline_V2_CommandLine_Log(
        "This has not yet been implemented.. please check https://github.com/nadirhamid/oneline-v2 for more"
     )
class Oneline_V2_CommandLine_util(object):
  @staticmethod
  def getCookieCutter(name):
      return cookiecutter( Oneline_V2_CommandLine_Defines.COOKIECUTTER_REPO, 
	 checkout=None, no_input=False, extra_context={"repo_name": name }, overwrite_if_exists=True )

## hold the current server and its PID
##
class Oneline_V2_CommandLine_Server(object):
   def __init__(self,ip="0.0.0.0", port=9000):
      current_pid = self.get_pid()
      if  current_pid:
         settings=self.get_settings( )
   	 self.ip=settings['ip']
	 self.port = int( settings['port'] )
      else:
	 self.ip =ip 
	 self.port=port
   def get_settings( self ):
     settings  = json.loads( open("/usr/local/onelinev2/oneline.json", "r").read())
     return settings
	
   def start(self, *args, **kwargs):
      #os.chdir('/usr/local/onelinev2/')
      self.process = subprocess.Popen(['/usr/bin/onelinev2-server'])
      self.write_pid( str(self.process.pid) )
      self.write_settings( self.ip,  self.port )
   def stop( self, *args, **kwargs):
      pid = self.get_pid()
      process = psutil.Process(int(pid))   
      status =process.status()
      process.kill()
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
   def __init__(self):
      pass
   def _check_name(self, module_name):
     reg_match=re.compile("[a-z0-9_]+")
     return False if not re.findall(reg_match,module_name) else True

   def _files(self, name):
     files= [
                dict(
                name="{0}.py".format(name), placeholder="/usr/local/onelinev2/placeholders/module.py",
                link="/usr/local/onelinev2/modules/{0}.py".format(name)
                ),
                dict(
                name="{0}.conf".format(name), placeholder="/usr/local/onelinev2/placeholders/config.conf",
                link="/usr/local/onelinev2/conf/{0}".format(name)
                ),
                dict( 
                name="{0}.controller.py".format(name),  placeholder="/usr/local/onelinev2/placeholders/controller.py",
                link="/usr/local/onelinev2/controllers/{0}.controller.py".format(name)
                ),
                dict(
                name="{0}.html".format(name), placeholder="/usr/local/onelinev2/placeholders/html.html",
                link="/usr/local/onelinev2/html/{0}.html".format(name)
                ),
                dict(
                name="{0}.js".format(name), placeholder="/usr/local/onelinev2/placeholders/javascript.js",
                link="/usr/local/onelinev2/js/{0}.js".format(name)
                ),
                dict(
                name="{0}.css".format(name), placeholder="/usr/local/onelinev2/placeholders/css.css",
                link="/usr/local/onelinev2/css/{0}.css".format(name)
                )
           ]
     return files

   def _libraries(self):
        libraries =[
             dict(
                name="oneline.min.js", placeholder="/usr/local/onelinev2/lib/oneline.min.js",
                link=False
            )
           ]
        return libraries


   def pack(self,*args,**kwargs):
       Oneline_V2_CommandLine_Log.write(     
            Oneline_V2_CommandLine_Messages.PACKING_MODULE
        )
    
       if  len( args ) > 0: 
          if  self._check_name( args[0] ):
            files=  self._files(args[0])
            for i in files:
              full_name=os.getcwd()+"/"+i['name']
              os.system("ln -s {0} {1}".format( full_name, i['link'] ) )
              Oneline_V2_CommandLine_Log.write("Linked %s"%(i['name']) )
          else:
            Oneline_V2_CommandLine_Log.error(
                Oneline_V2_CommandLine_Errors.CLIENT_NAME_NOT_USABLE)
       else:
          Oneline_V2_CommandLine_Log.error(
            Oneline_V2_CommandLine_Errors.ARGS_TOO_LESS)
            
        

   def controller(self,*args,**kwargs):
      Oneline_V2_CommandLine_Log.write(
        Oneline_V2_CommandLine_Messages.USING_CONTROLLER
      )
      if  len(args)>1:
          cwd=os.getcwd() 
          if os.path.isfile( "/usr/local/onelinev2/controllers/{0}.controller.py".format( args[0] ) ):
             os.chdir("/usr/local/onelinev2/controllers/")
             module=dict() 
             with open("/usr/local/onelinev2/controllers/{0}.controller.py".format(args[0])) as f:
                exec(f.read(),module)
             if args[1] in module.keys():
                function=module[args[1]]
                if hasattr( function, '__call__'): 
                    function()
                else:
                    Oneline_V2_CommandLine.write(
                        Oneline_V2_CommandLine_Errors.CONTROLLER_FN_UNCALLABLE
                    )
             else:
                Oneline_V2_CommandLine.write(
                    Oneline_V2_CommandLine_Errors.CONTROLLER_FN_MISSING
                )
          else:
            Oneline_V2_CommandLine.write(
                Oneline_V2_CommandLine_Errors.CONTROLLER_MISSING
            )
      else:
        Oneline_V2_CommandLine.write(
            Oneline_V2_CommandLine.ARGS_TOO_LESS
        )
        
        
      return Oneline_V2_CommandLine_Not_Implemented()

   def remove(self,*args,**kwargs):
      Oneline_V2_CommandLine_Log.write(
        Oneline_V2_CommandLine_Messages.REMOVING_MODULE
        )
      if len(args)>0:
         files=self._files( args[0] )
         for i in files:
            ##symbolic link could also be dead
            #if  i['link'] and (os.path.exists( i['link'] ) ) :
            Oneline_V2_CommandLine_Log.write("Removing %s"%( i['name'] ) )
            os.remove( i['link'] )
      else:
          Oneline_V2_CommandLine_Log.error(
            Oneline_V2_CommandLine_Errors.ARGS_TOO_LESS
         )
    

   def make(self,*args,**kwargs):
      self.makev2(*args,**kwargs)

   def makev2(self,*args,**kwargs):
      current_dir = os.getcwd()
      Oneline_V2_CommandLine_Log.write(
	 Oneline_V2_CommandLine_Messages.CREATING_MODULE
	)
      if len( args )>0:
	 if  self._check_name( args[0] ):
	     name = args[0]
	     cookieDir = Oneline_V2_CommandLine_util.getCookieCutter(name)
	     os.chdir(cookieDir)
	     name= os.path.basename( cookieDir )
	     libs= self._files(name)+self._libraries()
	     for i in libs:
		if i['link']:
	 	   full_name = cookieDir +"/"+ i['name']
		   Oneline_V2_CommandLine_Log.write(
			"Linking  %s"%(full_name)
			)
	  	   os.system("ln -s {0} {1}".format( full_name, i['link']))
	     Oneline_V2_CommandLine_Log.write(
			"Made module %s"%(name)
		)
	 else:
	     Oneline_V2_CommandLine_Log.write(
		Oneline_V2_CommandLine_Errors.CLIENT_NAME_NOT_USABlE
		)
      else: 
	  Oneline_V2_CommandLine_Log.write(
		Oneline_V2_CommandLine_Errors.ARGS_TOO_LESS
		)
	
			
			

 
   def makev1(self,*args,**kwargs):
      current_dir=os.getcwd()
    
      Oneline_V2_CommandLine_Log.write(
        Oneline_V2_CommandLine_Messages.CREATING_MODULE
       )
      if len( args ) > 0:
          if self._check_name( args[0] ):
               name =args[0]
               libs_and_files=self._files(name)+self._libraries()
               for i in libs_and_files:
                  full_name = current_dir+"/"+i['name']
                  new_file= open(full_name,"w+")
                  placeholder_file= open(i['placeholder'], "r")
                  new_file.write( placeholder_file.read() )
                  new_file.close()
                  placeholder_file.close()
                  Oneline_V2_CommandLine_Log.write("Wrote %s"%(i['name']) )
                  if  i['link']:
                     Oneline_V2_CommandLine_Log.write("Linking %s"%(i['name']) )
                     os.system("ln -s {0} {1}".format(full_name, i['link']) )
          else:
            Oneline_V2_CommandLine_Log.error(
                Oneline_V2_CommandLine_Errors.CLIENT_NAME_NOT_USABLE
            )
      else:
          Oneline_V2_CommandLine_Log.error(
               Oneline_V2_CommandLine_Errors.ARGS_TOO_LESS
           )
       


class Oneline_V2_CommandLine(object):
  def __init__(self,arguments=[]):
     self.arguments=arguments

     self.server_cmds=[]
     self.client_cmds=[]
     self.called_help=False
	 
     self.known_opts=dict(
	server=[
		dict( cmd='start',  needs_arg=False, callable=True ),
		dict(  cmd='stop',  needs_arg=False, callable=True),
	  	dict( cmd='restart', needs_arg=False, callable=True ),
		dict( cmd='ip',  needs_arg=True, callable=False ),
		dict( cmd='port', needs_arg=True, callable=False)
		],
    	client=[
		dict( cmd='pack', needs_arg=True, callable=True),
		dict( cmd='controller', needs_arg=True, callable=True ),
		dict( cmd='make', needs_arg=True, callable=True),
                dict( cmd='remove', needs_arg=True, callable=True)
	   ]
       )
     self.run()
  def  run( self ):
     for i in range(0, len( self.arguments) ):
        try:
           next_argument=  self.arguments[i +  1]
           all_remaining_arguments=self.arguments[i+1:len(self.arguments)]
	except Exception, e:
   	   next_argument = None
           all_remaining_arguments=[]
        
        this_argument=self.arguments[ i ] 
        server_arg = self.type_of_cmd("server", this_argument)
        if server_arg:
           self.server_cmds.append( dict(
	 	arg=server_arg,
		key=this_argument,
		val=next_argument,
                args=all_remaining_arguments
		 ))
        client_arg=self.type_of_cmd("client", this_argument)
	if client_arg:
	   self.client_cmds.append( dict(
		 arg=client_arg,
		key=this_argument,
		val=next_argument,
                args=all_remaining_arguments
	  	))
        if this_argument=="help":
	    self.called_help =True
     self.run_server_cmds()
     self.run_client_cmds()
     if self.called_help or (len(self.client_cmds)==0 and len(self.server_cmds) == 0) :
	  self.help()
  def help( self  ):
        help_string="""
Welcome to Oneline C-V2 Command Line Help
server options:
start: starts the  server
stop: stops the server
restart: restarts the server

client options:
pack:  pack an existing oneline module
make: initialize a new oneline module
remove: remove a module
	 """

        Oneline_V2_CommandLine_Log.stdout( help_string )

  def type_of_cmd(self, check1, arg):
       for i in self.known_opts['client']:
	   if  (arg==i['cmd']  and check1== 'client'):
	  	return  i
       for i in self.known_opts['server']:
	   if (arg==i['cmd']  and check1 == 'server'):
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
       if cmd['arg']['callable']:
  	   callable_fn = getattr( instance,cmd['key'])
           if  len( cmd['args'] ) > 0 :
	       callable_fn( *cmd['args'] )
           else:
               callable_fn( )
          
	  
  
if __name__ == "__main__":  		     
  obj = Oneline_V2_CommandLine(sys.argv)
   
      
     
 

