/*
  preferences.hpp - Header description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file preferences.hpp */

#include "element/juce.hpp"

#include "guiapp.hpp"
#include "preferences.hpp"

namespace element {
namespace gui {

    PreferencesComponent::PreferencesComponent (GuiApp& gui)
        : PreferencesWidget (gui)
    {
    }

    PreferencesComponent::~PreferencesComponent()
    {
        std::clog << "~PreferencesComponent()\n";
    }

    void
    PreferencesComponent::resized()
    {
        Component::resized();
    }
}}



