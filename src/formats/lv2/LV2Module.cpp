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
    
    namespace lv2 {

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

   private:


   };

   LV2Module::LV2Module (LV2World& world_, const LilvPlugin* plugin_)
       : instance (nullptr), plugin (plugin_), world(world_), worker (nullptr),
         active (false), hub (4096), workResponses (1024),
         currentSampleRate (44100.0), savedFeatures (nullptr),
         numPorts (plugin.get_num_ports())
   {
       priv = new Private (*this);
       //FIXME: createPorts (numPorts);
   }

   LV2Module::~LV2Module ()
   {
       deactivate();
       freeInstance();
   }
#if 0
   ProcessorEditor*
   LV2Module::createEditor()
   {
       if (this->hasEditor())
       {
           Scoped<Component> comp (new Component());
           Scoped<LV2UIModule> ui (new LV2UIModule (*this, *comp));
           ui->open();
           ui->close();
       }
       return nullptr;
   }
#endif

   void LV2Module::init()
   {
#if 0
       Lilv::Plugin plugin (model->getPlugin());
       LV2World&    nodes  (model->getNodes());

       // Get min/max/default values for input control ports.
       float mins[numPorts], maxes[numPorts], defaults[numPorts];
       plugin.get_port_ranges_float (mins, maxes, defaults);

       for (int i = 0; i < numPorts; ++i)
       {
           Lilv::Port modelPort (plugin.get_port_by_index (i));

           const Port::Type type = modelPort.is_a (nodes.lv2_AudioPort) ? Bke::audioPort
                                                                        : modelPort.is_a (nodes.lv2_ControlPort) ? Bke::controlPort
                                                                                                                 : modelPort.is_a (nodes.lv2_AtomPort) ? Bke::atomPort
                                                                                                                                                       : modelPort.is_a (nodes.lv2_EventPort) ? Bke::eventPort
                                                                                                                                                                                              : modelPort.is_a (nodes.lv2_CVPort) ? Bke::cvPort
                                                                                                                                                                                                                                  : Bke::unknownPort;
           assert (type != unknownPort);

           const Bke::PortFlow flow = modelPort.is_a (nodes.lv2_InputPort)
                   ? Bke::inputPort : Bke::outputPort;

           setPortConfig ((uint32) i, type, flow);

       } /* foreach port */
#endif
   }

   Result
   LV2Module::instantiate (double samplerate, LV2_Feature** features)
   {
       currentSampleRate = samplerate;
       savedFeatures     = features;

       freeInstance();
       jassert (instance == nullptr);

       instance = Lilv::Instance::create (plugin, samplerate, savedFeatures);
       if (instance == nullptr)
           return Result::fail ("Could not instantiate plugin.");

       if (const void* data = getExtensionData (LV2_WORKER__interface))
           worker = (LV2_Worker_Interface*) data;

       // FIXME: activatePorts();
       return allocateEventBuffers();
   }

#if 0
   SuilInstance*
   LV2Module::instantiateUI (const LV2_Feature* const * uiFeatures)
   {

       Lilv::Node nativeType (model->nativeEditorType());
       Lilv::Node juceType   (model->getWorld().new_uri (LV2_UI__JuceUI));

       if (const LilvUI* ui = model->hasUIWithWidgetType (juceType))
       {
           std::clog << "LV2: UI Selected: " << juceType.as_uri() << std::endl;
           return instantiateUI (ui, juceType, juceType, uiFeatures);
       }

       if (const LilvUI* ui = model->hasUIWithWidgetType (nativeType))
       {
           std::clog << "LV2: UI Selected: " << nativeType.as_uri() << std::endl;
           return instantiateUI (ui, nativeType, nativeType, uiFeatures);
       }

       return nullptr;
   }

   SuilInstance*
   LV2Module::instantiateUI (const LilvUI* ui,
                             const LilvNode* containerType,
                             const LilvNode* widgetType,
                             const LV2_Feature* const * features)
   {
       return priv->instantiateUI (ui, containerType, widgetType, features);
   }
#endif

   void
   LV2Module::activate()
   {
       if (instance != nullptr && ! active)
       {
           if (const void* data = getExtensionData (LV2_WORKER__interface))
               this->worker = (LV2_Worker_Interface*) data;
           else
               this->worker = nullptr;

           //FIXME: activatePorts();

           instance->activate();
           active = true;
       }
   }

   void
   LV2Module::cleanup()
   {
       if (instance != nullptr)
       {
           const LV2_Descriptor* desc = instance->get_descriptor();
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
       if (newSampleRate != currentSampleRate)
       {
#ifndef NDEBUG
           std::clog << "LV2Module::setSampleRate (...); " << newSampleRate << std::endl;
#endif
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
   }


   uint32
   LV2Module::countPorts (PortType type, bool inputs) const
   {
       Lilv::World& w (world.lilvWorld());

       LilvNode* tnode = w.new_uri (type.uri().toUTF8());
       const LilvNode* fnode = (inputs) ? world.lv2_InputPort : world.lv2_OutputPort;
       uint32 cnt = lilv_plugin_get_num_ports_of_class (plugin, tnode, fnode, nullptr);
       lilv_node_free (tnode);

       return cnt;
   }


   LV2_Handle
   LV2Module::getHandle()
   {
       if (instance != nullptr)
           return instance->get_handle();

       return nullptr;
   }

   String
   LV2Module::getAuthorName() const
   {
       if (LilvNode* node = lilv_plugin_get_author_name (plugin))
       {
           String name (lilv_node_as_string (node));
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

   String
   LV2Module::getURI() const
   {
       const LilvNode* uriNode = lilv_plugin_get_uri (plugin);
       return lilv_node_as_string (uriNode);
   }

   Result
   LV2Module::allocateEventBuffers()
   {
       for (int i = 0; i < numPorts; ++i)
       {
#if 0
           const PortInfo& port (getPortInfo (i));

           if (port.type == Bke::eventPort || port.type == Bke::atomPort)
           {
               bool isInput = port.flow == Bke::inputPort ? true : false;
               // managed evbufs aren't currently supported.
               // hint hint, just use a PortGraph object which will handle
               // setting up port buffers and connections for you
           }
#endif
       }

       return Result::ok();
   }

   bool
   LV2Module::isLoaded() const
   {
       return instance != nullptr;
   }

   void
   LV2Module::connectPort (uint32 port, void* data)
   {
       if (instance != nullptr)
           instance->connect_port (port, data);
   }

   void LV2Module::run (uint32 nframes)
   {
       if (isLoaded())
       {
           const bool haveWorker = (worker != nullptr);
           if (haveWorker && workResponses.canRead (sizeof(uint32_t)))
           {
               uint32_t size;
               workResponses.read ((char*) &size, sizeof (uint32_t));
               std::cout << "SIZE: " << size << std::endl;
               char data [size];
               workResponses.read (data, size);
               worker->work_response (getHandle(), size, (const void*) data);
           }

           instance->run (nframes);

           if (haveWorker && worker->end_run != nullptr)
               worker->end_run (getHandle());

       }
   }

}  /* namespace element */

