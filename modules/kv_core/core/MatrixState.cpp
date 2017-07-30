
void MatrixState::resize (int r, int c)
{
    if (r < 0) r = 0;
    if (c < 0) c = 0;
    
    BigInteger bits;
    bits.setRange (0, r * c, false);
    
    // TODO: retain set bits
    
    numRows = r; numColumns = c;
    toggled.swapWith (bits);
}
