/*
  preferences.hpp - Header description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file preferences.hpp */

#ifndef PREFERENCES_HPP_
#define PREFERENCES_HPP_

#include "element/juce.hpp"

#include "preferences-widget.h"

namespace Element {
namespace gui {

    class GuiApp;

    class PreferencesComponent :  public PreferencesWidget
    {
    public:

        PreferencesComponent (GuiApp& gui);
        ~PreferencesComponent();

        virtual void resized();

    private:

        OwnedArray<Component> pages;
    };

}}


#endif  /* PREFERENCES_HPP_ */
