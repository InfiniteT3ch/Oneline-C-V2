
from .command_line_exec import Oneline_V2_CommandLine_Server

def startServer():
   server = Oneline_V2_CommandLine_Server()
   server.start()
def stopServer():
   server = Oneline_V2_CommandLine_Server()
   server.stop()

