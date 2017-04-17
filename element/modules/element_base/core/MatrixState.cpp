
void MatrixState::resize (int r, int c)
{
    if (r < 1) r = 1;
    if (c < 1) c = 1;
    
    BigInteger bits;
    bits.setRange (0, r * c, false);
    
    // TODO: retain set bits
    
    numRows = r; numColumns = c;
    toggled.swapWith (bits);
}
