#Oneline V2
#  @author Nadir Hamid
#  @license MIT


import bsonlib
import json
ONELINE_SETTINGS = dict(
   root_dir="/usr/local/oneline/"
)
def oneline_parse_message(message):
  if isinstance(message,str):
    literal = ast.literal_eval(message.__str__())
    return bsonlib.loads(bytearray(literal).__str__())
  return message
def oneline_pack_message(message):
  bytes = map(ord, bsonlib.dumps(message)).__str__()
  return bytes

## basics for getting configurations
## and  other things 
class Oneline_V2_Util(object):
  # OnelineV2 module configs are json based
  # @param  module_name the name of the module
  @staticmethod
  def get_config(module_name):
    full_name="{0}/{1}.conf".format(ONELINE['root_dir'], "conf/"+ module_name)
    if os.path.isfile( full_name ):
      file = json.loads(open( ONELINE_SETTINGS['root_dir'] + "/conf/{0}.conf".format(module_name ).read()) )
      return file
    return dict()

class Oneline(object):
  ## parse and pack a BSON
  ## message
  ## @paramany message, dict based
  @staticmethod
  def pack( dict ):
      return oneline_pack_message(dict)
  @staticmethod
  ## parse a BSON message
  ## @param msg raw  BSON msg
  def parse( msg ):
      return oneline_parse_message( msg )
  ## run the pipeline
  ## @param module  to be used
  @staticmethod 
  def run( msg ):
      return msg


