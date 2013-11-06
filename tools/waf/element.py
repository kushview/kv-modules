#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, platform
from waflib.Configure import conf

import autowaf

def is_mac():
    return "Darwin" in platform.system()

def is_linux():
    return "Linux" in platform.system()

@conf
def check_qt5 (self):
    self.qt5_vars_debug = []
    self.qt5_vars = "QtGui QtWidgets QtCore"
    self.load ("qt5")

@conf
def display_msg (self, a, b):
    autowaf.display_msg (self, a, b)

@conf
def check_cxx11 (self):
    if is_mac():
        # Test for c++11 and libc++
        self.check_cxx (linkflags=["-stdlib=libc++", "-lc++"], cxxflags=["-stdlib=libc++", "-std=c++11"])
        self.env.append_unique ("CXXFLAGS", ["-stdlib=libc++", "-std=c++11"])
        self.env.append_unique ("LINKFLAGS", ["-stdlib=libc++", "-lc++"])
    elif is_linux():
        self.check_cxx (cxxflags=["-std=c++11"])
        self.env.append_unique ("CXXFLAGS", ["-std=c++11"])
    else:
        print os.getCwd() + "  element.py"
        print "!!!!! SETUP CXX11 FOR " + platform.system()
        exit (1)

@conf
def check_rez (self):
    self.find_program("Rez")

def options (opts):
    opts.add_option ("--no-headers", dest="install_headers", action="store_false", default=True)
    opts.add_option ("--app-config", dest="app_config", type='string', default='')

def configure (conf):

    if conf.options.app_config == '':
        conf.env.append_unique ("CXXFLAGS", ['-I' + os.getcwd() + "/project/JuceLibraryCode"])
        conf.env.append_unique ("CFLAGS", ['-I' + os.getcwd() + "/project/JuceLibraryCode"])
    else:
        conf.env.append_unique ("CXXFLAGS", ['-I' + conf.options.app_config])
        conf.env.append_unique ("CFLAGS", ['-I' + conf.options.app_config])

    pat = conf.env['cshlib_PATTERN']
    if not pat:
        pat = conf.env['cxxshlib_PATTERN']
    if pat.startswith('lib'):
        pat = pat[3:]
    conf.env['modulelib_PATTERN'] = pat
    conf.env['modulelib_EXT'] = pat[pat.rfind('.'):]

    if "Darwin" in platform.system():
        conf.env.append_unique ("CXXFLAGS", ["-I/opt/element/include"])
        conf.env.append_unique ("CFLAGS", ["-I/opt/element/include"])
        conf.env.append_unique ("LINKFLAGS", ["-L/opt/element/lib"])
        conf.env.FRAMEWORK_AUDIO_TOOLBOX = 'AudioToolbox'
        conf.env.FRAMEWORK_AUDIO_UNIT = 'AudioUnit'
        conf.env.FRAMEWORK_ACCELERATE = 'Accelerate'
        conf.env.FRAMEWORK_APP_KIT = 'AppKit'
        conf.env.FRAMEWORK_CARBON = 'Carbon'
        conf.env.FRAMEWORK_COCOA = 'Cocoa'
        conf.env.FRAMEWORK_CORE_AUDIO = 'CoreAudio'
        conf.env.FRAMEWORK_CORE_AUDIO_KIT = 'CoreAudioKit'
        conf.env.FRAMEWORK_CORE_FOUNDATION = 'CoreFoundation'
        conf.env.FRAMEWORK_CORE_IMAGE = 'CoreImage'
        conf.env.FRAMEWORK_CORE_MIDI = 'CoreMIDI'
        conf.env.FRAMEWORK_CORE_SERVICES = 'CoreServices'
        conf.env.FRAMEWORK_DISK_RECORDING = "DiscRecording"
        conf.env.FRAMEWORK_FOUNDATION = "Foundation"
        conf.env.FRAMEWORK_IOKIT = 'IOKit'
        conf.env.FRAMEWORK_OPENGL = 'OpenGL'
        conf.env.FRAMEWORK_QUARTZ_CORE = 'QuartzCore'
        conf.env.FRAMEWORK_WEB_KIT = 'WebKit'
        conf.env.FRAMEWORK_ELEMENT = 'Element'
        conf.env.ARCH_COCOA = ['x86_64']

    elif "Windows" in platform.system():
        pass
    elif "Linux" in platform.system():
        pass
    else:
        pass

def module_environ(bld):
    modenv = bld.env.derive()
    modenv['cxxshlib_PATTERN'] = modenv['cshlib_PATTERN'] = bld.env['modulelib_PATTERN']
    return modenv


# Task to keep track of Qt generated headers and moc files
from waflib.TaskGen import feature, before_method, after_method
@feature('cxx qt5')
@after_method('process_source')
@before_method('apply_incpaths')
def add_includes_paths(self):
    incs = set(self.to_list(getattr(self, 'includes', '')))
    for x in self.compiled_tasks:
        incs.add(x.inputs[0].parent.path_from(self.path))
    self.includes = list(incs)
