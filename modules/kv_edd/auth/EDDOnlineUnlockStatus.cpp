
namespace edd {

static const char* nodeName = "edd";

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

String EDDOnlineUnlockStatus::activateLicense (const String& license, const String& email,
                                               const String& password)
{
    return String();
}

String EDDOnlineUnlockStatus::dectivateLicense (const String& license)
{
    return String();
}
