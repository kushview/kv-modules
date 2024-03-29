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

#pragma once

namespace kv {

using namespace juce;

/** Base class for unlocking from a site which is using edd-juce for WordPress */
class EDDOnlineUnlockStatus : public OnlineUnlockStatus
{
public:
    /** Creates a new status. This should probably be a single object living somewhere
        in your code. see @juce::OnlineUnlockStatus
     */
    EDDOnlineUnlockStatus();
    
    virtual ~EDDOnlineUnlockStatus();
    
    /** Returns a property from EDD extra data */
    inline var getProperty (const Identifier& key, const var& defaultValue = var()) const
    { 
        return edd.getProperty (key, defaultValue); 
    }
    
    /** Returns true if a property exists on the edd node */
    inline bool hasProperty (const Identifier& key) const { return edd.hasProperty (key); }
    
    /** Set the license key. Use this if you are using software licensing along with
        EDD JUCE
     */
    inline void setLicenseKey (const String& licenseKey)
    {
        edd.setProperty (licenseName, licenseKey.trim(), nullptr);
    }
    
    /** Returns the currently stored license key */
    inline String getLicenseKey() const { return edd[licenseName].toString(); }
    
    /** Activate a license with the webserver. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    UnlockResult activateLicense (const String& license, 
                                  const String& email = String(),
                                  const String& password = String(),
                                  const StringPairArray& params = StringPairArray());
    
    /** Deactivate a license on this machine. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    UnlockResult deactivateLicense (const String& license, 
                                    const StringPairArray& params = StringPairArray());
    
    /** Checks the status of a license and updates the keyfile */
    UnlockResult checkLicense (const String& license,
                               const StringPairArray& params = StringPairArray());
    
    /** Reads the reply from your server and decrypts the edd ValueTree */
    String readReplyFromWebserver (const String& email, const String& password) override;
    
protected:
    /** Call this in your restoreState callback to update the edd node */
    void eddRestoreState (const String& state);
    
private:
    /** Custom data sent back during authorization will be stored here. by default
        this only has two properties version and priceIDs. Depending on the
        EDD extensions you have installed, the structure of this object will change.
     */
    ValueTree edd;
    
    static const char* activeName;
    static const char* enabledName;
    static const char* licenseName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EDDOnlineUnlockStatus);
};

}
