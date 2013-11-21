/*
    LV2Module.cpp - This file is part of Element
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

#include "element/formats/lv2/LV2Module.h"

namespace Element {

    namespace LV2Callbacks {

        static void
        process (uint32 nframes, void* arg)
        {
            LV2Module* module = (LV2Module*) arg;
            module->run (nframes);
        }

        static void
        connect_port (uint32_t port, void* datalocation, void* arg)
        {
            LV2Module* module = (LV2Module*) arg;
            module->connectPort (port, datalocation);
        }

    }
    

    //==============================================================================
#if 0
    class LV2UIModule
    {
    public:

        LV2UIModule (LV2Module& module_, Component& comp_)
          : module (module_), comp (comp_), instance (nullptr) { }

       ~LV2UIModule()
       {
          close();
       }

       template <typename WidgetType>
       WidgetType getWidget()
       {
          if (instance != nullptr)
             return (WidgetType) suil_instance_get_widget (instance);
          return (WidgetType) nullptr;
       }

       void close()
       {
          if (instance != nullptr)
          {
             SuilInstance* old = instance;
             instance = nullptr;
             suil_instance_free (old);
          }
       }

       void open()
       {
          if (isOpen())
              close();
    #if 1
          Shared<LV2Module::Model> model (module.getModel());
          LV2FeatureArray& modelFeats (model->features());

          LV2_Feature parent_feature   = { LV2_UI__parent, comp.getWindowHandle() };
          LV2_Feature instance_feature = { LV2_INSTANCE_ACCESS_URI, module.getHandle() };

          //LV2UI_Resize rsz = { &comp, lv2::ui_resize };
          //LV2_Feature ui_resize = { LV2_UI__resize, &rsz };

          LV2_Feature* ui_features[] = {
                 modelFeats.getFeature (LV2_URID__map),
                 modelFeats.getFeature (LV2_URID__unmap),
                 &instance_feature,
                 &parent_feature,
                // &ui_resize,
                 nullptr
          };

          if (SuilInstance* i = module.instantiateUI (ui_features))
          {
             instance = i;
          }
          else
          {
             // failed
          }


    #endif
       }

       bool isOpen()             { return instance != nullptr; }
       bool isNotOpen()          { return ! isOpen(); }

    private:
       LV2Module&       module;
       Component&       comp;
       SuilInstance*    instance;

    };

#endif

   class LV2Module::Private
   {
      LV2Module& owner;

   public:

      Private (LV2Module& module)
         : owner (module) { }

      ~Private() { }

      SuilInstance* instantiateUI (const LilvUI* ui,
                                   const LilvNode* containerType,
                                   const LilvNode* widgetType,
                                   const LV2_Feature* const * features)
      {
#if 0
         {
            Lilv::Node uri (lilv_ui_get_uri (ui));
            std::cout << "LV2: Instantiating UI: " << uri.as_uri() << std::endl;
         }

         Lilv::Plugin plugin (owner.getModel()->getPlugin());

         SuilInstance* instance = suil_instance_new (
               suil, &owner,
               lilv_node_as_uri (containerType),
               plugin.get_uri().as_uri(),
               lilv_node_as_uri (lilv_ui_get_uri (ui)),
               lilv_node_as_uri (widgetType),
               lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_bundle_uri (ui))),
               lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_binary_uri (ui))),
               features
         );

         if (instance != nullptr)
         {
            // do something here?
         }

         return instance;
#endif
         return nullptr;
      }

      HeapBlock<float> mins, maxes, defaults, values;

   };

   LV2Module::LV2Module (LV2World& world_, const LilvPlugin* plugin_)
       : instance (nullptr),
         plugin (plugin_),
         world (world_),
         active (false),
         currentSampleRate (44100.0),
         numPorts (lilv_plugin_get_num_ports (plugin_)),
         savedFeatures (nullptr),
         worker (nullptr),
         hub (nullptr),
         workResponses (nullptr)
   {
       priv = new Private (*this);
   }

   LV2Module::~LV2Module ()
   {
       deactivate();
       freeInstance();
   }

   void
   LV2Module::activatePorts()
   {
       jassert (instance != nullptr);

       priv->mins.allocate (numPorts, true);
       priv->maxes.allocate (numPorts, true);
       priv->defaults.allocate (numPorts, true);
       priv->values.allocate (numPorts, true);

       plugin.get_port_ranges_float (priv->mins, priv->maxes, priv->defaults);

       for (uint32 i = 0; i < numPorts; ++i)
           priv->values[i] = priv->defaults[i];

       for (int32 p = 0; p < numPorts; ++p)
       {
           const PortType type = getPortType (p);

           if (type == PortType::Control)
           {
               // normally this would ONLY be done during 'run'. However,
               // control ports are only connected once, here, during activation
               connectPort (p, priv->values.getData() + p);
           }
       }
   }

   void
   LV2Module::init()
   {

   }

   Result
   LV2Module::instantiate (double samplerate, const LV2_Feature* const* features)
   {
       currentSampleRate = samplerate;
       savedFeatures     = const_cast<LV2_Feature**> (features);

       freeInstance();
       instance = Lilv::Instance::create (plugin, samplerate, savedFeatures);

       if (instance == nullptr)
           return Result::fail ("Could not instantiate plugin.");

       if (const void* data = getExtensionData (LV2_WORKER__interface))
       {
           worker = (LV2_Worker_Interface*) data;
           if (! workResponses)
               workResponses = new RingBuffer (4096);
       }
       else
       {
           worker = nullptr;
           if (workResponses)
               workResponses = nullptr;
       }

       return Result::ok();
   }

   void
   LV2Module::activate()
   {
       if (instance && ! active)
       {
           activatePorts();
           instance->activate();
           active = true;
       }
   }

   void
   LV2Module::cleanup()
   {
       if (instance != nullptr)
       {
           // TODO: Check if Lilv takes care of calling cleanup
       }
   }

   void
   LV2Module::deactivate()
   {
       if (instance != nullptr && active)
       {
           instance->deactivate();
           active = false;
       }
   }

   bool
   LV2Module::isActive() const
   {
       return active;
   }

   void
   LV2Module::freeInstance()
   {
       if (instance != nullptr)
       {
           lilv_instance_free (*instance);
           instance = nullptr;
       }
   }

   void
   LV2Module::setSampleRate (double newSampleRate)
   {
       if (newSampleRate == currentSampleRate)
           return;

       currentSampleRate = newSampleRate;

       if (instance != nullptr)
       {
           const bool wasActive = isActive();

           deactivate();

           // TODO: need to save state here

           freeInstance();
           instance = Lilv::Instance::create (plugin, currentSampleRate, savedFeatures);

           // TODO: and restore it here

           if (wasActive)
               activate();
       }
   }


   Result
   LV2Module::allocateEventBuffers()
   {
       for (int i = 0; i < numPorts; ++i)
       { }

       return Result::ok();
   }

   void
   LV2Module::connectPort (uint32 port, void* data)
   {
       if (instance != nullptr)
           instance->connect_port (port, data);
   }

   String
   LV2Module::getAuthorName() const
   {
       if (LilvNode* node = lilv_plugin_get_author_name (plugin))
       {
           String name (CharPointer_UTF8 (lilv_node_as_string (node)));
           lilv_node_free (node);
           return name;
       }
       return String::empty;
   }

   String
   LV2Module::getClassLabel() const
   {
       if (const LilvPluginClass* klass = lilv_plugin_get_class (plugin))
           if (const LilvNode* node = lilv_plugin_class_get_label (klass))
               return lilv_node_as_string (node);
       return String::empty;
   }

   LV2_Handle LV2Module::getHandle() { return instance != nullptr ? instance->get_handle() : nullptr; }

   String
   LV2Module::getName() const
   {
       if (LilvNode* node = lilv_plugin_get_name (plugin))
       {
           String name = lilv_node_as_string (node);
           lilv_node_free (node);
           return name;
       }

       return String::empty;
   }

   uint32
   LV2Module::getNumPorts() const { return lilv_plugin_get_num_ports (plugin); }

   uint32
   LV2Module::getNumPorts (PortType type, bool isInput) const
   {
       if (type == PortType::Unknown)
           return 0;

       const LilvNode* flow = isInput ? world.lv2_InputPort : world.lv2_OutputPort;
       const LilvNode* kind = type == PortType::Audio ? world.lv2_AudioPort
                            : type == PortType::Atom  ? world.lv2_AtomPort
                            : type == PortType::Control ? world.lv2_ControlPort
                            : type == PortType::CV ? world.lv2_CVPort : nullptr;

       if (kind == nullptr || flow == nullptr)
           return 0;

       return lilv_plugin_get_num_ports_of_class (plugin, kind, flow, nullptr);
   }

   const LilvPort* LV2Module::getPort (uint32 port) const { return lilv_plugin_get_port_by_index (plugin, port); }

   uint32
   LV2Module::getMidiPort() const
   {
       for (uint32 i = 0; i < getNumPorts(); ++i)
       {
           const LilvPort* port (getPort (i));
           if (lilv_port_is_a (plugin, port, world.lv2_AtomPort) &&
               lilv_port_is_a (plugin, port, world.lv2_InputPort) &&
               lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
               return i;
       }

       return ELEMENT_INVALID_PORT;
   }

   const LilvPlugin* LV2Module::getPlugin() const { return plugin.me; }

   uint32
   LV2Module::getNotifyPort() const
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

   PortType
   LV2Module::getPortType (uint32 i) const
   {
       const LilvPort* port (getPort (i));

       if (lilv_port_is_a (plugin, port, world.lv2_AudioPort))
           return PortType::Audio;
       else if (lilv_port_is_a (plugin, port, world.lv2_AtomPort))
           return PortType::Atom;
       else if (lilv_port_is_a (plugin, port, world.lv2_ControlPort))
           return PortType::Control;
       else if (lilv_port_is_a (plugin, port, world.lv2_CVPort))
           return PortType::CV;
       else if (lilv_port_is_a (plugin, port, world.lv2_EventPort))
           { jassertfalse; /* XXX Handle (deprecated) event ports */ }

       return PortType::Unknown;
   }


   String
   LV2Module::getURI() const
   {
       return lilv_node_as_string (lilv_plugin_get_uri (plugin));
   }

   bool LV2Module::isLoaded() const { return instance != nullptr; }

   bool
   LV2Module::isPortInput (uint32 index) const
   {
       return lilv_port_is_a (plugin, getPort (index), world.lv2_InputPort);
   }

   bool
   LV2Module::isPortOutput (uint32 index) const
   {
       return lilv_port_is_a (plugin, getPort (index), world.lv2_OutputPort);
   }

   void
   LV2Module::run (uint32 nframes)
   {
       if (! isLoaded())
           return;

       const bool haveWorker = (worker && workResponses);

       if (haveWorker && workResponses->canRead (sizeof (uint32)))
       {
           uint32 size = 0;
           workResponses->read ((char*) &size, sizeof (uint32));

           char data [size];
           workResponses->read (data, size);

           worker->work_response (getHandle(), size, (const void*) data);
       }

       instance->run (nframes);

       if (haveWorker && worker->end_run != nullptr)
           worker->end_run (getHandle());

   }

}  /* namespace Element */

