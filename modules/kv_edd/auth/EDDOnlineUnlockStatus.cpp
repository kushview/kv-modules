
namespace Crypto {

static ValueTree decryptEDDValueTree (String hexData, RSAKey rsaPublicKey)
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

EDDOnlineUnlockStatus::EDDOnlineUnlockStatus() { }
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
        params.set (useLicenseKey() ? "license" : "password", password);
    
    url = url.withParameters (params);
    DBG("[edd] authenticating @ " << url.toString (true));
    
    if (ScopedPointer<XmlElement> xml = url.readEntireXmlStream (true))
    {
        if (auto* eddNode = xml->getChildByName ("EDD"))
        {
            const String keyText (eddNode->getAllSubText().trim());
            edd = Crypto::decryptEDDValueTree (keyText.fromFirstOccurrenceOf ("#", true, true),
                                               publicKey);
        }
        
        return xml->createDocument (String());
    }
    
    return String();
}
