
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
    
    /** Activate a license with the webserver. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    String activateLicense (const String& license, const String& email = String(),
                            const String& password = String());
    
    /** Deactivate a license on this machine. Note this requires you have EDD
        Software Licensing setup for your product.
     */
    String deactivateLicense (const String& license);
    
    /** Set the license key. Use this if you are using software licensing along with
        EDD JUCE
     */
    void setLicenseKey (const String& licenseKey) { edd.setProperty("license", licenseKey.trim(), nullptr); }
    
    /** Returns the currently stored license key */
    String getLicenseKey() const { return edd["license"].toString(); }
    
    /** Reads the reply from your server and decrypts the edd ValueTree */
    String readReplyFromWebserver (const String& email, const String& password) override;

protected:
    virtual URL getApiEndPoint() { return URL(); }
    
    /** Override this to add additional query params sent to the web server */
    virtual StringPairArray getQueryParams() { return StringPairArray(); }
    
    /** Call this in your restoreState callback to update the edd node */
    void eddRestoreState (const String& state);
    
private:
    /** Custom data sent back during authorization will be stored here. by default
        this only has two properties ownedPriceIDs and downloadID. Depending on the
        EDD extensions you have installed, the structure of this object will change.
     */
    ValueTree edd;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EDDOnlineUnlockStatus);
};
