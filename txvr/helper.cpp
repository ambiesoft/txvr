// txvr.cpp : Defines the entry point for the application.
//

#include "framework.h"
#pragma warning(disable : 4309)
#include "../compact_enc_det/compact_enc_det/compact_enc_det.h"
#include "../compact_enc_det/compact_enc_det/compact_enc_det_hint_code.h"

#include "helper.h"

static struct {
    int googleenc;
    int cp;
} GE2CP[] = {
    {ISO_8859_1, 28591},
    {ISO_8859_2, 28592},
    {ISO_8859_3,28593},
    {ISO_8859_4,28594},
    {ISO_8859_5,28595},
    {ISO_8859_6,28596},
    {ISO_8859_7,28597},
    {ISO_8859_8,28598},
    {ISO_8859_9,28599},
    {ISO_8859_10,28600}, // not in web
    {JAPANESE_EUC_JP,20932},
    {JAPANESE_SHIFT_JIS,932},
    {JAPANESE_JIS,50220}, // 2022-jp
    {CHINESE_BIG5,950},
    {CHINESE_GB,936}, // gb2312
    {CHINESE_EUC_CN,51936},


    {KOREAN_EUC_KR,51949},
    {UNICODE,1200}, // utf16 little
    {CHINESE_EUC_DEC,51950}, // euc trad

    {CHINESE_CNS,20000},

    {CHINESE_BIG5_CP950,950},
    {JAPANESE_CP932,932},
    {UTF8,CP_UTF8},
    {UNKNOWN_ENCODING,0},
    {ASCII_7BIT,20127},



    {RUSSIAN_KOI8_R,20866},
    {RUSSIAN_CP1251,1251},


    //------------------
    // These are _not_ o
    // detected in the h
    {MSFT_CP1252, 1252},
    {RUSSIAN_KOI8_RU,21866},


{MSFT_CP1250,1250},
    {ISO_8859_15,28605},
    //------------------

    //------------------
    // These are in Basi
    // needed for new in
    // research langid
    {MSFT_CP1254,1254},
    {MSFT_CP1257,1257},
    //------------------

    //------------------
    //------------------
    // New encodings det
    {ISO_8859_11,28601},
    {MSFT_CP874,874},
    {MSFT_CP1256,1256},

    //------------------
    // Detected as ISO_8
    {MSFT_CP1255,1255},
    {ISO_8859_8_I,38598},
    {HEBREW_VISUAL,38598}, //same as above
    //------------------

    //------------------
    // Detected by resea
    {CZECH_CP852,852},
    {CZECH_CSN_369103,369103}, // nazo
    {MSFT_CP1253,1253},
    {RUSSIAN_CP866,866},
    //------------------

    //------------------
    // Handled by iconv 
    {ISO_8859_13,28603},
    {ISO_2022_KR,50225},
    {GBK,0}, // nazo
    {GB18030,54936},
    {BIG5_HKSCS,10002}, // mac?
    {ISO_2022_CN,50227}, // nazo

    //------------------
    // Detected by xin l
    // Handled by transc
    // (Indic encodings)

    {TSCII,0},
    {TAMIL_MONO,57004},
    {TAMIL_BI,57004},
    {JAGRAN,0},


    {MACINTOSH_ROMAN,10000},
    {UTF7,CP_UTF7},
    {BHASKAR,0},
    {HTCHANAKYA,0},

    //------------------
    // These allow a sin
    // to do UTF-16 <==>
    // bulk conversions,
    // fallback if neede
    { UTF16BE,1201 },
    { UTF16LE,1200 },
    { UTF32BE,12001 },
    { UTF32LE,12000 },
    //------------------

    //------------------
    // An encoding that 
    // simple ASCII text
    // implemented) is t
    // (follow each kept
    // the bytes. This w
    // in JPEGs. No outp
    { BINARYENC,0 },
    //------------------

    //------------------
    // Some Web pages al
    // ~{ ... ~} for 2-b
    { HZ_GB_2312,52936 },
    //------------------

    //------------------
    // Some external ven
    // converting MSFT_C
    { UTF8UTF8,CP_UTF8 },
    //------------------

    //------------------
    // Handled by transc
    // encodings without
    { TAM_ELANGO,0 },
    { TAM_LTTMBARANI,0 },
    { TAM_SHREE,0 },
    { TAM_TBOOMIS,0 },
    { TAM_TMNEWS,0 },
    { TAM_WEBTAMIL,0 },
    //------------------

    //------------------
    // Shift_JIS variant
    { KDDI_SHIFT_JIS,932 },
    { DOCOMO_SHIFT_JIS,932 },
    { SOFTBANK_SHIFT_JIS,932 },
    // ISO-2022-JP varia
    { KDDI_ISO_2022_JP,50220 },
    { SOFTBANK_ISO_2022_JP,50220 },
};

static int GetCPFromGE(int ge)
{
    for (int i = 0; i < sizeof(GE2CP); ++i)
    {
        if (GE2CP[i].googleenc == ge)
            return GE2CP[i].cp;
    }
    return 0;
}
int GetDetectedCodecGoogle(const BYTE* pByte, size_t size)
{
    if (size==0)
    {
        return CP_UTF8;
    }
    int bytes_consumed = 0;
    bool is_reliable = false;
    Encoding enc = CompactEncDet::DetectEncoding(
        (const char*)pByte,
        size,
        nullptr,                                // url hint
        nullptr,                               // http hint
        nullptr,                               // meta hint
        UNKNOWN_ENCODING,                     // enc hint
        UNKNOWN_LANGUAGE,  // lang hint
        CompactEncDet::WEB_CORPUS,
        true,  // Include 7-bit encodings
        &bytes_consumed, &is_reliable);
    //if (!is_reliable)
    //{
    //    return CP_UTF8;
    //}

    return GetCPFromGE(enc);
}
