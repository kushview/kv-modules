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

    # Do pkg-config stuff
    autowaf.check_pkg (conf, "lv2", uselib_store="LV2", mandatory=False)
    autowaf.check_pkg (conf, "lilv-0", uselib_store="LILV", mandatory=False)
    autowaf.check_pkg (conf, "suil-0", uselib_store="SUIL", mandatory=False)
    autowaf.check_pkg (conf, "jack", uselib_store="JACK", mandatory=False)
    pkg_defs = ['HAVE_LILV', 'HAVE_JACK', 'HAVE_SUIL', 'HAVE_LV2']

    if element.is_linux():
        autowaf.check_pkg (conf, "alsa", uselib_store="ALSA", mandatory=True)
        autowaf.check_pkg (conf, "x11", uselib_store="X11", mandatory=True)
        autowaf.check_pkg (conf, "xext", uselib_store="XEXT", mandatory=True)
        autowaf.check_pkg (conf, "freetype2", uselib_store="FREETYPE2", mandatory=True)
    for d in pkg_defs: conf.env[d] = conf.is_defined (d)

    conf.env.ELEMENT_VERSION_STRING = version_string()
    conf.define ("ELEMENT_VERSION_STRING", conf.env.VERSION_STRING)

    if element.is_mac():
        conf.env.MODULEDIR = "/Library/Application Support/Element/Plug-Ins"
    else:
        conf.env.MODULEDIR = conf.env.LIBDIR + "/element/modules"

    conf.env.append_unique ("MODULE_PATH", [conf.env.MODULEDIR])
    conf.define ("ELEMENT_DEFAULT_MODULE_PATH", ":".join(conf.env.MODULE_PATH))

    conf.define ("JUCE_LOG_ASSERTIONS", 1)

    print # Do the summary
    autowaf.display_header ("Element Build Summary")
    conf.display_msg ("Build Jack Module", conf.env.HAVE_JACK)
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
    thelib.includes += [".", "element", "libs"]

    build_frameworks = False
    if element.is_mac() and build_frameworks:
        thelib.mac_framework = True
        thelib.target = "Frameworks/%s" % name
    else:
        thelib.vnum = bld.env.ELEMENT_VERSION_STRING

    build_pc_file (bld, name, libname)
    install_module_headers (bld, mods)

    return thelib

core_modules = '''
    juce_core
'''.split()

audio_modules = '''
    juce_audio_basics
    juce_audio_formats
'''.split()

gui_modules = '''

'''.split()

engine_modules = '''

'''.split()

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
'''.split()

def wipe (ctx):
    if element.is_mac(): wipe_mac_packages (ctx)

def build(bld):

    # Let these headers appear as if they lived inside the juce folder
    #bld(rule="cp -f ${SRC} ${TGT}", source="element/juce/JuceHeader.h",
    #                            target="libs/juce/JuceHeader.h")
    #bld(rule="cp -f ${SRC} ${TGT}", source="element/juce/AppConfig.h",
    #                             target="libs/juce/AppConfig.h")

    bld.add_group()

    glob = bld.path.ant_glob

    # The main element library/framework
    e = make_library (bld, "Element", "element-0", element_modules)
    e.source += glob ('src/**/*.cpp')
    e.includes += ['libs/juce']
    if element.is_linux():
        e.use += ["LV2", "LILV", "SUIL", "ALSA", "X11", "XEXT", "FREETYPE2"]
    elif element.is_mac():
        e.use += ["LV2", "LILV", "SUIL", "AUDIO_TOOLBOX", "APP_KIT"]
    bld.add_group()

    #other_dirs = ["plugins", "modules", "tools", "tests", "src"]
    other_dirs = ['modules', 'src', 'tools']
    for o in other_dirs:
        bld.recurse (o)
        bld.add_group()

    juce_extras = "libs/juce/extras"
    introjucer  = os.path.join (juce_extras, "Introjucer")



    install_headers (bld)

def install_headers (bld):
    if bld.options.install_headers:
        include_dir = bld.env.INCLUDEDIR + "/element-0"
        bld.install_files (os.path.join (include_dir, "libs/juce"), bld.path.ant_glob ("element/juce/*.h"), relative_trick=False)
        bld.install_files (include_dir, bld.path.ant_glob ("element/**/*.hpp"), relative_trick=True)

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
    call (["bash", "tools/patch-namespace", "juce", "element"])

def unpatch(bld):
    call (["bash", "tools/patch-namespace", "element", "juce"])

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

