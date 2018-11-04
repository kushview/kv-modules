
#pragma once

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
    inline var getProperty (const Identifier& key) const { return edd [key]; }
    
    /** Returns true if a property exists on the edd node */
    inline bool hasProperty (const Identifier& key) const { return edd.hasProperty (key); }
    
    /** Set the license key. Use this if you are using software licensing along with
        EDD JUCE
     */
    inline void setLicenseKey (const String& licenseKey) {
        edd.setProperty (licenseName, licenseKey.trim(), nullptr);
    }
    
    /** Returns the currently stored license key */
    inline String getLicenseKey() const { return edd[licenseName].toString(); }
    
    /** Activate a license with the webserver. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    UnlockResult activateLicense (const String& license, const String& email = String(),
                                  const String& password = String());
    
    /** Deactivate a license on this machine. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    UnlockResult deactivateLicense (const String& license);
    
    /** Checks the status of a license and updates the keyfile */
    UnlockResult checkLicense (const String& license);
    
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
