from .commands import startServer,stopServer

class Controller(object):
   """
   function to specify properties
   of Controller
   """
   def __init__(self, name=""):
      self.name=name

   """ 
   Base function for 
   init calls on controllers
   """
   def start(self, *args, **kwargs):
      print "Starting module %s."%(self.name)
      startServer()

   """
   load function for
   loading calls on controllers
   """
   def load(self, *args, **kwargs):
      return None

   """
   stop function for
   stopping module
   """
   def stop(self, *args, **kwargs):
      print "Stopping module %s."%(self.name)
      stopServer()

