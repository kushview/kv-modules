#ifndef LV2MODEL_HPP
#define LV2MODEL_HPP

namespace Element {

    class Model
    {
    public:

        ~Model () { }

        inline static Shared<Model>
        create (Lilv::World& world, Lilv::Plugin plugin, LV2World& nodes, LV2FeatureArray& features, SymbolMap& symbols)
        {
            if (plugin != nullptr)
            {
                return Shared<Model> (new Model (world, plugin, nodes, features, symbols));
            }

            return Shared<Model> ();
        }

        const LilvPlugin* getLilvPlugin() { return this->plugin.me; }
#if 0
        void fillParameters (std::vector<Parameter>& params)
        {
            params.clear();

            uint32 nports = ports.size();
            float min [nports], max [nports], defaults [nports];
            lilv_plugin_get_port_ranges_float (plugin, min, max, defaults);

            for (uint32 i = 0; i < nports; ++i)
            {
                if (ports[i].is_a (nodes.lv2_ControlPort))
                {
                    params.push_back (PortParameter (ports[i]));

                    Parameter& param = *params.rbegin();
                    param.set (min[i], max[i], defaults[i]);
                }
            }
        }
#endif

        String getClassLabel()
        {
           Lilv::Node node (plugin.get_class().get_label());
           return node.as_string();
        }

        String getURI()     { return uri.as_string(); }
        String getName()    { return name.as_string(); }

        LV2World& getNodes() { return nodes; }

        uint32 getNumChannels (bool isInput)
        {
            if (isInput)
                return countPorts (nodes.lv2_AudioPort, nodes.lv2_InputPort);

            return countPorts (nodes.lv2_AudioPort, nodes.lv2_OutputPort);
        }

        uint32 getNumPorts() { return ports.size(); }
        Lilv::Plugin getPlugin()
        {
           return Lilv::Plugin (plugin);
        }

        String getPortName (uint32 port)
        {
            if (port < ports.size())
            {
                Lilv::Node n (ports[port].get_name());
                return n.as_string();
            }

            return String::empty;
        }

        String getPortSymbol (uint32 port)
        {
           if (port < ports.size())
              return Lilv::Node(ports[port].get_symbol()).as_string();
           return String::empty;
        }

        bool hasMidiPort (bool isInput)
        {
            Lilv::Node flowType = isInput ? nodes.lv2_InputPort : nodes.lv2_OutputPort;

            for (uint32 i = 0; i < ports.size(); ++i)
            {
                Lilv::Port port (plugin, lilv_plugin_get_port_by_index (plugin, i));

                if (! port.is_a (flowType))
                    continue;
#if 0
                if (port.is_a (nodes.lv2_AtomPort) || port.is_a (nodes.lv2_EventPort))
                    if (port.supports_event (nodes.midi_MidiEvent))
                        return true;
#endif
            }

            return false;
        }

        Lilv::Node nativeEditorType()
        {
#if JUCE_LINUX
           return world.new_uri (LV2_UI__X11UI);
#elif JUCE_MAC
           return world.new_uri (LV2_UI__CocoaUI);
#elif JUCE_WINDOWS
           return world.new_uri (LV2_UI__WindowsUI);
#endif
        }

        Lilv::World& getWorld()          { return world; }
        LV2FeatureArray& features()      { return providerFeatures; }

        // UI functions
        inline bool hasEditor()    const { return uis.size() > 0; }
        inline uint32 numEditors() const { return uis.size(); }

    protected:

        Model (Lilv::World& world_, Lilv::Plugin plugin_,
               LV2World& nodes_, LV2FeatureArray& feats,
               SymbolMap& symbols_)
            : world (world_),
              symbols (symbols_),
              plugin (plugin_),
              nodes (nodes_),
              author (plugin.get_author_name()),
              name (plugin.get_name()),
              uri (plugin.get_uri()),
              providerFeatures (feats)
        {
            const uint32 nports (lilv_plugin_get_num_ports (plugin_));
            for (int i = 0; i < nports; ++i)
               ports.push_back (Lilv::Port (plugin, lilv_plugin_get_port_by_index (plugin_, i)));

            Lilv::Node   native   (nativeEditorType());
            Lilv::Node   juceType (world.new_uri("http://lv2plug.in/ns/extensions/ui#JuceUI"));
            Lilv::Nodes  uiNodes  (plugin.get_uis());

            LILV_FOREACH (uis, iter, uiNodes)
            {
                std::clog << "checking ui:\n";
               const LilvUI*   thisUI = lilv_uis_get (world.me, iter);
               const LilvNode* type   = nullptr;

               if (lilv_ui_is_supported (thisUI, Element::lv2::ui_supported, native, &type))
                  uis.push_back (std::make_pair (type, thisUI));
            }
        }


    public:

        inline const LilvUI*
        hasUIWithWidgetType (const LilvNode* type)
        {
           for (auto ui : uis)
              if (lilv_node_equals (type, ui.first))
                 return ui.second;

           return nullptr;
        }

    private:

        Lilv::World& world;
        SymbolMap& symbols;
        Lilv::Plugin plugin;
        LV2World& lv2;

        typedef std::pair<const LilvNode*, const LilvUI*> TypeEditorPair;
        Array<const LilvUI*> supportedUIs;
        std::vector<TypeEditorPair> uis;

        Lilv::Node author, name, uri;
        std::vector<Lilv::Port> ports;

        LV2FeatureArray& providerFeatures;

        inline uint32 countPorts (Lilv::Node type, Lilv::Node flow)
        {
            uint32 count (0);

            for (uint32 i = 0; i < ports.size(); ++i)
            {
                Lilv::Port port (plugin, lilv_plugin_get_port_by_index (plugin, i));
                if (port.is_a (type) && port.is_a (flow))
                    ++count;
            }

            return count;
        }

        friend class LV2Module;

    }; // LV2Module::Model

}
#endif // LV2MODEL_HPP
