

#include "../JuceLibraryCode/JuceHeader.h"

int main (int argc, char* argv[])
{
    Logger::writeToLog ("juce-keygen v0.0.1");
    
    RSAKey publicKey, privateKey;
    
    int numBits = 1024;
    if (argc > 1)
    {
        const var userNumBits (argv[1]);
        numBits = (int) userNumBits;
    }

    if (numBits < 16)
        numBits = 16;
    
    String msg = "Generating Keypair: "; msg << numBits << " bits";
    Logger::writeToLog (msg);
    RSAKey::createKeyPair (publicKey, privateKey, numBits);
    
    Logger::writeToLog ("Public Key:");
    Logger::writeToLog (publicKey.toString());
    Logger::writeToLog ("\nPrivateKey:");
    Logger::writeToLog (privateKey.toString());
    
    return 0;
}
