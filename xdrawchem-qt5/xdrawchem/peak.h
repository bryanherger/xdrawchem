#ifndef PEAK_H
#define PEAK_H

#define PEAK_REACTANT 1
#define PEAK_PRODUCT 2

class Peak
{
public:
    int r_p;  // reactant or product?
    double value;
    int multiplicity;
    int intensity;
    QString comment;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
