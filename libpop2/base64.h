#ifndef  __BASE64__
#define __BASE64__

#include "stringz.h"

us32* HexDigest(us32 *data, i32 length);
i8 Base64_FindPositionFor(us32 chr);
i32 Base64_GetTableSize();
s32 *Base64_Encode(s32 *text, i32 length);
s32 *Base64_Decode2(us32* encodedText, int *length);
s32 *Base64_Decode(us32* encodedText);
#endif
