#include "oneline.h"
static oneline_modules_t_ptr  oneline_modules;
static oneline_apis_t_ptr oneline_apis;

static char* oneline_module_directory;
static char* suffix =".py";
 
void oneline_log(char* msg, oneline_log_t_ptr  log_msg)
{
   printf("ONELINE Cv2 (MSG): %s\n", "Entering Oneline Log");
   printf("ONELINE Cv2 (MSG):  %s\n", msg);
   printf("ONELINE Cv2 (DATA): LINENO: %d\n", log_msg->line_no);
   printf("ONELINE Cv2 (DATA): TYPE %s\n", log_msg->type);
   printf("ONELINE Cv2 (DATA): DATA %s\n", log_msg->data);
   //free(log_msg);
}
oneline_log_t_ptr oneline_log_msg_init(char* msg, int line_no, char*data, char* type)
{
   oneline_log_t_ptr oneline_log_obj =  (oneline_log_t_ptr) malloc( sizeof(oneline_log_t) );
   oneline_log_obj->line_no=line_no;
   oneline_log_obj->type=type;
   oneline_log_obj->data=data;
   return oneline_log_obj;
}
	 

// Oneline V2 Core Spec:
// find all modules and register them
// as OnelineModules according to their Python
// module file which should be located
//  in the configuration setting under the naming convention
void oneline_find_all_modules() 
{
   DIR* directory;
   struct dirent* entry;
   oneline_log("oneline_find_all_modules()", oneline_log_msg_init("", __LINE__, "Entering", "INFO"));
    if ( (directory = opendir( oneline_module_directory ) ) !=NULL) {
	  oneline_log("oneline_find_all_modules()", oneline_log_msg_init("", __LINE__, "Entered directory", "INFO"));
	  
	 while ( (entry =readdir( directory) ) != NULL ) {
	  if (  strlen(entry->d_name)> strlen(suffix) ) {
	   	   char* last_three_characters=(char*)malloc(sizeof(entry->d_name)); 
		    strcpy(last_three_characters,"");
	  	    strncpy(last_three_characters, entry->d_name+(strlen(entry->d_name)-3), strlen(entry->d_name));
		    last_three_characters[ strlen(entry->d_name) -3 ] ='\0';
	  	    if ( strcmp( last_three_characters, suffix )  == 0) {
			   oneline_log("oneline_find_all_modules()", oneline_log_msg_init("", __LINE__, entry->d_name, "INFO"));
			   oneline_module_t_ptr oneline_module_ptr = (oneline_module_t_ptr) oneline_generate_module(entry->d_name);
			   oneline_append_module(oneline_module_ptr);
			   oneline_api_t_ptr api = (oneline_api_t_ptr) oneline_import_module( oneline_module_ptr );

			   oneline_init_module( oneline_module_ptr, api );
		   }
	      }
	 }
	closedir(directory);
   } else {
      //  could not open the directory
      //
      //	
      char* msg = "oneline_find_all_modules()";
      oneline_log_t_ptr log_obj = oneline_log_msg_init(msg, __LINE__, "Could not find Oneline directory", "ERROR");
     oneline_log(msg,log_obj);
  } 
  
}
 
// Oneline V2 Core Spec:
// generate a oneline module making it available
// as a Python Object in C
oneline_module_t_ptr oneline_generate_module(char* module_name)
{
     FILE* oneline_module_file; 
     char* full_configuration_name;
    oneline_log("oneline_generate_module()", oneline_log_msg_init("", __LINE__, "Module parsing", "INFO"));
     char* real_module_name=oneline_parse_module_name(module_name);
     oneline_module_t_ptr oneline_module_obj = (oneline_module_t_ptr) malloc(sizeof(oneline_module_t));
     oneline_module_obj->module_name = (char*) malloc( sizeof( module_name) );
     strcpy(oneline_module_obj->module_name, real_module_name);
   oneline_log("oneline_generate_module()", oneline_log_msg_init("", __LINE__,  real_module_name, "INFO"));
    return oneline_module_obj;
}

// Oneline V2 Core Spec:
// parse the module name of the  object  provided this can be the filename
// example: MyModule.py we just need MyModule
// 
// @param   module_name : full module name
//
char* oneline_parse_module_name(char* module_name) 
{
  int  iterator=0;
  char* new_char=(char*)malloc(strlen(module_name));
  strcpy(new_char,"");
  strncpy( new_char, module_name, strlen(module_name)-3);
  new_char[ strlen(module_name)-3 ] = '\0';

  return (char*) new_char;
}



// Oneline V1 Core Spec:
//  parse a configuration for  python finding its
// database properties as well as basic stream types
// for a full list of properties we should parse please
//  visit:
oneline_configuration_t_ptr oneline_parse_configuration( char* configuration_file_name) 
{ 
   oneline_configuration_t_ptr configuration;
   return configuration;
}

// Oneline V1 Core Spec:
// handle a oneline message, this will receive a BSON
// packet and send it to the oneline object in Python
// it should parse the packet and provide the request layer
// @param module_name: The Registered Oneline Module Name
// @param  oneline_method: A valid Oneline module callback start|end|receiver|provider
// @param message: The BSON Message
char* oneline_invoke_object_callback(oneline_module_t_ptr module, char* oneline_method, char*  message) 
{
   PyObject* arguments = Py_BuildValue("(s)", message);
   PyObject* kwargs = PyDict_New();
   PyObject* method = PyObject_GetAttrString(module->obj_obj, oneline_method);
   PyObject* return_obj;
   oneline_log("oneline_invoke_object_callback()", oneline_log_msg_init("", __LINE__,module->module_name, "INFO"));
   if ( module != NULL && PyCallable_Check(method) ) {
	   if ( strcmp(oneline_method, "start") != -1 ) {
	    return_obj=PyObject_Call(
		method,
	        arguments,
	        kwargs);
	 	oneline_log("oneline_invoke_object_callback()", oneline_log_msg_init("", __LINE__, "start", "INFO"));

	  } else if(strcmp(oneline_method, "end") != -1 ) {
	    return_obj=PyObject_Call(
	  	 method,
	  	arguments,
	  	  kwargs);
	     oneline_log("oneline_invoke_object_callback()", oneline_log_msg_init("", __LINE__, "end", "INFO"));
	  } else if  (strcmp(oneline_method, "receiver") != -1 ){
	    return_obj=PyObject_Call(
		  method,
		  arguments,
		 kwargs);
	     oneline_log("oneline_invoke_object_callback()", oneline_log_msg_init("", __LINE__, "receiver", "INFO"));
	   }

  	Py_DECREF(arguments);
	Py_DECREF(kwargs);

        //PyObject* repr = PyObject_Repr( return_obj );


	char* result =  PyString_AsString(return_obj);
	oneline_log("oneline_invoke_object_callback()", oneline_log_msg_init("", __LINE__, result, "INFO"));
	 return result;
   }

  return NULL;
}

// Oneline V2 Core Spec:
// find a module from the heap of modules in
// the oneline modules container
oneline_module_t_ptr oneline_find_module(char* module_name) 
{
  if (oneline_modules->head != NULL ) {
	  oneline_module_t_ptr module =(oneline_module_t_ptr)malloc(sizeof(oneline_module_t));
		 module = oneline_modules->head;
	  oneline_module_t_ptr oneline_modules_head_ptr= (oneline_module_t_ptr)malloc(sizeof(oneline_module_t));
	  oneline_module_t_ptr pending = (oneline_module_t_ptr)malloc(sizeof(oneline_module_t));
	   pending = oneline_modules_head_ptr= oneline_modules->head;
	   while ( module != NULL ) {
		if (strcmp(module->module_name,module_name)==0) {
		    pending=module;
		    oneline_modules->head=(oneline_module_t_ptr)malloc(sizeof(oneline_module_t));
		    oneline_modules->head =(oneline_module_t_ptr)  oneline_modules_head_ptr;
		   return pending;
		}
		module=(oneline_module_t_ptr)module->head;
	   }
	   oneline_modules->head=(oneline_module_t_ptr)oneline_modules_head_ptr;
    }
     return NULL;
}
// Oneline V2 Core Spec:
// find a registered API  from the heap of APIS in
// the oneline apis container
oneline_api_t_ptr oneline_find_api(char* api_name) 
{
   oneline_log("oneline_find_api()", oneline_log_msg_init("", __LINE__, "entering find api", "INFO"));
   if ( oneline_apis->head != NULL ) {
	   oneline_api_t_ptr api = (oneline_api_t_ptr)malloc(sizeof(oneline_api_t) );
	   
	   api= oneline_apis->head;
	   oneline_api_t_ptr oneline_apis_head = (oneline_api_t_ptr)malloc(sizeof(oneline_api_t));
	   oneline_apis_head=oneline_apis->head;
	    oneline_api_t_ptr pending = NULL;
	   while ( api != NULL ) {
		  if ( strcmp(api->module_name, api_name) == 0 ) {
			 pending=(oneline_api_t_ptr)malloc(sizeof(oneline_api_t));
			 pending= api;
			oneline_apis->head=(oneline_api_t_ptr)malloc(sizeof(oneline_api_t));
			 oneline_apis->head= (oneline_api_t_ptr) oneline_apis_head; 
			return pending;
		  }
		 api=(oneline_api_t_ptr)api->head;
	   }
	  oneline_apis->head=(oneline_api_t_ptr)oneline_apis_head;
   }
  return NULL;
}
// Oneline V2 Core Spec:
// append a new module to the heap of oneline modules
// @param new_module: A allocated oneline_module_ptr_t
void oneline_append_module(oneline_module_t_ptr new_module) 
{
   if ( oneline_modules->head==NULL) {
	  new_module->head=NULL;
	   oneline_modules->head=(oneline_module_t_ptr)new_module;
    } else {
      new_module->head=NULL;
      if (oneline_modules->head->head  != NULL) {
      oneline_modules->head=oneline_modules->head->head;
	}
      oneline_modules->head->head= new_module;
   }

}

// Oneline V2 Core Spec:
// append a new api to the the heap of oneline apis
// @param new_api: A allocated oneline_api_ptr_t
void oneline_append_api(oneline_api_t_ptr new_api) 
{
  if ( oneline_apis->head==NULL ) {
  new_api->head=NULL;
  oneline_apis->head=(oneline_api_t_ptr)new_api;
   } else {
   new_api->head=NULL;
    if (oneline_apis->head->head != NULL ) {
	 oneline_apis->head=oneline_apis->head->head;
    }
   oneline_apis->head->head=new_api;
   }
}

 
// Oneline V2 Core Spec:
// import a module making it available
// to the Python program. This can be used
// with:
// oneline_import_module("ol") -- import oneline directory
// oneline_import_module("MyModule") -- import provisioned module
//
// @param module_name: A module name, registered OR "oneline"
oneline_api_t_ptr oneline_import_module(oneline_module_t_ptr module) 
{
    if ( (oneline_find_api (module->module_name) == NULL ) ) {
	  oneline_log("oneline_import_module()", oneline_log_msg_init("", __LINE__, "Importing Python Module", "INFO"));
	 PyObject* pymodule =   PyImport_ImportModule(module->module_name);
	 oneline_api_t_ptr  oneline_api = (oneline_api_t_ptr)malloc(sizeof( oneline_api_t ) );
	  oneline_api->module_name=module->module_name;
	 oneline_log("oneline_import_module()", oneline_log_msg_init("", __LINE__, "Appending API", "INFO"));
    	 oneline_log("oneline_import_module()", oneline_log_msg_init("", __LINE__, "Registering Module", "INFO"));
	 module->module_obj=pymodule;

	 oneline_append_api( oneline_api );
   	return oneline_api; 
    }
   return  NULL;
}

// Oneline V2 Core Spec:
// Initialize a oneline module,
// adding it to the OnelineModule's PyObject
//
// @param module_name the module name to  register the
// object in
void oneline_init_module(oneline_module_t_ptr module, oneline_api_t_ptr api) 
{
   
  if ( module != NULL &&  api != NULL ) {
	   PyObject* dict = PyModule_GetDict(module->module_obj);
	   PyObject* obj = PyDict_GetItemString(dict, module->module_name);
	   
           if (PyCallable_Check(obj)) {
		  oneline_log("oneline_init_module()", oneline_log_msg_init("oneline_init_module()", __LINE__, "Object is callable", "INFO"));
		   PyObject* args = PyTuple_New(0);
		   module->obj_obj= PyObject_CallObject(obj, args);
           } else {
		oneline_log("oneline_init_module()", oneline_log_msg_init("oneline_init_module()", __LINE__, "Object is NOT callable", "INFO"));
		oneline_log("oneline_init_module()", oneline_log_msg_init("oneline_init_module()", __LINE__, module->module_name, "INFO"));
	       module->obj_obj =NULL;
	  }
   } else {
     module->module_obj=NULL;
  }
 
}
// Oneline Core V2:
// register defaults at runtime
//
void oneline_init_basics()
{
  Py_Initialize();
  Py_SetProgramName("oneline");
  PyObject* pathObj=  PySys_GetObject("path");
  oneline_log("oneline_init_basics()", oneline_log_msg_init("", __LINE__, "Registering /usr/local/oneline/modules", "INFO"));
  PyList_Append(pathObj, PyString_FromString("/usr/local/oneline/modules/"));
  printf("ONELINE: Init Basics %s", "oneline_init_basics()");
  oneline_log("oneline_init_basics()", (oneline_log_t_ptr) oneline_log_msg_init("", __LINE__, "", "INFO"));
  oneline_modules=(oneline_modules_t_ptr)malloc(sizeof(oneline_modules_t));
  oneline_modules->head = NULL;
  oneline_apis=(oneline_apis_t_ptr)malloc(sizeof(oneline_apis_t));
  oneline_apis->head= NULL;
  oneline_log("oneline_init_basics()", (oneline_log_t_ptr) oneline_log_msg_init("", __LINE__, "", "INFO"));

  oneline_module_directory =(char*)malloc( strlen("/usr/local/oneline/modules/") +1 ); // null pointer
  oneline_log("oneline_init_basics()", oneline_log_msg_init("", __LINE__, "", "INFO"));
  strcpy(oneline_module_directory, "/usr/local/oneline/modules/");

  oneline_find_all_modules();
}

char* get_substring(char* input_character, int start_offset, int end_offset)
{
  char* new_ptr=(char*)malloc(sizeof(input_character));
  return (char*) new_ptr;
}


