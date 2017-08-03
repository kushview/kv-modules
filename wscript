#!/usr/bin/evn python
# encoding: utf-8
# Copyright (C) 2012-2017 Michael Fisher <mfisher@kushview.net>

''' This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public Licence as published by
the Free Software Foundation, either version 3 of the Licence, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
file COPYING for more details. '''

import sys, os, platform
from subprocess import call

sys.path.insert(0, "tools/waf")
import juce

KV_MODULES_VERSION  = '0.0.1'
EXTRA_VERSION = ''
KV_MODULES_VERSION=KV_MODULES_VERSION+EXTRA_VERSION
KV_MAJOR_VERSION=0
KV_MINOR_VERSION=0
KV_MICRO_VERSION=1
KV_EXTRA_VERSION=EXTRA_VERSION

APPNAME = 'kv-modules'
VERSION = KV_MODULES_VERSION
top = '.'
out = 'build'

library_modules = '''
    kv_core
    kv_engines
    kv_gui
    kv_lv2
    kv_models
    kv_video
    kv_ffmpeg
'''.split()

def options(opts):
    opts.load ('compiler_c compiler_cxx juce')

    opts.add_option('--no-headers', default=True, action="store_false", \
        dest="install_headers", help="Don't install headers")
    opts.add_option('--static', default=False, action="store_true", \
        dest="static", help="Build Static Libraries [ Default: False ]")
    opts.add_option('--ziptype', default='gz', type='string', \
        dest='ziptype', help="Zip type for waf dist (gz/bz2/zip) [ Default: gz ]")

def configure(conf):
    # Put version defines in a header file
    conf.define ("KV_MODULES_VERSION", VERSION)
    conf.define ("KV_MAJOR_VERSION",KV_MAJOR_VERSION)
    conf.define ("KV_MINOR_VERSION",KV_MINOR_VERSION)
    conf.define ("KV_MICRO_VERSION",KV_MICRO_VERSION)
    conf.define ("KV_EXTRA_VERSION",KV_EXTRA_VERSION)
    conf.write_config_header ('element/modules/version.h', 'KV_MODULES_VERSION_H')

    conf.prefer_clang()
    conf.load ('compiler_c compiler_cxx juce')
    conf.check_inline()

    conf.env.DATADIR    = conf.env.PREFIX + '/share'
    conf.env.LIBDIR     = conf.env.PREFIX + '/lib'
    conf.env.BINDIR     = conf.env.PREFIX + '/bin'
    conf.env.INCLUDEDIR = conf.env.PREFIX + '/include'

    # Setup JUCE
    conf.load ('juce')
    conf.env.JUCE_MODULE_PATH = '/opt/kushview/JUCE/modules'
    conf.check_cxx11()

    # Export version to the environment
    conf.env.KV_MAJOR_VERSION = KV_MAJOR_VERSION
    conf.env.KV_MINOR_VERSION = KV_MINOR_VERSION
    conf.env.KV_MICRO_VERSION = KV_MICRO_VERSION
    conf.env.APPNAME = APPNAME

    # Store options in environment
    conf.env.BUILD_DEBUGGABLE   = conf.options.debug
    conf.env.BUILD_STATIC       = conf.options.static
    conf.env.INSTALL_HEADERS    = conf.options.install_headers

    if juce.is_linux():
        conf.check_juce_cfg()
        conf.check_cfg(package='lilv-0', uselib_store='LILV', args=['--cflags', '--libs'], mandatory=True)
        conf.check_cfg(package='suil-0', uselib_store='SUIL', args=['--cflags', '--libs'], mandatory=True)
        conf.check_cfg(package='alsa', uselib_store='ALSA', args=['--cflags', '--libs'], mandatory=True)
        conf.check_cfg(package='jack', uselib_store='JACK', args=['--cflags', '--libs'], mandatory=False)
        conf.check_cfg(package='gl', uselib_store='GL', args=['--cflags', '--libs'], mandatory=False)
        conf.check_cfg(package='x11', uselib_store='X11', args=['--cflags', '--libs'], mandatory=False)

    # this is just to clear all the defines up to this point
    conf.write_config_header('dummy.h')

    conf.define("KV_USE_JACK", len(conf.env.LIB_JACK) > 0)
    for mod in library_modules:
        conf.define('JUCE_MODULE_AVAILABLE_%s' % mod, 1)
    conf.write_config_header ('element/modules/config.h', 'KV_MODULES_CONFIG_H')

    if conf.options.debug:
        conf.define ("DEBUG", 1)
        conf.define ("_DEBUG", 1)
        conf.env.append_unique ('CXXFLAGS', ['-g', '-ggdb', '-O0'])
        conf.env.append_unique ('CFLAGS', ['-g', '-ggdb', '-O0'])
    else:
        conf.define ("NDEBUG", 1)
        conf.env.append_unique ('CXXFLAGS', ['-Os'])
        conf.env.append_unique ('CFLAGS', ['-Os'])

    conf.define('KV_USE_LIBJUCE', 1)

    conf.env.append_unique ('CXXFLAGS', '-I' + os.getcwd() + '/build')
    conf.env.append_unique ('CFLAGS', '-I' + os.getcwd() + '/build')

    print
    juce.display_header ('KV Modules Configuration')
    juce.display_msg (conf, 'Library Version', VERSION)
    juce.display_msg (conf, 'Prefix', conf.env.PREFIX)
    juce.display_msg (conf, 'Build Debuggable Libraries', conf.env.BUILD_DEBUGGABLE)
    juce.display_msg (conf, 'Build Static Libraries', conf.env.BUILD_STATIC)
    print
    juce.display_header ('Global Compiler Flags')
    juce.display_msg (conf, 'CFLAGS', conf.env.CFLAGS)
    juce.display_msg (conf, 'CXXFLAGS', conf.env.CXXFLAGS)
    juce.display_msg (conf, 'LDFLAGS', conf.env.LINKFLAGS)

def get_include_path(bld):
    return bld.env.INCLUDEDIR + '/element-%s/element' % KV_MAJOR_VERSION

def install_module_headers (bld, modules):
    for mod in modules:
        bld.install_files (get_include_path (bld), \
                           bld.path.ant_glob ("element/modules/" + mod + "/**/*.h"), \
                           relative_trick=True, cwd=bld.path.find_dir ('element'))

def install_misc_header(bld, h, subpath=''):
    p = get_include_path(bld) + subpath
    bld.install_files (p, h)

def module_slug(mod, debug=False):
    slug = mod.replace('_', '-')
    if debug: slug += '-debug'
    slug += '-%s' % KV_MAJOR_VERSION
    return slug

def library_slug(mod, debug=False):
    mv = KV_MAJOR_VERSION
    slug = mod + '_debug-%s' % mv if debug else mod + '-%s' % mv
    return slug

def build_modules(bld):
    is_debug = bld.env.BUILD_DEBUGGABLE
    postfix = '_debug' if is_debug else ''
    libs = juce.build_modular_libs (bld, library_modules, KV_MODULES_VERSION, postfix)
    for lib in libs:
        lib.includes = ['.', './element/modules', './build/element'] + lib.includes
        for dep in lib.module_info.dependencies():
            if 'juce_' in dep:
                lib.use.append(dep.upper())
            elif 'element_' in dep:
                lib.use.append(module_slug(dep, is_debug))
        if 'engines' in lib.name:
            lib.use += ['LILV', 'SUIL', 'JACK']
        if 'lv2' in lib.name:
            lib.use += ['LILV', 'SUIL']

    # Create pkg-config files for all built modules
    is_debug = bld.env.BUILD_DEBUGGABLE

    for m in library_modules:
        module = juce.get_module_info (bld, m)
        slug = module_slug(m, is_debug)
        required_packages = ' '.join (module.requiredPackages (is_debug))
        deplibs = ' '.join (module.linuxLibs())

        deplibs += ' -l%s' % library_slug(m, is_debug)

        pcobj = bld (
            features     = 'subst',
            source       = 'module.pc.in',
            target       = slug + '.pc',
            install_path = bld.env.LIBDIR + '/pkgconfig',
            MAJOR_VERSION= '%s' % KV_MAJOR_VERSION,
            PREFIX       = bld.env.PREFIX,
            INCLUDEDIR   = bld.env.INCLUDEDIR,
            LIBDIR       = bld.env.LIBDIR,
            DEPLIBS      = deplibs,
            REQUIRED     = required_packages,
            NAME         = module.name(),
            DESCRIPTION  = module.description(),
            VERSION      = module.version(),
        )

    if bld.env.INSTALL_HEADERS:
        install_module_headers (bld, library_modules)
        for header in ['element/element.h']:
            install_misc_header (bld, header)
        install_misc_header (bld, 'build/element/modules/config.h', '/modules')
        install_misc_header (bld, 'build/element/modules/version.h', '/modules')

def build (bld):
    # bld.env.INSTALL_HEADERS = bld.options.install_headers
    # build_modules(bld)
    # bld.add_group()
    # bld.program(
    #     source   = ['tools/linktest.cpp'],
    #     includes = ['.', './element/modules'],
    #     use      = ['JUCE_OPENGL', 'LILV', 'SUIL', 'GL', 'X11', 'ALSA', \
    #                 'element-base-debug-0', 'element-gui-debug-0', \
    #                 'element-engines-debug-0', 'element-models-debug-0', \
    #                 'element-lv2-debug-0'],
    #     name     = 'linktest',
    #     target   = 'linktest',
    #     install_path = None
    # )
    return

def dist(ctx):
    z = ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo = ziptype
