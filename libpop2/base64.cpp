#include "base64.h"

char base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
i32 base64TableSize = ArrayCount(base64Table);

i32 Base64_GetTableSize()
{
	return base64TableSize;
}

i8 Base64_FindPositionFor(us32 chr)
{
	i8 result = 0;

	//NOTES(): Doing this the most stupid easy way possible

	for (i8 index = 0;index < Base64_GetTableSize();index++)
	{
		if (base64Table[index] == chr)
		{
			result = index;
			break;
		}
	}

	return result;
}



s32 *Base64_Encode(s32 *text, i32 length)
{
	s32 *result = NULL;
	ui8 bitResult = 0;
	ui8 needed = 0;
	ui8 totalBits = 8;
	ui8 shiftRBy = 2;
	ui8 shiftLBy = 6;
	i32 resultIndex = 0;
	ui8 tmp=0;
	ui8 remainder = 0;
	ui8 bitTaken = 6;
	ui8 have = 0;
	ui32 textLength = 0;


	
	/*
		01101000 01100101 01101100 01101100 0110 111100

		011010 000110 010101 101100 011011 000110
	*/

	if (length == -1)
	{
		textLength = Strlen(text);
	} else {
		textLength = length;
	}
	result = (s32*) MemoryRaw(textLength*2);

	needed = 6;
	have = 0;
	remainder  = 0;

	for (i32 textIndex = 0; textIndex < textLength;textIndex++)
	{
		unsigned char eightBitChar = (*(text + textIndex) & 0x000000FF);

	
		remainder = totalBits - needed; //2 | 4 | 6
		have = remainder; // 2 | 4 | 6
		needed = 6 - remainder; //4 | 2 | 0
		tmp = eightBitChar >> remainder; //2 | 4 | 6
		bitResult |= tmp;

		result[resultIndex] = base64Table[bitResult];

		if (needed != 0)
		{
			bitResult = eightBitChar;
			bitResult <<= totalBits - have;
			bitResult >>= 2;

		} else {
			ui32 shiftAmount = 0;
			shiftAmount = totalBits - have;
			bitResult = eightBitChar << shiftAmount;
			bitResult >>= shiftAmount;
			resultIndex++;
			result[resultIndex] = base64Table[bitResult];

			bitResult = 0;
			needed = 6;
			have = 0;
			remainder = 0;
			
		}

		resultIndex++;
	}

	if (remainder != 0)
	{
		ui32 padding = 0;
		padding = 6 - remainder;
		
		result[resultIndex] = base64Table[bitResult];
	}
	
	return result;
}

s32 *Base64_Decode2(us32* encodedText, int *length)
{
	//TODO(): This is a simple slow version. This need to be optimize this later.
	ui8 *copyOfEncodedText = NULL;
	s32 *result = NULL;
	i32 realTextLength = 0;
	i32 textLength = 0;
	ui8 have = 0;
	ui8 need = 6;
	ui8 bitResult = 0;
	ui8 firstHalf = 0;
	ui8 secondHalf = 0;
	ui32 resultIndex = 0;

	textLength = Strlen(encodedText);
	realTextLength = ((Strlen(encodedText) * 6) / 8);

	
	result = (s32*) MemoryRaw(textLength*2);

	copyOfEncodedText = (ui8*) MemoryRaw(textLength);

	have = 6;
	need = 0;

	for (ui32 copyOfEncodedIndex = 0; copyOfEncodedIndex < textLength; copyOfEncodedIndex++)
	{
		ui8 position = 0;
		ui8 encodedChar = 0;

		encodedChar = (i8) encodedText[copyOfEncodedIndex];

		position = Base64_FindPositionFor(encodedChar);

		copyOfEncodedText[copyOfEncodedIndex] = position << 2;
	}


	for (ui32 encodedTextIndex = 0; encodedTextIndex < textLength-1; encodedTextIndex++)
	{	
		ui8 shiftAmount = 0;
		ui8 eightBitRemainder = 0;

		firstHalf = copyOfEncodedText[encodedTextIndex];
		need = 8 - have;
		shiftAmount = 8 - need;
		secondHalf = copyOfEncodedText[encodedTextIndex+1];
		secondHalf = secondHalf >> shiftAmount;
		firstHalf |= secondHalf; 
		result[resultIndex] = firstHalf;
	
		copyOfEncodedText[encodedTextIndex+1] <<= need;

		have =  6 - need;  

		if (have == 0)
		{
			encodedTextIndex++;
			have = 6;
		}
		resultIndex++;
	}

	*length = resultIndex;

	if (copyOfEncodedText)
	{
		Free(copyOfEncodedText);
		copyOfEncodedText=NULL;
	}

	return result;
}

s32 *Base64_Decode(us32* encodedText)
{
	s32* result = NULL; 
	int resultLen = 0;

	result = Base64_Decode2(encodedText, &resultLen);

	return result;
}

us32* HexDigest(us32 *data, i32 length)
{
	us32* result = NULL;
	us32 *resultAddr = NULL;
	us32* temp = NULL;

	result = (us32*)  MemoryRaw( (length*2) + 1);
	resultAddr = result;

	for (i32 dataIndex = 0; dataIndex < length;dataIndex++)
	{
		temp  = BaseToUString(data[dataIndex] ,16);

		memcpy(resultAddr,temp,2);
		resultAddr+=2;

		if (temp)
		{
			Free(temp);
			temp = NULL;
		}
	}

	if (temp)
	{
		Free(temp);
		temp=NULL;
	}

	return result;
}


