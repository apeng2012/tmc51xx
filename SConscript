# RT-Thread building script for bridge

from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c') + Glob('*.cpp')
path    = [cwd]

group = DefineGroup('tmc51xx', src, depend = ['PKG_USING_TMC51XX'], CPPPATH = path)

Return('group')
