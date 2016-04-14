#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json, os, platform, re, sys, unicodedata
from xml.etree import ElementTree as ET

from waflib import Utils, Logs, Errors
from waflib.Configure import conf

def convert_camel (words, upper=False):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', words)
    if upper: return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).upper()
    else: return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

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
def prefer_clang(self):
    '''Use clang by default on non-windows'''
    if is_windows(): return
    if not 'CC' in os.environ or not 'CXX' in os.environ:
        if None != self.find_program ('clang', mandatory=False):
            self.env.CC  = 'clang'
            self.env.CXX = 'clang++'

@conf
def check_juce (self):
    '''this just checks that a version of juce exists'''

    display_msg (self, "Checking for JUCE")
    mpath = self.env.JUCE_MODULES_PATH = self.options.juce_modules

    if os.path.exists (mpath):
        minfo = open(mpath + "/juce_core/juce_module_info")
        mdata = json.load(minfo)
        minfo.close()
        self.end_msg (mdata["version"])
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

@conf
def check_juce_cfg (self, mods=None, major_version='4', mandatory=False):
    line_just = self.line_just
    if mods == None: modules = '''
        juce_audio_basics
        juce_audio_devices
        juce_audio_formats
        juce_audio_processors
        juce_audio_utils
        juce_box2d
        juce_core
        juce_cryptography
        juce_data_structures
        juce_events
        juce_graphics
        juce_gui_basics
        juce_gui_extra
        juce_opengl
        juce_video'''.split()
    else: modules = mods

    useflags = []

    for mod in modules:
        mod = mod.replace('-', '_')
        if not mod[:5] == 'juce_':
            mod = 'juce_' + mod
        if self.options.debug:
            mod += '_debug'
        pkgslug = '%s-%s' % (mod.replace ('_', '-'), major_version)
        self.check_cfg (package=pkgslug, uselib_store=mod.replace('_debug','').upper(),  \
                        args=['--libs', '--cflags'], mandatory=mandatory)
        useflags.append (mod.upper())

    self.line_just = line_just
    return useflags

def is_mac():
    return 'Darwin' in platform.system()

def is_linux():
    return 'Linux' in platform.system()

def is_win32():
    return 'Windows' in platform.system()

def is_windows():
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
            if None != res:
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
        if None == self.data or not 'dependencies' in self.data:
            return []

        if not len(self.data ['dependencies']) > 0:
            return []

        deps = []
        for dep in self.data ['dependencies']:
            if None != dep ['id']:
                deps.append (dep ['id'])

        return deps

    def requiredPackages (self, debug=False):
        pkgs = []

        for dep in self.dependencies():
            pkg = dep.replace ('_', '-')
            mv = self.version()[:1]
            if 'juce-' in pkg:
                mv = '4'
            pkg += '-debug-%s' % (mv) if debug else '-%s' % (mv)
            pkgs.append (pkg)

        if is_mac():
            pkgs += self.osxFrameworks()

        return list (set (pkgs))

    def website (self):
        return self.data ['website']

    def license (self):
        return self.data ['license']

    def linuxLibs (self):
        libs = []

        if None == self.data or not is_linux() or not 'LinuxLibs' in self.data:
            return libs

        for lib in self.data ['LinuxLibs'].split():
            l = '-l%s' % lib
            libs.append (l)

        return libs

    def linkFlags (self):
        '''returns a list of linker flags in an array'''
        if is_linux ():
            return self.linuxLibs()
        return []

    def osxFrameworks (self):
        ''' Returns an array of frameworks (as useflags) this module
            requires'''
        fwks = []

        if None == self.data or not is_mac() or not 'OSXFrameworks' in self.data:
            return fwks

        for fw in self.data['OSXFrameworks'].split():
            if not fw in fwks:
                fwks.append (convert_camel (fw, True))

        return fwks


def get_module_info (ctx, mod):
    nodes = find (ctx, os.path.join (mod, 'juce_module_info'))
    infofile = "%s" % nodes[0].relpath()
    return ModuleInfo (infofile)

def plugin_pattern (bld):
    ''' this is only valid after 'juce.py' has been loading during configure'''
    return bld.env['plugin_PATTERN']

def plugin_extension (bld):
    ''' this is only valid after 'juce.py' has been loading during configure'''
    return bld.env['plugin_EXT']

def options (opt):
    opt.add_option ('--debug', default=False, action="store_true", dest="debug", help="Compile debuggable binaries [ Default: False ]")

def configure (conf):

    # debugging option
    if conf.options.debug:
        conf.define ("DEBUG", 1)
        conf.define ("_DEBUG", 1)
        conf.env.append_unique ('CXXFLAGS', ['-g', '-ggdb', '-O0'])
        conf.env.append_unique ('CFLAGS', ['-g', '-ggdb', '-O0'])
    else:
        conf.define ("NDEBUG", 1)
        conf.env.append_unique ('CXXFLAGS', ['-Os'])
        conf.env.append_unique ('CFLAGS', ['-Os'])

    # output dir (build dir)
    outdir = conf.options.out
    if len (outdir) == 0:
        outdir = "build"

    # module path
    if not conf.env.JUCE_MODULE_PATH:
        conf.env.JUCE_MODULE_PATH = os.path.join (os.path.expanduser("~"), 'juce/modules')

    # define a library pattern suitable for plugins/modules
    # (e.g. remove the 'lib' from libplugin.XXX)
    pat = conf.env['cshlib_PATTERN']
    if not pat:
        pat = conf.env['cxxshlib_PATTERN']
    if pat.startswith('lib'):
        pat = pat[3:]
    conf.env['plugin_PATTERN'] = pat
    conf.env['plugin_EXT'] = pat[pat.rfind('.'):]

    # do platform stuff
    if is_linux():
        conf.define ('LINUX', 1)
    elif is_mac():
        conf.env.FRAMEWORK_ACCELERATE     = 'Accelerate'
        conf.env.FRAMEWORK_AUDIO_TOOLBOX  = 'AudioToolbox'
        conf.env.FRAMEWORK_CORE_AUDIO     = 'CoreAudio'
        conf.env.FRAMEWORK_CORE_MIDI      = 'CoreMIDI'
        conf.env.FRAMEWORK_COCOA          = 'Cocoa'
        conf.env.FRAMEWORK_CARBON         = 'Carbon'
        conf.env.FRAMEWORK_DISC_RECORDING = 'DiscRecording'
        conf.env.FRAMEWORK_IO_KIT         = 'IOKit'
        conf.env.FRAMEWORK_OPEN_GL        = 'OpenGL'
        conf.env.FRAMEWORK_QT_KIT         = 'QTKit'
        conf.env.FRAMEWORK_QuickTime      = 'QuickTime'
        conf.env.FRAMEWORK_QUARTZ_CORE    = 'QuartzCore'
        conf.env.FRAMEWORK_WEB_KIT        = 'WebKit'
    elif is_win32(): pass

def extension():
    if platform.system() != "Darwin":
        return ".cpp"
    else:
        return ".mm"

def find (ctx, pattern):
    '''find resources in the juce module path'''

    if len(pattern) <= 0:
        return None

    pattern = '%s/**/%s' % (ctx.env.JUCE_MODULE_PATH, pattern)
    return ctx.path.ant_glob (pattern)

def build_modular_libs (bld, mods, vnum='4.0.2', postfix=''):
    '''compile the passed modules into individual targets. returns
        a list of waf bld objects in case further setup is required'''
    libs = []
    mext = extension()
    opengl_wanted = 'juce_opengl' in mods;

    for mod in mods:
        info = get_module_info (bld, mod)
        src  = find (bld, mod + mext)
        slug = mod.replace('_', '-')
        use  = info.requiredPackages()
        major_version = vnum[:1]
        if is_linux() and mod == 'juce_graphics':
            use += ['FREETYPE2']

        obj = bld (
            features  = "cxx cxxshlib",
            source    = list (set (src)),
            name      = '%s-%s' % (slug + postfix.replace('_', '-'), major_version),
            target    = '%s-%s' % (mod + postfix, major_version),
            use       = list (set (use)),
            includes  = [],
            linkflags = info.linkFlags()
        )

        if len(vnum) > 0:
            obj.vnum = vnum

        obj.module_info = info
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
    us = list (set (us))

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
    ctx  = None
    data = None
    proj = None
    root = None

    def __init__ (self, context, project):
        if os.path.exists (project):
            self.ctx = context
            self.proj = project
            data = ET.parse (self.proj)

            if data.getroot().tag == "JUCERPROJECT":
                self.data = data
                self.root = data.getroot()
            else:
                self.data = None

    def isValid (self):
        return self.data != None and self.proj != None

    def getProperty (self, prop):
        if self.isValid(): return self.root.attrib [prop]
        else: return ''

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

        if None == self.root:
            return []

        mods = []

        # have to iterate over tags MODULE and MODULES
        # because some older projects might have one, the other
        # or both

        for mod in self.root.iter ("MODULE"):
            if 'id' in mod.attrib:
                mods += [mod.attrib ["id"]]

        for mod in self.root.iter ("MODULES"):
            if 'id' in mod.attrib:
                if not mod.attrib ['id'] in mods:
                    mods += [mod.attrib ['id']]

        return mods

    def getDefaultExporterTag (self):
        tag = 'INVALID'

        if is_mac():
            tag = 'XCODE_MAC'
        elif is_linux():
            tag = 'LINUX_MAKE'

        return tag

    def getModulePath (self, module):

        tag = self.getDefaultExporterTag()
        paths = self.root.find ('EXPORTFORMATS')

        if None != paths: paths = paths.find (tag)
        else: return ''

        if None != paths: paths = paths.find('MODULEPATHS')
        else: return ''

        for path in paths.iter ('MODULEPATH'):
            if module == path.attrib ['id']:
                return os.path.join (self.getProjectDir(), '%s/%s' % (path.attrib ['path'], module))

        return ''

    def getProjectDir(self):
        if self.isValid():
            return os.path.relpath (os.path.join (self.proj, ".."))
        else: return ''

    def getProjectCode(self):
        code = []

        if None == self.root:
            return code

        join   = os.path.join
        depth  = os.path.relpath (self.ctx.launch_dir, self.ctx.path.abspath())
        parent = os.path.dirname (self.proj)
        parent = os.path.relpath (parent, self.ctx.launch_dir)

        for c in self.root.iter ("FILE"):
            if "compile" in c.attrib and c.attrib["compile"] == "1":
                f = "%s" % (c.attrib ["file"])
                #parent = join (self.proj, "..")
                source = join (depth, join (parent, os.path.relpath (f)))
                code.append (source)

        return code

    def getLibraryCode (self):
        code = []
        for mod in self.getModules():
            local_path = os.path.join (self.getProjectDir(), 'JuceLibraryCode/modules/%s/juce_module_info' % (mod))
            if os.path.exists (local_path):
                module_path = os.path.join (self.getProjectDir(), 'JuceLibraryCode/modules/%s' % mod)
            else:
                module_path = self.getModulePath (mod)

            infofile = os.path.join (module_path, 'juce_module_info')
            obj = ModuleInfo (infofile)

            if os.path.exists (infofile):
                res = open (infofile)
                data = json.load (res)
                res.close()

                if "compile" in data:
                    for i in data["compile"]:
                        if is_mac(): target_key = 'xcode'
                        else: target_key = '! xcode'

                        if 'target' in i and i['target'] == target_key:
                            f = '%s/%s' % (module_path, i['file'])
                            f = os.path.relpath (unicodedata.normalize("NFKD", f).encode ('ascii','ignore'))
                            code.append (f)
                        elif 'file' in i and not 'target' in i:
                            f = '%s/%s' % (module_path, i["file"])
                            f = os.path.relpath(unicodedata.normalize("NFKD", f).encode('ascii','ignore'))
                            code.append (f)
            else:
                print "Module file doesn't exist: " + infofile
                print "Project Dir: " + self.getProjectDir()
                print "Module Path = " + module_path
                exit(1)


        # Add binary data file if it exists
        bd = os.path.join (self.getLibraryCodePath(), 'BinaryData.cpp')
        if os.path.exists(bd): code.append (bd)

        library_code = []
        depth = os.path.relpath (self.ctx.launch_dir, self.ctx.path.abspath())
        for c in code:
            library_code.append (os.path.join (depth, c))

        return library_code

    def getLibraryCodePath (self):
        return os.path.join (self.getProjectDir(), "JuceLibraryCode")

    def getBuildableCode (self):
        return self.getProjectCode() + self.getLibraryCode()

    def getModuleInfo (self, mod):
        return ModuleInfo (os.path.join (self.getModulePath (mod), 'juce_module_info'))

    def getTargetName (self, configName):

        tag = self.getDefaultExporterTag()

        configs = self.root.find ('EXPORTFORMATS')
        if configs == None: return 'JuceTarget'

        configs = configs.find(tag)
        if configs == None: return 'JuceTarget'

        configs = configs.find('CONFIGURATIONS')
        for config in configs:
            if config.attrib['name'] == configName:
                return config.attrib['targetName']

        return 'JuceTarget'

    def getLinkFlags (self):
        flags = []
        for mod in self.getModules():
            info = self.getModuleInfo (mod)
            if is_linux():
                linkFlagsFunc = info.linuxLibs
            elif is_mac():
                linkFlagsFunc = info.osxFrameworks
            if None != linkFlagsFunc:
                flags += linkFlagsFunc()
        return flags

    def getUseFlags (self):
        flags = []

        for mod in self.getModules():
            info = self.getModuleInfo (mod)
            if is_linux():
                func = None
            elif is_mac():
                func = info.requiredPackages
            elif is_win32():
                func = None

            if None != func:
                flags += func()

        return list (set (flags))

    def compile (self, wafBuild, includeModuleCode=True):

        features = 'cxx '
        type = self.getProjectType()

        if type == 'guiapp':
            features += 'cxxprogram'
        elif type == 'dll':
            features += 'cxxshlib'

        # TODO: figure out which compiler we're using

        code      = self.getProjectCode()
        cxxflags  = []
        includes  = []
        linkflags = []
        useflags  = []

        depth = os.path.relpath (self.ctx.launch_dir, self.ctx.path.abspath())

        # Do special things when modules are included
        if includeModuleCode:
            code += self.getLibraryCode()
            includes += [os.path.join (depth, self.getLibraryCodePath())]
            for mod in self.getModules():
                info = self.getModuleInfo (mod)
                linkFlagsFunc = None

                if is_linux():
                    linkflags += info.linuxLibs()
                elif is_mac():
                    useflags += info.osxFrameworks()
                else: pass


        # Figure a target name
        target = self.getTargetName ('Debug')
        if '' == target:
            target = 'a.out'

        object = wafBuild (
            features  = features,
            source    = code,
            includes  = includes,
            name      = self.getName(),
            target    = target,
            use       = useflags,
            cflags    = [],
            cxxflags  = [],
            linkflags = linkflags,
        )

        # do mac bundling if applicable
        if is_mac():
            if type == 'guiapp':
                object.mac_app = True

        return object


from waflib import TaskGen
@TaskGen.extension ('.mm')
def juce_mm_hook (self, node):
    return self.create_compiled_task ('cxx', node)

from waflib import TaskGen
@TaskGen.extension ('.m')
def juce_m_hook (self, node):
    return self.create_compiled_task ('c', node)
