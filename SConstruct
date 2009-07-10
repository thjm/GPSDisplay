#
# SConstruct for GPSDisplay base directory
#
# $Id: SConstruct,v 1.1 2009/07/10 10:59:00 avr Exp $
#

import os

# define the options for this project
#
opts = Options('options.cache', ARGUMENTS)

# general (SCons etc.) options
#
opts.AddOptions(BoolOption('debug', 'set debug flags', 1),
                BoolOption('warning', 'use extended warning options', 0),
	       )

# get THE environment with some basic environment settings
#
env = Environment(CCFLAGS = ['-W', '-Wall', '-Wwrite-strings',
			      '-Wunused','-Wpointer-arith'],
		  CXXFLAGS= ['-Woverloaded-virtual','-fstrict-aliasing'],
		  options = opts
		 )

# to speed things up:
# warning: the implicit cache may miss changes after an cvs update
#
env.SetOption('max_drift', 1)
if env.get('nocache',0): 
   env.SetOption('implicit_cache', 0)
   print "implicit cache disabled"
else:
   env.SetOption('implicit_cache', 1)

# save options for the next SCons run
#
opts.Save('options.cache', env )

# generate the help text for the options
#
Help(opts.GenerateHelpText(env))

# we need those propably to run our tools
#
env.Append(ENV = {'PATH' : os.environ['PATH']})
env.Append(ENV = {'LD_LIBRARY_PATH' : os.environ['LD_LIBRARY_PATH']})
env.Append(ENV = {'HOME' : os.environ['HOME']})

# set some global variables and build options
#
env.AppendUnique(CPPPATH  = ['.'])

# set debug flag accordingly
#
if env.get('debug',0):
   env.AppendUnique(CCFLAGS = ['-g','-ggdb'])

#Export('env')

# libserial library
#
env.Append(LIBSERIALDIR = '/usr/local')
env.Append(LIBSERIALINCDIR = env['LIBSERIALDIR'] + '/include')
env.Append(LIBSERIALLIBDIR = env['LIBSERIALDIR'] + '/lib')
env.Append(LIBSERIALLIB = 'serial')

# some more build options
#
#env.AppendUnique(CPPDEFINES = ['APRS'])
env.AppendUnique(CPPPATH = [ env['LIBSERIALINCDIR'] ])
env.AppendUnique(CFLAGS= ['-std=c99'])
env.AppendUnique(LIBPATH = [ env['LIBSERIALLIBDIR'] ])

# program gpstest
#
srcs = Split('gpstest.cc LcdDisplay.c GPS.c ui.c')

env.Program('gpstest', srcs, LIBS = env['LIBSERIALLIB'])

# the default target
#
Default(['.'])

# --- eof
