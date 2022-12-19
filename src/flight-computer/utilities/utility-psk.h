/**
 * @file utility-psk.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Header for the psk utility class.
 * @cite Source: https://github.com/joshua-jerred/PSK31-Modulation-Generator
 * @version 0.1
 * @date 2022-10-21
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_PSK_H_
#define UTILITY_PSK_H_

#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <math.h>
#include <sstream>
#include <iostream>

class PSK {
    public:
        enum Mode { 
            BPSK,
            QPSK
            };

        enum SymbolRate {
            S31, 
            S63, 
            S125, 
            S250, 
            S500,
            S1000
        };

        PSK(std::string file_path, Mode mode, SymbolRate sym_rate);
        //PSK(std::string file_path, Mode mode, SymbolRate sym_rate, std::string call_sign);
        ~PSK();

        bool encodeTextData(std::string message);
        bool encodeRawData(unsigned char *data, int length);
        //void dumpBitStream();
        int getLength();
        

    private:
        void setup(Mode mode, SymbolRate baud);
        const int morse_frequency_ = 600; // Audio frequency
        const int morse_dit_length_ = 100; // milliseconds
        const int morse_dah_length_ = 300; // milliseconds
        
        const int preamble_length_ = 64; // symbols
        const int postamble_length_ = 64; // symbols

        std::string file_path_;
        Mode mode_ = Mode::BPSK; 
        //bool morse_callsign_; // Not implemented yet
        //std::string call_sign_;


        // WAV file members and methods
        bool openFile(std::string file_path);
        void writeHeader();
        void writeBytes(int data, int size);
        void finalizeFile();
        void addCallSign();
        
        const int sample_rate_ = 44100; // Sample rate of the WAV file in Hz (44100)
        const int bits_per_sample_ = 16;
        const int max_amplitude_ = pow(2, bits_per_sample_ - 1) - 1;
        std::ofstream wav_file_; // File descriptor for the WAV file
        int data_start_ = 0;

        // Bit Stream members and methods
        void addVaricode(char c);
        void addBits(unsigned char *data, int num_bits);
        void pushBufferToBitStream();
        void addPreamble();
        void addPostamble();
        int popNextBit();
        int peakNextBit();
        
        /**
         * @details Bit stream is an array of 32 bit integers.
         * The bits are stored in a 32 bit integer in the following way:
         * 
         * Characters: 
         * a a b b c c
         * 
         * varicode with 2x 0 bit padding added to the end of each character: 
         * '1011' '00' '1011' '00'  '1011111' '00' '1011111' '00' '101111' '00' '101111' '00'
         * 
         * 32 bit array:
         * [0](10110010110010111110010111110010) {a, a, b, b, half of c    }
         * [1](11110010111100000000000000000000) {other half of c, 0 bits..}
         * 
         * Although this may not be the best way to store the bit stream,
         * it makes it easier to understand.
         */
        std::vector<uint32_t> bit_stream_ = std::vector<uint32_t>();
        int bit_stream_index_ = 0;
        uint32_t bit_stream_buffer_ = 0;
        int bit_stream_offset_ = 0; // Write left to right. [0 - 31]

        // Modulation members and methods
        void encodeBitStream();
        void addSymbol(double shift, int next_shift);

        double symbol_rate_ = 125; // Symbol rate of the PSK modulation in Sym/s (125, 250, 500)
        int carrier_freq_ = 1500; // Carrier frequency in Hz (1500)
        int samples_per_symbol_ = 0; // floor(sample_rate_ / symbol_rate_)

        double carrier_wave_angle_ = 0.0f;
        double angle_delta_ = 0.0f;
        int last_symbol_end_filtered_ = 1;
        
        int last_bit_ = 0;
        unsigned char conv_code_buffer_ = 0;

        long int num_samples_ = 0;
};

#endif // UTILITY_PSK_H_