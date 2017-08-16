/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:               kv_edd
  vendor:           Kusvhiew
  version:          1.1.0
  name:             Classes for EDD Integration
  description:      Essential classes for communicating with an Easy Digital Downloads store front.
  website:          https://kushview.net/edd-juce-product-unlocking
  license:

  dependencies:     juce_core juce_data_structures juce_product_unlocking

 END_JUCE_MODULE_DECLARATION
*/

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_product_unlocking/juce_product_unlocking.h>

namespace kv {
using namespace juce;
#include "auth/EDDOnlineUnlockStatus.h"
}
