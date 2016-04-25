# -*- coding: utf-8 -*- 
import os
import sys
import shutil
import stat

try:
	from setuptools import setup
except ImportError:
	from distutils.core import setup

from distutils.command.build_py import build_py

def create_all_paths(dirs):
  current_dir=os.getcwd()
  for i in dirs:
     full_name="{0}/{1}".format(current_dir,i)
     target_name="/usr/local/onelinev2/{0}".format(i)
     shutil.copytree(full_name,target_name)
def create_all_links(links):
   current_dir=os.getcwd()
   for i in links:
      full_name="{0}/{1}".format(current_dir,i)
      target_name="/usr/local/onelinev2/{0}".format(i)
      os.system("ln -s {0} {1}".format(full_name,target_name))
      

def create_cmd_exec():
   cwd=os.getcwd()
   shutil.copyfile(cwd+"/onelinev2/command_line_exec.py", "/usr/bin/onelinev2")
   os.chmod("/usr/bin/onelinev2",777)

class OnelineBuild(build_py):  
	def run(self):
	  paths = [
		  'ext',
		'modules',
		'conf',
                'controllers',
                'html',
                'js',
                'css'
		]
          links= [
               'placeholders',
               'lib'
           ]
	  if not os.path.isdir("/usr/local/onelinev2/"):
	     os.mkdir("/usr/local/onelinev2/")
	     create_all_paths(paths)
             create_all_links(links)
	     create_cmd_exec()
	  else:
	     print "Removing previous Oneline V2 installation"
	     shutil.rmtree("/usr/local/onelinev2/")
             if os.path.isfile("/usr/bin/onelinev2"):
               os.remove("/usr/bin/onelinev2")

	     os.mkdir("/usr/local/onelinev2/")
	     create_all_paths(paths)
             create_all_links(links)
	     create_cmd_exec()
          super(OnelineBuild,self).__init__()
     


setup(name="onelinev2",
      version="2.0.0",
      description="",
      maintainer="Nadir Hamid",
      maintainer_email="matrix.nad@gmail.com",
      url="https://",
      download_url = "https://pypi.python.org/pypi/",
      license="MIT",
      long_description="",
      cmdclass=dict(build_py=OnelineBuild),
      packages=["onelinev2"], 
      classifiers=[
          'Development Status :: 5 - Production/Stable',
          'Intended Audience :: Developers',
          'License :: OSI Approved :: BSD License',
          'Operating System :: OS Independent',
          'Programming Language :: Python',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 2.7',
          'Programming Language :: Python :: 3',
          'Programming Language :: Python :: 3.3',
          'Programming Language :: Python :: 3.4',
          'Programming Language :: Python :: Implementation :: CPython',
          'Programming Language :: Python :: Implementation :: PyPy',
          'Topic :: Communications',
      ])
