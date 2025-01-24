/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_MME_ATTRIBUTES_H
#define P2K_SDK_MME_ATTRIBUTES_H

#include "P2kT.h"

#ifdef __cplusplus
extern "C" {
#endif

// Tested on v3x
#define MAX_ATTRIBUTE 0x10D

// INDEX_001 = ID + 1. ID = 0..0x10C
typedef enum {
	ATTRIBUTE_INDEX_001,  // Size: 0x5c bytes //MEDIA_FILE_INFO_T
	ATTRIBUTE_INDEX_002,  // Size: 0x04 bytes //DURATION (in seconds)
	ATTRIBUTE_INDEX_003,  // Size: 0x04 bytes //FILE_SIZE (in bytes)
	ATTRIBUTE_INDEX_004,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_005,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_006,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_007,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_008,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_009,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_010,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_011,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_012,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_013,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_014,  // Size: 0x30 bytes
	ATTRIBUTE_INDEX_015,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_016,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_017,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_018,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_019,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_020,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_021,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_022,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_023,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_024,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_025,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_026,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_027,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_028,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_029,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_030,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_031,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_032,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_033,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_034,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_035,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_036,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_037,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_038,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_039,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_040,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_041,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_042,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_043,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_044,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_045,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_046,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_047,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_048,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_049,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_050,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_051,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_052,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_053,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_054,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_055,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_056,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_057,  // Size: 0x06 bytes
	ATTRIBUTE_INDEX_058,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_059,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_060,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_061,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_062,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_063,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_064,  // Size: 0x3c bytes
	ATTRIBUTE_INDEX_065,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_066,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_067,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_068,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_069,  // Size: 0x0c bytes
	ATTRIBUTE_INDEX_070,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_071,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_072,  // Size: 0x30 bytes
	ATTRIBUTE_INDEX_073,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_074,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_075,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_076,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_077,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_078,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_079,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_080,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_081,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_082,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_083,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_084,  // Size: 0x0c bytes
	ATTRIBUTE_INDEX_085,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_086,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_087,  // Size: 0x48 bytes //AUDIO_FORMAT_T
	ATTRIBUTE_INDEX_088,  // Size: 0x50 bytes
	ATTRIBUTE_INDEX_089,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_090,  // Size: 0x04 bytes //CURRENT_POSITION (in seconds)
	ATTRIBUTE_INDEX_091,  // Size: 0x07 bytes
	ATTRIBUTE_INDEX_092,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_093,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_094,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_095,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_096,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_097,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_098,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_099,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_100,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_101,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_102,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_103,  // Size: 0x12 bytes
	ATTRIBUTE_INDEX_104,  // Size: 0x21 bytes
	ATTRIBUTE_INDEX_105,  // Size: 0x14 bytes
	ATTRIBUTE_INDEX_106,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_107,  // Size: 0x0c bytes
	ATTRIBUTE_INDEX_108,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_109,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_110,  // Size: 0x09 bytes
	ATTRIBUTE_INDEX_111,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_112,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_113,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_114,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_115,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_116,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_117,  // Size: 0x06 bytes
	ATTRIBUTE_INDEX_118,  // Size: 0x0c bytes
	ATTRIBUTE_INDEX_119,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_120,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_121,  // Size: 0x40 bytes
	ATTRIBUTE_INDEX_122,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_123,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_124,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_125,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_126,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_127,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_128,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_129,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_130,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_131,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_132,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_133,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_134,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_135,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_136,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_137,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_138,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_139,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_140,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_141,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_142,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_143,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_144,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_145,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_146,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_147,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_148,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_149,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_150,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_151,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_152,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_153,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_154,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_155,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_156,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_157,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_158,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_159,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_160,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_161,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_162,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_163,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_164,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_165,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_166,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_167,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_168,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_169,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_170,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_171,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_172,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_173,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_174,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_175,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_176,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_177,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_178,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_179,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_180,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_181,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_182,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_183,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_184,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_185,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_186,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_187,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_188,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_189,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_190,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_191,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_192,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_193,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_194,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_195,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_196,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_197,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_198,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_199,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_200,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_201,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_202,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_203,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_204,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_205,  // Size: 0x10 bytes
	ATTRIBUTE_INDEX_206,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_207,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_208,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_209,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_210,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_211,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_212,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_213,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_214,  // Size: 0x03 bytes
	ATTRIBUTE_INDEX_215,  // Size: 0x03 bytes
	ATTRIBUTE_INDEX_216,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_217,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_218,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_219,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_220,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_221,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_222,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_223,  // Size: 0x02 bytes
	ATTRIBUTE_INDEX_224,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_225,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_226,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_227,  // Size: 0x01 bytes
	ATTRIBUTE_INDEX_228,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_229,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_230,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_231,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_232,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_233,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_234,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_235,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_236,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_237,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_238,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_239,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_240,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_241,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_242,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_243,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_244,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_245,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_246,  // Size: 0x09 bytes
	ATTRIBUTE_INDEX_247,  // Size: 0x04 bytes
	ATTRIBUTE_INDEX_248,  // Size: 0x1c bytes
	ATTRIBUTE_INDEX_249,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_250,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_251,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_252,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_253,  // Size: 0x08 bytes
	ATTRIBUTE_INDEX_254,  // Size: 0x0c bytes
	ATTRIBUTE_INDEX_255,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_256,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_257,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_258,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_259,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_260,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_261,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_262,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_263,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_264,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_265,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_266,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_267,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_268,  // Size: 0x00 bytes
	ATTRIBUTE_INDEX_269   // Size: 0x01 bytes
} ATTRIBUTE_NAME_FULL_T;

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_MME_ATTRIBUTES_H */

/** @} */ /* end of P2K_Types */
