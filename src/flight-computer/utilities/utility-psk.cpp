/**
 * @file utility-psk.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Implementation of PSK31 modulation with varicode and convolutional
 * code tables.
 * @version 0.1
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 */

#include "utility-psk.h"

/**
 * 
 * @brief A lookup table for converting char -> varicode string.
 * Supports all ASCII control characters and printable characters 0-127.
 */
static uint16_t ascii_to_varicode[128] = {
    // ASCII Control Characters (0 - 31)
    0b1010101011, // 0	    [NUL]	Null character
    0b1011011011, // 1	    [SOH]	Start of Header
    0b1011101101, // 2	    [STX]	Start of Text
    0b1101110111, // 3	    [ETX]	End of Text
    0b1011101011, // 4	    [EOT]	End of Transmission
    0b1101011111, // 5	    [ENQ]	Enquiry
    0b1011101111, // 6	    [ACK]	Acknowledgment
    0b1011111101, // 7	    [BEL]	Bell
    0b1011111111, // 8	    [BS]	Backspace
    0b11101111,	  // 9	    [HT]	Horizontal Tab
    0b11101,	  // 10	    [LF]	Line feed
    0b1101101111, // 11	    [VT]	Vertical Tab
    0b1011011101, // 12	    [FF]	Form feed
    0b11111,	  // 13	    [CR]	Carriage return
    0b1101110101, // 14	    [SO]	Shift Out
    0b1110101011, // 15	    [SI]	Shift In
    0b1011110111, // 16	    [DLE]	Data Link Escape
    0b1011110101, // 17	    [DC1]	Device Control 1 (XON)
    0b1110101101, // 18	    [DC2]	Device Control 2
    0b1110101111, // 19	    [DC3]	Device Control 3 (XOFF)
    0b1101011011, // 20	    [DC4]	Device Control 4
    0b1101101011, // 21	    [NAK]	Negative Acknowledgement
    0b1101101101, // 22	    [SYN]	Synchronous Idle
    0b1101010111, // 23	    [ETB]	End of Trans. Block
    0b1101111011, // 24	    [CAN]	Cancel
    0b1101111101, // 25	    [EM]	End of Medium
    0b1110110111, // 26	    [SUB]	Substitute
    0b1101010101, // 27	    [ESC]	Escape
    0b1101011101, // 28	    [FS]	File Separator
    0b1110111011, // 29	    [GS]	Group Separator
    0b1011111011, // 30	    [RS]	Record Separator
    0b1101111111, // 31	    [US]	Unit Separator
    
    // ASCII Printable Characters (32 - 126)
    0b1,          //    32	[Space]
    0b111111111,  //    33	!
    0b101011111,  //    34	"
    0b111110101,  //    35	#
    0b111011011,  //    36	$
    0b1011010101, //    37	%
    0b1010111011, //    38	&
    0b101111111,  //    39	'
    0b11111011,   //    40	(
    0b11110111,   //    41	)
    0b101101111,  //    42	*
    0b111011111,  //    43	+
    0b1110101,    //    44	, 
    0b110101,     //	45	-
    0b1010111,    //	46	.
    0b110101111,  //	47	/
    0b10110111,   //	48	0
    0b10111101,   //	49	1
    0b11101101,   //	50	2
    0b11111111,   //	51	3
    0b101110111,  //	52	4
    0b101011011,  //	53	5
    0b101101011,  //	54	6
    0b110101101,  //	55	7
    0b110101011,  //	56	8
    0b110110111,  //	57	9
    0b11110101,   //	58	:
    0b110111101,  //	59	;
    0b111101101,  //	60	<
    0b1010101,    //	61	=
    0b111010111,  //	62	>
    0b1010101111, //	63	?
    0b1010111101, //	64	@
    0b1111101,    //	65	A
    0b11101011,   //	66	B
    0b10101101,   //	67	C
    0b10110101,   //	68	D
    0b1110111,    //	69	E
    0b11011011,   //	70	F
    0b11111101,   //	71	G
    0b101010101,  //	72	H
    0b1111111,    //	73	I
    0b111111101,  //	74	J
    0b101111101,  //	75	K
    0b11010111,   //	76	L
    0b10111011,   //	77	M
    0b11011101,   //	78	N
    0b10101011,   //	79	O
    0b11010101,   //	80	P
    0b111011101,  //	81	Q
    0b10101111,   //	82	R
    0b1101111,    //	83	S
    0b1101101,    //	84	T
    0b101010111,  //	85	U
    0b110110101,  //	86	V
    0b101011101,  //	87	W
    0b101110101,  //	88	X
    0b101111011,  //	89	Y
    0b1010101101, //	90	Z
    0b111110111,  //	91	[
    0b111101111,  //	92	[backslash]
    0b111111011,  //	93	]
    0b1010111111, //	94	^
    0b101101101,  //	95	_
    0b1011011111, //	96	'
    0b1011,       //	97	a
    0b1011111,    //	98	b
    0b101111,     //	99	c
    0b101101,     //	100	d
    0b11,         //	101	e
    0b111101,     //	102	f
    0b1011011,    //	103	g
    0b101011,     //	104	h
    0b1101,       //	105	i
    0b111101011,  //	106	j
    0b10111111,   //	107	k
    0b11011,      //	108	l
    0b111011,     //	109	m
    0b1111,       //	110	n
    0b111,        //	111	o
    0b111111,     //	112	p
    0b110111111,  //	113	q
    0b10101,      //	114	r
    0b10111,      //	115	s
    0b101,        //	116	t
    0b110111,     //	117	u
    0b1111011,    //	118	v
    0b1101011,    //	119	w
    0b11011111,   //	120	x
    0b1011101,    //	121	y
    0b111010101,  //	122	z
    0b1010110111, //	123	{
    0b110111011,  //	124	|
    0b1010110101, //	125	}
    0b1011010111, //	126	~
    0b1110110101  //	127	[DEL]
};

/**
 * @brief An unordered_map to convert a string representation of varicode
 * to a char. Supports all Varicode/ASCII characters 0-127.
 */
static std::unordered_map<std::string, int> varicode_to_ascii = {
    // ASCII Control Characters (0 - 31)
    {"1010101011"   ,   0   }, // 0	    [NUL]	Null character
    {"1011011011"   ,   1   }, // 1	    [SOH]	Start of Header
    {"1011101101"   ,   2   }, // 2	    [STX]	Start of Text
    {"1101110111"   ,   3   }, // 3	    [ETX]	End of Text
    {"1011101011"   ,   4   }, // 4	    [EOT]	End of Transmission
    {"1101011111"   ,   5   }, // 5	    [ENQ]	Enquiry
    {"1011101111"   ,   6   }, // 6	    [ACK]	Acknowledgment
    {"1011111101"   ,   7   }, // 7	    [BEL]	Bell
    {"1011111111"   ,   8   }, // 8	    [BS]	Backspace
    {"11101111"     ,   9   }, // 9	    [HT]	Horizontal Tab
    {"11101"        ,   10  }, // 10	[LF]	Line feed
    {"1101101111"   ,   11  }, // 11	[VT]	Vertical Tab
    {"1011011101"   ,   12  }, // 12	[FF]	Form feed
    {"11111"        ,   13  }, // 13	[CR]	Carriage return
    {"1101110101"   ,   14  }, // 14	[SO]	Shift Out
    {"1110101011"   ,   15  }, // 15	[SI]	Shift In
    {"1011110111"   ,   16  }, // 16	[DLE]	Data Link Escape
    {"1011110101"   ,   17  }, // 17	[DC1]	Device Control 1 (XON)
    {"1110101101"   ,   18  }, // 18	[DC2]	Device Control 2
    {"1110101111"   ,   19  }, // 19	[DC3]	Device Control 3 (XOFF)
    {"1101011011"   ,   20  }, // 20	[DC4]	Device Control 4
    {"1101101011"   ,   21  }, // 21	[NAK]	Negative Acknowledgement
    {"1101101101"   ,   22  }, // 22	[SYN]	Synchronous Idle
    {"1101010111"   ,   23  }, // 23	[ETB]	End of Trans. Block
    {"1101111011"   ,   24  }, // 24	[CAN]	Cancel
    {"1101111101"   ,   25  }, // 25	[EM]	End of Medium
    {"1110110111"   ,   26  }, // 26	[SUB]	Substitute
    {"1101010101"   ,   27  }, // 27	[ESC]	Escape
    {"1101011101"   ,   28  }, // 28	[FS]	File Separator
    {"1110111011"   ,   29  }, // 29	[GS]	Group Separator
    {"1011111011"   ,   30  }, // 30	[RS]	Record Separator
    {"1101111111"   ,   31  }, // 31	[US]	Unit Separator

    // ASCII Printable Characters (32 - 126)
    {"1"            ,	32	}, //	Space
    {"111111111"    ,   33  }, //	!
    {"101011111"	,	34	}, //	"
    {"111110101"	,	35	}, //	#
    {"111011011"	,	36	}, //	$
    {"1011010101"	,	37	}, //	%
    {"1010111011"	,	38	}, //	&
    {"101111111"	,	39	}, //	'
    {"11111011"	    ,	40	}, //	(
    {"11110111"	    ,	41	}, //	)
    {"101101111"	,	42	}, //	*
    {"111011111"	,	43	}, //	+
    {"1110101"	    ,	44	}, //	,
    {"110101"	    ,	45	}, //	-
    {"1010111"	    ,	46	}, //	.
    {"110101111"	,	47	}, //	/
    {"10110111"	    ,	48	}, //	0
    {"10111101"	    ,	49	}, //	1
    {"11101101"	    ,	50	}, //	2
    {"11111111"	    ,	51	}, //	3
    {"101110111"	,	52	}, //	4
    {"101011011"	,	53	}, //	5
    {"101101011"	,	54	}, //	6
    {"110101101"	,	55	}, //	7
    {"110101011"	,	56	}, //	8
    {"110110111"	,	57	}, //	9
    {"11110101"	    ,	58	}, //	:
    {"110111101"	,	59	}, //	;
    {"111101101"	,	60	}, //	<
    {"1010101"	    ,	61	}, //	=
    {"111010111"	,	62	}, //	>
    {"1010101111"	,	63	}, //	?
    {"1010111101"	,	64	}, //	@
    {"1111101"	    ,	65	}, //	A
    {"11101011"	    ,	66	}, //	B
    {"10101101"	    ,	67	}, //	C
    {"10110101"	    ,	68	}, //	D
    {"1110111"	    ,	69	}, //	E
    {"11011011"	    ,	70	}, //	F
    {"11111101"	    ,	71	}, //	G
    {"101010101"	,	72	}, //	H
    {"1111111"	    ,	73	}, //	I
    {"111111101"	,	74	}, //	J
    {"101111101"	,	75	}, //	K
    {"11010111"	    ,	76	}, //	L
    {"10111011"	    ,	77	}, //	M
    {"11011101"	    ,	78	}, //	N
    {"10101011"	    ,	79	}, //	O
    {"11010101"	    ,	80	}, //	P
    {"111011101"	,	81	}, //	Q
    {"10101111"	    ,	82	}, //	R
    {"1101111"	    ,	83	}, //	S
    {"1101101"	    ,	84	}, //	T
    {"101010111"	,	85	}, //	U
    {"110110101"	,	86	}, //	V
    {"101011101"	,	87	}, //	W
    {"101110101"	,	88	}, //	X
    {"101111011"	,	89	}, //	Y
    {"1010101101"   ,   90  }, //	Z
    {"111110111"    ,   91  }, //	[
    {"111101111"    ,   92  }, //	[backslash]
    {"111111011"    ,   93  }, //	]
    {"1010111111"   ,   94  }, //	^
    {"101101101"    ,   95  }, //	_
    {"1011011111"   ,   96  }, //	'
    {"1011"         ,   97  }, //	a
    {"1011111"      ,   98  }, //	b
    {"101111"       ,   99  }, //	c
    {"101101"	    ,	100	}, //	d
    {"11"	        ,	101	}, //	e
    {"111101"	    ,	102	}, //	f
    {"1011011"	    ,	103	}, //	g
    {"101011"	    ,	104	}, //	h
    {"1101"	        ,	105	}, //	i
    {"111101011"	,	106	}, //	j
    {"10111111"	    ,	107	}, //	k
    {"11011"	    ,	108	}, //	l
    {"111011"	    ,	109	}, //	m
    {"1111"	        ,	110	}, //	n
    {"111"	        ,	111	}, //	o
    {"111111"	    ,	112	}, //	p
    {"110111111"	,	113	}, //	q
    {"10101"	    ,	114	}, //	r
    {"10111"	    ,	115	}, //	s
    {"101"	        ,	116	}, //	t
    {"110111"	    ,	117	}, //	u
    {"1111011"	    ,	118	}, //	v
    {"1101011"	    ,	119	}, //	w
    {"11011111"	    ,	120	}, //	x
    {"1011101"	    ,	121	}, //	y
    {"111010101"	,	122	}, //	z
    {"1010110111"	,	123	}, //	{
    {"110111011"	,	124	}, //	|
    {"1010110101"	,	125	}, //	}
    {"1011010111"	,	126	}, //	~
    {"1110110101"	,	127	}  //	[DEL]
};

/**
 * @brief Convolutional Code. Left shifting 5 bit values mapped to their
 * respective phase shift values in degrees. Supports all binary values
 * 0x00 to 0x1F.
 * @cite http://www.arrl.org/psk31-spec
 */
static std::map <unsigned char, double> conv_code = {
    {0b00000, M_PI},
    {0b00001, M_PI/2.0 },
    {0b00010, -(M_PI/2.0)},
    {0b00011, 0  },
    {0b00100, -(M_PI/2.0)},
    {0b00101, 0  },
    {0b00110, M_PI},
    {0b00111, M_PI/2.0 },
    {0b01000, 0  },
    {0b01001, -(M_PI/2.0)},
    {0b01010, M_PI/2.0 },
    {0b01011, M_PI},
    {0b01100, M_PI/2.0 },
    {0b01101, M_PI},
    {0b01110, 0 },
    {0b01111, -(M_PI/2.0)},
    {0b10000, M_PI/2.0 },
    {0b10001, M_PI},
    {0b10010, 0  },
    {0b10011, -(M_PI/2.0)},
    {0b10100, 0  },
    {0b10101, -(M_PI/2.0)},
    {0b10110, M_PI/2.0 },
    {0b10111, M_PI},
    {0b11000, -(M_PI/2.0)},
    {0b11001, 0  },
    {0b11010, M_PI},
    {0b11011, M_PI/2.0 },
    {0b11100, M_PI},
    {0b11101, M_PI/2.0 },
    {0b11110, -(M_PI/2.0)},
    {0b11111, 0  }
};

/**
 * @brief Construct a PSK Modulator object without morse callsign.
 * 
 * @param file_path Path to where the wav file will be saved
 * @param mode BPSK125, BPSK250, BPSK500, QPSK125, QPSK250, QPSK500
 */
PSK::PSK(std::string file_path, Mode mode, SymbolRate sym_rate) {
    file_path_ = file_path;
    call_sign_ = "";
    morse_callsign_ = false;
    setup(mode, sym_rate);
}

/**
 * @brief Construct a PSK Modulator object with morese code callsign added to 
 * beginning and end of wav file.
 * 
 * @param file_path Path to where the wav file will be saved
 * @param mode BPSK125, BPSK250, BPSK500, QPSK125, QPSK250, QPSK500
 * @param call_sign Callsign - must be at least 4 characters
 */
PSK::PSK(std::string file_path, Mode mode, SymbolRate sym_rate, std::string call_sign) {
    file_path_ = file_path;
    mode_ = mode;
    call_sign_ = call_sign;
    morse_callsign_ = true;
    setup(mode, sym_rate);
}

/**
 * @todo
 */
PSK::~PSK() {

}

/**
 * @brief Encode a string of text data into PSK audio data and save to wave
 * file. 
 * @details Adds preamble and postamble to the audio data and if a callsign was
 * specified, adds morse code callsign to the beginning and end of the audio.
 * 
 * This method will use varicode with two 0's between each character.
 * If in QPSK mode, it will use the convolutional code.
 * 
 * @param message 
 * @return true - Success, you can find the wav file at the file path specified
 * @return false - Failure, check the console for more information
 */
bool PSK::encodeTextData(std::string message) {
    if (!openFile(file_path_)) { // Open Wav File at Path
        throw std::invalid_argument("Failed to open file at path: " + file_path_);
    } else {
        writeHeader(); // Write Wav Header on success
    }

    //if (morse_callsign_) { // If callsign specified, add morse code callsign
    //    if (call_sign_.length() < 4) {
    //        throw std::invalid_argument("Callsign must be at least 4 characters");
    //    }
    //    addCallSign();
    //}

    addPreamble(); // Add PSK Preamble to bitstream (0's)
    for (char &c : message) {
        addVaricode(c); // Add each char of the message to the bitstream (varicode)
        static unsigned char zeros[1] = {0x00};
        addBits(zeros, 2); // Add two 0's between each character
    }
    addPostamble(); // Add PSK Postamble to bitstream
    pushBufferToBitStream(); // Push any remaining bits in the buffer to the bitstream

    encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file

    //if (morse_callsign_) { // If callsign specified, add morse code callsign to end of audio
    //    addCallSign();
    //}

    finalizeFile(); // Close wav file
    return true;
}

/**
 * @brief Encode raw data into PSK audio.
 * @details Adds preamble and postamble to the audio, and callsign if specified.
 * Does *not* use varicode or convolutional code! This should only be used
 * if you are manually decoding. You will be forced to use a morse callsign
 * if you use this method. You can of course change this if you want to, but
 * please keep in mind that if you use your own encoding people will not
 * be able to identify your station!
 * 
 * @param data 
 * @param length 
 * @return true 
 * @return false - Failure, check the console for more information.
 */
bool PSK::encodeRawData(unsigned char *data, int length) {
    if (!openFile(file_path_)) { // Open Wav File at Path
        throw std::invalid_argument("Failed to open file at path: " + file_path_);
    } else {
        writeHeader(); // Write Wav Header on success
    }

    if (!morse_callsign_) { // If callsign specified, add morse code callsign
        throw std::invalid_argument("Callsign required for raw data");
    }
    if (call_sign_.length() < 4) {
        throw std::invalid_argument("Callsign must be at least 4 characters");
    }

    addCallSign();

    addPreamble(); // Add PSK Preamble to bitstream
    //addBits(data, length*8); // Add raw data to bitstream
    addPostamble(); // Add PSK Postamble to bitstream
    encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file

    addCallSign();

    finalizeFile(); // Close wav file
    return true;
}

/*
void PSK::dumpBitStream() {
    std::cout << "Bitstream:" << std::endl;
    for (int i = 0; i < bit_stream_.size(); i++) {
        std::cout << "[" << i << "]" << std::bitset<32>(bit_stream_[i]) << std::endl;
    }
}
*/

/**
 * @todo write docs
 * 
 * @param mode 
 * @param symbol_rate 
 */
void PSK::setup(Mode mode, SymbolRate symbol_rate) {
    mode_ = mode;
    switch (mode) {
        case BPSK:
            break;
        case QPSK:
            break;
        default:
            throw std::invalid_argument("Invalid mode");
            break;
    }

    switch (symbol_rate) {
        case S31:
            symbol_rate_ = 31.5;
            break;
        case S63:
            symbol_rate_ = 63.5;
            break;
        case S125:
            symbol_rate_ = 125.0;
            break;
        case S250:
            symbol_rate_ = 250.0;
            break;
        case S500:
            symbol_rate_ = 500.0;
            break;
        case S1000:
            symbol_rate_ = 1000.0;
            break;
        default:
            throw std::invalid_argument("Invalid symbol rate");
            break;
    }

    angle_delta_ = 2.0 * M_PI * ( (double) carrier_freq_ / (double) sample_rate_ );
    samples_per_symbol_ = std::floor(sample_rate_ / symbol_rate_);
}

/**
 * @brief Opens the wav file and writes the header.
 * @param file_path 
 * @return true - Success
 * @return false - Failure
 */
bool PSK::openFile(std::string file_path) {
    wav_file_.open(file_path, std::ios::binary);
    if (wav_file_.is_open()) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Writes the header to the wav file, this happens before audio data.
 * @details This method will save the location of the 'data size' field in
 * the header so that it can be updated later after adding data.
 */
void PSK::writeHeader() {
    wav_file_ << "RIFF****WAVE"; // RIFF header
    wav_file_ << "fmt "; // format
    writeBytes(16, 4); // size
    writeBytes(1, 2); // compression code
    writeBytes(1, 2); // number of channels
    writeBytes(sample_rate_, 4); // sample rate
    writeBytes(sample_rate_ * bits_per_sample_ / 8, 4 ); // Byte rate
    writeBytes(bits_per_sample_ / 8, 2); // block align
    writeBytes(bits_per_sample_, 2); // bits per sample
    wav_file_ << "data****"; // data section follows this

    // Save the location of the data size field so that it can be updated later
    data_start_ = wav_file_.tellp();
}

/**
 * @brief Writes the audio data to the wav file.
 * @param data - int of data to write to wav file
 * @param num_bytes - number of bytes to write
 */
void PSK::writeBytes(int data, int size) {
    wav_file_.write(reinterpret_cast<const char*> (&data), size);
}

/**
 * @brief Updates the 'data segment size' field in the wav header and closes
 * the file.
 */
void PSK::finalizeFile() {
    int data_end_ = wav_file_.tellp(); // Save the position of the end of the 
                                       // data chunk
    wav_file_.seekp(data_start_ - 4); // Go to the beginning of the data chunk
    writeBytes(data_end_ - data_start_, 4); // and write the size of the chunk.
    wav_file_.seekp(4, std::ios::beg); // Go to the beginning of the file
    writeBytes(data_end_ - 8, 4); // Write the size of the overall file
    wav_file_.close();
}

/**
 * @brief Adds callsign in morse audio form to the wav file
 * @todo implement
 */
void PSK::addCallSign() {

}

/**
 * @brief Converts a char to varicode and adds it to the bitstream
 * 
 * @param c char to add to the bistream
 */
void PSK::addVaricode(char c) {
    uint16_t varicode = ascii_to_varicode[c];
    //std::cout << std::endl << "Adding varicode for " << c << ":" << std::bitset<16>(varicode) << " ";
    unsigned char bits[2];
    int previous_bit = 1;
    for (int i = 0; i < 16; i++) {
        if (!(varicode & (1 << i)) && (!previous_bit)) {
            //std::cout << "size: " << i - 1;

            bits[0] = i > 9 ? varicode >> 1 : varicode << (9 - i);
            bits[1] = i > 9 ? varicode << (17 - i) : 0x00;

            //std::cout << " shifted: " << std::bitset<8>(bits[0]) << " " << std::bitset<8>(bits[1]) << std::endl;
            addBits(bits, i - 1);
            break;
        } else {
            previous_bit = varicode & (1 << i);
        }
    }
}

/**
 * @brief Adds a bit to the bit stream.
 * @details see format of 'bit stream' in header file. This function assumes
 * that you're adding bits in order (left to right). So if you want to add 1
 * you must add 0x80 with a 'num_bits' of 1. All numbers past 'num_bits'
 * must be 0.
 * @param data 
 * @param num_bits 
 */
void PSK::addBits(unsigned char *data, int num_bits) {
    int data_index = 0;
    int bit_index = 0; // Index of the bit in the byte, left to right [0 - 7]
    int buffer_space;
    
    while (num_bits > 0) {
        buffer_space = 32 - bit_stream_offset_;
        
        if (buffer_space == 0) { // Buffer is full, write to bit stream
            bit_stream_.push_back(bit_stream_buffer_);
            bit_stream_buffer_ = 0;
            bit_stream_offset_ = 0;
            buffer_space = 32;
        }
        if (buffer_space >= num_bits && num_bits > 8) { // Write a byte at a time
            bit_stream_buffer_ |= (data[data_index] << (32 - bit_stream_offset_ - 8));
            bit_stream_offset_ += 8;
            num_bits -= 8;
            data_index++;
        } else { // Write a bit at a time
            bit_stream_buffer_ |= (data[data_index] & (1 << (7 - bit_index))) ? 1 << (32 - bit_stream_offset_ - 1) : 0;
            bit_stream_offset_++;
            num_bits--;
            bit_index++;
            if (bit_index == 8) {
                bit_index = 0;
                data_index++;
            }
        }
    }
}

/**
 * @brief After adding all of the data to the bit stream, this method will
 * write the data left in the buffer to the bit stream. This should be called
 * last.
 */
void PSK::pushBufferToBitStream() {
    bit_stream_.push_back(bit_stream_buffer_);
    bit_stream_buffer_ = 0;
    bit_stream_offset_ = 0;
    bit_stream_index_ = 0;
}

/**
 * @brief Adds the preamble to the bit stream.
 */
void PSK::addPreamble() {
    static unsigned char zeros[1] = {0x00};
    for (int i = 0; i < preamble_length_; i++) {
        addBits(zeros, 1);
    }
}

/**
 * @brief Adds the postamble to the bit stream.
 * Fldigi seems to use the incorrect postamble in QPSK mode.
 * It uses reapeated 0s instead of 1s. This is a workaround.
 * If you are not using fldigi, set the flag to 0.
 * @cite https://www.mail-archive.com/fldigi-alpha@lists.berlios.de/msg01032.html
 */
void PSK::addPostamble() {
    const int fldigi_postamble_mode_ = 0;
    static unsigned char zeros[1] = {0x00};
    static unsigned char ones[1] = {0xFF};
    while ((32 - bit_stream_offset_) >= 1) {
        addBits((mode_ == QPSK && fldigi_postamble_mode_ ? zeros : ones), 1);
    }
    for (int i = 0; i < postamble_length_; i++) {
        addBits((mode_ == QPSK && fldigi_postamble_mode_ ? zeros : ones), 1);
    }
}

/**
 * @brief Returns the next bit of the bit stream that will be encoded.
 * 
 * @return int [1, 0, -1] -1 = no more bits in bit stream
 */
int PSK::popNextBit() {
    if (bit_stream_index_ >= bit_stream_.size()) { // No more bits in bit stream
        return -1;
    }

    uint32_t bit = bit_stream_[bit_stream_index_] & (1 << (31 - bit_stream_offset_));
    
    bit_stream_offset_++;
    if (bit_stream_offset_ == 32) {
        bit_stream_offset_ = 0;
        bit_stream_index_++;
    }

    return bit ? 1 : 0;
}

/**
 * @brief Returns the next bit of the bit stream without removing it.
 * @return int [1, 0, -1] -1 = no more bits in bit stream
 */
int PSK::peakNextBit() {
    if (bit_stream_index_ >= bit_stream_.size()) { // No more bits in bit stream
        return -1;
    }
    uint32_t bit = bit_stream_[bit_stream_index_] & (1 << (31 - bit_stream_offset_));
    return bit ? 1 : 0;
}

/**
 * @brief Goes throught he bit stream and modulates with the addSymbol method.
 * @details With BPSK31, the bit 0 is encoded by switching phases and the bit 1 
 * is encoded by keeping the phase shift the same.
 */
void PSK::encodeBitStream() {
    if (mode_ == BPSK) { // BPSK modulation
        int bit = popNextBit();
        int next_bit = peakNextBit();
        int last_phase = 0; // 0 = 0, 1 = M_PI
        while (bit != -1) { 
            if (bit) { // Encode a 1 by keeping the phase shift the same
                int filter_end = next_bit == 1 ? 0 : 1; // If next bit is 1, do not filter end of symbol.
                addSymbol(last_phase ? 0 : M_PI, filter_end);
            } else if (!bit) { // Encode a 0 by switching phase
                int filter_end = next_bit == 1 ? 0 : 1; // If next bit is 1, do not filter end of symbol.
                addSymbol(last_phase ? M_PI : 0, filter_end);
                last_phase = !last_phase;
            }
            last_phase ? "1" : "0";
            bit = popNextBit();
            next_bit = peakNextBit();
        }
    } else if (mode_ == QPSK) { // QPSK modulation
        unsigned char buffer = 0;
        int bit = popNextBit();
        int shift = 0;
        while (bit != -1) {
            buffer = ((buffer << 1) | bit) & 0x1f;
            int filter_end = 0;
            if (shift != conv_code[buffer]) {
                filter_end = 1;
            }
            shift += conv_code[buffer];
            addSymbol(shift, filter_end);
            bit = popNextBit();
        }
    }
}

/**
 * @brief Modulates a single symbol in BPSK/QPSK and saves the audio data to the
 * wav file.
 * 
 * @param shift The shift of the carrier wave in degrees
 * @param filter_end Whether or not to apply the filter to the end of the symbol
 */
void PSK::addSymbol(double shift, int filter_end) {
    const double power = 2.0;
    const double roll_off = 2.9;
    const double amplitude = .5;

    double time = 0 - (samples_per_symbol_ / 2);
    for (int i = 0; i < samples_per_symbol_; i++) {
        double unfiltered = std::cos(carrier_wave_angle_ + shift);
        double filter = std::pow(std::cos( (abs(time) / samples_per_symbol_) * roll_off ), power);
        if (!last_symbol_end_filtered_ && (time < 0)) {
            filter = 1;
        }
        if (!filter_end && (time > 0)) { // Remove filter from end of symbol
            filter = 1;
        }
        int sample = amplitude * filter * unfiltered * max_amplitude_;
        carrier_wave_angle_ += angle_delta_;
        time += 1;
        if (carrier_wave_angle_ > 2 * M_PI) {
            carrier_wave_angle_ -= 2 * M_PI;
        }
        writeBytes(sample, 2); // write sample to wav file
    }
    last_symbol_end_filtered_ = filter_end;
}

