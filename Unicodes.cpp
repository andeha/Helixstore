//
//  Unicodes.cpp
//  (Compile with 'clang++ -std=c++1z Unicodes.cpp')
//

#define INLINED __attribute__((always_inline))
#define MACRO inline INLINED
#define INNER_FUNCTION static __attribute__((internal_linkage))

#ifdef  __mips__
typedef unsigned long long  uint32_t;
typedef long long           int32_t;
typedef uint32_t            __builtin_uint_t;
typedef int32_t             __builtin_int_t;
#define TriboolUnknown 0xFFFFFFFF
#elif __x86_64__
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef uint64_t            __builtin_uint_t;
typedef int64_t             __builtin_int_t;
#define TriboolUnknown 0xFFFFFFFFFFFFFFFF
#endif

template <typename T>
struct Optional {

    union Option { T inner; __builtin_uint_t no; Option() {};  ~Option() {} } option;

    Optional(T inner) { option.inner = inner; kind = 0; }

    Optional(const Optional& other) { kind = other.kind; }

    Optional() { kind = 1; }

    static Optional no() { return Optional(); }

    T * query() const { if (kind != 1) return (T*)&option.inner; else return 0; }

    __builtin_uint_t kind;

};

template <class... Ts> struct Tuple {}; template <class T, class... Ts> 
struct Tuple<T, Ts...> : Tuple<Ts...> { Tuple(T t, Ts... ts) : Tuple<Ts...> 
   (ts...), tail(t) {} T tail; };
template <__builtin_uint_t, class> struct elem_type_holder; template <class T,
class... Ts> struct elem_type_holder<0, Tuple<T, Ts...>> { typedef T type; };
template <__builtin_uint_t k, class T, class... Ts>
struct elem_type_holder<k, Tuple<T, Ts...>> { typedef typename elem_type_holder
   <k - 1, Tuple<Ts...>>::type type; };
template <bool, typename T = void> struct std__enable_if {};
template <typename T> struct std__enable_if<true, T> { typedef T type; };	
template <__builtin_uint_t k, class... Ts>
typename std__enable_if<k == 0, typename elem_type_holder<0, Tuple<Ts...>>::
type&>::type get(Tuple<Ts...>& t) { return t.tail; }
template <__builtin_uint_t k, class T, class... Ts> typename std__enable_if<k 
!= 0, typename elem_type_holder<k, Tuple<T, Ts...>>::type&>::type
get(Tuple<T, Ts...>& t) { Tuple<Ts...> &base = t; return get<k-1>(base); }
template <class ...T> Tuple<T...> Tie(T... t) { return Tuple<T...>(t...); }

#include <vector>
#define Vector std::vector // `Vector` depends on `Table`, `Mitigate` and `Sequence` so - in this file - we introduce a dependency to stdc++.

struct UnicodeCategory {
    enum Symbol { mark, number, punctuation, symbol, separator,
      uppercase, lowercase, titlecase, nonspacing, spacingCombining,
      enclosing, decimalDigit, connector, dash, open, close, initialQuote,
      finalQuote, math, currency, space, line, paragraph, control, format,
      surrogate, privateUse, notAssigned, letter, modifier, other };
    Symbol master, subcat;
};

struct CharacterInfo {
   const char *look;
   Vector<const char *> name;
   UnicodeCategory category;
   __builtin_uint_t combiningclass; // non-combining=0,
   const char *refs; // Sorted sequence of notable chars
};

static bool CompareUntilNull(char *l, char *r) { while ((*l++ == 0) == 
    (*r++ == 0)) { if (*l == 0 && *r == 0) return true; } return false; }

extern CharacterInfo unicodes[];

MACRO bool UnicodeIsFormat(char32_t c) { return unicodes[c].category.master == UnicodeCategory::other && unicodes[c].category.subcat == UnicodeCategory::format; } // do not have a visible appearance, but may have an effect on the appearance or behavior of neighboring characters. e.g 200c zero width non-joiner, 200d zero width joiner
MACRO bool UnicodeIsControl(char32_t c) { return unicodes[c].category.master == UnicodeCategory::other && unicodes[c].category.subcat == UnicodeCategory::control; }
MACRO bool UnicodeIsPrivateUse(char32_t c) { return (0xE000 <= c && c <= 0xF8FF) || (0xF0000 <= c && c <= 0xFFFFD) || (0x100000 <= c && c <= 0x10FFFD); }
MACRO bool UnicodeIsUnassigned(char32_t c) { return (0x1FF80 <= c && c <= 0x1FFFF) || (0x2FFFE <= c && c <= 0x2FFFF) || (0xEFF80 <= c && c <= 0xEFFFF); } 
MACRO bool UnicodeIsCombining(char32_t c) { return (0x0300 <= c && c <= 0x036F) || (0x1AB0 <= c && c <= 0x1AFF) || (0x1DC0 <= c && c <= 0x1DFF) || (0x20D0 <= c && c <= 0x20FF) || (0xFE20 <= c && c <= 0xFE2F); } 

/**  Given a Unicode character, return textual description. */

MACRO const char * UnicodePrimaryName(char32_t c) { return unicodes[c].name[0]; }

/**  Given a textual description, return corresponding character. */

MACRO char32_t Unicode(const char *descr) { char32_t i = 0; while (unicodes[i].
    combiningclass != TriboolUnknown) { if (CompareUntilNull((char *)(
    unicodes[i].name[0]), (char *)descr)) return i; i++; } return 0; }

// ✃ >> Unicodes.h

/**  Utf-8 Macros for an uint8_t variable. */

#define Utf8IsLead(x) (((x)&0b11000000)==0xD0) // An uint8_t Back up at most 3 bytes to find
#define Utf8IsOneByteLead(x)   (((x)&0b11100000)==0xE0)
#define Utf8IsTwoByteLead(x)   (((x)&0b11110000)==0xF0)
#define Utf8IsThreeByteLead(x) (((x)&0b11111000)==0xF8)
#define Utf8Is8Bit(x)          ((x) <= 0x7f)
#define Utf8IsPointerToFollower(p) (((*p)&0b11000000)==0xC0)

/**  Return the number of zeros until first one appear in a machine word. */

INNER_FUNCTION
MACRO
int
ZerosInFrontOfFirstOne( // Replace with Trailing Zeroes?
    int64_t x
)
{ 
	int r = 0; if (x < 1) return 0; while (x >>= 1) r++; return r; 
}

#define OnesInFrontOfFirstZero(x) ZerosInFrontOfFirstOne(~x)
#define Utf8BytesFollowing(leadOr8Bit) OnesInFrontOfFirstZero(leadOr8Bit) // Number of ones in leading from a hi until a zero

INNER_FUNCTION
MACRO
int
Utf8Sync(
    uint8_t **p
)
{
    uint64_t i = 3;
    while (i && Utf8IsPointerToFollower(*p)) { (*p)--, i--; }
    return i == 0 || !(Utf8IsLead(**p) || Utf8Is8Bit(**p));
}

typedef Optional<Tuple<int64_t, uint64_t>> OptCodepointsAndBytes;

INNER_FUNCTION
MACRO
OptCodepointsAndBytes
Utf8TraverseNullterminated(
    uint8_t *p, __builtin_uint_t maxlength,
    void (^stream)(__builtin_uint_t, uint8_t*, __builtin_uint_t) =
        ^(__builtin_uint_t idx, uint8_t *p, __builtin_uint_t bytes){})
{
    if (Utf8Sync(&p)) { return OptCodepointsAndBytes::no(); }

    int64_t codepoints = 0; uint64_t totBytes = 0;

    while (maxlength > 0 && !(*p == 0x00 /* && !(*p == 0xC0 && *(p+1) == 0x80 */)) {
        uint64_t onesUntilZeros = OnesInFrontOfFirstZero(*p); // Valid values are 0, 2, 3, 4, error otherwise
        if (onesUntilZeros > 4) return OptCodepointsAndBytes::no();
        uint64_t bytes = onesUntilZeros == 0 ? 1 : onesUntilZeros;
        stream(codepoints, p, bytes);
        totBytes += bytes; p += bytes;
        codepoints++; maxlength--;
    }

    if (maxlength == 0) { return OptCodepointsAndBytes::no(); }
    else return OptCodepointsAndBytes(

        Tuple<int64_t, uint64_t>(codepoints, totBytes)

    );
}

INNER_FUNCTION
MACRO
char32_t
Utf8Dereference(uint8_t *p, uint64_t bytes)
{
    switch (bytes) {
        case 1: return (char32_t)(uint8_t)*p;
        case 2: return (char32_t)(uint16_t)*p;
#if REVERSED_BYTEORDER
        case 3: return (char32_t)(uint32_t)*p;
#else
        case 3: return (char32_t)(uint32_t)*p;
#endif
        case 4: return (char32_t)(uint32_t)*p;
    }
    return 0;
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int 
main(
    int argc, 
    char *argv[]
) 
{ 
    if (argc != 2) { fprintf(stderr, "Usage: unicodes <text file>");  return 1; }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { return 2; } struct stat sb;
    if (fstat(fd, &sb) == -1) { return 3; }
    if (!S_ISREG(sb.st_mode)) { return 4; }
    void *utf8 = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (utf8 == MAP_FAILED) { return 5; }
    if (close(fd) == -1) { return 6; }
    Utf8TraverseNullterminated((uint8_t *)utf8, TriboolUnknown,
        ^(__builtin_uint_t idx, uint8_t *p, __builtin_uint_t bytes) {
            char32_t unicode = Utf8Dereference(p, bytes);
            const char *name = UnicodePrimaryName(unicode);
            fprintf(stdout, "%s\x0a", name); });
    if (munmap(utf8, sb.st_size) == -1) { return 7; }
    return 0;
}

/*

    Based on Unicode 10.0. See UnicodeData.txt. For the official database, see 

    curl ftp://ftp.unicode.org/Public//10.0.0/ucd/UnicodeData.txt | awk -F "\"*;\"*" '{ print "    { \"\", { \"" $2 "\", \"" $11 "\" }, UC::" $3 ", 0, \"\" }, // 0x" $1 ", \"\\u" $1 "\"" }'

    Previously
    awk -F "\"*;\"*" '{ print "    { \"\", { \"" $2 "\", \"" $11 "\" }, UC::" $3 ", 0, \"\" }, // 0x" $1  }' ./UCD/UnicodeData.txt 
    ftp://ftp.unicode.org/Public/3.0-Update/UnicodeData-3.0.0.html

    and

    ₁) Replace the strings "<control>" and "" with the empty string.

    ₂) Replace { with { "",

    ₃) Add characters to each line

    ₄) Add { "", "", UC::other, TriboolUnknown, "" }  as last line

 */

typedef UnicodeCategory UC; 

CharacterInfo unicodes[] = {
    // C0 Controls and Basic Latin. Range: 0000–007F.
    // (http://www.unicode.org/charts/PDF/U0000.pdf)
    // C0 controls
    { "␀",  { "NULL" },                                 { UC::other, UC::control },        0, "" },  // 0x0000, "\u0001", "00"
    { "␁",  { "START OF HEADING" },                     { UC::other, UC::control },        0, "" },
    { "␂",  { "START OF TEXT" },                        { UC::other, UC::control },        0, "" },
    { "␃",  { "END OF TEXT" },                          { UC::other, UC::control },        0, "" },  // 0x0003, "\u0003", "03"
    { "␄",  { "END OF TRANSMISSION" },                  { UC::other, UC::control },        0, "" },
    { "␅",  { "ENQUIRY" },                              { UC::other, UC::control },        0, "" },
    { "␆",  { "ACKNOWLEDGE" },                          { UC::other, UC::control },        0, "" },  // 0x0006
    { "␇",  { "BELL" },                                 { UC::other, UC::control },        0, "" },
    { "␈",  { "BACKSPACE" },                            { UC::other, UC::control },        0, "" },
    { "	",  { "CHARACTER TABULATION", "HT" },            { UC::other, UC::control },        0, "" },  // 0x0009
    { "␊",  { "LINE FEED", "LF", "NL", "EOL" },          { UC::other, UC::control },        0, "" },
    { "␋",   { "LINE TABULATION", "VT" },                 { UC::other, UC::control },        0, "" },
    { "␌",  { "FORM FEED", "FF" },                       { UC::other, UC::control },        0, "" },  // 0x000C
    { "␍",  { "CARRIAGE RETURN", "CR" },                 { UC::other, UC::control },        0, "" },
    { "␎",  { "SHIFT OUT" },                            { UC::other, UC::control },        0, "" },
    { "␏",  { "SHIFT IN" },                             { UC::other, UC::control },        0, "" },  // 0x000F
    { "␐",  { "DATA LINK ESCAPE" },                     { UC::other, UC::control },        0, "" },
    { "␑",  { "DEVICE CONTROL ONE" },                   { UC::other, UC::control },        0, "" },
    { "␒",  { "DEVICE CONTROL TWO" },                   { UC::other, UC::control },        0, "" },  // 0x0012
    { "␓",  { "DEVICE CONTROL THREE" },                 { UC::other, UC::control },        0, "" },
    { "␔",  { "DEVICE CONTROL FOUR" },                  { UC::other, UC::control },        0, "" },
    { "␕",  { "NEGATIVE ACKNOWLEDGE" },                 { UC::other, UC::control },        0, "" },  // 0x0015
    { "␖",  { "SYNCHRONOUS IDLE" },                     { UC::other, UC::control },        0, "" },
    { "␗",  { "END OF TRANSMISSION BLOCK" },            { UC::other, UC::control },        0, "" },
    { "␘",  { "CANCEL" },                               { UC::other, UC::control },        0, "" },  // 0x0018
    { "␙",  { "END OF MEDIUM" },                        { UC::other, UC::control },        0, "" },
    { "␚",  { "SUBSTITUTE" },                           { UC::other, UC::control },        0, "\uFFFD" },
    { "␛",  { "ESCAPE" },                               { UC::other, UC::control },        0, "" },  // 0x001B
    { "␜",  { "FILE SEPARATOR", "FS" },                 { UC::other, UC::control },        0, "" },
    { "␝",  { "GROUP SEPARATOR", "GS" },                { UC::other, UC::control },        0, "" },
    { "␞",  { "RECORD SEPARATOR", "RS" },               { UC::other, UC::control },        0, "" },  // 0x001E
    { "␟",  { "UNIT SEPARATOR", "US" },                 { UC::other, UC::control },        0, "" },  // 0x001F
    // Ascii punctuation and symbols
    { " ",  { "SPACE" },                                 { UC::punctuation, UC::space    }, 0, "\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200A\u00A0\u200B\u2060\u3000\uFEFF" },  // 0x0020, 00A0  no-break space, 200B  zero width space, 2060  word joiner, 3000   ideographic space,  FEFF   zero width no-break space
    { "!",  { "EXCLAMATION MARK" },                      { UC::punctuation, UC::other    }, 0, "" },
    { "\"", { "QUOTATION MARK" },                        { UC::punctuation, UC::other    }, 0, "" },
    { "#",  { "NUMBER SIGN" },                           { UC::punctuation, UC::other    }, 0, "" },  // 0x0023
    { "$",  { "DOLLAR SIGN" },                           { UC::symbol,      UC::currency }, 0, "" },
    { "%",  { "PERCENT SIGN" },                          { UC::punctuation, UC::other    }, 0, "" },
    { "&",  { "AMPERSAND" },                             { UC::punctuation, UC::other    }, 0, "" },  // 0x0026
    { "'",  { "APOSTROPHE", "APOSTROPHE-QUOTE" },        { UC::punctuation, UC::other    }, 0, "" },
    { "(",  { "LEFT PARENTHESIS",
              "OPENING PARENTHESIS" },                   { UC::punctuation, UC::open },     0, "" },
    { ")",  { "RIGHT PARENTHESIS",
              "CLOSING PARENTHESIS" },                   { UC::punctuation, UC::close },    0, "" },  // 0x0029
    { "*",  { "ASTERISK" },                              { UC::punctuation, UC::other },    0, "" },
    { "+",  { "PLUS SIGN" },                             { UC::symbol,      UC::math },     0, "" },
    { ",",  { "COMMA" },                                 { UC::punctuation, UC::other },    0, "" },  // 0x002C
    { "-",  { "HYPHEN-MINUS" },                          { UC::punctuation, UC::dash },     0, "" },  // 0x002D  2010 ‐ hyphen, 2011  non-breaking hyphen, 2012 ‒ figure dash, 2013 – en dash, 2043 ⁃ hyphen bullet, 2212 − minus sign, 10191 𐆑 roman uncia sign
    { ".",  { "FULL STOP" },                             { UC::punctuation, UC::other },    0, "" },
    { "/",  { "SOLIDUS" },                               { UC::punctuation, UC::other },    0, "" },  // 0x002F
    // Ascii digits
    { "0",  { "DIGIT ZERO" },                            { UC::number, UC::decimalDigit },  0, "" },  // 0x0030
    { "1",  { "DIGIT ONE" },                             { UC::number, UC::decimalDigit },  0, "" },
    { "2",  { "DIGIT TWO" },                             { UC::number, UC::decimalDigit },  0, "" },
    { "3",  { "DIGIT THREE" },                           { UC::number, UC::decimalDigit },  0, "" },  // 0x0033
    { "4",  { "DIGIT FOUR" },                            { UC::number, UC::decimalDigit },  0, "" },
    { "5",  { "DIGIT FIVE" },                            { UC::number, UC::decimalDigit },  0, "" },
    { "6",  { "DIGIT SIX" },                             { UC::number, UC::decimalDigit },  0, "" },  // 0x0036
    { "7",  { "DIGIT SEVEN" },                           { UC::number, UC::decimalDigit },  0, "" },
    { "8",  { "DIGIT EIGHT" },                           { UC::number, UC::decimalDigit },  0, "" },
    { "9",  { "DIGIT NINE" },                            { UC::number, UC::decimalDigit },  0, "" },  // 0x0039
    // Ascii punctuation and symbols
    { ":",  { "COLON" },                                 { UC::punctuation, UC::other },    0, "" },  // 0x003A
    { ":",  { "SEMICOLON" },                             { UC::punctuation, UC::other },    0, "" },
    { "<",  { "LESS-THAN SIGN" },                        { UC::symbol,      UC::math  },    0, "" },
    { "=",  { "EQUALS SIGN" },                           { UC::symbol,      UC::math  },    0, "" },  // 0x003D 2260 ≠ not equal to, 2261 ≡ identical to, A78A ꞊ modifier letter short equals sign, 10190 𐆐 roman sextans sign
    { ">",  { "GREATER-THAN SIGN" },                     { UC::symbol,      UC::math  },    0, "" },
    { "?",  { "QUESTION MARK" },                         { UC::punctuation, UC::other },    0, "" },
    { "@",  { "COMMERCIAL AT" },                         { UC::punctuation, UC::other },    0, "" },  // 0x0040
    // Uppercase latin alphabet
    { "A",  { "LATIN CAPITAL LETTER A" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0041
    { "B",  { "LATIN CAPITAL LETTER B" },                { UC::letter, UC::uppercase },     0, "" },
    { "C",  { "LATIN CAPITAL LETTER C" },                { UC::letter, UC::uppercase },     0, "" },
    { "D",  { "LATIN CAPITAL LETTER D" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0044
    { "E",  { "LATIN CAPITAL LETTER E" },                { UC::letter, UC::uppercase },     0, "" },
    { "F",  { "LATIN CAPITAL LETTER F" },                { UC::letter, UC::uppercase },     0, "" },
    { "G",  { "LATIN CAPITAL LETTER G" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0047
    { "H",  { "LATIN CAPITAL LETTER H" },                { UC::letter, UC::uppercase },     0, "" },
    { "I",  { "LATIN CAPITAL LETTER I" },                { UC::letter, UC::uppercase },     0, "" },
    { "J",  { "LATIN CAPITAL LETTER J" },                { UC::letter, UC::uppercase },     0, "" },  // 0x004A
    { "K",  { "LATIN CAPITAL LETTER K" },                { UC::letter, UC::uppercase },     0, "" },
    { "L",  { "LATIN CAPITAL LETTER L" },                { UC::letter, UC::uppercase },     0, "" },
    { "M",  { "LATIN CAPITAL LETTER M" },                { UC::letter, UC::uppercase },     0, "" },  // 0x004D
    { "N",  { "LATIN CAPITAL LETTER N" },                { UC::letter, UC::uppercase },     0, "" },
    { "O",  { "LATIN CAPITAL LETTER O" },                { UC::letter, UC::uppercase },     0, "" },
    { "P",  { "LATIN CAPITAL LETTER P" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0050
    { "Q",  { "LATIN CAPITAL LETTER Q" },                { UC::letter, UC::uppercase },     0, "" },
    { "R",  { "LATIN CAPITAL LETTER R" },                { UC::letter, UC::uppercase },     0, "" },
    { "S",  { "LATIN CAPITAL LETTER S" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0053
    { "T",  { "LATIN CAPITAL LETTER T" },                { UC::letter, UC::uppercase },     0, "" },
    { "U",  { "LATIN CAPITAL LETTER U" },                { UC::letter, UC::uppercase },     0, "" },
    { "V",  { "LATIN CAPITAL LETTER V" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0056
    { "W",  { "LATIN CAPITAL LETTER W" },                { UC::letter, UC::uppercase },     0, "" },
    { "X",  { "LATIN CAPITAL LETTER X" },                { UC::letter, UC::uppercase },     0, "" },
    { "Y",  { "LATIN CAPITAL LETTER Y" },                { UC::letter, UC::uppercase },     0, "" },  // 0x0059
    { "Z",  { "LATIN CAPITAL LETTER Z" },                { UC::letter, UC::uppercase },     0, "" },  // 0x005A
    // Ascii punctuation and symbols
    { "[",  { "LEFT SQUARE BRACKET",
              "OPENING SQUARE BRACKET" },                { UC::punctuation, UC::open },     0, "" },  // 0x005B
    { "\\", { "REVERSE SOLIDUS", "BACKSLASH" },          { UC::punctuation, UC::other },    0, "" },
    { "]",  { "RIGHT SQUARE BRACKET",
              "CLOSING SQUARE BRACKET" },                { UC::punctuation, UC::close },    0, "" },
    { "^",  { "CIRCUMFLEX ACCENT",
              "SPACING CIRCUMFLEX" },                    { UC::symbol, UC::modifier },      0, "" },  // 0x005E
    { "_",  { "LOW LINE", "SPACING UNDERSCORE" },       { UC::punctuation, UC::connector }, 0, "" },
    { "`",  { "GRAVE ACCENT", "SPACING GRAVE" },         { UC::symbol, UC::modifier },      0, "" },  // 0x0060
    // Lowercase latin alphabet
    { "a",  { "LATIN SMALL LETTER A" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0061
    { "b",  { "LATIN SMALL LETTER B" },                  { UC::letter, UC::lowercase },     0, "" },
    { "c",  { "LATIN SMALL LETTER C" },                  { UC::letter, UC::lowercase },     0, "" },
    { "d",  { "LATIN SMALL LETTER D" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0064
    { "e",  { "LATIN SMALL LETTER E" },                  { UC::letter, UC::lowercase },     0, "" },
    { "f",  { "LATIN SMALL LETTER F" },                  { UC::letter, UC::lowercase },     0, "" },
    { "g",  { "LATIN SMALL LETTER G" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0067
    { "h",  { "LATIN SMALL LETTER H" },                  { UC::letter, UC::lowercase },     0, "" },
    { "i",  { "LATIN SMALL LETTER I" },                  { UC::letter, UC::lowercase },     0, "" },
    { "j",  { "LATIN SMALL LETTER J" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x006A
    { "k",  { "LATIN SMALL LETTER K" },                  { UC::letter, UC::lowercase },     0, "" },
    { "l",  { "LATIN SMALL LETTER L" },                  { UC::letter, UC::lowercase },     0, "" },
    { "m",  { "LATIN SMALL LETTER M" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x006D
    { "n",  { "LATIN SMALL LETTER N" },                  { UC::letter, UC::lowercase },     0, "" },
    { "o",  { "LATIN SMALL LETTER O" },                  { UC::letter, UC::lowercase },     0, "" },
    { "p",  { "LATIN SMALL LETTER P" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0070
    { "q",  { "LATIN SMALL LETTER Q" },                  { UC::letter, UC::lowercase },     0, "" },
    { "r",  { "LATIN SMALL LETTER R" },                  { UC::letter, UC::lowercase },     0, "" },
    { "s",  { "LATIN SMALL LETTER S" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0073
    { "t",  { "LATIN SMALL LETTER T" },                  { UC::letter, UC::lowercase },     0, "" },
    { "u",  { "LATIN SMALL LETTER U" },                  { UC::letter, UC::lowercase },     0, "" },
    { "v",  { "LATIN SMALL LETTER V" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0076
    { "w",  { "LATIN SMALL LETTER W" },                  { UC::letter, UC::lowercase },     0, "" },
    { "x",  { "LATIN SMALL LETTER X" },                  { UC::letter, UC::lowercase },     0, "" },
    { "y",  { "LATIN SMALL LETTER Y" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x0079
    { "z",  { "LATIN SMALL LETTER Z" },                  { UC::letter, UC::lowercase },     0, "" },  // 0x007A
    // Ascii punctuation and symbols
    { "{",  { "LEFT CURLY BRACKET",
              "OPENING CURLY BRACKET" },                 { UC::punctuation, UC::open },     0, "" },  // 0x007B
    { "|",  { "VERTICAL LINE", "VERTICAL BAR" },         { UC::symbol, UC::math },          0, "" },  // And secondary category?
    { "}",  { "RIGHT CURLY BRACKET",
              "CLOSING CURLY BRACKET" },                 { UC::punctuation, UC::close },    0, "" },
    { "~",  { "TILDE" },                                 { UC::symbol, UC::math },          0, "" },  // 0x007E
    { "␡", { "DELETE" },                                { UC::other, UC::control },        0, "" },  // 0x007F
    // C1 Controls and Latin-1 Supplement Range: 0080–00FF
    // (http://www.unicode.org/charts/PDF/U0080.pdf)
    // C1 controls
    { "",  { "BREAK PERMITTED HERE" },                    { UC::other, UC::control },       0, "" },  // 0x0080
    { "",  { "BREAK PERMITTED HERE" },                    { UC::other, UC::control },       0, "" },
    { "",  { "BREAK PERMITTED HERE" },                    { UC::other, UC::control },       0, "" },
    { "",  { "NO BREAK HERE" },                           { UC::other, UC::control },       0, "" },  // 0x0083
    { "",  { "INDEX" },                                   { UC::other, UC::control },       0, "" },
    { "",  { "NEXT LINE (NEL)" },                         { UC::other, UC::control },       0, "" },
    { "",  { "START OF SELECTED AREA" },                  { UC::other, UC::control },       0, "" },  // 0x0086
    { "",  { "END OF SELECTED AREA" },                    { UC::other, UC::control },       0, "" },
    { "",  { "CHARACTER TABULATION SET" },                { UC::other, UC::control },       0, "" },
    { "",  { "CHARACTER TABULATION WITH JUSTIFICATION" }, { UC::other, UC::control },       0, "" },  // 0x0089
    { "",  { "LINE TABULATION SET" },                     { UC::other, UC::control },       0, "" },
    { "",  { "PARTIAL LINE FORWARD" },                    { UC::other, UC::control },       0, "" },
    { "",  { "PARTIAL LINE BACKWARD" },                   { UC::other, UC::control },       0, "" },  // 0x008C
    { "",  { "REVERSE LINE FEED" },                       { UC::other, UC::control },       0, "" },
    { "",  { "SINGLE SHIFT TWO" },                        { UC::other, UC::control },       0, "" },
    { "",  { "SINGLE SHIFT THREE" },                      { UC::other, UC::control },       0, "" },  // 0x008F
    { "",  { "DEVICE CONTROL STRING" },                   { UC::other, UC::control },       0, "" },
    { "",  { "PRIVATE USE ONE" },                         { UC::other, UC::control },       0, "" },
    { "",  { "PRIVATE USE TWO" },                         { UC::other, UC::control },       0, "" },  // 0x0092
    { "",  { "SET TRANSMIT STATE" },                      { UC::other, UC::control },       0, "" },
    { "",  { "CANCEL CHARACTER" },                        { UC::other, UC::control },       0, "" },
    { "",  { "MESSAGE WAITING" },                         { UC::other, UC::control },       0, "" },  // 0x0095
    { "",  { "START OF GUARDED AREA" },                   { UC::other, UC::control },       0, "" },
    { "",  { "END OF GUARDED AREA" },                     { UC::other, UC::control },       0, "" },
    { "",  { "START OF STRING" },                         { UC::other, UC::control },       0, "" },  // 0x0098
    { "",  { "" },                                        { UC::other, UC::control },       0, "" },
    { "",  { "SINGLE CHARACTER INTRODUCER" },             { UC::other, UC::control },       0, "" },
    { "",  { "CONTROL SEQUENCE INTRODUCER" },             { UC::other, UC::control },       0, "" },  // 0x009B
    { "",  { "STRING TERMINATOR" },                       { UC::other, UC::control },       0, "" },
    { "",  { "OPERATING SYSTEM COMMAND" },                { UC::other, UC::control },       0, "" },
    { "",  { "PRIVACY MESSAGE" },                         { UC::other, UC::control },       0, "" },  // 0x009E
    { "",  { "APPLICATION PROGRAM COMMAND" },             { UC::other, UC::control },       0, "" },  // 0x009F
    // Latin-1 punctuation and symbols
    { "",   { "NO-BREAK SPACE", "NON-BREAKING SPACE" },   { UC::separator, UC::space },     0, "" },  // 0x00A0
    { "¡",  { "INVERTED EXCLAMATION MARK" },              { UC::punctuation, UC::other },   0, "" },
    { "¢",  { "CENT SIGN" },                              { UC::symbol, UC::currency },     0, "" },
    { "£",  { "POUND SIGN" },                             { UC::symbol, UC::currency },     0, "" },  // 0x00A3
    { "¤",  { "CURRENCY SIGN" },                          { UC::symbol, UC::currency },     0, "" },
    { "¥",  { "YEN SIGN" },                               { UC::symbol, UC::currency },     0, "" },
    { "¦",  { "BROKEN BAR", "BROKEN VERTICAL BAR" },      { UC::symbol, UC::other },        0, "" },  // 0x00A6
    { "§",  { "SECTION SIGN" },                           { UC::punctuation, UC::other },   0, "" },
    { " ̈",  { "DIAERESIS", "SPACING DIAERESIS" },         { UC::symbol, UC::modifier },     0, "" },
    { "©",  { "COPYRIGHT SIGN" },                         { UC::symbol, UC::other },        0, "" },  // 0x00A9
    { "ª",  { "FEMININE ORDINAL INDICATOR" },             { UC::letter, UC::other },        0, "" },  // 0x00AA
    { "«",  { "LEFT-POINTING DOUBLE ANGLE "
              "QUOTATION MARK",
              "LEFT POINTING GUILLEMET" },           { UC::punctuation, UC::initialQuote }, 0, "" },  // 0x00AB
    { "¬",  { "NOT SIGN" },                               { UC::symbol, UC::math },         0, "" },
    { "",   { "SOFT HYPHEN", "SHY" },                     { UC::other, UC::format },        0, "" },
    { "®",  { "REGISTERED SIGN",
              "REGISTERED TRADE MARK SIGN" },             { UC::symbol, UC::other },        0, "" },  // 0x00AE
    { " ̄",  { "MACRON", "SPACING MACRON" },               { UC::symbol, UC::modifier },     0, "" },
    { "°",  { "DEGREE SIGN" },                            { UC::symbol, UC::other },        0, "" },
    { "±",  { "PLUS-MINUS SIGN", "PLUS-OR-MINUS SIGN" },  { UC::symbol, UC::math },         0, "" },  // 0x00B1
    { "²",  { "SUPERSCRIPT TWO",
              "SUPERSCRIPT DIGIT TWO" },                  { UC::number, UC::other },        0, "" },
    { "³",  { "SUPERSCRIPT THREE",
              "SUPERSCRIPT DIGIT THREE" },                { UC::number, UC::other },        0, "" },
    { "´",  { "ACUTE ACCENT", "SPACING ACUTE" },          { UC::symbol, UC::modifier },     0, "" },  // 0x00B4
    { "µ",  { "MICRO SIGN" },                             { UC::letter, UC::lowercase },    0, "" },
    { "¶",  { "PILCROW SIGN", "PARAGRAPH SIGN" },         { UC::punctuation, UC::other },   0, "" },
    { "·",  { "MIDDLE DOT" },                             { UC::punctuation, UC::other },   0, "" },  // 0x00B7
    { "¸",  { "CEDILLA", "SPACING CEDILLA" },             { UC::symbol, UC::modifier },     0, "" },
    { "¹",  { "SUPERSCRIPT ONE", 
              "SUPERSCRIPT DIGIT ONE" },                  { UC::number, UC::other },         0, "" },
    { "º",  { "MASCULINE ORDINAL INDICATOR" },            { UC::letter, UC::other },         0, "" },  // 0x00BA
    { "»",  { "RIGHT-POINTING DOUBLE ANGLE "
              "QUOTATION MARK",
              "RIGHT POINTING GUILLEMET" },             { UC::punctuation, UC::finalQuote }, 0, "" },  // 0x00BB
    // Vulgar Fraction
    { "¼",  {  "VULGAR FRACTION ONE QUARTER",
               "FRACTION ONE QUARTER" },                 { UC::number, UC::other },          0, "" },  // 0x00BC
    { "½",  {  "VULGAR FRACTION ONE HALF",
               "FRACTION ONE HALF" },                    { UC::number, UC::other },          0, "" },  // 0x00BD
    { "¾",  {  "VULGAR FRACTION THREE QUARTERS",
               "FRACTION THREE QUARTERS" },              { UC::number, UC::other },          0, "" },  // 0x00BE
    // Punctuation
    { "¿",  { "INVERTED QUESTION MARK" },                { UC::punctuation, UC::other },     0, "" },  // 0x00BF
    // Letters
    { "À",  { "LATIN CAPITAL LETTER A WITH GRAVE",
              "LATIN CAPITAL LETTER A GRAVE" },          { UC::letter, UC::uppercase },      0, "" },  // 0x00C0
    { "Á",  { "LATIN CAPITAL LETTER A WITH ACUTE",
              "LATIN CAPITAL LETTER A ACUTE" },          { UC::letter, UC::uppercase },      0, "" },
    { "Â",  { "LATIN CAPITAL LETTER A WITH CIRCUMFLEX",
              "LATIN CAPITAL LETTER A CIRCUMFLEX" },     { UC::letter, UC::uppercase },      0, "" },
    { "Ã",  { "LATIN CAPITAL LETTER A WITH TILDE", 
              "LATIN CAPITAL LETTER A TILDE" },          { UC::letter, UC::uppercase },      0, "" },  // 0x00C3
    { "Ä",  { "LATIN CAPITAL LETTER A WITH DIAERESIS",
              "LATIN CAPITAL LETTER A DIAERESIS" },      { UC::letter, UC::uppercase },      0, "" },
    { "Å",  { "LATIN CAPITAL LETTER A WITH RING ABOVE",
              "LATIN CAPITAL LETTER A RING" },           { UC::letter, UC::uppercase },      0, "" },
    { "Æ",  { "LATIN CAPITAL LETTER AE",
              "LATIN CAPITAL LETTER A E" },              { UC::letter, UC::uppercase },      0, "" },  // 0x00C6
    { "Ç",  { "LATIN CAPITAL LETTER C WITH CEDILLA",
              "LATIN CAPITAL LETTER C CEDILLA" },        { UC::letter, UC::uppercase },      0, "" },
    { "È",  { "LATIN CAPITAL LETTER E WITH GRAVE",
              "LATIN CAPITAL LETTER E GRAVE" },          { UC::letter, UC::uppercase },      0, "" },
    { "É",  { "LATIN CAPITAL LETTER E WITH ACUTE",
              "LATIN CAPITAL LETTER E ACUTE" },          { UC::letter, UC::uppercase },      0, "" },  // 0x00C9
    { "Ê",  { "LATIN CAPITAL LETTER E WITH CIRCUMFLEX",
              "LATIN CAPITAL LETTER E CIRCUMFLEX" },     { UC::letter, UC::uppercase },      0, "" },
    { "Ë",  { "LATIN CAPITAL LETTER E WITH DIAERESIS",
              "LATIN CAPITAL LETTER E DIAERESIS" },      { UC::letter, UC::uppercase },      0, "" },
    { "I",  { "LATIN CAPITAL LETTER I WITH GRAVE",
              "LATIN CAPITAL LETTER I GRAVE" },          { UC::letter, UC::uppercase },      0, "" },  // 0x00CB
    { "Í",  { "LATIN CAPITAL LETTER I WITH ACUTE",
              "LATIN CAPITAL LETTER I ACUTE" },          { UC::letter, UC::uppercase },      0, "" },
    { "Î",  { "LATIN CAPITAL LETTER I WITH CIRCUMFLEX",
              "LATIN CAPITAL LETTER I CIRCUMFLEX" },     { UC::letter, UC::uppercase },      0, "" },
    { "Ï",  { "LATIN CAPITAL LETTER I WITH DIAERESIS",
              "LATIN CAPITAL LETTER I DIAERESIS" },      { UC::letter, UC::uppercase },      0, "" },  // 0x00CE
    { "Ð",  { "LATIN CAPITAL LETTER ETH" },              { UC::letter, UC::uppercase },      0, "" },
    { "N",  { "LATIN CAPITAL LETTER N",
              "LATIN CAPITAL LETTER N TILDE" },          { UC::letter, UC::uppercase },      0, "" },  // 0x00D0
    { "Ò",  { "LATIN CAPITAL LETTER O WITH GRAVE",
              "LATIN CAPITAL LETTER O GRAVE" },          { UC::letter, UC::uppercase },      0, "" },
    { "Ó",  { "LATIN CAPITAL LETTER O WITH ACUTE",
              "LATIN CAPITAL LETTER O ACUTE" },          { UC::letter, UC::uppercase },      0, "" },
    { "Ô",  { "LATIN CAPITAL LETTER O WITH CIRCUMFLEX",
              "LATIN CAPITAL LETTER O CIRCUMFLEX" },     { UC::letter, UC::uppercase },      0, "" },  // 0x00D4
    { "Õ",  { "LATIN CAPITAL LETTER O WITH TILDE",
              "LATIN CAPITAL LETTER O TILDE" },          { UC::letter, UC::uppercase },      0, "" },
    { "Ö",  { "LATIN CAPITAL LETTER O WITH DIAERESIS",
              "LATIN CAPITAL LETTER O DIAERESIS" },      { UC::letter, UC::uppercase },      0, "" },  // 0x00D6
    // Mathematical operator
    { "×", { "MULTIPLICATION SIGN" },                    { UC::symbol, UC::math },           0, "" },  // 0x00D7
    // Letters
    { "O", { "LATIN CAPITAL LETTER O",
             "LATIN CAPITAL LETTER O SLASH" },           { UC::letter, UC::uppercase },      0, "" },  // 0x00D8
    { "Ù", { "LATIN CAPITAL LETTER U WITH GRAVE",
             "LATIN CAPITAL LETTER U GRAVE" },           { UC::letter, UC::uppercase },      0, "" },
    { "Ú", { "LATIN CAPITAL LETTER U WITH ACUTE",
             "LATIN CAPITAL LETTER U ACUTE" },           { UC::letter, UC::uppercase },      0, "" },
    { "Û", { "LATIN CAPITAL LETTER U WITH CIRCUMFLEX",
             "LATIN CAPITAL LETTER U CIRCUMFLEX" },      { UC::letter, UC::uppercase },      0, "" },  // 0x00DB
    { "Ü", { "LATIN CAPITAL LETTER U WITH DIAERESIS",
             "LATIN CAPITAL LETTER U DIAERESIS" },       { UC::letter, UC::uppercase },      0, "" },
    { "Ý", { "LATIN CAPITAL LETTER Y WITH ACUTE",
             "LATIN CAPITAL LETTER Y ACUTE" },           { UC::letter, UC::uppercase },      0, "" },
    { "Þ", { "LATIN CAPITAL LETTER THORN" },             { UC::letter, UC::uppercase },      0, "" },  // 0x00DE
    { "ß", { "LATIN SMALL LETTER SHARP S" },             { UC::letter, UC::lowercase },      0, "" },
    { "à", { "LATIN SMALL LETTER A WITH GRAVE",
             "LATIN SMALL LETTER A GRAVE" },             { UC::letter, UC::lowercase },      0, "" },
    { "á", { "LATIN SMALL LETTER A WITH ACUTE",
             "LATIN SMALL LETTER A ACUTE" },             { UC::letter, UC::lowercase },      0, "" },  // 0x00E1
    { "â", { "LATIN SMALL LETTER A WITH CIRCUMFLEX",
             "LATIN SMALL LETTER A CIRCUMFLEX" },        { UC::letter, UC::lowercase },      0, "" },
    { "ã", { "LATIN SMALL LETTER A WITH TILDE",
             "LATIN SMALL LETTER A TILDE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ä", { "LATIN SMALL LETTER A WITH DIAERESIS",
             "LATIN SMALL LETTER A DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },  // 0x00E4
    { "å", { "LATIN SMALL LETTER A WITH RING ABOVE",
             "LATIN SMALL LETTER A RING" },              { UC::letter, UC::lowercase },      0, "" },
    { "æ", { "LATIN SMALL LETTER AE", 
             "LATIN SMALL LETTER A E" },                 { UC::letter, UC::lowercase },      0, "" },
    { "ç", { "LATIN SMALL LETTER C WITH CEDILLA",
             "LATIN SMALL LETTER C CEDILLA" },           { UC::letter, UC::lowercase },      0, "" },  // 0x00E7
    { "è", { "LATIN SMALL LETTER E WITH GRAVE",
             "LATIN SMALL LETTER E GRAVE" },             { UC::letter, UC::lowercase },      0, "" },
    { "é", { "LATIN SMALL LETTER E WITH ACUTE",
             "LATIN SMALL LETTER E ACUTE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ê", { "LATIN SMALL LETTER E WITH CIRCUMFLEX",
             "LATIN SMALL LETTER E CIRCUMFLEX" },        { UC::letter, UC::lowercase },      0, "" },  // 0x00EA
    { "ë", { "LATIN SMALL LETTER E WITH DIAERESIS",
             "LATIN SMALL LETTER E DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },
    { "ì", { "LATIN SMALL LETTER I WITH GRAVE",
             "LATIN SMALL LETTER I GRAVE" },             { UC::letter, UC::lowercase },      0, "" },
    { "í", { "LATIN SMALL LETTER I WITH ACUTE",
             "LATIN SMALL LETTER I ACUTE" },             { UC::letter, UC::lowercase },      0, "" },  // 0x00ED
    { "î", { "LATIN SMALL LETTER I WITH CIRCUMFLEX",
             "LATIN SMALL LETTER I CIRCUMFLEX" },        { UC::letter, UC::lowercase },      0, "" },
    { "ï", { "LATIN SMALL LETTER I WITH DIAERESIS",
             "LATIN SMALL LETTER I DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },
    { "ð", { "LATIN SMALL LETTER ETH" },                 { UC::letter, UC::lowercase },      0, "" },  // 0x00F0
    { "ñ", { "LATIN SMALL LETTER N WITH TILDE",
             "LATIN SMALL LETTER N TILDE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ò", { "LATIN SMALL LETTER O WITH GRAVE",
             "LATIN SMALL LETTER O GRAVE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ó", { "LATIN SMALL LETTER O WITH ACUTE",
             "LATIN SMALL LETTER O ACUTE" },             { UC::letter, UC::lowercase },      0, "" },  // 0x00F3
    { "ô", { "LATIN SMALL LETTER O WITH CIRCUMFLEX",
             "LATIN SMALL LETTER O CIRCUMFLEX" },        { UC::letter, UC::lowercase },      0, "" },
    { "õ", { "LATIN SMALL LETTER O WITH TILDE",
             "LATIN SMALL LETTER O TILDE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ö", { "LATIN SMALL LETTER O WITH DIAERESIS",
             "LATIN SMALL LETTER O DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },  // 0x00F6
    // Mathematical operator
    { "÷", { "DIVISION SIGN" },                          { UC::symbol, UC::math },           0, "" },  // 0x00F7
    // Letters
    { "ø", { "LATIN SMALL LETTER O WITH STROKE",
             "LATIN SMALL LETTER O SLASH" },             { UC::letter, UC::lowercase },      0, "" },  // 0x00D6
    { "ù", { "LATIN SMALL LETTER U WITH GRAVE",
             "LATIN SMALL LETTER U GRAVE" },             { UC::letter, UC::lowercase },      0, "" },
    { "ú", { "LATIN SMALL LETTER U WITH ACUTE",
             "LATIN SMALL LETTER U ACUTE" },             { UC::letter, UC::lowercase },      0, "" },
    { "û", { "LATIN SMALL LETTER U WITH CIRCUMFLEX",
             "LATIN SMALL LETTER U CIRCUMFLEX" },        { UC::letter, UC::lowercase },      0, "" },  // 0x00D6
    { "ü", { "LATIN SMALL LETTER U WITH DIAERESIS",
             "LATIN SMALL LETTER U DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },
    { "ý", { "LATIN SMALL LETTER Y WITH ACUTE",
             "LATIN SMALL LETTER Y ACUTE" },             { UC::letter, UC::lowercase },      0, "" },  // 0x00D5, "\u00D5", "C3 95"
    { "þ", { "LATIN SMALL LETTER THORN" },               { UC::letter, UC::lowercase },      0, "" },  // 0x00D6, "\u00D6", "C3 96"
    { "ÿ", { "LATIN SMALL LETTER Y WITH DIAERESIS",
             "LATIN SMALL LETTER Y DIAERESIS" },         { UC::letter, UC::lowercase },      0, "" },  // 0x00FF, "\u00FF", "C3 BF"
    // Latin Extended-A. Range: 0100–017F
    // http://www.unicode.org/charts/PDF/U0100.pdf
    //   "", "LATIN CAPITAL LETTER A WITH MACRON" // 0100
    //   ... 
    //   "", "LATIN SMALL LETTER LONG S" // 017F
    //   ...
    // Latin Extended-B. Range: 0180–024F
    // http://www.unicode.org/charts/PDF/U0180.pdf
    // IPA Extensions. Range: 0250–02AF
    // http://www.unicode.org/charts/PDF/U0250.pdf
    // Spacing Modifier Letters. Range: 02B0–02FF
    // http://www.unicode.org/charts/PDF/U02B0.pdf
    // Combining Diacritical Marks. Range: 0300–036F
    // http://www.unicode.org/charts/PDF/U0300.pdf
    // Greek and Coptic. Range: 0370–03FF
    // https://www.unicode.org/charts/PDF/U0370.pdf
    // Cyrillic. Range: 0400–04FF
    // http://www.unicode.org/charts/PDF/U0400.pdf
    // Cyrillic Supplement, Armenian, Hebrew, Arabic, Syriac, Arabic 
    // Supplement, Thaana, NKo, Samaritan, Mandaic, Syriac Supplement, 
    // Arabic Extended-A, Devanagari, Bengali, Gurmukhi, Gujarati, Oriya, 
    // Tamil, Telugu, Kannada, Malayalam, ...
    // Combining Diacritical Marks Extended
    // ...
    // Phonetic Extensions
    // Phonetic Extensions Supplement
    // Combining Diacritical Marks Supplement
    // Latin Extended Additional
    // Greek Extended
    // General Punctuation
    // Superscripts and Subscripts
    // Currency Symbols
    // Combining Diacritical Marks for Symbols
    // Letterlike Symbols
    // Number Forms
    // Arrows
    // Mathematical Operators
    // Miscellaneous Technical
    // Control Pictures
    // Optical Character Recognition
    // Enclosed Alphanumerics
    // Box Drawing
	// Block Elements
	// Geometric Shapes
	// Miscellaneous Symbols
	// Dingbats
	// Miscellaneous Mathematical Symbols-A
	// Supplemental Arrows-A
	// Braille Patterns
	// Supplemental Arrows-B
	// Miscellaneous Mathematical Symbols-B
	// Miscellaneous Symbols and Arrows
	// ...
    // Tags. Range E0001-E007F
    // Variation Selectors Supplement. Range: E0100-E01EF
    // Unassigned. Range: EFFFE-EFFFF
    // Supplementary Private Use Area-A. Range: FFF80–FFFFF
    // FFFFE, FFFFF is non-characters.
    // Supplementary Private Use Area-B. Range: 10FFFE–10FFFF
    // 10FFFE, 10FFFF is non-characters.
    { "", { "" }, { UC::other, UC::other }, TriboolUnknown, "" }
};

// Canonical equivalences [0065, 0303] -> 00f1, ... consider using NFA
// Canonical equivalences 00f1 -> [0065, 0303], ... consider using DFA
