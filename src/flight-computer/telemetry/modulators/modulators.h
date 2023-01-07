/**
 * @file modulators.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief See https://github.com/joshua-jerred/MWAV
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */

#ifndef MODULATORS_H_
#define MODULATORS_H_

#include "wavgen.h"

namespace modulators {
    enum class PskMode {
        BPSK,
        QPSK
    };

    enum class PskSymbolRate {
        S125,
        S250,
        S500,
        S1000 // BPSK only
    };

    bool Morse(WavGen &wavgen, std::string callsign);
    
    bool PskAscii(
        WavGen &wavgen, 
        const std::string &message,
        const PskMode &mode,
        const PskSymbolRate &sym_rate
                  );

    bool AfskAscii(
        WavGen &wavgen, 
        const std::string &message);
}

#endif // MODULATORS_H_