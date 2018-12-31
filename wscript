#!/usr/bin/env python

from subprocess import call, Popen, PIPE
import os, sys

sys.path.append (os.getcwd() + "/tools/waf")
import juce

VERSION='0.0.1'

def options (opt):
    opt.load ("compiler_c compiler_cxx juce")

def silence_warnings(conf):
    '''TODO: resolve these'''
    # conf.env.append_unique ('CFLAGS', ['-Wno-deprecated-register'])
    # conf.env.append_unique ('CXXFLAGS', ['-Wno-deprecated-register'])
    # conf.env.append_unique ('CFLAGS', ['-Wno-dynamic-class-memaccess'])
    # conf.env.append_unique ('CXXFLAGS', ['-Wno-dynamic-class-memaccess'])
    # conf.env.append_unique ('CFLAGS', ['-Wno-deprecated-declarations'])
    # conf.env.append_unique ('CXXFLAGS', ['-Wno-deprecated-declarations'])

def configure (conf):
    conf.env.DATADIR = os.path.join (conf.env.PREFIX, 'share/kushview/kv-modules')
    
    conf.check_ccache()
    conf.prefer_clang()
    conf.load ("compiler_c compiler_cxx juce")
    conf.check_cxx_version()

    conf.env.DEBUG = conf.options.debug
    conf.env.EL_VERSION_STRING = VERSION

    conf.check(lib='curl', mandatory=False)
    conf.check_cfg(path='python-config', args='--cflags --libs', package='', uselib_store='PYTHON')
    if juce.is_linux():
        conf.check(lib='pthread', mandatory=True)
        conf.check(lib='dl', mandatory=True)
        conf.check_cfg(package='freetype2', args='--cflags --libs', mandatory=True)
        conf.check_cfg(package='x11', args='--cflags --libs', mandatory=True)
        conf.check_cfg(package='xext', args='--cflags --libs', mandatory=True)
        conf.check_cfg(package='alsa', args='--cflags --libs', mandatory=True)
    if juce.is_windows():
        conf.check(lib='ws2_32', mandatory=True)
        conf.check(lib='pthread', mandatory=True)

    print
    juce.display_header ("Library Configuration")
    juce.display_msg (conf, "Installation PREFIX", conf.env.PREFIX)
    juce.display_msg (conf, "Installation DATADIR", conf.env.DATADIR)
    juce.display_msg (conf, "Debugging Symbols", conf.options.debug)

    print
    juce.display_header ("Compiler")
    juce.display_msg (conf, "CFLAGS", conf.env.CFLAGS)
    juce.display_msg (conf, "CXXFLAGS", conf.env.CXXFLAGS)
    juce.display_msg (conf, "LINKFLAGS", conf.env.LINKFLAGS)

def build_desktop (bld, slug):
    if not juce.is_linux():
        return

    src = "data/%s.desktop.in" % (slug)
    tgt = "%s.desktop" % (slug)

    kv_data = '%s/kushview/kv' % (bld.env.DATADIR)
    kv_bin  = '%s/bin' % (bld.env.PREFIX)

    if os.path.exists (src):
        bld (features = "subst",
             source    = src,
             target    = tgt,
             name      = tgt,
             BINDIR    = kv_bin,
             DATADIR   = kv_data,
             install_path = None # bld.env.DATADIR + "/applications"
        )

def build_linux (bld):
    libEnv = bld.env.derive()
    bld.program (
        source      = [
            'src/modules.cpp'
        ],
        includes    = [ os.path.expanduser('~') + '/SDKs/JUCE/modules',
                        'modules', 'src' ],
        target      = 'lib/kv',
        name        = 'KV',
        env         = libEnv,
        use         = [ 'FREETYPE2', 'X11', 'DL', 'PTHREAD', 'ALSA', 'XEXT', 'PYTHON', 'CURL' ]
    )

def build (bld):
    build_linux(bld)

def check (ctx):
    call (["build/tests/tests"])
