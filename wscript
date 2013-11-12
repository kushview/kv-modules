#!/usr/bin/env python

from subprocess import call, Popen, PIPE
import os, sys

sys.path.append (os.getcwd() + "/tools/waf")
import autowaf, audiounit, element, framework, juce, vst

common_tools = "element juce vst"

def options(opt):
    autowaf.set_options (opt, True)
    opt.load ("compiler_c compiler_cxx")
    opt.load (common_tools)
    if element.is_mac():
        opt.load ("apple audiounit framework");

def configure (conf):
    conf.load ("compiler_c compiler_cxx")
    autowaf.configure (conf)
    conf.load (common_tools)
    if element.is_mac():
        conf.load ("apple audiounit framework");

    print
    autowaf.display_header ("Element Configuration")

    if element.is_mac() and not conf.env.REZ: conf.check_rez()

    conf.check_vst()
    conf.check_cxx11()
    conf.line_just = 40

    conf.find_program ('ttl2c')

    # Do pkg-config stuff
    autowaf.check_pkg (conf, "lv2", uselib_store="LV2", mandatory=False)
    autowaf.check_pkg (conf, "lilv-0", uselib_store="LILV", mandatory=False)
    autowaf.check_pkg (conf, "lvtk-plugin-1", uselib_store="LVTK_PLUGIN", mandatory=False)
    autowaf.check_pkg (conf, "lvtk-ui-1", uselib_store="LVTK_UI", mandatory=False)
    autowaf.check_pkg (conf, "suil-0", uselib_store="SUIL", mandatory=False)
    autowaf.check_pkg (conf, "jack", uselib_store="JACK", mandatory=False)
    pkg_defs = ['HAVE_LILV', 'HAVE_JACK', 'HAVE_SUIL', 'HAVE_LV2', 'HAVE_LVTK_PLUGIN', 'HAVE_LVTK_UI']

    if element.is_linux():
        autowaf.check_pkg (conf, "juce-audio-processors", uselib_store="JUCE_AUDIO_PROCESSORS", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-audio-devices", uselib_store="JUCE_AUDIO_DEVICES", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-core", uselib_store="JUCE_CORE", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-cryptography", uselib_store="JUCE_CRYPTOGRAPHY", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-gui-basics", uselib_store="JUCE_GUI_BASICS", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-gui-extra", uselib_store="JUCE_GUI_EXTRA", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-graphics", uselib_store="JUCE_GRAPHICS", minimum_version='2.1.8', mandatory=True)
        autowaf.check_pkg (conf, "juce-opengl", uselib_store="JUCE_OPENGL", minimum_version='2.1.8', mandatory=True)

        autowaf.check_pkg (conf, "alsa", uselib_store="ALSA", mandatory=True)
        autowaf.check_pkg (conf, "x11", uselib_store="X11", mandatory=True)
        autowaf.check_pkg (conf, "xext", uselib_store="XEXT", mandatory=True)
        autowaf.check_pkg (conf, "freetype2", uselib_store="FREETYPE2", mandatory=True)
        autowaf.check_pkg (conf, "gl", uselib_store="GL", mandatory=True)
        pkg_defs += ['HAVE_ALSA', 'HAVE_X11', 'HAVE_XEXT', 'HAVE_FREETYPE2', 'HAVE_GL']

    for d in pkg_defs: conf.env[d] = conf.is_defined (d)

    conf.env.ELEMENT_VERSION_STRING = version_string()
    conf.define ("ELEMENT_VERSION_STRING", conf.env.VERSION_STRING)

    if element.is_mac():
        conf.env.MODULEDIR = "/Library/Application Support/Element/Plug-Ins"
    else:
        conf.env.MODULEDIR = conf.env.LIBDIR + "/element/modules"

    conf.env.append_unique ("MODULE_PATH", [conf.env.MODULEDIR])
    conf.define ("ELEMENT_DEFAULT_MODULE_PATH", ":".join(conf.env.MODULE_PATH))

    print
    autowaf.display_header ("Element Build Summary")
    conf.display_msg ("Build Introjucer", conf.env.BUILD_INTROJUCER)
    conf.display_msg ("Jack Audio Support", conf.env.HAVE_JACK)
    conf.display_msg ("LV2 Plugin Support", conf.env.HAVE_LILV)
    conf.display_msg ("LV2 Plugin GUI Support", conf.env.HAVE_SUIL)
    conf.display_msg ("Library Version", conf.env.VERSION_STRING)
    conf.display_msg ("Module Install Dir", conf.env.MODULEDIR)
    conf.display_msg ("Module Search Path", conf.env.MODULE_PATH)

    if element.is_mac():
        print
        autowaf.display_header ("Apple Configuration Summary")
        conf.display_msg ("Apple Framework Dir", conf.env.FRAMEWORKDIR)
        conf.display_msg ("Apple Deployment Target", conf.env.APPLE_VERSION_MIN)

    print
    autowaf.display_header ("Global Compiler Flags")
    conf.display_msg ("Compile flags (c)", conf.env.CFLAGS)
    conf.display_msg ("Compile flags (c++)", conf.env.CXXFLAGS)
    conf.display_msg ("Linker flags", conf.env.LINKFLAGS)

def build_pc_file (bld, name, slug):
    src = "%s.pc.in" % slug
    tgt = "%s.pc" % slug

    if not os.path.exists (src):
        return

    pc = bld (
        features     = "subst",
        source       = src,
        target       = tgt,
        INCLUDEDIR   = bld.env.INCLUDEDIR,
        VERSION      = bld.env.VERSION_STRING,
        LIBRARIES    = "",
        LDFLAGS      = "",
        REQUIRES     = "",
        install_path = bld.env.LIBDIR + "/pkgconfig"
    )

    if element.is_mac():
        pc.LIBRARIES += " -framework " + name
        if bld.env.FRAMEWORKDIR != "/Library/Frameworks":
            pc.LDFLAGS += " -F " + bld.env.FRAMEWORKDIR

def make_library (bld, name, libname, mods):
    thelib = juce.create_unified_lib (bld, libname, mods)
    thelib.includes += [".", "element", "project/JuceLibraryCode"]

    build_frameworks = False
    if element.is_mac() and build_frameworks:
        thelib.mac_framework = True
        thelib.target = "Frameworks/%s" % name
    else:
        thelib.vnum = bld.env.ELEMENT_VERSION_STRING

    build_pc_file (bld, name, libname)
    install_module_headers (bld, mods)

    return thelib

element_modules = '''
    juce_core
    juce_audio_basics
    juce_audio_formats
    juce_audio_devices
    juce_audio_processors
    juce_audio_utils
    juce_cryptography
    juce_data_structures
    juce_events
    juce_graphics
    juce_gui_basics
    juce_gui_extra
    juce_opengl
'''.split()

def wipe (ctx):
    if element.is_mac(): wipe_mac_packages (ctx)

def build(bld):

    bld.add_group()

    glob = bld.path.ant_glob

    # The main element library/framework
    e = make_library (bld, "Element", "element", element_modules)
    e.source += glob ('src/**/*.cpp')

    if element.is_linux():
        e.use += ['LV2', 'LILV', 'SUIL', 'JACK', 'X11', 'XEXT', 'ALSA', 'GL', 'FREETYPE2']
    elif element.is_mac():
        e.use += ["LV2", "LILV", "SUIL", "AUDIO_TOOLBOX", "APP_KIT"]

    #bld.recurse ('plugins')

    bld.program (
        source = glob ('project/Source/**/*.cpp'),
        name = 'Element Application',
        target = 'bin/element',
        use = 'element',
        includes = ['project/JuceLibraryCode']
    )

    if bld.env.BUILD_INTROJUCER:
        jucer = juce.IntrojucerProject ('libs/juce/extras/Introjucer/Introjucer.jucer')
        obj = jucer.compile (bld)
        obj.install_path = None

    install_headers (bld)

def install_headers (bld):
    if bld.options.install_headers:
        include_dir = bld.env.INCLUDEDIR + "/element-0"
        bld.install_files (os.path.join (include_dir, "libs/juce"), bld.path.ant_glob ("element/juce/*.h"), relative_trick=False)
        bld.install_files (include_dir, bld.path.ant_glob ("element/**/*.h"), relative_trick=True)

def install_module_headers (bld, modules):
    if bld.options.install_headers:
        # Juce modules go to PREFIX/include/element-0/juce right now
        include_dir = bld.env.INCLUDEDIR + "/element-0"
        for mod in modules:
            bld.install_files (include_dir + "/libs", bld.path.ant_glob ("libs/juce/modules/" + mod + "/**/*.h"), \
                relative_trick=True, cwd=bld.path.find_dir ("libs"))

def check(ctx):
    call (["bash", "tools/run-tests"])

def patch(bld):
    call (["bash", "tools/patch-namespace", "juce", "Element"])

def unpatch(bld):
    call (["bash", "tools/patch-namespace", "Element", "juce"])

def version(bld):
    call (["bash", "tools/version", "verbose"])

## Helpers ##

def version_string():
    p2 = Popen (["bash", "tools/version", "standard"], stdout=PIPE)
    return p2.communicate()[0].strip()

def wipe_mac_packages (bld):
    if element.is_mac():
        call (["rm", "-rf", "build/Frameworks"])
        call (["rm", "-rf", "build/Applications"])
        call (["rm", "-rf", "build/Plug-Ins"])

from waflib import TaskGen
@TaskGen.extension ('.mm')
def mm_hook (self, node):
    return self.create_compiled_task ('cxx', node)
