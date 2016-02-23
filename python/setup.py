# -*- coding: utf-8 -*- 
import os
import sys
import shutil

try:
	from setuptools import setup
except ImportError:
	from distutils.core import setup

from distutils.command.build_py import build_py

def create_all_paths(dirs):
  for i in dirs:
     os.mkdir("/usr/local/onelinev2/{0}".format(i))
def create_cmd_exec():
   cwd=os.getcwd()
   shutil.copyfile(cwd+"/oneline/command_line_exec.py", "/usr/local/onelinev2/ext/command_line_exec.py")

class OnelineBuild(build_py):  
	def run(self):
	  paths = [
		  'ext',
		'modules',
		'conf'
		]
	  if not os.path.isdir("/usr/local/onelinev2/"):
	     os.mkdir("/usr/local/onelinev2/")
	     create_all_paths(paths)
	     create_cmd_exec()
	  else:
	     print "Removing previous Oneline V2 installation"
	     shutil.rmtree("/usr/local/onelinev2/")
	     os.mkdir("/usr/local/onelinev2/")
	     create_all_paths(paths)
	     create_cmd_exec()
     


setup(name="oneline",
      version="2.0.0",
      description="",
      maintainer="Nadir Hamid",
      maintainer_email="matrix.nad@gmail.com",
      url="https://",
      download_url = "https://pypi.python.org/pypi/",
      license="MIT",
      long_description="",
      cmdclass=dict(build_py=OnelineBuild),
      packages=["oneline"], 
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
