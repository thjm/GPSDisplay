#
# SConscript for GPSDisplay base directory
#
# $Id: SConscript,v 1.4 2009/08/10 15:05:53 avr Exp $
#

Import('env')

# libserial library
#
env.Append(LIBSERIALDIR = '/usr/local')
env.Append(LIBSERIALINCDIR = env['LIBSERIALDIR'] + '/include')
env.Append(LIBSERIALLIBDIR = env['LIBSERIALDIR'] + '/lib')
env.Append(LIBSERIALLIB = 'serial')

# some more build options
#
#env.AppendUnique(CPPDEFINES = ['APRS'])
env.AppendUnique(CCFLAGS= ['-std=c99'])
env.AppendUnique(CPPPATH = [ env['LIBSERIALINCDIR'] ])
env.AppendUnique(LIBPATH = [ env['LIBSERIALLIBDIR'] ])

# program gpstest
#
srcs1 = Split('gpstest.cc LCDDisplay.c GPS.c ui.c')

env.Program('gpstest', srcs1, LIBS = env['LIBSERIALLIB'])

# program gpssim
#
srcs2 = Split('gpssim.cc ui.c')

env.Program('gpssim', srcs2, LIBS = env['LIBSERIALLIB'])

# --- eof
