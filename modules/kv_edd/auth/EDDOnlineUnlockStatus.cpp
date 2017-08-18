
namespace edd {

static const char* nodeName = "edd";
static const char* keyName  = "key";

static ValueTree decryptValueTree (String hexData, RSAKey rsaPublicKey)
{
    BigInteger val;
    val.parseString (hexData, 16);
    
    RSAKey key (rsaPublicKey);
    jassert (key.isValid());
    
    ValueTree edd;
    
    if (! val.isZero())
    {
        key.applyToValue (val);
        const MemoryBlock mb (val.toMemoryBlock());
        if (CharPointer_UTF8::isValidString (static_cast<const char*> (mb.getData()), (int) mb.getSize()))
            if (ScopedPointer<XmlElement> xml = XmlDocument::parse (mb.toString()))
                edd = ValueTree::fromXml (*xml);
    }
    
    return edd;
}

}

EDDOnlineUnlockStatus::EDDOnlineUnlockStatus() : edd (edd::nodeName) { }
EDDOnlineUnlockStatus::~EDDOnlineUnlockStatus() { }

String EDDOnlineUnlockStatus::readReplyFromWebserver (const String& email, const String& password)
{
    const RSAKey publicKey (getPublicKey());
    URL url (getServerAuthenticationURL());
    StringPairArray params (getQueryParams());
    params.set ("app", getProductID());
    
    if (email.isNotEmpty())
        params.set (URL::isProbablyAnEmailAddress (email) ? "email" : "username", email);
    
    if (password.isNotEmpty())
        params.set ("password", password);
    
    if (getLicenseKey().isNotEmpty())
        params.set ("license", getLicenseKey());
    
    url = url.withParameters (params);
    DBG("[edd] authenticating @ " << url.toString (true));
    
    if (ScopedPointer<XmlElement> xml = url.readEntireXmlStream (true))
    {
        if (auto* keyElement = xml->getChildByName ("KEY"))
        {
            const String keyText (keyElement->getAllSubText().trim());
            edd = edd::decryptValueTree (keyText.fromFirstOccurrenceOf ("#", true, true),
                                         publicKey);
            DBG(edd.toXmlString());
            edd = edd.getChildWithName (edd::nodeName);
        }
        
        return xml->createDocument (String());
    }
    
    return String();
}

void EDDOnlineUnlockStatus::eddRestoreState (const String& state)
{
    const RSAKey publicKey (getPublicKey());
    MemoryBlock mb; mb.fromBase64Encoding (state);
    edd = ValueTree (edd::nodeName);
    
    if (mb.getSize() > 0)
    {
        const ValueTree reg = ValueTree::readFromGZIPData (mb.getData(), mb.getSize());
        const String keyText = reg[edd::keyName].toString().fromFirstOccurrenceOf ("#", true, true);
        edd = edd::decryptValueTree (keyText, publicKey);
        edd = edd.getChildWithName (edd::nodeName);
    }
    
    if (! edd.isValid() || !edd.hasType (edd::nodeName))
        edd = ValueTree (edd::nodeName);
}

String EDDOnlineUnlockStatus::activateLicense (const String& license, const String& email,
                                               const String& password)
{
    URL url ("http://kushview.dev/");
    url = url.withParameter ("edd_action", "activate_license")
             .withParameter ("item_id", getProductID())
             .withParameter ("license", license)
             .withParameter ("url", getLocalMachineIDs()[0]);
    
    OnlineUnlockStatus::UnlockResult r;
    r.succeeded = false;
    
    URL url ("http://kushview.dev/");
    url = url.withParameter ("edd_action", "deactivate_license")
    .withParameter ("item_id", "15")
    .withParameter ("license", license)
    .withParameter ("url", getLocalMachineIDs()[0]);
    DBG("connecting: " << url.toString(true));
    
    const String raw (url.readEntireTextStream());
    var response;
    Result result (JSON::parse (raw, response));
    if (result.failed())
    {
        r.errorMessage = result.getErrorMessage();
        r.succeeded = false;
        return r.errorMessage;
    }
    
    auto* object = response.getDynamicObject();
    jassert (nullptr != object);
    jassert (object->hasProperty ("success"));
    jassert (object->hasProperty ("license"));
    jassert (object->hasProperty ("key"));
    
    String keyText;
    
    MemoryOutputStream mo;
    if (Base64::convertFromBase64 (mo, object->getProperty("key").toString()))
    {
        MemoryBlock mb = mo.getMemoryBlock();
        if (CharPointer_UTF8::isValidString ((const char*) mb.getData(), (int) mb.getSize()))
            keyText = String::fromUTF8 ((const char*) mb.getData());
        
    }
    mo.flush();
    applyKeyFile (keyText);
    
    r.succeeded = (bool) object->getProperty ("success");
    edd.setProperty ("active", object->getProperty("license").toString().trim() == "deactivated", 0);
    
#if JUCE_DEBUG
    DBG("deactivation response:");
    for (int i = 0; i < object->getProperties().size(); ++i) {
        DBG("  " << object->getProperties().getName(i) << " = " <<
            object->getProperties().getValueAt(i).toString());
    }
#endif
    
    return r.errorMessage;
}

String EDDOnlineUnlockStatus::deactivateLicense (const String& license)
{
    OnlineUnlockStatus::UnlockResult r;
    r.succeeded = false;
    
    const URL url (getApiEndPoint()
             .withParameter ("edd_action", "activate_license")
             .withParameter ("item_id", "15")
             .withParameter ("license", license)
             .withParameter ("url", getLocalMachineIDs()[0]));

    DBG("connecting: " << url.toString (true));
    
    const String raw (url.readEntireTextStream());
    var response;
    Result result (JSON::parse (raw, response));
    if (result.failed())
    {
        r.errorMessage = result.getErrorMessage();
        r.succeeded = false;
        return r.errorMessage;
    }
    
    auto* object = response.getDynamicObject();
    jassert (nullptr != object);
    jassert (object->hasProperty ("success"));
    jassert (object->hasProperty ("license"));
    jassert (object->hasProperty ("key"));
    
    String keyText;
    
    MemoryOutputStream mo;
    if (Base64::convertFromBase64 (mo, object->getProperty("key").toString()))
    {
        MemoryBlock mb = mo.getMemoryBlock();
        if (CharPointer_UTF8::isValidString ((const char*) mb.getData(), (int) mb.getSize()))
            keyText = String::fromUTF8 ((const char*) mb.getData());
        
    }
    mo.flush();
    applyKeyFile (keyText);
    
    r.succeeded = (bool) object->getProperty ("success");
    edd.setProperty ("active", object->getProperty("license").toString().trim() == "deactivated", 0);
    
   #if JUCE_DEBUG
    DBG("deactivation response:");
    for (int i = 0; i < object->getProperties().size(); ++i) {
        DBG("  " << object->getProperties().getName(i) << " = " <<
            object->getProperties().getValueAt(i).toString());
    }
   #endif
    
    return r.errorMessage;
}
