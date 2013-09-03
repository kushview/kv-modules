#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, platform

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

'''
def build_modular_libs (bld, mods, vnum=""):

    libs = []
    for mod in mods:
        src = ["juce/modules/" + mod + "/" + mod + mext]
        obj = bld (
            features    = "cxx cxxshlib",
            source      = src,
            includes    = common_inc,
            name        = mod,
            target      = mod,
            use         = get_use_libs(mod) + get_deps(mod)
        )
        if len(vnum) > 0:
            obj.vnum = vnum
        libs += [obj]
    return libs
'''

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
        includes    = ["element/juce"],
        name        = tgt,
        target      = tgt,
        use         = us
    )

    return obj
