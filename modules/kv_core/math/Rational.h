/** Representation of a rational number */
struct Rational
{
    explicit Rational (const int n = 1, const int d = 1)
        : num(n), den(d) { }
    
    Rational (const Rational& o) { operator= (o); }
    Rational& operator= (const Rational& o)
    {
        num = o.num;
        den = o.den;
        return *this;
    }
    
    /** The numerator */
    int num;
    
    /** The denominator */
    int den;
    
    /** Returns numerator / denominator */
    double ratio() const { jassert(den > 0); return (double)num / (double)den; }
};
