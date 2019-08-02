#!/usr/bin/evn python
# encoding: utf-8
# Copyright (C) 2019 Michael Fisher <mfisher@kushview.net>

''' This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public Licence as published by
the Free Software Foundation, either version 2 of the Licence, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
file COPYING for more details. '''

import sys, os, platform
from subprocess import call

sys.path.insert (0, "tools/waf")
import juce

KV_VERSION = '0.1.0'
KV_MAJOR_VERSION = KV_VERSION[0]
KV_MINOR_VERSION = KV_VERSION[2]
KV_MICRO_VERSION = KV_VERSION[4]
KV_EXTRA_VERSION = ''

APPNAME = 'libkv'
VERSION = KV_VERSION

top = '.'
out = 'build'

experimental_modules = '''
    kv_ffmpeg
    kv_lv2
    kv_video
    kv_edd
'''.split()

library_modules = '''
    kv_core
    kv_engines
    kv_gui
    kv_models
'''.split()

all_modules = library_modules + experimental_modules

cpponly_modules = '''
'''.split()

def options (opts):
    opts.load ('compiler_c compiler_cxx juce')
    opts.add_option ('--enable-edd', default=False, action="store_true", dest="edd",
        help="Enable support for Easy Digital Downloads")
    opts.add_option('--no-headers', default=True, action="store_false", \
        dest="install_headers", help="Don't install headers")

def silence_warnings (conf):
    # TODO: update LV2 module to use latest LV2 / LILV / SUIL
    conf.env.append_unique ('CXXFLAGS', ['-Wno-deprecated-declarations'])

def configure (conf):
    conf.load ('compiler_c compiler_cxx')
    silence_warnings (conf)

    conf.env.DATADIR    = conf.env.PREFIX + '/share'
    conf.env.LIBDIR     = conf.env.PREFIX + '/lib'
    conf.env.BINDIR     = conf.env.PREFIX + '/bin'
    conf.env.INCLUDEDIR = conf.env.PREFIX + '/include'

    conf.env.HEADERS    = conf.options.install_headers
    conf.env.DEBUG      = conf.options.debug

    # Write out the version header
    conf.define ("KV_VERSION", KV_VERSION)
    conf.define ("KV_MAJOR_VERSION", KV_MAJOR_VERSION)
    conf.define ("KV_MINOR_VERSION", KV_MINOR_VERSION)
    conf.define ("KV_MICRO_VERSION", KV_MICRO_VERSION)
    conf.define ("KV_EXTRA_VERSION", KV_EXTRA_VERSION)
    conf.write_config_header ('kv/version.h', 'KV_VERSION_H')

    conf.check_cfg (package='juce_debug-5' if conf.options.debug else 'juce-5', 
                    uselib_store='JUCE', args=['--libs', '--cflags'], mandatory=True)
    conf.check_cfg (package='lv2',    uselib_store='LV2',  args=['--libs', '--cflags'], mandatory=False)
    conf.check_cfg (package='lilv-0', uselib_store='LILV', args=['--libs', '--cflags'], mandatory=False)
    conf.check_cfg (package='suil-0', uselib_store='SUIL', args=['--libs', '--cflags'], mandatory=False)
    conf.write_config_header ('libkv_config.h')

    conf.env.MODULES = library_modules

    if conf.options.edd:
        conf.env.MODULES.append ('kv_edd')
    
    if conf.env.HAVE_LILV and conf.env.HAVE_SUIL:
        conf.define ('KV_LV2_PLUGIN_HOST', 1)
        conf.env.MODULES.append ('kv_lv2')
    else:
        conf.define ('KV_LV2_PLUGIN_HOST', 0)
    
    for mod in conf.env.MODULES:
        conf.define('JUCE_MODULE_AVAILABLE_%s' % mod, True)
    conf.write_config_header ('kv/config.h', 'KV_MODULES_CONFIG_H')
    conf.define ('JUCE_APP_CONFIG_HEADER', 'kv/config.h')
    
    conf.load ('juce')
    conf.check_cxx_version ('c++14', True)

    print
    juce.display_header ("Modules")
    for m in all_modules:
        juce.display_msg (conf, m, m in conf.env.MODULES)

    if juce.is_mac():
        print
        juce.display_header ('Mac Options')
        juce.display_msg (conf, 'OSX Arch', conf.env.ARCH)
        juce.display_msg (conf, 'OSX Min Version', conf.options.mac_version_min if len(conf.options.mac_version_min) else 'default')
        juce.display_msg (conf, 'OSX SDK', conf.options.mac_sdk if len(conf.options.mac_sdk) else 'default')
    
    print
    juce.display_header ('Compiler')
    juce.display_msg (conf, 'Debug',    conf.env.DEBUG)
    juce.display_msg (conf, 'CFLAGS',   conf.env.CFLAGS)
    juce.display_msg (conf, 'CXXFLAGS', conf.env.CXXFLAGS)
    juce.display_msg (conf, 'LDFLAGS',  conf.env.LINKFLAGS)

def library_slug (bld):
    return 'kv_debug-0' if bld.env.DEBUG else 'kv-0'

def get_include_path (bld, subpath=''):
    ip = '%s/kv-%s' % (bld.env.INCLUDEDIR, KV_MAJOR_VERSION)
    ip = os.path.join (ip, subpath) if len(subpath) > 0 else ip
    return ip

def install_module_headers (bld, modules):
    for mod in modules:
        bld.install_files (get_include_path (bld), \
                           bld.path.ant_glob ("modules/" + mod + "/**/*.h"), \
                           relative_trick=True, cwd=bld.path.find_dir ('modules'))

def install_misc_header (bld, header, subpath=''):
    destination = get_include_path (bld, subpath)
    bld.install_files (destination, header)

def maybe_install_headers (bld):
    if not bld.env.HEADERS:
        return
    
    install_module_headers (bld, all_modules)
    for header in [ 'kv/kv.h', 'build/kv/config.h', 'build/kv/version.h' ]:
        install_misc_header (bld, header, 'kv')
    for module in all_modules:
        install_misc_header (bld, 'build/kv/%s.h' % module.replace ('kv_', ''), 'kv')

def generate_code (bld):
    tasks = []
    for mod in all_modules:
        tasks.append (bld (
            features     = 'subst',
            source       = 'module_header.h.in',
            target       = 'kv/%s.h' % mod.replace ('kv_', ''),
            name         = mod + "_h",
            install_path = None,
            MODULE       = mod
        ))

        tasks.append (bld (
            features     = 'subst',
            source       = 'module_code.cpp.in',
            target       = 'code/include_%s.cpp' % mod,
            name         = mod + "_cpp",
            install_path = None,
            MODULE       = mod
        ))

        if not mod in cpponly_modules:
            tasks.append (bld (
                features     = 'subst',
                source       = 'module_code.mm.in',
                target       = 'code/include_%s.mm' % mod,
                name         = mod + "_mm",
                install_path = None,
                MODULE       = mod
            ))
    bld.add_group()
    return tasks

def build (bld):
    bld.env.HEADERS = bld.options.install_headers
    generate_code (bld)
    
    library_source = []
    for mod in bld.env.MODULES:
        library_source.append ('build/code/include_%s.cpp' % mod)
    
    library = bld (
        features    = 'cxx cxxshlib',
        source      = library_source,
        includes    = [ 'build', 'modules' ],
        name        = 'KV',
        cxxflags    = [],
        target      = 'lib/%s' % library_slug (bld),
        use         = [ 'JUCE', 'LILV', 'SUIL' ],
        vnum        = VERSION
    )

    pcobj = bld (
        features      = 'subst',
        source        = 'kv.pc.in',
        target        = '%s.pc' % library_slug (bld),
        install_path  = bld.env.LIBDIR + '/pkgconfig',
        MAJOR_VERSION = KV_MAJOR_VERSION,
        PREFIX        = bld.env.PREFIX,
        INCLUDEDIR    = bld.env.INCLUDEDIR,
        LIBDIR        = bld.env.LIBDIR,
        CFLAGSS       = '',
        DEPLIBS       = '-l%s' % library_slug (bld),
        REQUIRED      = 'juce_debug-5' if bld.env.DEBUG else 'juce-5',
        VERSION       = KV_VERSION
    )

    if bld.env.HAVE_SUIL: pcobj.REQUIRED += ' suil-0'
    if bld.env.HAVE_LILV: pcobj.REQUIRED += ' lilv-0'

    bld.program (
        source          = [ 'tools/lv2show.cpp' ],
        includes        = library.includes + [ '.' ],
        target          = 'bin/lv2show',
        cxxflags        = [ '-std=c++14' ],
        install_path    = None,
        use             = [ 'KV' ]
    )

    maybe_install_headers (bld)
