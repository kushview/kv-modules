
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
    
    
    
    /** Reads the reply from your server and decrypts the edd ValueTree */
    String readReplyFromWebserver (const String& email, const String& password) override;
    
    /** Returns the owned price IDs purchased by this user.
        Note: This only returns the price ids with a completed payment.
        You must perform your own checks to to provide additional unlocking features.
     */
    inline var getOwnedPriceIDs() const      { return edd["ownedPriceIDs"]; }

    /** Returns the EDD download/product id on your website */
    inline var getDownloadID() const         { return edd["downloadID"]; }

    /** Override this to specify use of license key */

protected:
    /** Custom data sent back during authorization will be stored here. by default
        this only has two properties ownedPriceIDs and downloadID. Depending on the
        EDD extensions you have installed, the structure of this object may change
     */
    ValueTree edd;

    /** Override this to add additional query params sent to the web server */
    virtual StringPairArray getQueryParams() { return StringPairArray(); }
    
    /** Override this to denote whether you are authenticating a license key or not.
        It is ok to return false even if you are using EDD Software Licensing to track
        activations */
    inline virtual bool useLicenseKey() const { return false; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EDDOnlineUnlockStatus);
};
