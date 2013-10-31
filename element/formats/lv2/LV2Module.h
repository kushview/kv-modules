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
    class Private;
    Scoped<Private> priv;

public:

    LV2Module (LV2World& world, const LilvPlugin* plugin_);
    ~LV2Module();

    void init();
    Result instantiate (double samplerate, LV2_Feature** features);

#if 0
    SuilInstance* instantiateUI (const LV2_Feature* const * uiFeatures);

    SuilInstance* instantiateUI (const LilvUI* ui,
                                 const LilvNode* containerType,
                                 const LilvNode* widgetType,
                                 const LV2_Feature* const * uiFeatures);
#endif

    const LV2_Feature* const* features() { return savedFeatures; }

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

    LV2_Worker_Status
    work (LV2_Worker_Respond_Function respond, uint32_t size, const void* data)
    {
       if (instance != nullptr && worker != nullptr)
          return worker->work (getHandle(), respond, this, size, data);
       return LV2_WORKER_ERR_UNKNOWN;
    }

    inline LV2_Worker_Status
    writeWorkResponse (uint32_t size, const void* data)
    {
#if 0
       if (worker != nullptr)
       {
          workResponses.write ((const char*) &size, sizeof (uint32_t));
          workResponses.write ((const char*) data, size);
          return LV2_WORKER_SUCCESS;
       }
#endif
       return LV2_WORKER_ERR_UNKNOWN;
    }

    uint32 countPorts (PortType type, bool inputs) const;

    String getAuthorName() const;
    String getClassLabel() const;
    String getName() const;
    String getURI() const;

    inline bool hasEditor() const { return false; /* XXX */ }

    inline uint32
    getMidiPort() const
    {
#if 0
        for (int i = 0; i < countPorts(); ++i)
        {
            const PortInfo& info (getPortInfo (i));
            if (info.flow == inputPort && (info.type == atomPort ||
                                           info.type == eventPort))
                return info.index;
        }
#endif
        return uint32(-1);
    }

    inline uint32
    getNotifyPort() const
    {
#if 0
        for (int i = 0; i < countPorts(); ++i)
        {
            const PortInfo& info (getPortInfo(i));
            if (info.flow == outputPort && (info.type == atomPort ||
                                            info.type == eventPort))
                return info.index;
        }

        return Port::invalidIndex;
#endif
        return 0;
    }

    LV2_Handle getHandle();
    void setSampleRate (double newSampleRate);
    bool isLoaded() const;
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
    RingBuffer hub, workResponses;
    Result allocateEventBuffers();

    /** @internal Free the plugin instance */
    void freeInstance();
};

}   /* namespace element */

#endif /* ELEMENT_LV2MODULE_HPP */
