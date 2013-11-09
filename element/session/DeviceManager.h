/*
    device-manager.hpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#ifndef ELEMENT_DEVICE_MANAGER_HPP
#define ELEMENT_DEVICE_MANAGER_HPP

#include "element/Juce.h"
#include "element/Pointer.h"

namespace Element {

    class Engine;

    class DeviceManager :  public AudioDeviceManager
    {
    public:

        typedef AudioDeviceManager::AudioDeviceSetup AudioSettings;

        DeviceManager();
        ~DeviceManager();

        void createAudioDeviceTypes (OwnedArray <AudioIODeviceType>& list);

        void getAudioDrivers (StringArray& drivers);
        void selectAudioDriver (const String& name);

        void attach (Shared<Engine> engine);

    private:

        friend class World;

        class Private;
        Scoped<Private> impl;

    };

}

#endif // ELEMENT_DEVICE_MANAGER_HPP
