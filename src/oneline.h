

#include "Python.h"
#include "json.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Oneline Python Object interpreter
// @author Nadir Hamid (02/21/2016)
//
// handle anonymous oneline objects, there modules
// according to:
// Oneline.module:
// start/0
// end/0
// receive/2
// provide/2
// 
// This should perform the allocation  of all oneline modules
// found in /usr/local/oneline/modules as well as create there Python
// based objects. The objects should then follow the normal oneline v1
//  approach. Providing the configuration layer and database
//  
// This implementation should be portable and not dependant on any WebSockets
// implementation. It should directly provide the entry point, exit and cleanup
// of all objects in this file. 
//   
//
//



struct OnelineAPI {
   //struct PyObject* api;
   char*  module_name;
   struct OnelineAPI* head;
} oneline_api_t;

struct OnelineModule {
   struct OnelineModule* head;
   PyObject* module_obj;
   PyObject* obj_obj;
   char* module_name;
} oneline_module_t;

struct OnelineAPIS {
   struct OnelineAPI* head;
} oneline_apis_t;


struct OnelineModules {
  struct OnelineModule* head;
} oneline_modules_t;
struct OnelineConfiguration {
   char* oneline_stream_type;
   char* oneline_stream_name;
} oneline_configuration_t;

struct OnelineLog {
   char* type; 
   char* data;
   int line_no;
} oneline_log_t;
struct OnelineMessage {
   char* stream_type;
   char* data;
} oneline_message_t;


typedef struct OnelineAPI* oneline_api_t_ptr;
typedef struct OnelineAPIS* oneline_apis_t_ptr;
typedef struct OnelineModule* oneline_module_t_ptr;
typedef struct OnelineModules* oneline_modules_t_ptr;
typedef struct OnelineLog* oneline_log_t_ptr;
typedef struct OnelineConfiguration* oneline_configuration_t_ptr;
typedef struct OnelineMessage* oneline_message_t_ptr;

 

void oneline_init_basics(); // register the structs
void oneline_find_all_modules();
void oneline_log(char* msg, oneline_log_t_ptr  log_msg);
oneline_log_t_ptr oneline_log_msg_init(char*msg,int line_no, char* data, char* type);
char* oneline_parse_module_name(char* module_name);


oneline_configuration_t_ptr oneline_parse_configuration(char* configuration_file_name);
// return null on no finding of module
oneline_module_t_ptr oneline_find_module(char* module_name);
oneline_module_t_ptr oneline_generate_module(char* module_name);
oneline_api_t_ptr oneline_find_api(char* module_name);



void oneline_init_module( oneline_module_t_ptr module, oneline_api_t_ptr api);
oneline_api_t_ptr oneline_import_module(oneline_module_t_ptr module);
char* oneline_invoke_object_callback(oneline_module_t_ptr module, char*  oneline_method, char* message);
void oneline_append_module(oneline_module_t_ptr module_name);
void oneline_append_api(oneline_api_t_ptr api_ptr);
char* get_substring(char* input_character,int start_offset, int end_offset);
oneline_message_t_ptr oneline_message_from_string( char* message );





