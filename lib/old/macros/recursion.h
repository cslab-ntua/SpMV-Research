#ifndef RECURSION_H
#define RECURSION_H

#include "macrolib.h"
#include "count_macro_arguments.h"

/*
 * Macro arguments are completely macro-expanded before they are substituted into a macro body,
 * unless they are stringized or pasted with other tokens.
 * 
 * After substitution, the entire macro body, including the substituted arguments, is scanned again for macros to be expanded.
 * The result is that the arguments are scanned twice to expand macro calls in them.
 * 
 * If an argument is stringized or concatenated, the prescan does NOT occur.
 * If you want to expand a macro, then stringize or concatenate its expansion,
 * you can do that by causing one macro to call another macro that does the stringizing or concatenation.
 */


#define _RECURSION_200(fun_rec, fun_0, res)    _RECURSION_199(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 200, UNPACK(res))))
#define _RECURSION_199(fun_rec, fun_0, res)    _RECURSION_198(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 199, UNPACK(res))))
#define _RECURSION_198(fun_rec, fun_0, res)    _RECURSION_197(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 198, UNPACK(res))))
#define _RECURSION_197(fun_rec, fun_0, res)    _RECURSION_196(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 197, UNPACK(res))))
#define _RECURSION_196(fun_rec, fun_0, res)    _RECURSION_195(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 196, UNPACK(res))))
#define _RECURSION_195(fun_rec, fun_0, res)    _RECURSION_194(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 195, UNPACK(res))))
#define _RECURSION_194(fun_rec, fun_0, res)    _RECURSION_193(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 194, UNPACK(res))))
#define _RECURSION_193(fun_rec, fun_0, res)    _RECURSION_192(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 193, UNPACK(res))))
#define _RECURSION_192(fun_rec, fun_0, res)    _RECURSION_191(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 192, UNPACK(res))))
#define _RECURSION_191(fun_rec, fun_0, res)    _RECURSION_190(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 191, UNPACK(res))))
#define _RECURSION_190(fun_rec, fun_0, res)    _RECURSION_189(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 190, UNPACK(res))))
#define _RECURSION_189(fun_rec, fun_0, res)    _RECURSION_188(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 189, UNPACK(res))))
#define _RECURSION_188(fun_rec, fun_0, res)    _RECURSION_187(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 188, UNPACK(res))))
#define _RECURSION_187(fun_rec, fun_0, res)    _RECURSION_186(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 187, UNPACK(res))))
#define _RECURSION_186(fun_rec, fun_0, res)    _RECURSION_185(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 186, UNPACK(res))))
#define _RECURSION_185(fun_rec, fun_0, res)    _RECURSION_184(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 185, UNPACK(res))))
#define _RECURSION_184(fun_rec, fun_0, res)    _RECURSION_183(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 184, UNPACK(res))))
#define _RECURSION_183(fun_rec, fun_0, res)    _RECURSION_182(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 183, UNPACK(res))))
#define _RECURSION_182(fun_rec, fun_0, res)    _RECURSION_181(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 182, UNPACK(res))))
#define _RECURSION_181(fun_rec, fun_0, res)    _RECURSION_180(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 181, UNPACK(res))))
#define _RECURSION_180(fun_rec, fun_0, res)    _RECURSION_179(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 180, UNPACK(res))))
#define _RECURSION_179(fun_rec, fun_0, res)    _RECURSION_178(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 179, UNPACK(res))))
#define _RECURSION_178(fun_rec, fun_0, res)    _RECURSION_177(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 178, UNPACK(res))))
#define _RECURSION_177(fun_rec, fun_0, res)    _RECURSION_176(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 177, UNPACK(res))))
#define _RECURSION_176(fun_rec, fun_0, res)    _RECURSION_175(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 176, UNPACK(res))))
#define _RECURSION_175(fun_rec, fun_0, res)    _RECURSION_174(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 175, UNPACK(res))))
#define _RECURSION_174(fun_rec, fun_0, res)    _RECURSION_173(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 174, UNPACK(res))))
#define _RECURSION_173(fun_rec, fun_0, res)    _RECURSION_172(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 173, UNPACK(res))))
#define _RECURSION_172(fun_rec, fun_0, res)    _RECURSION_171(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 172, UNPACK(res))))
#define _RECURSION_171(fun_rec, fun_0, res)    _RECURSION_170(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 171, UNPACK(res))))
#define _RECURSION_170(fun_rec, fun_0, res)    _RECURSION_169(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 170, UNPACK(res))))
#define _RECURSION_169(fun_rec, fun_0, res)    _RECURSION_168(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 169, UNPACK(res))))
#define _RECURSION_168(fun_rec, fun_0, res)    _RECURSION_167(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 168, UNPACK(res))))
#define _RECURSION_167(fun_rec, fun_0, res)    _RECURSION_166(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 167, UNPACK(res))))
#define _RECURSION_166(fun_rec, fun_0, res)    _RECURSION_165(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 166, UNPACK(res))))
#define _RECURSION_165(fun_rec, fun_0, res)    _RECURSION_164(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 165, UNPACK(res))))
#define _RECURSION_164(fun_rec, fun_0, res)    _RECURSION_163(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 164, UNPACK(res))))
#define _RECURSION_163(fun_rec, fun_0, res)    _RECURSION_162(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 163, UNPACK(res))))
#define _RECURSION_162(fun_rec, fun_0, res)    _RECURSION_161(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 162, UNPACK(res))))
#define _RECURSION_161(fun_rec, fun_0, res)    _RECURSION_160(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 161, UNPACK(res))))
#define _RECURSION_160(fun_rec, fun_0, res)    _RECURSION_159(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 160, UNPACK(res))))
#define _RECURSION_159(fun_rec, fun_0, res)    _RECURSION_158(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 159, UNPACK(res))))
#define _RECURSION_158(fun_rec, fun_0, res)    _RECURSION_157(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 158, UNPACK(res))))
#define _RECURSION_157(fun_rec, fun_0, res)    _RECURSION_156(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 157, UNPACK(res))))
#define _RECURSION_156(fun_rec, fun_0, res)    _RECURSION_155(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 156, UNPACK(res))))
#define _RECURSION_155(fun_rec, fun_0, res)    _RECURSION_154(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 155, UNPACK(res))))
#define _RECURSION_154(fun_rec, fun_0, res)    _RECURSION_153(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 154, UNPACK(res))))
#define _RECURSION_153(fun_rec, fun_0, res)    _RECURSION_152(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 153, UNPACK(res))))
#define _RECURSION_152(fun_rec, fun_0, res)    _RECURSION_151(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 152, UNPACK(res))))
#define _RECURSION_151(fun_rec, fun_0, res)    _RECURSION_150(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 151, UNPACK(res))))
#define _RECURSION_150(fun_rec, fun_0, res)    _RECURSION_149(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 150, UNPACK(res))))
#define _RECURSION_149(fun_rec, fun_0, res)    _RECURSION_148(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 149, UNPACK(res))))
#define _RECURSION_148(fun_rec, fun_0, res)    _RECURSION_147(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 148, UNPACK(res))))
#define _RECURSION_147(fun_rec, fun_0, res)    _RECURSION_146(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 147, UNPACK(res))))
#define _RECURSION_146(fun_rec, fun_0, res)    _RECURSION_145(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 146, UNPACK(res))))
#define _RECURSION_145(fun_rec, fun_0, res)    _RECURSION_144(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 145, UNPACK(res))))
#define _RECURSION_144(fun_rec, fun_0, res)    _RECURSION_143(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 144, UNPACK(res))))
#define _RECURSION_143(fun_rec, fun_0, res)    _RECURSION_142(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 143, UNPACK(res))))
#define _RECURSION_142(fun_rec, fun_0, res)    _RECURSION_141(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 142, UNPACK(res))))
#define _RECURSION_141(fun_rec, fun_0, res)    _RECURSION_140(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 141, UNPACK(res))))
#define _RECURSION_140(fun_rec, fun_0, res)    _RECURSION_139(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 140, UNPACK(res))))
#define _RECURSION_139(fun_rec, fun_0, res)    _RECURSION_138(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 139, UNPACK(res))))
#define _RECURSION_138(fun_rec, fun_0, res)    _RECURSION_137(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 138, UNPACK(res))))
#define _RECURSION_137(fun_rec, fun_0, res)    _RECURSION_136(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 137, UNPACK(res))))
#define _RECURSION_136(fun_rec, fun_0, res)    _RECURSION_135(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 136, UNPACK(res))))
#define _RECURSION_135(fun_rec, fun_0, res)    _RECURSION_134(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 135, UNPACK(res))))
#define _RECURSION_134(fun_rec, fun_0, res)    _RECURSION_133(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 134, UNPACK(res))))
#define _RECURSION_133(fun_rec, fun_0, res)    _RECURSION_132(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 133, UNPACK(res))))
#define _RECURSION_132(fun_rec, fun_0, res)    _RECURSION_131(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 132, UNPACK(res))))
#define _RECURSION_131(fun_rec, fun_0, res)    _RECURSION_130(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 131, UNPACK(res))))
#define _RECURSION_130(fun_rec, fun_0, res)    _RECURSION_129(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 130, UNPACK(res))))
#define _RECURSION_129(fun_rec, fun_0, res)    _RECURSION_128(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 129, UNPACK(res))))
#define _RECURSION_128(fun_rec, fun_0, res)    _RECURSION_127(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 128, UNPACK(res))))
#define _RECURSION_127(fun_rec, fun_0, res)    _RECURSION_126(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 127, UNPACK(res))))
#define _RECURSION_126(fun_rec, fun_0, res)    _RECURSION_125(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 126, UNPACK(res))))
#define _RECURSION_125(fun_rec, fun_0, res)    _RECURSION_124(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 125, UNPACK(res))))
#define _RECURSION_124(fun_rec, fun_0, res)    _RECURSION_123(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 124, UNPACK(res))))
#define _RECURSION_123(fun_rec, fun_0, res)    _RECURSION_122(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 123, UNPACK(res))))
#define _RECURSION_122(fun_rec, fun_0, res)    _RECURSION_121(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 122, UNPACK(res))))
#define _RECURSION_121(fun_rec, fun_0, res)    _RECURSION_120(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 121, UNPACK(res))))
#define _RECURSION_120(fun_rec, fun_0, res)    _RECURSION_119(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 120, UNPACK(res))))
#define _RECURSION_119(fun_rec, fun_0, res)    _RECURSION_118(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 119, UNPACK(res))))
#define _RECURSION_118(fun_rec, fun_0, res)    _RECURSION_117(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 118, UNPACK(res))))
#define _RECURSION_117(fun_rec, fun_0, res)    _RECURSION_116(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 117, UNPACK(res))))
#define _RECURSION_116(fun_rec, fun_0, res)    _RECURSION_115(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 116, UNPACK(res))))
#define _RECURSION_115(fun_rec, fun_0, res)    _RECURSION_114(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 115, UNPACK(res))))
#define _RECURSION_114(fun_rec, fun_0, res)    _RECURSION_113(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 114, UNPACK(res))))
#define _RECURSION_113(fun_rec, fun_0, res)    _RECURSION_112(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 113, UNPACK(res))))
#define _RECURSION_112(fun_rec, fun_0, res)    _RECURSION_111(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 112, UNPACK(res))))
#define _RECURSION_111(fun_rec, fun_0, res)    _RECURSION_110(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 111, UNPACK(res))))
#define _RECURSION_110(fun_rec, fun_0, res)    _RECURSION_109(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 110, UNPACK(res))))
#define _RECURSION_109(fun_rec, fun_0, res)    _RECURSION_108(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 109, UNPACK(res))))
#define _RECURSION_108(fun_rec, fun_0, res)    _RECURSION_107(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 108, UNPACK(res))))
#define _RECURSION_107(fun_rec, fun_0, res)    _RECURSION_106(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 107, UNPACK(res))))
#define _RECURSION_106(fun_rec, fun_0, res)    _RECURSION_105(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 106, UNPACK(res))))
#define _RECURSION_105(fun_rec, fun_0, res)    _RECURSION_104(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 105, UNPACK(res))))
#define _RECURSION_104(fun_rec, fun_0, res)    _RECURSION_103(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 104, UNPACK(res))))
#define _RECURSION_103(fun_rec, fun_0, res)    _RECURSION_102(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 103, UNPACK(res))))
#define _RECURSION_102(fun_rec, fun_0, res)    _RECURSION_101(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 102, UNPACK(res))))
#define _RECURSION_101(fun_rec, fun_0, res)    _RECURSION_100(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 101, UNPACK(res))))
#define _RECURSION_100(fun_rec, fun_0, res)     _RECURSION_99(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 100, UNPACK(res))))
#define  _RECURSION_99(fun_rec, fun_0, res)     _RECURSION_98(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 99, UNPACK(res))))
#define  _RECURSION_98(fun_rec, fun_0, res)     _RECURSION_97(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 98, UNPACK(res))))
#define  _RECURSION_97(fun_rec, fun_0, res)     _RECURSION_96(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 97, UNPACK(res))))
#define  _RECURSION_96(fun_rec, fun_0, res)     _RECURSION_95(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 96, UNPACK(res))))
#define  _RECURSION_95(fun_rec, fun_0, res)     _RECURSION_94(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 95, UNPACK(res))))
#define  _RECURSION_94(fun_rec, fun_0, res)     _RECURSION_93(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 94, UNPACK(res))))
#define  _RECURSION_93(fun_rec, fun_0, res)     _RECURSION_92(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 93, UNPACK(res))))
#define  _RECURSION_92(fun_rec, fun_0, res)     _RECURSION_91(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 92, UNPACK(res))))
#define  _RECURSION_91(fun_rec, fun_0, res)     _RECURSION_90(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 91, UNPACK(res))))
#define  _RECURSION_90(fun_rec, fun_0, res)     _RECURSION_89(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 90, UNPACK(res))))
#define  _RECURSION_89(fun_rec, fun_0, res)     _RECURSION_88(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 89, UNPACK(res))))
#define  _RECURSION_88(fun_rec, fun_0, res)     _RECURSION_87(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 88, UNPACK(res))))
#define  _RECURSION_87(fun_rec, fun_0, res)     _RECURSION_86(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 87, UNPACK(res))))
#define  _RECURSION_86(fun_rec, fun_0, res)     _RECURSION_85(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 86, UNPACK(res))))
#define  _RECURSION_85(fun_rec, fun_0, res)     _RECURSION_84(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 85, UNPACK(res))))
#define  _RECURSION_84(fun_rec, fun_0, res)     _RECURSION_83(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 84, UNPACK(res))))
#define  _RECURSION_83(fun_rec, fun_0, res)     _RECURSION_82(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 83, UNPACK(res))))
#define  _RECURSION_82(fun_rec, fun_0, res)     _RECURSION_81(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 82, UNPACK(res))))
#define  _RECURSION_81(fun_rec, fun_0, res)     _RECURSION_80(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 81, UNPACK(res))))
#define  _RECURSION_80(fun_rec, fun_0, res)     _RECURSION_79(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 80, UNPACK(res))))
#define  _RECURSION_79(fun_rec, fun_0, res)     _RECURSION_78(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 79, UNPACK(res))))
#define  _RECURSION_78(fun_rec, fun_0, res)     _RECURSION_77(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 78, UNPACK(res))))
#define  _RECURSION_77(fun_rec, fun_0, res)     _RECURSION_76(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 77, UNPACK(res))))
#define  _RECURSION_76(fun_rec, fun_0, res)     _RECURSION_75(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 76, UNPACK(res))))
#define  _RECURSION_75(fun_rec, fun_0, res)     _RECURSION_74(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 75, UNPACK(res))))
#define  _RECURSION_74(fun_rec, fun_0, res)     _RECURSION_73(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 74, UNPACK(res))))
#define  _RECURSION_73(fun_rec, fun_0, res)     _RECURSION_72(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 73, UNPACK(res))))
#define  _RECURSION_72(fun_rec, fun_0, res)     _RECURSION_71(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 72, UNPACK(res))))
#define  _RECURSION_71(fun_rec, fun_0, res)     _RECURSION_70(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 71, UNPACK(res))))
#define  _RECURSION_70(fun_rec, fun_0, res)     _RECURSION_69(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 70, UNPACK(res))))
#define  _RECURSION_69(fun_rec, fun_0, res)     _RECURSION_68(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 69, UNPACK(res))))
#define  _RECURSION_68(fun_rec, fun_0, res)     _RECURSION_67(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 68, UNPACK(res))))
#define  _RECURSION_67(fun_rec, fun_0, res)     _RECURSION_66(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 67, UNPACK(res))))
#define  _RECURSION_66(fun_rec, fun_0, res)     _RECURSION_65(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 66, UNPACK(res))))
#define  _RECURSION_65(fun_rec, fun_0, res)     _RECURSION_64(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 65, UNPACK(res))))
#define  _RECURSION_64(fun_rec, fun_0, res)     _RECURSION_63(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 64, UNPACK(res))))
#define  _RECURSION_63(fun_rec, fun_0, res)     _RECURSION_62(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 63, UNPACK(res))))
#define  _RECURSION_62(fun_rec, fun_0, res)     _RECURSION_61(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 62, UNPACK(res))))
#define  _RECURSION_61(fun_rec, fun_0, res)     _RECURSION_60(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 61, UNPACK(res))))
#define  _RECURSION_60(fun_rec, fun_0, res)     _RECURSION_59(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 60, UNPACK(res))))
#define  _RECURSION_59(fun_rec, fun_0, res)     _RECURSION_58(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 59, UNPACK(res))))
#define  _RECURSION_58(fun_rec, fun_0, res)     _RECURSION_57(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 58, UNPACK(res))))
#define  _RECURSION_57(fun_rec, fun_0, res)     _RECURSION_56(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 57, UNPACK(res))))
#define  _RECURSION_56(fun_rec, fun_0, res)     _RECURSION_55(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 56, UNPACK(res))))
#define  _RECURSION_55(fun_rec, fun_0, res)     _RECURSION_54(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 55, UNPACK(res))))
#define  _RECURSION_54(fun_rec, fun_0, res)     _RECURSION_53(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 54, UNPACK(res))))
#define  _RECURSION_53(fun_rec, fun_0, res)     _RECURSION_52(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 53, UNPACK(res))))
#define  _RECURSION_52(fun_rec, fun_0, res)     _RECURSION_51(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 52, UNPACK(res))))
#define  _RECURSION_51(fun_rec, fun_0, res)     _RECURSION_50(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 51, UNPACK(res))))
#define  _RECURSION_50(fun_rec, fun_0, res)     _RECURSION_49(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 50, UNPACK(res))))
#define  _RECURSION_49(fun_rec, fun_0, res)     _RECURSION_48(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 49, UNPACK(res))))
#define  _RECURSION_48(fun_rec, fun_0, res)     _RECURSION_47(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 48, UNPACK(res))))
#define  _RECURSION_47(fun_rec, fun_0, res)     _RECURSION_46(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 47, UNPACK(res))))
#define  _RECURSION_46(fun_rec, fun_0, res)     _RECURSION_45(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 46, UNPACK(res))))
#define  _RECURSION_45(fun_rec, fun_0, res)     _RECURSION_44(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 45, UNPACK(res))))
#define  _RECURSION_44(fun_rec, fun_0, res)     _RECURSION_43(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 44, UNPACK(res))))
#define  _RECURSION_43(fun_rec, fun_0, res)     _RECURSION_42(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 43, UNPACK(res))))
#define  _RECURSION_42(fun_rec, fun_0, res)     _RECURSION_41(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 42, UNPACK(res))))
#define  _RECURSION_41(fun_rec, fun_0, res)     _RECURSION_40(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 41, UNPACK(res))))
#define  _RECURSION_40(fun_rec, fun_0, res)     _RECURSION_39(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 40, UNPACK(res))))
#define  _RECURSION_39(fun_rec, fun_0, res)     _RECURSION_38(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 39, UNPACK(res))))
#define  _RECURSION_38(fun_rec, fun_0, res)     _RECURSION_37(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 38, UNPACK(res))))
#define  _RECURSION_37(fun_rec, fun_0, res)     _RECURSION_36(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 37, UNPACK(res))))
#define  _RECURSION_36(fun_rec, fun_0, res)     _RECURSION_35(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 36, UNPACK(res))))
#define  _RECURSION_35(fun_rec, fun_0, res)     _RECURSION_34(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 35, UNPACK(res))))
#define  _RECURSION_34(fun_rec, fun_0, res)     _RECURSION_33(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 34, UNPACK(res))))
#define  _RECURSION_33(fun_rec, fun_0, res)     _RECURSION_32(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 33, UNPACK(res))))
#define  _RECURSION_32(fun_rec, fun_0, res)     _RECURSION_31(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 32, UNPACK(res))))
#define  _RECURSION_31(fun_rec, fun_0, res)     _RECURSION_30(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 31, UNPACK(res))))
#define  _RECURSION_30(fun_rec, fun_0, res)     _RECURSION_29(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 30, UNPACK(res))))
#define  _RECURSION_29(fun_rec, fun_0, res)     _RECURSION_28(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 29, UNPACK(res))))
#define  _RECURSION_28(fun_rec, fun_0, res)     _RECURSION_27(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 28, UNPACK(res))))
#define  _RECURSION_27(fun_rec, fun_0, res)     _RECURSION_26(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 27, UNPACK(res))))
#define  _RECURSION_26(fun_rec, fun_0, res)     _RECURSION_25(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 26, UNPACK(res))))
#define  _RECURSION_25(fun_rec, fun_0, res)     _RECURSION_24(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 25, UNPACK(res))))
#define  _RECURSION_24(fun_rec, fun_0, res)     _RECURSION_23(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 24, UNPACK(res))))
#define  _RECURSION_23(fun_rec, fun_0, res)     _RECURSION_22(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 23, UNPACK(res))))
#define  _RECURSION_22(fun_rec, fun_0, res)     _RECURSION_21(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 22, UNPACK(res))))
#define  _RECURSION_21(fun_rec, fun_0, res)     _RECURSION_20(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 21, UNPACK(res))))
#define  _RECURSION_20(fun_rec, fun_0, res)     _RECURSION_19(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 20, UNPACK(res))))
#define  _RECURSION_19(fun_rec, fun_0, res)     _RECURSION_18(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 19, UNPACK(res))))
#define  _RECURSION_18(fun_rec, fun_0, res)     _RECURSION_17(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 18, UNPACK(res))))
#define  _RECURSION_17(fun_rec, fun_0, res)     _RECURSION_16(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 17, UNPACK(res))))
#define  _RECURSION_16(fun_rec, fun_0, res)     _RECURSION_15(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 16, UNPACK(res))))
#define  _RECURSION_15(fun_rec, fun_0, res)     _RECURSION_14(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 15, UNPACK(res))))
#define  _RECURSION_14(fun_rec, fun_0, res)     _RECURSION_13(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 14, UNPACK(res))))
#define  _RECURSION_13(fun_rec, fun_0, res)     _RECURSION_12(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 13, UNPACK(res))))
#define  _RECURSION_12(fun_rec, fun_0, res)     _RECURSION_11(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 12, UNPACK(res))))
#define  _RECURSION_11(fun_rec, fun_0, res)     _RECURSION_10(fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 11, UNPACK(res))))
#define  _RECURSION_10(fun_rec, fun_0, res)     _RECURSION_9 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec, 10, UNPACK(res))))
#define   _RECURSION_9(fun_rec, fun_0, res)     _RECURSION_8 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  9, UNPACK(res))))
#define   _RECURSION_8(fun_rec, fun_0, res)     _RECURSION_7 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  8, UNPACK(res))))
#define   _RECURSION_7(fun_rec, fun_0, res)     _RECURSION_6 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  7, UNPACK(res))))
#define   _RECURSION_6(fun_rec, fun_0, res)     _RECURSION_5 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  6, UNPACK(res))))
#define   _RECURSION_5(fun_rec, fun_0, res)     _RECURSION_4 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  5, UNPACK(res))))
#define   _RECURSION_4(fun_rec, fun_0, res)     _RECURSION_3 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  4, UNPACK(res))))
#define   _RECURSION_3(fun_rec, fun_0, res)     _RECURSION_2 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  3, UNPACK(res))))
#define   _RECURSION_2(fun_rec, fun_0, res)     _RECURSION_1 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  2, UNPACK(res))))
#define   _RECURSION_1(fun_rec, fun_0, res)     _RECURSION_0 (fun_rec, fun_0, (_RECURSION_EXPAND(fun_rec,  1, UNPACK(res))))
#define   _RECURSION_0(fun_rec, fun_0, res)     _RECURSION_EXPAND(fun_0, 0, UNPACK(res))

// Step needed when 'fun_rec' or 'fun_0' is actually another macro with mutliple args,
// so that the args formed from 'UNPACK' are not considered as only one argument.
#define _RECURSION_EXPAND(fun, ...)           fun(__VA_ARGS__)

#define RECURSION_FORM_ITER(...)              CONCAT(_RECURSION_, COUNT_ARGS(__VA_ARGS__))


// Version with no braces, for when we don't want to create a new namespace.
#define RECURSION_UNGUARDED(fun_rec, fun_0, res, iter)  CONCAT(_RECURSION_, iter)(fun_rec, fun_0, res)

#define RECURSION(fun_rec, fun_0, res, iter)            ({ RECURSION_UNGUARDED(fun_rec, fun_0, res, iter); })


#endif /* RECURSION_H */
