#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "cff.hh"
#include "error.hh"
#include "t1item.hh"
#include "straccum.hh"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cmath>

#ifndef static_assert
#define static_assert(c) switch (c) case 0: case (c):
#endif

const char * const EfontCFF::operator_names[] = {
    "version", "Notice", "FullName", "FamilyName",
    "Weight", "FontBBox", "BlueValues", "OtherBlues",
    "FamilyBlues", "FamilyOtherBlues", "StdHW", "StdVW",
    "UNKNOWN_12", "UniqueID", "XUID", "charset",
    "Encoding", "CharStrings", "Private", "Subrs",
    "defaultWidthX", "nominalWidthX", "UNKNOWN_22", "UNKNOWN_23",
    "UNKNOWN_24", "UNKNOWN_25", "UNKNOWN_26", "UNKNOWN_27",
    "UNKNOWN_28", "UNKNOWN_29", "UNKNOWN_30", "UNKNOWN_31",
    "Copyright", "isFixedPitch", "ItalicAngle", "UnderlinePosition",
    "UnderlineThickness", "PaintType", "CharstringType", "FontMatrix",
    "StrokeWidth", "BlueScale", "BlueShift", "BlueFuzz",
    "StemSnapH", "StemSnapV", "ForceBold", "UNKNOWN_12_15",
    "UNKNOWN_12_16", "LanguageGroup", "ExpansionFactor", "initialRandomSeed",
    "SyntheticBase", "PostScript", "BaseFontName", "BaseFontBlend",
    "UNKNOWN_12_24", "UNKNOWN_12_25", "UNKNOWN_12_26", "UNKNOWN_12_27",
    "UNKNOWN_12_28", "UNKNOWN_12_29", "ROS", "CIDFontVersion",
    "CIDFontRevision", "CIDFontType", "CIDCount", "UIDBase",
    "FDArray", "FDSelect", "FontName"
};

const int EfontCFF::operator_types[] = {
    tSID, tSID, tSID, tSID,	// version, Notice, FullName, FamilyName
    tSID, tArray4, tP+tArray, tP+tArray, // Weight, FontBBox, BlueValues, OtherBlues
    tP+tArray, tP+tArray, tP+tNumber, tP+tNumber, // FamBlues, FamOthBlues, StdHW, StdVW
    tNone, tNumber, tArray, tOffset, // escape, UniqueID, XUID, charset
    tOffset, tOffset, tPrivateType, tP+tLocalOffset, // Encoding, CharStrings, Private, Subrs
    tP+tNumber, tP+tNumber, tNone, tNone, // defaultWX, nominalWX, 22, 23
    tNone, tNone, tNone, tNone,	// 24, 25, 26, 27
    tNone, tNone, tNone, tNone,	// 28, 29, 30, 31
    tSID, tBoolean, tNumber, tNumber, // Copyright, isFixedPitch, ItalicAngle, UnderlinePosition
    tNumber, tNumber, tNumber, tArray6,	// UnderlineThickness, PaintType, CharstringType, FontMatrix
    tNumber, tP+tNumber, tP+tNumber, tP+tNumber, // StrokeWidth, BlueScale, BlueShift, BlueFuzz
    tP+tArray, tP+tArray, tP+tBoolean, tNone, // StemSnapH, StemSnapV, ForceBold, 12 15
    tNone, tP+tNumber, tP+tNumber, tP+tNumber, // 12 16, LanguageGroup, ExpansionFactor, initialRandomSeed
    tNumber, tSID, tSID, tArray, // SyntheticBase, PostScript, BaseFontName, BaseFontBlend
    tNone, tNone, tNone, tNone,	// 12 24, 12 25, 12 26, 12 27
    tNone, tNone, tArray, tNumber, // 12 28, 12 29, ROS, CIDFontVersion
    tNumber, tNumber, tNumber, tNumber,	// CIDFontRevision, CIDFontType, CIDCount, UIDBase
    tOffset, tOffset, tSID	// FDArray, FDSelect, FontName
};

static PermString::Initializer initializer;
static const char *standard_strings[] = {
    // Automatically generated from Appendix A of the CFF specification; do
    // not edit. Size should be 391.
    ".notdef", "space", "exclam", "quotedbl", "numbersign", "dollar",
    "percent", "ampersand", "quoteright", "parenleft", "parenright",
    "asterisk", "plus", "comma", "hyphen", "period", "slash", "zero", "one",
    "two", "three", "four", "five", "six", "seven", "eight", "nine", "colon",
    "semicolon", "less", "equal", "greater", "question", "at", "A", "B", "C",
    "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z", "bracketleft", "backslash",
    "bracketright", "asciicircum", "underscore", "quoteleft", "a", "b", "c",
    "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r",
    "s", "t", "u", "v", "w", "x", "y", "z", "braceleft", "bar", "braceright",
    "asciitilde", "exclamdown", "cent", "sterling", "fraction", "yen",
    "florin", "section", "currency", "quotesingle", "quotedblleft",
    "guillemotleft", "guilsinglleft", "guilsinglright", "fi", "fl", "endash",
    "dagger", "daggerdbl", "periodcentered", "paragraph", "bullet",
    "quotesinglbase", "quotedblbase", "quotedblright", "guillemotright",
    "ellipsis", "perthousand", "questiondown", "grave", "acute", "circumflex",
    "tilde", "macron", "breve", "dotaccent", "dieresis", "ring", "cedilla",
    "hungarumlaut", "ogonek", "caron", "emdash", "AE", "ordfeminine", "Lslash",
    "Oslash", "OE", "ordmasculine", "ae", "dotlessi", "lslash", "oslash", "oe",
    "germandbls", "onesuperior", "logicalnot", "mu", "trademark", "Eth",
    "onehalf", "plusminus", "Thorn", "onequarter", "divide", "brokenbar",
    "degree", "thorn", "threequarters", "twosuperior", "registered", "minus",
    "eth", "multiply", "threesuperior", "copyright", "Aacute", "Acircumflex",
    "Adieresis", "Agrave", "Aring", "Atilde", "Ccedilla", "Eacute",
    "Ecircumflex", "Edieresis", "Egrave", "Iacute", "Icircumflex", "Idieresis",
    "Igrave", "Ntilde", "Oacute", "Ocircumflex", "Odieresis", "Ograve",
    "Otilde", "Scaron", "Uacute", "Ucircumflex", "Udieresis", "Ugrave",
    "Yacute", "Ydieresis", "Zcaron", "aacute", "acircumflex", "adieresis",
    "agrave", "aring", "atilde", "ccedilla", "eacute", "ecircumflex",
    "edieresis", "egrave", "iacute", "icircumflex", "idieresis", "igrave",
    "ntilde", "oacute", "ocircumflex", "odieresis", "ograve", "otilde",
    "scaron", "uacute", "ucircumflex", "udieresis", "ugrave", "yacute",
    "ydieresis", "zcaron", "exclamsmall", "Hungarumlautsmall",
    "dollaroldstyle", "dollarsuperior", "ampersandsmall", "Acutesmall",
    "parenleftsuperior", "parenrightsuperior", "twodotenleader",
    "onedotenleader", "zerooldstyle", "oneoldstyle", "twooldstyle",
    "threeoldstyle", "fouroldstyle", "fiveoldstyle", "sixoldstyle",
    "sevenoldstyle", "eightoldstyle", "nineoldstyle", "commasuperior",
    "threequartersemdash", "periodsuperior", "questionsmall", "asuperior",
    "bsuperior", "centsuperior", "dsuperior", "esuperior", "isuperior",
    "lsuperior", "msuperior", "nsuperior", "osuperior", "rsuperior",
    "ssuperior", "tsuperior", "ff", "ffi", "ffl", "parenleftinferior",
    "parenrightinferior", "Circumflexsmall", "hyphensuperior", "Gravesmall",
    "Asmall", "Bsmall", "Csmall", "Dsmall", "Esmall", "Fsmall", "Gsmall",
    "Hsmall", "Ismall", "Jsmall", "Ksmall", "Lsmall", "Msmall", "Nsmall",
    "Osmall", "Psmall", "Qsmall", "Rsmall", "Ssmall", "Tsmall", "Usmall",
    "Vsmall", "Wsmall", "Xsmall", "Ysmall", "Zsmall", "colonmonetary",
    "onefitted", "rupiah", "Tildesmall", "exclamdownsmall", "centoldstyle",
    "Lslashsmall", "Scaronsmall", "Zcaronsmall", "Dieresissmall", "Brevesmall",
    "Caronsmall", "Dotaccentsmall", "Macronsmall", "figuredash",
    "hypheninferior", "Ogoneksmall", "Ringsmall", "Cedillasmall",
    "questiondownsmall", "oneeighth", "threeeighths", "fiveeighths",
    "seveneighths", "onethird", "twothirds", "zerosuperior", "foursuperior",
    "fivesuperior", "sixsuperior", "sevensuperior", "eightsuperior",
    "ninesuperior", "zeroinferior", "oneinferior", "twoinferior",
    "threeinferior", "fourinferior", "fiveinferior", "sixinferior",
    "seveninferior", "eightinferior", "nineinferior", "centinferior",
    "dollarinferior", "periodinferior", "commainferior", "Agravesmall",
    "Aacutesmall", "Acircumflexsmall", "Atildesmall", "Adieresissmall",
    "Aringsmall", "AEsmall", "Ccedillasmall", "Egravesmall", "Eacutesmall",
    "Ecircumflexsmall", "Edieresissmall", "Igravesmall", "Iacutesmall",
    "Icircumflexsmall", "Idieresissmall", "Ethsmall", "Ntildesmall",
    "Ogravesmall", "Oacutesmall", "Ocircumflexsmall", "Otildesmall",
    "Odieresissmall", "OEsmall", "Oslashsmall", "Ugravesmall", "Uacutesmall",
    "Ucircumflexsmall", "Udieresissmall", "Yacutesmall", "Thornsmall",
    "Ydieresissmall", "001.000", "001.001", "001.002", "001.003", "Black",
    "Bold", "Book", "Light", "Medium", "Regular", "Roman", "Semibold"
};
static PermString standard_permstrings[EfontCFF::NSTANDARD_STRINGS];
static HashMap<PermString, int> standard_permstrings_map(-1);

static const int standard_encoding[] = {
    // Automatically generated from Appendix B of the CFF specification; do
    // not edit. Size should be 256.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
    59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
    69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
    79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
    89, 90, 91, 92, 93, 94, 95, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 96, 97, 98, 99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 0, 111, 112, 113,
    114, 0, 115, 116, 117, 118, 119, 120, 121, 122,
    0, 123, 0, 124, 125, 126, 127, 128, 129, 130,
    131, 0, 132, 133, 0, 134, 135, 136, 137, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 138, 0, 139, 0, 0,
    0, 0, 140, 141, 142, 143, 0, 0, 0, 0,
    0, 144, 0, 0, 0, 145, 0, 0, 146, 147,
    148, 149, 0, 0, 0, 0
};

static const int expert_encoding[] = {
    // Automatically generated from Appendix B of the CFF specification; do
    // not edit. Size should be 256.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 229, 230, 0, 231, 232, 233, 234,
    235, 236, 237, 238, 13, 14, 15, 99, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 27, 28,
    249, 250, 251, 252, 0, 253, 254, 255, 256, 257,
    0, 0, 0, 258, 0, 0, 259, 260, 261, 262,
    0, 0, 263, 264, 265, 0, 266, 109, 110, 267,
    268, 269, 0, 270, 271, 272, 273, 274, 275, 276,
    277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
    287, 288, 289, 290, 291, 292, 293, 294, 295, 296,
    297, 298, 299, 300, 301, 302, 303, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 304, 305, 306, 0, 0, 307, 308, 309, 310,
    311, 0, 312, 0, 0, 313, 0, 0, 314, 315,
    0, 0, 316, 317, 318, 0, 0, 0, 158, 155,
    163, 319, 320, 321, 322, 323, 324, 325, 0, 0,
    326, 150, 164, 169, 327, 328, 329, 330, 331, 332,
    333, 334, 335, 336, 337, 338, 339, 340, 341, 342,
    343, 344, 345, 346, 347, 348, 349, 350, 351, 352,
    353, 354, 355, 356, 357, 358, 359, 360, 361, 362,
    363, 364, 365, 366, 367, 368, 369, 370, 371, 372,
    373, 374, 375, 376, 377, 378
};

static const int iso_adobe_charset[] = {
    // Automatically generated from Appendix C of the CFF specification; do
    // not edit.
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
    130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
    170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
    190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    220, 221, 222, 223, 224, 225, 226, 227, 228
};

static const int expert_charset[] = {
    // Automatically generated from Appendix C of the CFF specification; do
    // not edit.
    0, 1, 229, 230, 231, 232, 233, 234, 235, 236,
    237, 238, 13, 14, 15, 99, 239, 240, 241, 242,
    243, 244, 245, 246, 247, 248, 27, 28, 249, 250,
    251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
    261, 262, 263, 264, 265, 266, 109, 110, 267, 268,
    269, 270, 271, 272, 273, 274, 275, 276, 277, 278,
    279, 280, 281, 282, 283, 284, 285, 286, 287, 288,
    289, 290, 291, 292, 293, 294, 295, 296, 297, 298,
    299, 300, 301, 302, 303, 304, 305, 306, 307, 308,
    309, 310, 311, 312, 313, 314, 315, 316, 317, 318,
    158, 155, 163, 319, 320, 321, 322, 323, 324, 325,
    326, 150, 164, 169, 327, 328, 329, 330, 331, 332,
    333, 334, 335, 336, 337, 338, 339, 340, 341, 342,
    343, 344, 345, 346, 347, 348, 349, 350, 351, 352,
    353, 354, 355, 356, 357, 358, 359, 360, 361, 362,
    363, 364, 365, 366, 367, 368, 369, 370, 371, 372,
    373, 374, 375, 376, 377, 378
};

static const int expert_subset_charset[] = {
    // Automatically generated from Appendix C of the CFF specification; do
    // not edit.
    0, 1, 231, 232, 235, 236, 237, 238, 13, 14,
    15, 99, 239, 240, 241, 242, 243, 244, 245, 246,
    247, 248, 27, 28, 249, 250, 251, 253, 254, 255,
    256, 257, 258, 259, 260, 261, 262, 263, 264, 265,
    266, 109, 110, 267, 268, 269, 270, 272, 300, 301,
    302, 305, 314, 315, 158, 155, 163, 320, 321, 322,
    323, 324, 325, 326, 150, 164, 169, 327, 328, 329,
    330, 331, 332, 333, 334, 335, 336, 337, 338, 339,
    340, 341, 342, 343, 344, 345, 346
};


#define POS_GT(pos1, pos2)	((unsigned)(pos1) > (unsigned)(pos2))


EfontCFF::EfontCFF(const String &s, ErrorHandler *errh)
    : _data_string(s), _data(reinterpret_cast<const unsigned char *>(_data_string.data())), _len(_data_string.length()),
      _strings_map(-2)
{
    static_assert((sizeof(standard_strings) / sizeof(standard_strings[0])) == NSTANDARD_STRINGS);
    static_assert((sizeof(standard_encoding) / sizeof(standard_encoding[0])) == 256);
    static_assert((sizeof(expert_encoding) / sizeof(expert_encoding[0])) == 256);
    _error = parse_header(errh ? errh : ErrorHandler::silent_handler());
}

EfontCFF::~EfontCFF()
{
    for (int i = 0; i < _gsubrs_cs.size(); i++)
	delete _gsubrs_cs[i];
}

/*
 * Parsing the file header
 */

int
EfontCFF::parse_header(ErrorHandler *errh)
{
    if (_gsubrs_index.error() >= 0) // already done
	return 0;

    // parse header
    if (_len < HEADER_SIZE)
	return errh->error("CFF file too small for header"), -EFAULT;
    if (_data[0] != 1)		// major version number
	return errh->error("bad major version number %d", _data[0]), -ERANGE;
    int hdrSize = _data[2], offSize = _data[3];
    if (hdrSize < 4 || hdrSize > _len || offSize < 1 || offSize > 4)
	return errh->error("corrupted file header"), -EINVAL;
    int name_index_pos = hdrSize;

    // parse name INDEX
    IndexIterator niter(_data, name_index_pos, _len, errh, "Name INDEX");
    if (niter.error() < 0)
	return niter.error();
    _name_index.clear();
    for (; niter; niter++) {
	const unsigned char *d0 = niter[0];
	const unsigned char *d1 = niter[1];
	if (d0 == d1 || d0[0] == 0)
	    _name_index.push_back(PermString());
	else
	    _name_index.push_back(PermString(reinterpret_cast<const char *>(d0), d1 - d0));
    }
    int top_dict_index_pos = niter.index_end() - _data;

    // check top DICT INDEX
    _top_dict_index = IndexIterator(_data, top_dict_index_pos, _len, errh, "Top DICT INDEX");
    if (_top_dict_index.error() < 0)
	return _top_dict_index.error();
    else if (_top_dict_index.nitems() != nfonts())
	return errh->error("invalid font: Top DICT INDEX has %d elements, but there are %d fonts", _top_dict_index.nitems(), nfonts()), -EINVAL;
    int string_index_pos = _top_dict_index.index_end() - _data;

    // check strings INDEX
    _strings_index = IndexIterator(_data, string_index_pos, _len, errh, "Strings INDEX");
    if (_strings_index.error() < 0)
	return _strings_index.error();
    else if (NSTANDARD_STRINGS + _strings_index.nitems() - 1 > MAX_SID)
	return errh->error("too many strings defined in font"), -EINVAL;
    _strings.assign(_strings_index.nitems(), PermString());
    int global_subr_index_pos = _strings_index.index_end() - _data;

    // check gsubr INDEX
    _gsubrs_index = IndexIterator(_data, global_subr_index_pos, _len, errh, "Gsubrs INDEX");
    if (_gsubrs_index.error() < 0)
	return _gsubrs_index.error();
    _gsubrs_cs.assign(ngsubrs(), 0);
    
    return 0;
}

int
EfontCFF::sid(PermString s)
{
    // check standard strings
    if (standard_permstrings_map["a"] < 0)
	for (int i = 0; i < NSTANDARD_STRINGS; i++) {
	    if (!standard_permstrings[i])
		standard_permstrings[i] = PermString(standard_strings[i]);
	    standard_permstrings_map.insert(standard_permstrings[i], i);
	}
    int sid = standard_permstrings_map[s];
    if (sid >= 0)
	return sid;

    // check user strings
    sid = _strings_map[s];
    if (sid >= -1)
	return sid;

    for (int i = 0; i < _strings.size(); i++)
	if (!_strings[i] && s.length() == _strings_index[i+1] - _strings_index[i] && memcmp(s.cc(), _strings_index[i], s.length()) == 0) {
	    _strings[i] = s;
	    _strings_map.insert(s, i + NSTANDARD_STRINGS);
	    return i + NSTANDARD_STRINGS;
	}

    _strings_map.insert(s, -1);
    return -1;
}

String
EfontCFF::sid_string(int sid) const
{
    if (sid < 0)
	return String();
    else if (sid < NSTANDARD_STRINGS)
	return String(sid_permstring(sid));
    else {
	sid -= NSTANDARD_STRINGS;
	if (sid >= _strings.size())
	    return String();
	else if (_strings[sid])
	    return String(_strings[sid]);
	else
	    return String(reinterpret_cast<const char *>(_strings_index[sid]), _strings_index[sid + 1] - _strings_index[sid]);
    }
}

PermString
EfontCFF::sid_permstring(int sid) const
{
    if (sid < 0)
	return PermString();
    else if (sid < NSTANDARD_STRINGS) {
	if (!standard_permstrings[sid])
	    standard_permstrings[sid] = PermString(standard_strings[sid]);
	return standard_permstrings[sid];
    } else {
	sid -= NSTANDARD_STRINGS;
	if (sid >= _strings.size())
	    return PermString();
	else if (_strings[sid])
	    return _strings[sid];
	else {
	    PermString s = PermString(reinterpret_cast<const char *>(_strings_index[sid]), _strings_index[sid + 1] - _strings_index[sid]);
	    _strings[sid] = s;
	    _strings_map.insert(s, sid + NSTANDARD_STRINGS);
	    return s;
	}
    }
}

int
EfontCFF::font_offset(int findex, int &offset, int &length) const
{
    if (findex < 0 || findex >= nfonts())
	return -ENOENT;
    offset = _top_dict_index[findex] - _data;
    length = _top_dict_index[findex + 1] - _top_dict_index[findex];
    return 0;
}

int
EfontCFF::font_offset(PermString name, int &offset, int &length) const
{
    for (int i = 0; i < _name_index.size(); i++)
	if (_name_index[i] == name && name)
	    return font_offset(i, offset, length);
    return -ENOENT;
}

static inline int
subr_bias(int charstring_type, int nsubrs)
{
    if (charstring_type == 1)
	return 0;
    else if (nsubrs < 1240)
	return 107;
    else if (nsubrs < 33900)
	return 1131;
    else
	return 32768;
}

Charstring *
EfontCFF::gsubr(int i)
{
    i += subr_bias(2, ngsubrs());
    if (i < 0 || i >= ngsubrs())
	return 0;
    if (!_gsubrs_cs[i]) {
	const unsigned char *s1 = _gsubrs_index[i];
	int slen = _gsubrs_index[i + 1] - s1;
	String cs = data_string().substring(s1 - data(), slen);
	if (slen == 0)
	    return 0;
	else
	    _gsubrs_cs[i] = new Type2Charstring(cs);
    }
    return _gsubrs_cs[i];
}


/*****
 * EfontCFF::Charset
 **/

EfontCFF::Charset::Charset(const EfontCFF *cff, int pos, int nglyphs, ErrorHandler *errh)
{
    assign(cff, pos, nglyphs, errh);
}

void
EfontCFF::Charset::assign(const EfontCFF *cff, int pos, int nglyphs, ErrorHandler *errh)
{
    if (!errh)
	errh = ErrorHandler::silent_handler();
    
    _sids.reserve(nglyphs);
    
    if (pos == 0)
	assign(iso_adobe_charset, sizeof(iso_adobe_charset) / sizeof(int), nglyphs);
    else if (pos == 1)
	assign(expert_charset, sizeof(expert_charset) / sizeof(int), nglyphs);
    else if (pos == 2)
	assign(expert_subset_charset, sizeof(expert_subset_charset) / sizeof(int), nglyphs);
    else
	_error = parse(cff, pos, nglyphs, errh);

    if (_error >= 0) {
	_gids.assign(cff->max_sid() + 1, -1);
	for (int g = 0; g < _sids.size(); g++) {
	    if (_gids[_sids[g]] >= 0) {
		errh->error("glyph '%s' in charset twice", cff->sid_permstring(_sids[g]).cc());
		_error = -EEXIST;
	    }
	    _gids[_sids[g]] = g;
	}
    }
}

void
EfontCFF::Charset::assign(const int *data, int size, int nglyphs)
{
    if (size < nglyphs)
	size = nglyphs;
    _sids.resize(size);
    memcpy(&_sids[0], data, sizeof(const int) * size);
    _error = 0;
}

int
EfontCFF::Charset::parse(const EfontCFF *cff, int pos, int nglyphs, ErrorHandler *errh)
{
    const unsigned char *data = cff->data();
    int len = cff->length();
    int max_sid = cff->max_sid();
    
    if (pos + 1 > len)
	return errh->error("charset position out of range"), -EFAULT;

    _sids.push_back(0);

    int format = data[pos];
    if (format == 0) {
	if (pos + 1 + (nglyphs - 1) * 2 > len)
	    return errh->error("charset[0] out of range"), -EFAULT;
	const unsigned char *p = data + pos + 1;
	for (; _sids.size() < nglyphs; p += 2) {
	    int sid = (p[0] << 8) | p[1];
	    if (sid > max_sid)
		return errh->error("charset[0] uses bad SID %d", sid), -EINVAL;
	    _sids.push_back(sid);
	}
	
    } else if (format == 1) {
	const unsigned char *p = data + pos + 1;
	for (; _sids.size() < nglyphs; p += 3) {
	    if (p + 3 > data + len)
		return errh->error("charset[1] out of range"), -EFAULT;
	    int sid = (p[0] << 8) | p[1];
	    int n = p[2];
	    if (sid + n > max_sid)
		return errh->error("charset[1] uses bad SID %d", sid + n), -EINVAL;
	    for (int i = 0; i <= n; i++)
		_sids.push_back(sid + i);
	}

    } else if (format == 2) {
	const unsigned char *p = data + pos + 1;
	for (; _sids.size() < nglyphs; p += 4) {
	    if (p + 4 > data + len)
		return errh->error("charset[2] out of range"), -EFAULT;
	    int sid = (p[0] << 8) | p[1];
	    int n = (p[2] << 8) | p[3];
	    if (sid + n > max_sid)
		return errh->error("charset[2] uses bad SID %d", sid + n), -EINVAL;
	    for (int i = 0; i <= n; i++)
		_sids.push_back(sid + i);
	}
	
    } else
	return errh->error("unknown charset format %d", format), -EINVAL;

    _sids.resize(nglyphs);
    return 0;
}


/*****
 * EfontCFF::IndexIterator
 **/

EfontCFF::IndexIterator::IndexIterator(const unsigned char *data, int pos, int len, ErrorHandler *errh, const char *index_name)
    : _contents(0), _offset(0), _last_offset(0)
{
    if (!errh)
	errh = ErrorHandler::silent_handler();
    
    // check header
    int nitems = 0;
    if (POS_GT(pos + 2, len)) {
	errh->error("%s: position out of range", index_name);
	_offsize = -EFAULT;
    } else if (data[pos] == 0 && data[pos + 1] == 0) {
	_contents = data + 2;
	_offsize = 0;
    } else if (POS_GT(pos + 3, len)) {
	errh->error("%s: position out of range", index_name);
	_offsize = -EFAULT;
    } else if ((_offsize = data[pos + 2]), _offsize < 1 || _offsize > 4) {
	errh->error("%s: offset size %d out of range", index_name, _offsize);
	_offsize = -EINVAL;
    } else {
	nitems = (data[pos] << 8) | data[pos + 1];
	if (POS_GT(pos + 3 + (nitems + 1) * _offsize, len)) {
	    errh->error("%s: data out of range", index_name);
	    _offsize = -EFAULT;
	} else {
	    _offset = data + pos + 3;
	    _last_offset = _offset + nitems * _offsize;
	    _contents = _last_offset + _offsize - 1;
	}
    }

    // check items in offset array
    unsigned max_doff_allowed = len - (pos + 2 + (nitems + 1) * _offsize);
    unsigned last_doff = 1;
    for (const unsigned char *o = _offset; o <= _last_offset; o += _offsize) {
	unsigned doff = offset_at(o);
	if (doff > max_doff_allowed || doff < last_doff) {
	    errh->error("%s: garbled elements", index_name);
	    break;
	}
	last_doff = doff;
    }
}

const unsigned char *
EfontCFF::IndexIterator::index_end() const
{
    if (_offsize <= 0)
	return _contents;
    else
	return _contents + offset_at(_last_offset);
}

int
EfontCFF::IndexIterator::nitems() const
{
    if (_offsize <= 0)
	return 0;
    else
	return (_last_offset - _offset) / _offsize;
}



/*****
 * EfontCFF::Dict
 **/

EfontCFF::Dict::Dict()
    : _cff(0), _pos(0), _error(-ENOENT)
{
}

EfontCFF::Dict::Dict(EfontCFF *cff, int pos, int dict_len, ErrorHandler *errh, const char *dict_name)
{
    assign(cff, pos, dict_len, errh, dict_name);
}

int
EfontCFF::Dict::assign(EfontCFF *cff, int pos, int dict_len, ErrorHandler *errh, const char *dict_name)
{
    _cff = cff;
    _pos = pos;
    _operators.clear();
    _pointers.clear();
    _operands.clear();
    
    if (!errh)
	errh = ErrorHandler::silent_handler();
    
    const unsigned char *data = cff->data() + pos;
    const unsigned char *end_data = data + dict_len;
    
    _pointers.push_back(0);
    while (data < end_data)
	switch (data[0]) {
	    
	  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
	  case 8: case 9: case 10: case 11: case 13: case 14: case 15:
	  case 16: case 17: case 18: case 19: case 20: case 21:
	    _operators.push_back(data[0]);
	    _pointers.push_back(_operands.size());
	    data++;
	    break;
	    
	  case 22: case 23: case 24: case 25: case 26: case 27: case 31:
	  case 255:		// reserved
	    errh->error("%s: reserved operator %d", dict_name, data[0]);
	    return (_error = -ERANGE);
	    
	  case 12:
	    if (data + 1 >= end_data)
		goto runoff;
	    _operators.push_back(32 + data[1]);
	    _pointers.push_back(_operands.size());
	    data += 2;
	    break;

	  case 28: {
	      if (data + 2 >= end_data)
		  goto runoff;
	      int16_t val = (data[1] << 8) | data[2];
	      _operands.push_back(val);
	      data += 3;
	      break;
	  }

	  case 29: {
	      if (data + 4 >= end_data)
		  goto runoff;
	      int32_t val = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
	      _operands.push_back(val);
	      data += 5;
	      break;
	  }

	  case 30: {
	      char buf[1024];
	      int pos = 0;
	      if (data + 1 >= end_data)
		  goto runoff;
	      for (data++; data < end_data && pos < 1020; data++) {
		  int d = *data;
		  for (int i = 0; i < 2; i++, d <<= 8) {
		      int digit = (d >> 8) & 0xF;
		      switch (digit) {
			case 10:
			  buf[pos++] = '.';
			  break;
			case 11:
			  buf[pos++] = 'E';
			  break;
			case 12:
			  buf[pos++] = 'E';
			  buf[pos++] = '-';
			  break;
			case 13:
			  errh->error("%s: bad digit in real number", dict_name);
			  goto invalid;
			case 14:
			  buf[pos++] = '-';
			  break;
			case 15:
			  goto found;
			default:
			  buf[pos++] = digit + '0';
			  break;
		      }
		  }
	      }
	      // number not found
	      goto runoff;
	    found:
	      char *endptr;
	      buf[pos] = '\0';
	      _operands.push_back(strtod(buf, &endptr));
	      if (*endptr) {
		  errh->error("%s: real number syntax error", dict_name);
		  goto invalid;
	      }
	      data++;
	      break;
	  }
	    
	  case 247: case 248: case 249: case 250: {
	      if (data + 1 >= end_data)
		  goto runoff;
	      int val = ((data[0] - 247) << 8) + data[1] + 108;
	      _operands.push_back(val);
	      data += 2;
	      break;
	  }
	    
	  case 251: case 252: case 253: case 254: {
	      if (data + 1 >= end_data)
		  goto runoff;
	      int val = -((data[0] - 251) << 8) - data[1] - 108;
	      _operands.push_back(val);
	      data += 2;
	      break;
	  }

	  default:
	    _operands.push_back(data[0] - 139);
	    data++;
	    break;

	}

    // not closed by an operator?
    if (_pointers.back() != _operands.size()) {
	errh->error("%s: not closed by an operator", dict_name);
	goto invalid;
    }

    return (_error = 0);

  runoff:
    errh->error("%s: runoff end of DICT", dict_name);
    return (_error = -EFAULT);
    
  invalid:
    return (_error = -EINVAL);
}

int
EfontCFF::Dict::check(bool is_private, ErrorHandler *errh, const char *dict_name) const
{
    if (!errh)
	errh = ErrorHandler::silent_handler();
    int before_nerrors = errh->nerrors();

    // keep track of operator reuse
    Vector<int> operators_used;
    
    for (int i = 0; i < _operators.size(); i++) {
	int arity = _pointers[i+1] - _pointers[i];
	double num = (arity == 0 ? 0 : _operands[_pointers[i]]);
	double truncnum = trunc(num);
	int op = _operators[i];
	int type = (op > oLastOperator ? tNone : operator_types[op]);

	// check reuse
	if (op >= operators_used.size())
	    operators_used.resize(op + 1, 0);
	if (operators_used[op] && (type & tTypeMask) != tNone)
	    errh->error("%s: operator '%s' specified twice", dict_name, operator_names[op]);
	operators_used[op]++;
	
	// check data
	switch (type & tTypeMask) {
	    
	  case tNone:
	    if (op >= 32)
		errh->warning("%s: unknown operator '12 %d'", dict_name, op - 32);
	    else
		errh->warning("%s: unknown operator '%d'", dict_name, op);
	    continue;
	    
	  case tSID:
	    if (arity != 1 || num != truncnum || num < 0 || num > _cff->max_sid())
		goto bad_data;
	    break;
	    
	  case tFontNumber:
	    if (arity != 1 || num != truncnum || num < 0 || num >= _cff->nfonts())
		goto bad_data;
	    break;

	  case tBoolean:
	    if (arity != 1)
		goto bad_data;
	    else if (num != 0 && num != 1)
		errh->warning("%s: data for Boolean operator '%s' not 0 or 1", dict_name, operator_names[op]);
	    break;
	    
	  case tNumber:
	    if (arity != 1)
		goto bad_data;
	    break;
	    
	  case tOffset:
	    if (arity != 1 || num != truncnum || num < 0 || num >= _cff->length())
		goto bad_data;
	    break;
	    
	  case tLocalOffset:
	    if (arity != 1 || num != truncnum || _pos + num < 0 || _pos + num >= _cff->length())
		goto bad_data;
	    break;

	  case tPrivateType: {
	      if (arity != 2 || num != truncnum || num < 0)
		  goto bad_data;
	      double off = _operands[_pointers[i] + 1];
	      if (off < 0 || off + num >= _cff->length())
		  goto bad_data;
	      break;
	  }
	    
	  case tArray2: case tArray3: case tArray4:
	  case tArray5: case tArray6:
	    if (arity != (type & tTypeMask) - tArray2 + 2)
		goto bad_data;
	    break;

	  case tArray:
	    break;
	    
	}

	// check dict location
	if (((type & tPrivate) != 0) != is_private)
	    errh->warning("%s: operator '%s' in wrong DICT", dict_name, operator_names[op]);
	
	continue;

      bad_data:
	errh->error("%s: bad data for operator '%s'", dict_name, operator_names[op]);
    }

    return (errh->nerrors() != before_nerrors ? -1 : 0);
}

bool
EfontCFF::Dict::has(DictOperator op) const
{
    for (int i = 0; i < _operators.size(); i++)
	if (_operators[i] == op)
	    return true;
    return false;
}

bool
EfontCFF::Dict::value(DictOperator op, Vector<double> &out) const
{
    out.clear();
    for (int i = 0; i < _operators.size(); i++)
	if (_operators[i] == op) {
	    for (int j = _pointers[i]; j < _pointers[i+1]; j++)
		out.push_back(_operands[j]);
	    return true;
	}
    return false;
}

bool
EfontCFF::Dict::value(DictOperator op, int def, int *val) const
{
    *val = def;
    for (int i = 0; i < _operators.size(); i++)
	if (_operators[i] == op && _pointers[i] + 1 == _pointers[i+1]) {
	    *val = (int) _operands[_pointers[i]];
	    return true;
	}
    return false;
}

bool
EfontCFF::Dict::value(DictOperator op, double def, double *val) const
{
    *val = def;
    for (int i = 0; i < _operators.size(); i++)
	if (_operators[i] == op && _pointers[i] + 1 == _pointers[i+1]) {
	    *val = _operands[_pointers[i]];
	    return true;
	}
    return false;
}

void
EfontCFF::Dict::unparse(ErrorHandler *errh, const char *dict_name) const
{
    StringAccum sa;
    for (int i = 0; i < _operators.size(); i++) {
	sa.clear();
	if (_pointers[i] + 1 == _pointers[i+1])
	    sa << _operands[_pointers[i]];
	else {
	    sa << "[";
	    for (int j = _pointers[i]; j < _pointers[i+1]; j++)
		sa << _operands[j] << ' ';
	    sa.pop_back();
	    sa << "]";
	}
	errh->message("%s: %s %s", dict_name, operator_names[_operators[i]], sa.cc());
    }
}


/*****
 * EfontCFFFont
 **/

EfontCFF::Font::Font(EfontCFF *cff, PermString font_name, ErrorHandler *errh)
    : _cff(cff), _font_name(font_name), _t1encoding(0)
{
    if (!errh)
	errh = ErrorHandler::silent_handler();

    if (!cff->ok()) {
	errh->error("invalid CFF");
	_error = -EINVAL;
	return;
    }
    
    // check for a CFF file that contains exactly one font
    _error = -ENOENT;
    for (int i = 0; !_font_name && i < cff->nfonts(); i++)
	if (cff->font_name(i))
	    _font_name = cff->font_name(i);

    // find top DICT
    int td_offset, td_length;
    if (cff->font_offset(_font_name, td_offset, td_length) < 0) {
	errh->error("no font '%s' in CFF", _font_name.cc());
	return;
    }

    // parse top DICT
    _top_dict.assign(cff, td_offset, td_length, errh, "Top DICT");
    if ((_error = _top_dict.error()) < 0)
	return;
    _error = -EINVAL;
    if (_top_dict.check(false, errh, "Top DICT") < 0
	|| !_top_dict.has(oCharStrings)	|| !_top_dict.has(oPrivate))
	return;
    //_top_dict.unparse(errh, "Top DICT");

    // extract offsets and information from TOP DICT
    _top_dict.value(oCharstringType, 2, &_charstring_type);
    if (_charstring_type != 1 && _charstring_type != 2)
	return;
    
    int charstrings_offset;
    _top_dict.value(oCharStrings, 0, &charstrings_offset);
    _charstrings_index = EfontCFF::IndexIterator(cff->data(), charstrings_offset, cff->length(), errh, "CharStrings INDEX");
    if (_charstrings_index.error() < 0) {
	_error = _charstrings_index.error();
	return;
    }
    _charstrings_cs.assign(_charstrings_index.nitems(), 0);

    int charset;
    _top_dict.value(oCharset, 0, &charset);
    _charset.assign(cff, charset, _charstrings_index.nitems(), errh);
    if (_charset.error() < 0) {
	_error = _charset.error();
	return;
    }

    int Encoding;
    _top_dict.value(oEncoding, 0, &Encoding);
    if (parse_encoding(Encoding, errh) < 0)
	return;

    // extract information from Private DICT
    Vector<double> private_info;
    _top_dict.value(oPrivate, private_info);
    int private_offset = (int) private_info[1];
    _private_dict.assign(cff, private_offset, (int) private_info[0], errh, "Private DICT");
    if ((_error = _private_dict.error()) < 0)
	return;
    _error = -EINVAL;
    if (_private_dict.check(true, errh, "Private DICT") < 0)
	return;
    //_private_dict.unparse(errh, "Private DICT");

    _private_dict.value(oDefaultWidthX, 0, &_default_width_x);
    _private_dict.value(oNominalWidthX, 0, &_nominal_width_x);
    if (_private_dict.has(oSubrs)) {
	int subrs_offset;
	_private_dict.value(oSubrs, 0, &subrs_offset);
	_subrs_index = EfontCFF::IndexIterator(cff->data(), private_offset + subrs_offset, cff->length(), errh, "Subrs INDEX");
	if ((_error = _subrs_index.error()) < 0)
	    return;
    }
    _subrs_cs.assign(nsubrs(), 0);

    // success!
    _error = 0;
}

EfontCFF::Font::~Font()
{
    for (int i = 0; i < _charstrings_cs.size(); i++)
	delete _charstrings_cs[i];
    for (int i = 0; i < _subrs_cs.size(); i++)
	delete _subrs_cs[i];
    delete _t1encoding;
}

int
EfontCFF::Font::parse_encoding(int pos, ErrorHandler *errh)
{
    _encoding_pos = pos;
    for (int i = 0; i < 256; i++)
	_encoding[i] = 0;
    
    // check for standard encodings
    if (pos == 0)
	return assign_standard_encoding(standard_encoding);
    else if (pos == 1)
	return assign_standard_encoding(expert_encoding);

    // otherwise, a custom encoding
    const unsigned char *data = _cff->data();
    int len = _cff->length();
    if (pos + 1 > len)
	return errh->error("Encoding position out of range"), -EFAULT;
    bool supplemented = (data[pos] & 0x80) != 0;
    int format = (data[pos] & 0x7F);

    int retval = 0;
    int endpos, g = 1;
    if (format == 0) {
	endpos = pos + 2 + data[pos + 1];
	if (endpos > len)
	    return errh->error("Encoding[0] out of range"), -EFAULT;
	const unsigned char *p = data + pos + 2;
	int n = data[pos + 1];
	for (; g < n; g++, p++) {
	    int e = p[0];
	    if (_encoding[e])
		retval = 1;
	    _encoding[e] = g;
	}
	
    } else if (format == 1) {
	endpos = pos + 2 + data[pos + 1] * 2;
	if (endpos > len)
	    return errh->error("Encoding[1] out of range"), -EFAULT;
	const unsigned char *p = data + pos + 2;
	int n = data[pos + 1];
	for (int i = 0; i < n; i++, p += 2) {
	    int first = p[0];
	    int nLeft = p[1];
	    for (int e = first; e <= first + nLeft; e++) {
		if (_encoding[e])
		    retval = 1;
		_encoding[e] = g++;
	    }
	}

    } else
	return errh->error("unknown Encoding format %d", format), -EINVAL;

    if (g > _charset.nglyphs())
	return errh->error("Encoding glyph %d out of range", g), -EINVAL;
    
    // check supplements
    if (supplemented) {
	if (endpos + data[endpos] * 3 > len)
	    return -EINVAL;
	const unsigned char *p = data + endpos + 1;
	int n = data[endpos];
	for (int i = 0; i < n; i++, p += 3) {
	    int e = p[0];
	    int g = (p[1] << 8) | p[2];
	    if (_encoding[e])
		retval = 1;
	    if (g >= _charset.nglyphs())
		return errh->error("Encoding glyph %d out of range", g), -EINVAL;
	    _encoding[e] = g;
	}
    }

    // successfully done
    return retval;
}

int
EfontCFF::Font::assign_standard_encoding(const int *standard_encoding)
{
    for (int i = 0; i < 256; i++)
	_encoding[i] = _charset.sid_to_gid(standard_encoding[i]);
    return 0;
}

Charstring *
EfontCFF::Font::charstring(const IndexIterator &iiter, int which) const
{
    const unsigned char *s1 = iiter[which];
    int slen = iiter[which + 1] - s1;
    String cs = _cff->data_string().substring(s1 - _cff->data(), slen);
    if (slen == 0)
	return 0;
    else if (_charstring_type == 1)
	return new Type1Charstring(cs);
    else
	return new Type2Charstring(cs);
}

Charstring *
EfontCFF::Font::subr(int i) const
{
    i += subr_bias(_charstring_type, nsubrs());
    if (i < 0 || i >= nsubrs())
	return 0;
    if (!_subrs_cs[i])
	_subrs_cs[i] = charstring(_subrs_index, i);
    return _subrs_cs[i];
}

Charstring *
EfontCFF::Font::gsubr(int i) const
{
    return _cff->gsubr(i);
}

PermString
EfontCFF::Font::glyph_name(int gid) const
{
    if (gid >= 0 && gid < nglyphs())
	return _cff->sid_permstring(_charset.gid_to_sid(gid));
    else
	return PermString();
}

void
EfontCFF::Font::glyph_names(Vector<PermString> &gnames) const
{
    gnames.resize(nglyphs());
    for (int i = 0; i < nglyphs(); i++)
	gnames[i] = _cff->sid_permstring(_charset.gid_to_sid(i));
}

Charstring *
EfontCFF::Font::glyph(int gid) const
{
    if (gid < 0 || gid >= nglyphs())
	return 0;
    if (!_charstrings_cs[gid])
	_charstrings_cs[gid] = charstring(_charstrings_index, gid);
    return _charstrings_cs[gid];
}

Charstring *
EfontCFF::Font::glyph(PermString name) const
{
    int gid = _charset.sid_to_gid(_cff->sid(name));
    if (gid < 0)
	return 0;
    if (!_charstrings_cs[gid])
	_charstrings_cs[gid] = charstring(_charstrings_index, gid);
    return _charstrings_cs[gid];	
}

Type1Encoding *
EfontCFF::Font::type1_encoding() const
{
    if (_encoding_pos == 0)
	return Type1Encoding::standard_encoding();
    if (!_t1encoding)
	_t1encoding = type1_encoding_copy();
    return _t1encoding;
}

Type1Encoding *
EfontCFF::Font::type1_encoding_copy() const
{
    if (_encoding_pos == 0)
	return Type1Encoding::standard_encoding();
    Type1Encoding *e = new Type1Encoding;
    for (int i = 0; i < 256; i++)
	if (_encoding[i])
	    e->put(i, _cff->sid_permstring(_charset.gid_to_sid(_encoding[i])));
    return e;
}

double
EfontCFF::Font::global_width_x(bool is_nominal) const
{
    return (is_nominal ? _nominal_width_x : _default_width_x);
}

bool
EfontCFF::Font::dict_has(DictOperator op) const
{
    return dict_of(op).has(op);
}

String
EfontCFF::Font::dict_string(DictOperator op) const
{
    Vector<double> vec;
    dict_of(op).value(op, vec);
    if (vec.size() == 1 && vec[0] >= 0 && vec[0] <= _cff->max_sid())
	return _cff->sid_string((int) vec[0]);
    else
	return String();
}