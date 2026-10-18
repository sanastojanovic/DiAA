#ifndef __TYPES_HPP
#define __TYPES_HPP

enum class WINDOW_FUNCTION{
    NONE = 0,
    HANN = 1,
    WELCH = 2,
    PARZEN = 3,
    HAMMING = 4,
    FLAT_TOP = 5
};

enum class NOISE_TYPE{
    NORMAL = 0,
    PINK = 1,
    IMPULSIVE = 2
};


#endif