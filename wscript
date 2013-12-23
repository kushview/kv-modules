#!/usr/bin/env python

from subprocess import call, Popen, PIPE
import os, sys

sys.path.append (os.getcwd() + "/tools/waf")
import juce

common_tools = 'juce'

def options(opt):
    opt.load ("compiler_c compiler_cxx")
    opt.load (common_tools)

def configure (conf):
    conf.load ("compiler_c compiler_cxx")
    conf.load (common_tools)

    conf.env.DATADIR = os.path.join (conf.env.PREFIX, 'share')

    print
    juce.display_header ("Element Configuration")
    conf.check_cxx11()
    conf.line_just = 40

    # Do pkg-config stuff
    conf.check_cfg (package="lv2", uselib_store="LV2", args='--cflags --libs', mandatory=True)
    conf.check_cfg (package="lilv-0", uselib_store="LILV", args='--cflags --libs', mandatory=True)
    conf.check_cfg (package="lvtk-plugin-1", uselib_store="LVTK_PLUGIN", args='--cflags --libs', mandatory=False)
    conf.check_cfg (package="lvtk-ui-1", uselib_store="LVTK_UI", args='--cflags --libs', mandatory=False)
    conf.check_cfg (package="suil-0", uselib_store="SUIL", args='--cflags --libs', mandatory=True)
    conf.check_cfg (package="jack", uselib_store="JACK", args='--cflags --libs', mandatory=False)
    pkg_defs = ['HAVE_LILV', 'HAVE_JACK', 'HAVE_SUIL', 'HAVE_LV2', 'HAVE_LVTK_PLUGIN', 'HAVE_LVTK_UI']

    if juce.is_linux():
        conf.check_cfg (package="juce-audio-processors-3", uselib_store="JUCE_AUDIO_PROCESSORS", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-audio-devices-3", uselib_store="JUCE_AUDIO_DEVICES", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-core-3", uselib_store="JUCE_CORE", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-cryptography-3", uselib_store="JUCE_CRYPTOGRAPHY", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-gui-basics-3", uselib_store="JUCE_GUI_BASICS", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-gui-extra-3", uselib_store="JUCE_GUI_EXTRA", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-graphics-3", uselib_store="JUCE_GRAPHICS", minimum_version='2.1.8', mandatory=True)
        conf.check_cfg (package="juce-opengl-3", uselib_store="JUCE_OPENGL", minimum_version='2.1.8', mandatory=True)

        conf.check_cfg (package="alsa", uselib_store="ALSA", args='--cflags --libs', mandatory=True)
        conf.check_cfg (package="x11", uselib_store="X11", args='--cflags --libs', mandatory=True)
        conf.check_cfg (package="xext", uselib_store="XEXT", args='--cflags --libs', mandatory=True)
        conf.check_cfg (package="freetype2", uselib_store="FREETYPE2", args='--cflags --libs', mandatory=True)
        conf.check_cfg (package="gl", uselib_store="GL", args='--cflags --libs', mandatory=True)
        pkg_defs += ['HAVE_ALSA', 'HAVE_X11', 'HAVE_XEXT', 'HAVE_FREETYPE2', 'HAVE_GL']

    for d in pkg_defs: conf.env[d] = conf.is_defined (d)

    if 'clang' in conf.env.CXX[0]:
        conf.env.append_unique ('CFLAGS', ['-Wno-deprecated-register'])
        conf.env.append_unique ('CXXFLAGS', ['-Wno-deprecated-register'])

    conf.env.ELEMENT_VERSION_STRING = version_string()
    conf.define ("ELEMENT_VERSION_STRING", conf.env.VERSION_STRING)

    if juce.is_mac():
        conf.env.MODULEDIR = "/Library/Application Support/Element/Plug-Ins"
    else:
        conf.env.MODULEDIR = conf.env.LIBDIR + "/element/modules"

    conf.env.append_unique ("MODULE_PATH", [conf.env.MODULEDIR])
    conf.define ("ELEMENT_DEFAULT_MODULE_PATH", ":".join(conf.env.MODULE_PATH))

    print
    juce.display_header ("Element Build Summary")
    juce.display_msg (conf, "Installation Prefix", conf.env.PREFIX)
    juce.display_msg (conf, "Installed DATADIR", conf.env.DATADIR)
    juce.display_msg (conf, "Build Element (app)", conf.env.BUILD_ELEMENT_APP)
    juce.display_msg (conf, "Build Introjucer", conf.env.BUILD_INTROJUCER)
    juce.display_msg (conf, "Jack Audio Support", conf.env.HAVE_JACK)
    juce.display_msg (conf, "LV2 Plugin Support", conf.env.HAVE_LILV)
    juce.display_msg (conf, "LV2 Plugin GUI Support", conf.env.HAVE_SUIL)
    juce.display_msg (conf, "Library Version", conf.env.VERSION_STRING)
    juce.display_msg (conf, "Module Install Dir", conf.env.MODULEDIR)
    juce.display_msg (conf, "Module Search Path", conf.env.MODULE_PATH)

    if juce.is_mac():
        print
        juce.display_header ("Apple Configuration Summary")
        juce.display_msg (conf, "Apple Framework Dir", conf.env.FRAMEWORKDIR)
        juce.display_msg (conf, "Apple Deployment Target", conf.env.APPLE_VERSION_MIN)

    print
    juce.display_header ("Global Compiler Flags")
    juce.display_msg (conf, "Compile flags (c)", conf.env.CFLAGS)
    juce.display_msg (conf, "Compile flags (c++)", conf.env.CXXFLAGS)
    juce.display_msg (conf, "Linker flags", conf.env.LINKFLAGS)

def build_pc_file (bld, name, slug):

    # hack
    if slug == 'element':
        slug = 'element-0'

    src = "%s.pc.in" % slug
    tgt = "%s.pc" % slug

    if not os.path.exists (src):
        return

    pc = bld (
        features     = 'subst',
        source       = src,
        target       = tgt,
        INCLUDEDIR   = bld.env.INCLUDEDIR,
        VERSION      = bld.env.ELEMENT_VERSION_STRING,
        LIBRARIES    = '',
        LDFLAGS      = '',
        REQUIRES     = '',
        install_path = bld.env.LIBDIR + '/pkgconfig'
    )

    if bld.env.HAVE_LILV:
        pc.REQUIRES += ' lilv-0'
    if bld.env.HAVE_JACK:
        pc.REQUIRES += ' jack'
    if bld.env.HAVE_SUIL:
        pc.REQUIRES += ' suil-0'

    if juce.is_mac():
        pc.LIBRARIES += " -framework " + name
        if bld.env.FRAMEWORKDIR != "/Library/Frameworks":
            pc.LDFLAGS += " -F " + bld.env.FRAMEWORKDIR

def make_desktop (bld, slug):
    if not juce.is_linux():
        return

    src = "data/%s.desktop.in" % (slug)
    tgt = "%s.desktop" % (slug)

    element_data = '%s/element' % (bld.env.DATADIR)
    element_bin  = '%s/bin' % (bld.env.PREFIX)

    if os.path.exists (src):
        bld (features = "subst",
             source    = src,
             target    = tgt,
             name      = tgt,
             ELEMENT_BINDIR = element_bin,
             ELEMENT_DATA = element_data,
             install_path = bld.env.DATADIR + "/applications"
        )

        bld.install_files (element_data, 'data/element_icon.xpm')

def build (bld):
    node = bld.path.find_resource ('project/Element.jucer')
    proj = juce.IntrojucerProject (bld, node.relpath())
    
    if not proj.isValid():
        exit(1)

    obj = bld.shlib (
        source   = proj.getLibraryCode(),
        includes = ['project/JuceLibraryCode'],
        name = 'libelement',
        target = 'element',
        use = ['LILV', 'SUIL', 'X11', 'ALSA', 'FREETYPE2', 'GL'],
        vnum = '0.0.1',
    )

    bld.add_group()

    make_desktop (bld, 'element')
    obj = bld.program (
        source = proj.getProjectCode(),
        includes = ['project/JuceLibraryCode', 'project/Source'],
        use = ['libelement'],
        target = 'element'
    )

def install_headers (bld):
    if bld.options.install_headers:
        include_dir = bld.env.INCLUDEDIR + "/element-0"
        bld.install_files (include_dir, bld.path.ant_glob ("element/**/*.h"), relative_trick=True)
        bld.install_files (include_dir, bld.path.ant_glob ('project/JuceLibraryCode/AppConfig.h'))
        bld.install_files (include_dir, bld.path.ant_glob ('project/JuceLibraryCode/JuceHeader.h'))
        bld.install_files (include_dir + '/modules/element', 'element_module.h')

def install_module_headers (bld, modules):
    if bld.options.install_headers:
        # Juce modules go to PREFIX/include/element-0/modules
        include_dir = bld.env.INCLUDEDIR + "/element-0"
        for mod in modules:
            bld.install_files (include_dir, bld.path.ant_glob ("libs/juce/modules/" + mod + "/**/*.h"), \
                relative_trick=True, cwd=bld.path.find_dir ("libs/juce"))

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

