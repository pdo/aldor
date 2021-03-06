/*****************************************************************************
 *
 * buffer.c: Grow-on-demand buffers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "format.h"
#include "store.h"
#include "strops.h"
#include "xfloat.h"

struct buffer {
	Length		pos;		/* amount used == next position */
	Length		argc;		/* amount available */
	UByte		*argv;
};

Buffer
bufNew(void)
{
	Buffer	b;

	b	= (Buffer) stoAlloc(OB_Buffer, sizeof(*b));
	b->argv = (UByte *) stoAlloc(OB_Other,	BUF_INIT_SIZE);
	b->argc = stoSize(b->argv);
	b->pos	= 0;

	return b;
}

void
bufFree(Buffer b)
{
	stoFree((Pointer) (b->argv));
	stoFree((Pointer) (b));
}

Buffer
bufCapture(String s, Length l)
{
	Buffer	b;
	b	= (Buffer) stoAlloc(OB_Buffer, sizeof(*b));
	b->argv = (UByte *) s;
	b->argc = l;
	b->pos	= 0;

	return b;
}

String
bufLiberate(Buffer b)
{
	String s = (String) b->argv;
	if ((b->pos  == 0) && b->argc)
		s[0] = '\0';
	stoFree((Pointer) (b));
	return s;
}

UByte *
bufData(Buffer s)
{
	return s->argv;
}

String
bufChars(Buffer s)
{
	return (String) bufData(s);
}

Length
bufSize(Buffer s)
{
	return s->argc;
}

Length
bufPosition(Buffer s)
{
	return s->pos;
}

void
bufSetPosition(Buffer s, Length n)
{
	assert(n <= s->argc);
	s->pos = n;
}

void
bufSkip(Buffer s, Length n)
{
	bufSetPosition(s, bufPosition(s) + n);
}

void
bufNeed(Buffer b, Length n)
{
	if (b->argc < n) {
		b->argv = (UByte *) stoResize(b->argv, n);
		b->argc = stoSize(b->argv);
	}
}

void
bufGrow(Buffer b, Length inc)
{
	b->argv = (UByte *) stoResize(b->argv, b->argc + inc);
	b->argc = stoSize(b->argv);
}

int
bufAdd1(Buffer b, int c)
{
	if (b->pos == b->argc)
		bufGrow(b, b->argc / 2);
	return b->argv[b->pos++] = c;
}

void
bufAddn(Buffer b, const char *s, Length n)
{
	bufNeed(b, b->pos + n);
	memmove(b->argv + b->pos, (char *) s, n);
	bufSkip(b, n);
}

String
bufGetn(Buffer b, Length n)
{
	assert(b->pos + n <= b->argc);
	UByte	*s = b->argv + b->pos;
	bufSkip(b, n);
	return (String) s;
}

String
bufGets(Buffer b)
{
	UByte	*s = b->argv + b->pos;
	int	cc = strLength((String) s);
	bufSkip(b, cc + 1);
	return (String) s;
}

int
bufPuts(Buffer b, const char *s)
{
	int	cc = strLength(s);
	bufAddn(b, s, cc);
	bufAdd1(b,char0);
	bufBack1(b);
	return cc;
}

int
bufPutc(Buffer b, int c)
{
	bufAdd1(b, (char) c);
	bufAdd1(b, char0);
	bufBack1(b);
	return c;
}

int
bufPutcTimes(Buffer b, int c, int times)
{
	int i;
	for (i = 0; i < times; i++)
		bufPutc(b, c);
	return c;
}

int
bufPuti(Buffer buf, int i)
{
	Length	pos0;
	int	ndig, digits[4 * bitsizeof(int)];	/* 4 > lg 10 */

	pos0 = bufPosition(buf);

	if (i == 0) {
		bufAdd1(buf, '0');
	}
	else {
		if (i < 0) {
			bufAdd1(buf, '-');
			i = - i;
		}
		for (ndig = 0; i; i /= 10, ndig++)
			digits[ndig] = i % 10;
		while (ndig--)
			bufAdd1(buf, '0' + (char) digits[ndig]);
	}
	bufAdd1(buf, char0);
	bufBack1(buf);
	return bufPosition(buf) - pos0;
}

int
bufVPrintf(Buffer b, const char *fmt, va_list argp)
{
	int	cc;

	OStream ostream = ostreamNewFrBuffer(b);
	cc = ostreamVPrintf(ostream, fmt, argp);
	ostreamFree(ostream);
	return cc;
}

int
bufPrintf(Buffer b, const char *fmt, ...)
{
	va_list argp;
	int	cc;

	va_start(argp, fmt);
	cc = bufVPrintf(b, fmt, argp);
	va_end(argp);
	return cc;
}

int
bufPrint(FILE *fout, Buffer b)
{
	int	cc;

	cc  = fprintf(fout, "[%d/%d]", (int) b->pos, (int) b->argc);
	cc += strPrint(fout, (String) b->argv, '"', '"', '\\', "\\%#x");

	return cc;
}

/*****************************************************************************
 *
 * Functions for growing a buffer one character at a time.
 *
 ****************************************************************************/

void
bufStart(Buffer b)
{
	b->pos = 0;
}

UByte
bufGet1(Buffer b)
{
	assert(b->pos < b->argc);
	return b->argv[b->pos++];
}

void
bufBack1(Buffer b)
{
	assert(b->pos > 0);
	b->pos--;
}

UByte
bufNext1(Buffer b)
{
	return b->argv[b->pos];
}


/*
 * Functions for putting and getting characters to a buffer.
 */

void
bufPutChars(Buffer buf, char const *s, Length cc)
{
	bufAddn(buf, s, cc);
}

void
bufGetChars(Buffer buf, char *s, Length cc)
{
	strncpy(s, bufGetn(buf, cc), cc);
}


/*
 * Functions for putting and getting integers as byte sequences.
 */

void
bufPutByte(Buffer b, UByte c)
{
	bufAdd1(b, UNBYTE1(c));
}

void
bufPutHInt(Buffer b, UShort h)
{
	bufAdd1(b, HBYTE0(h));
	bufAdd1(b, HBYTE1(h));
}

void
bufPutSInt(Buffer b, ULong i)
{
	bufAdd1(b, BYTE0(i));
	bufAdd1(b, BYTE1(i));
	bufAdd1(b, BYTE2(i));
	bufAdd1(b, BYTE3(i));
}

UByte
bufGetByte(Buffer b)
{
	return bufGet1(b);
}

UShort
bufGetHInt(Buffer b)
{
	String	s = bufGetn(b, HINT_BYTES);
	return UNBYTE2(s[0], s[1]);
}

ULong
bufGetSInt(Buffer b)
{
	String	s = bufGetn(b, SINT_BYTES);
	return UNBYTE4(s[0], s[1], s[2], s[3]);
}


/*****************************************************************************
 *
 * Save integers in standard byte order.
 *
 ****************************************************************************/

UByte
bufRdUByte(Buffer buf)
{
	UByte result;

	result = bufGetByte(buf);

	return result;
}

UShort
bufRdUShort(Buffer buf)
{
	UShort result;

	result = bufGetHInt(buf);

	return result;
}

ULong
bufRdULong(Buffer buf)
{
	ULong result;

	result = bufGetSInt(buf);

	return result;
}

int
bufWrUByte(Buffer buf, UByte b)
{
	bufPutByte(buf, b);

	return BYTE_BYTES;
}

int
bufWrUShort(Buffer buf, UShort s)
{
	bufPutHInt(buf, s);

	return HINT_BYTES;
}

int
bufWrULong(Buffer buf, ULong l)
{
	bufPutSInt(buf, l);

	return SINT_BYTES;
}

/*****************************************************************************
 *
 * Save floating-point numbers in XFloat format.
 *
 ****************************************************************************/

SFloat
bufRdSFloat(Buffer buf)
{
	XSFloat *	pxs;
	SFloat		s;

	pxs = (XSFloat *) bufGetn(buf, XSFLOAT_BYTES);
	xsfToNative(pxs, &s);

	return s;
}

DFloat
bufRdDFloat(Buffer buf)
{
	XDFloat *	pxd;
	DFloat		d;

	pxd = (XDFloat *) bufGetn(buf, XDFLOAT_BYTES);
	xdfToNative(pxd, &d);

	return d;
}

int
bufWrSFloat(Buffer buf, SFloat s)
{
	XSFloat	xs;
	SFloat	bs = s;	/* Avoid problems when float is passed as double. */

	xsfFrNative(&xs, &bs);
	bufAddn(buf, (char *) &xs, XSFLOAT_BYTES);

	return XSFLOAT_BYTES;
}

int
bufWrDFloat(Buffer buf, DFloat d)
{
	XDFloat	xd;

	xdfFrNative(&xd, &d);
	bufAddn(buf, (char *) &xd, XDFLOAT_BYTES);

	return XDFLOAT_BYTES;
}

/*****************************************************************************
 *
 * Save a given number of characters in ASCII format.
 *
 ****************************************************************************/

/* Read a specified number of characters from the buffer.
 * Return the result as a null-terminated string.
 */
String
bufRdChars(Buffer buf, int cc)
{
	String	s;

	s = strAlloc(cc);
	bufGetChars(buf, s, cc);
	s = strnFrAscii(s,cc);

	return s;
}

int
bufWrChars(Buffer buf, int cc, String s)
{
	bufPutChars(buf, strnToAsciiStatic(s,cc), cc);

	return cc;
}


/*****************************************************************************
 *
 * Save strings in ASCII format, length included.
 *
 ****************************************************************************/

String
bufRdString(Buffer buf)
{
	String	s;
	int	cc;

	cc = bufGetSInt(buf);
	s = strAlloc(cc);
	bufGetChars(buf, s, cc);
	s = strnFrAscii(s,cc);

	return s;
}

int
bufWrString(Buffer buf, String s)
{
	int cc = strLength(s) + 1;

	bufPutSInt(buf, cc);
	bufPutChars(buf, strnToAsciiStatic(s,cc), cc);

	return SINT_BYTES + cc;
}

String
bufGetString(Buffer buf)
{
	String	s = strCopy(bufGets(buf));
	return strnFrAscii(s, strLength(s) + 1);
}


/*****************************************************************************
 *
 * Save data in binary format.
 *
 ****************************************************************************/

Buffer
bufRdBuffer(Buffer buf)
{
	String	s;
	int	cc;

	cc = bufGetSInt(buf);
	s = strAlloc(cc);
	bufGetChars(buf, s, cc);

	return bufCapture(s, cc);
}

int
bufWrBuffer(Buffer buf, Buffer b)
{
	int cc = bufPosition(b);

	bufPutSInt(buf, cc);
	bufPutChars(buf, bufChars(b), cc);

	return SINT_BYTES + cc;
}
