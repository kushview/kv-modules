/*
    Slugs.h - This file is part of BTV

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef BTV_SLUGS_H
#define BTV_SLUGS_H

#include <element/juce.hpp>

//namespace Element {

/** Commonly used Identifiers, these might represent different
    things depending on the context of use */
namespace Slugs {

static const Identifier assetId  = "assetId";
static const Identifier block    = "block";
static const Identifier clip     = "clip";
static const Identifier file     = "file";
static const Identifier fsid     = "fsid";
static const Identifier graph    = "graph";
static const Identifier group    = "group";
static const Identifier id       = "id";
static const Identifier keyId    = "keyId";
static const Identifier length   = "length";
static const Identifier name     = "name";
static const Identifier nodeId   = "nodeId";
static const Identifier note     = "note";
static const Identifier path     = "path";
static const Identifier pattern  = "pattern";
static const Identifier sampler  = "sampler";
static const Identifier sequence = "sequence";
static const Identifier track    = "track";
static const Identifier velocity = "velocity";

} //}

#endif // BTV_SLUGS_H
