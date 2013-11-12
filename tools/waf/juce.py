#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json, os, platform, sys, unicodedata
from xml.etree import ElementTree as ET

from waflib import Utils, Logs, Errors
from waflib.Configure import conf

def display_header (title):
    Logs.pprint ('BOLD', title)

def display_msg (conf, msg, status = None, color = None):
    color = 'CYAN'
    if type(status) == bool and status or status == "True":
        color = 'GREEN'
    elif type(status) == bool and not status or status == "False":
        color = 'YELLOW'
    Logs.pprint('BOLD', " *", sep='')
    Logs.pprint('NORMAL', "%s" % msg.ljust(conf.line_just - 3), sep='')
    Logs.pprint('BOLD', ":", sep='')
    Logs.pprint(color, status)

@conf
def check_juce (self):
    self.start_msg ("Checking for JUCE")
    mpath = self.env.JUCE_MODULES_PATH = self.options.juce_modules

    if os.path.exists(mpath):
        minfo = open(mpath + "/juce_core/juce_module_info")
        mdata = json.load(minfo)
        minfo.close()
        self.end_msg(mdata["version"])
    else:
        self.end_msg ("no")

@conf
def check_cxx11 (self, required=False):
    line_just = self.line_just

    if is_mac():
        self.check_cxx (linkflags=["-stdlib=libc++", "-lc++"],
                        cxxflags=["-stdlib=libc++", "-std=c++11"], mandatory=required)
        self.env.append_unique ("CXXFLAGS", ["-stdlib=libc++", "-std=c++11"])
        self.env.append_unique ("LINKFLAGS", ["-stdlib=libc++", "-lc++"])
    elif is_linux():
        self.check_cxx (cxxflags=["-std=c++11"], mandatory=required)
        self.env.append_unique ("CXXFLAGS", ["-std=c++11"])
    else:
        print "!!!!! SETUP CXX11 FOR " + platform.system()
        exit (1)

    self.line_just = line_just

def is_mac():
    return 'Darwin' in platform.system()

def is_linux():
    return 'Linux' in platform.system()

def is_win32():
    return 'Windows' in platform.system()

def get_deps(mod):
    if "juce_audio_basics" == mod:
        return ['juce_core']
    elif "juce_audio_devices" == mod:
        return ['juce_audio_basics', 'juce_audio_formats', 'juce_events']
    elif "juce_audio_formats" == mod:
        return ['juce_audio_basics']
    elif "juce_audio_processors" == mod:
        return ['juce_audio_basics', 'juce_gui_extra']
    elif "juce_audio_utils" == mod:
        return ['juce_gui_basics', 'juce_audio_devices', 'juce_audio_processors', 'juce_audio_formats']
    elif "juce_core" == mod:
        return []
    elif "juce_cryptography" == mod:
        return ['juce_core']
    elif "juce_data_structures" == mod:
        return ['juce_core', 'juce_events']
    elif "juce_events" == mod:
        return ['juce_core']
    elif "juce_events" == mod:
        return ['juce_core']
    elif "juce_graphics" == mod:
        return ['juce_core','juce_events']
    elif "juce_gui_basics" == mod:
        return ['juce_core', 'juce_events', 'juce_graphics', 'juce_data_structures']
    elif "juce_gui_extra" == mod:
        return ['juce_gui_basics']
    elif "juce_opengl" == mod:
        return ['juce_gui_extra']
    elif "juce_video" == mod:
        return ['juce_gui_extra']
    else:
        return []

class ModuleInfo:
    data     = None
    infofile = None

    def __init__ (self, juce_info_file):
        if os.path.exists (juce_info_file):
            self.infofile = juce_info_file
            res = open (self.infofile)
            self.data = json.load (res)
            res.close()

    def isValid (self):
        return self.data != None and self.infofile != None

    def id (self):
        return self.data ['id']

    def name (self):
        return self.data ['name']

    def version (self):
        return self.data ['version']

    def description (self):
        return self.data ['description']

    def dependencies (self):
        if not 'dependencies' in self.data:
            return []

        if not len(self.data ['dependencies']) > 0:
            return []

        deps = []
        for dep in self.data ['dependencies']:
            if None != dep['id']:
                deps.append (dep ['id'])

        return deps

    def requiredPackages (self):
        pkgs = []
        for dep in self.dependencies():
            pkgs.append (dep.replace ('_', '-'))
        return pkgs

    def website (self):
        return self.data ['website']

    def license (self):
        return self.data ['license']

    def linuxLibs (self):
        libs = []

        if not 'LinuxLibs' in self.data:
            return libs

        for lib in self.data ['LinuxLibs'].split():
            l = '-l%s' % lib
            libs.append (l)

        return libs

def get_module_info (ctx, mod):
    nodes = find (ctx, os.path.join (mod, 'juce_module_info'))
    infofile = "%s" % nodes[0].relpath()
    return ModuleInfo (infofile)

def get_frameworks (mod):
    if "juce_audio_basics" == mod:
        return ['COCOA', 'ACCELERATE']
    elif "juce_audio_devices" == mod:
        return ['CORE_AUDIO', 'CORE_MIDI', 'DISK_RECORDING']
    elif "juce_audio_formats" == mod:
        return ['CORE_AUDIO', 'CORE_MIDI', 'QUARTZ_CORE', 'AUDIO_TOOLBOX']
    elif "juce_audio_processors" == mod:
        return ['CORE_AUDIO', 'CORE_AUDIO_KIT', 'AUDIO_UNIT', 'CORE_MIDI', 'AUDIO_TOOLBOX']
    elif "juce_audio_utils" == mod:
        return []
    elif "juce_core" == mod:
        return ['COCOA', 'IOKIT']
    elif "juce_events" == mod:
        return ['IOKIT', 'COCOA']
    elif "juce_graphics" == mod:
        return ['COCOA', 'QUARTZ_CORE']
    elif "juce_gui_basics" == mod:
        return ['COCOA', 'CARBON', 'QUARTZ_CORE']
    elif "juce_gui_extra" == mod:
        return ['WEB_KIT']
    elif "juce_opengl" == mod:
        return ['OPENGL']
    else:
        return []

def get_use_libs (mod):
    return get_frameworks (mod)

def options(opt):
    pass

def configure (conf):

    outdir = conf.options.out
    if len (outdir) == 0:
        outdir = "build"

    conf.env.JUCE_MODULE_PATH = "libs/juce/modules"

def extension():
    if platform.system() != "Darwin":
        return ".cpp"
    else:
        return ".mm"

def find (ctx, pattern):
    if len(pattern) <= 0:
        return None
    pattern = '%s/**/%s' % (ctx.env.JUCE_MODULE_PATH, pattern)
    return ctx.path.ant_glob (pattern)

def build_modular_libs (bld, mods, vnum=""):

    libs = []
    mext = extension()

    for mod in mods:
        src = find (bld, mod + mext)
        obj = bld (
            features    = "cxx cxxshlib",
            source      = src,
            name        = mod,
            target      = mod,
            use         = get_use_libs (mod) + get_deps (mod),
            includes    = []
        )
        if len(vnum) > 0:
            obj.vnum = vnum
        libs += [obj]
    return libs

def create_unified_lib (bld, tgt, mods, feats="cxx cxxshlib"):

    mext = extension()

    mod_path = bld.env.JUCE_MODULE_PATH
    src = []
    ug  = []

    for mod in mods:
        src += [mod_path + "/" + mod + "/" + mod + mext]
        ug += get_use_libs (mod)

    # strip out duplicate use libs
    us = []
    for u in ug:
        if not u in us:
            us += [u]

    obj = bld (
        features    = feats,
        source      = src,
        includes    = [],
        name        = tgt,
        target      = tgt,
        use         = us
    )

    return obj

def module_path (ctx):
    return ctx.env.JUCE_MODULE_PATH

def available_modules (ctx):
    return os.listdir (module_path (ctx))

class IntrojucerProject:
    data = None
    proj = None
    root = None

    def __init__ (self, project):
        if os.path.exists (project):
            self.proj = project
            data = ET.parse (self.proj)

            if data.getroot().tag == "JUCERPROJECT":
                self.data = data
                self.root = data.getroot()
            else:
                self.data = None

    def isValid (self):
        return self.data != None and self.name != None

    def getProperty (self, prop):
        return self.root.attrib [prop]

    def getId (self):
        return self.getProperty ("id")

    def getName (self):
        return self.getProperty ("name")

    def getVersion (self):
        return self.getProperty ("version")

    def getJucerVersion (self):
        return self.getProperty ("jucerVersion")

    def getProjectType(self):
        return self.getProperty ("projectType")

    def getBundleIdentifier (self):
        return self.getProperty ("bundleIdentifier")

    def getModules (self):
        mods = []

        for mod in self.root.iter ("MODULE"):
            if 'id' in mod.attrib:
                mods += [mod.attrib ["id"]]

        if len(mods) == 0:
            for mod in self.root.iter ("MODULES"):
                if 'id' in mod.attrib:
                    mods += [mod.attrib ['id']]

        return mods

    def getModulePath (self, module):

        if is_mac():
            tag = 'XCODE_MAC'
        elif is_linux():
            tag = 'LINUX_MAKE'

        paths = self.root.find('EXPORTFORMATS').find(tag).find('MODULEPATHS')
        for path in paths.iter ('MODULEPATH'):
            if module == path.attrib ['id']:
                return os.path.join (self.getProjectDir(), path.attrib ['path'])

        return None

    def getProjectDir(self):
         return os.path.relpath (os.path.join (self.proj, ".."))

    def getProjectCode(self):
        code = []
        for c in self.root.iter ("FILE"):
            if "compile" in c.attrib and c.attrib["compile"] == "1":
                f = "%s" % (c.attrib ["file"])
                parent = os.path.join (self.proj, "..")
                code.append (os.path.join (parent, os.path.relpath (f)))
        return code

    def getLibraryCode (self):
        code = []
        for mod in self.getModules():
            module_path = self.getModulePath (mod)
            infofile = os.path.join (module_path, mod, "juce_module_info")
            if os.path.exists (infofile):
                res = open(infofile)
                data = json.load(res)
                res.close()

                if "compile" in data:
                    for i in data["compile"]:
                        if "target" in i and i["target"] == "! xcode":
                            f = "%s/%s/%s" % (module_path, mod, i["file"])
                            f = os.path.relpath(unicodedata.normalize("NFKD", f).encode('ascii','ignore'))
                            code.append(f)

        # Add binary data file if it exists
        bd = os.path.join (self.getLibraryCodePath(), 'BinaryData.cpp')
        if os.path.exists(bd): code.append (bd)

        return code

    def getLibraryCodePath (self):
        return os.path.join (self.getProjectDir(), "JuceLibraryCode")

    def getBuildableCode (self):
        return self.getProjectCode() + self.getLibraryCode()

    def getModuleInfo (self, mod):
        return ModuleInfo (os.path.join (self.getModulePath (mod), mod, 'juce_module_info'))

    def getTargetName (self, configName):
        if 'Darwin' in platform.system():
            tag = 'XCODE_MAC'
        elif 'Linux' in platform.system():
            tag = 'LINUX_MAKE'
        else:
            tag = 'CODEBLOCKS'

        configs = self.root.find ('EXPORTFORMATS').find(tag).find('CONFIGURATIONS')
        for config in configs:
            if config.attrib['name'] == configName:
                return config.attrib['targetName']

        return ''

    def compile (self, waf_build, config='Debug'):

        features = 'cxx '
        type = self.getProjectType()
        if type == 'guiapp':
            features += 'cxxprogram'
        elif type == 'dll':
            features += 'cxxshlib'

        # TODO: figure out which compiler we're using

        linkflags = []
        cxxflags  = []

        for mod in self.getModules():
            info = self.getModuleInfo (mod)

            if is_linux():
                linkFlagsFunc = info.linuxLibs
            else:
                linkFlagsFunc = None

            if None != linkFlagsFunc:
                linkflags += linkFlagsFunc()

        target = self.getTargetName (config)
        if '' == target:
            target = 'a.out'

        object = waf_build (
            features  = features,
            source    = self.getBuildableCode(),
            includes  = [self.getLibraryCodePath()],
            linkflags = linkflags,
            name      = self.getName(),
            target    = self.getTargetName (config)
        )

        return object
