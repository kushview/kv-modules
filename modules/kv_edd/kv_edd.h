/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2018-2019  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
BEGIN_JUCE_MODULE_DECLARATION

    ID:               kv_edd
    vendor:           Kusvhiew
    version:          1.1.0
    name:             KV EDD Integration
    description:      Essential classes for communicating with an Easy Digital Downloads store front.
    website:          https://kushview.net
    license:          GPL

    dependencies:     juce_core, juce_data_structures, juce_product_unlocking

END_JUCE_MODULE_DECLARATION
*/

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_product_unlocking/juce_product_unlocking.h>

#include "auth/EDDOnlineUnlockStatus.h"
