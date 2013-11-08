/*
    LV2Module.h - This file is part of Element
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

/** @file LV2Module.h */

#ifndef ELEMENT_LV2MODULE_HPP
#define ELEMENT_LV2MODULE_HPP

#include <suil/suil.h>

#include "element/juce.hpp"

#include "element/formats/lv2/LV2Features.h"
#include "element/formats/lv2/LV2World.h"

#include "element/PortWriter.h"
#include "element/Pointer.h"
#include "element/RingBuffer.h"
#include "element/SymbolMap.h"

#define LV2_UI__JuceUI           LV2_UI_PREFIX "JuceUI"

namespace Element {

   class LV2Evbuf;

namespace LV2Callbacks {

   inline unsigned
   uiSupported (const char* hostType, const char* uiType)
   {
      String host (hostType);
      String ui (uiType);

      if (host == LV2_UI__X11UI && ui == LV2_UI__X11UI)
         return 2;
      else if (ui == LV2_UI__JuceUI)
         return 1;

      return suil_ui_supported (hostType, uiType);
   }
}

#if 0
class PortParameter : public Parameter
{
    typedef Lilv::Port Port;
    typedef Lilv::Node Node;

public:
    PortParameter (Port port_)
        : Parameter(Node (port_.get_name()).as_string(),
                    Node (port_.get_symbol()).as_string()),
          port (port_)
    { }

private:
    Lilv::Port port;

};
#endif

class LV2Module// : public PortProcessor
{
public:

    LV2Module (LV2World& world, const LilvPlugin* plugin_);
    ~LV2Module();


    Result instantiate (double samplerate, LV2_Feature** features);

    const LV2_Feature* const* features() const { return savedFeatures; }

    void activate();
    void cleanup();
    void deactivate();
    bool isActive() const;

    inline const void*
    getExtensionData (const String& uri)
    {
       return (instance != nullptr) ? instance->get_extension_data (uri.toUTF8())
                                    : nullptr;
    }

    uint32 getNumPorts() const;
    uint32 getNumPorts (PortType type, bool isInput) const;

    LV2_Worker_Status
    work (LV2_Worker_Respond_Function respond, uint32_t size, const void* data)
    {
       if (instance != nullptr && worker != nullptr)
          return worker->work (getHandle(), respond, this, size, data);
       return LV2_WORKER_ERR_UNKNOWN;
    }

    inline LV2_Worker_Status
    writeWorkResponse (uint32 size, const void* data)
    {
       if (worker && workResponses)
       {
          workResponses->write (&size, sizeof (uint32));
          workResponses->write (data, size);
          return LV2_WORKER_SUCCESS;
       }

       return LV2_WORKER_ERR_UNKNOWN;
    }

    String getAuthorName() const;
    String getClassLabel() const;
    String getName() const;
    String getURI() const;

    inline bool hasEditor() const { return false; /* XXX */ }

    uint32 getMidiPort() const;
    uint32 getNotifyPort() const;
    LV2_Handle getHandle();
    const LilvPlugin* getPlugin() const;
    const LilvPort* getPort (uint32 index) const;
    PortType getPortType (uint32 index) const;

    void setSampleRate (double newSampleRate);

    bool isLoaded() const;
    bool isPortInput (uint32 port) const;
    bool isPortOutput (uint32 port) const;

    void run (uint32 nframes);
    void connectPort (uint32 port, void* data);

private:

    ScopedPointer<Lilv::Instance> instance;
    Lilv::Plugin plugin;
    LV2World&    world;

    bool active;
    double currentSampleRate;
    uint32 numPorts;
    LV2_Feature** savedFeatures;
    LV2_Worker_Interface *worker;

    ScopedPointer<RingBuffer> hub, workResponses;

    Result allocateEventBuffers();
    void activatePorts();
    void freeInstance();
    void init();

    class Private;
    ScopedPointer<Private> priv;

};

}   /* namespace element */

#endif /* ELEMENT_LV2MODULE_HPP */
