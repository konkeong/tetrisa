#include <stdio.h>
#include <string.h>

//#ifdef WIN32
#include <io.h>
//#else
//#include <unistd.h>
//#endif

#include "kutil.h"

/***************************************************************/
/*
0101 0101  0101 0101  0101 0101  0101 0101
*/
int fprintBit(FILE *stream, char *sbuf, int nbuf)
{
	int nRet;
	int i;

	if (nbuf <= 0)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
	{
		if ( (i>0) && (i%4==0) )
			nRet += fprintf(stream, "\n");
		nRet += fprintf(stream, "%d%d%d%d %d%d%d%d  ", 
				((sbuf[i]   )&0x01), ((sbuf[i]>>1)&0x01),
				((sbuf[i]>>2)&0x01), ((sbuf[i]>>3)&0x01),
				((sbuf[i]>>4)&0x01), ((sbuf[i]>>5)&0x01),
				((sbuf[i]>>6)&0x01), ((sbuf[i]>>7)&0x01) );
	}
	nRet += fprintf(stream, "\n");
	return nRet;
}

/***************************************************************/
int fprintHex(FILE *stream, char *sbuf, int nbuf)
{
	int nRet;
	int i;

	if (nbuf <= 0)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
	{
		if ( (i>0) && (i%8==0) )
		{
			if (i%16 == 0)
				nRet += fprintf(stream, "\n");
			else
				nRet += fprintf(stream, " ");
		}
		nRet += fprintf(stream, "%02x ", (unsigned char)sbuf[i]);
	}
	nRet += fprintf(stream, "\n");
	return nRet;
}

/***************************************************************/
int fprintHexA(FILE *stream, char *sbuf, int nbuf)
{
	int nRet;
	int i;

	if (nbuf <= 0)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
		nRet += fprintf(stream, "%02x", (unsigned char)sbuf[i]);
	nRet += fprintf(stream, "\n");
	return nRet;
}

/***************************************************************/
int fprintHexA2(FILE *stream, char *sbuf, int nbuf)
{
	int nRet;
	int i;

	if (nbuf <= 0)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
		nRet += fprintf(stream, "%02x", (unsigned char)sbuf[i]);
	return nRet;
}

/***************************************************************/
int fprintHexB(FILE *stream, char *sbuf, int nbuf)
{
	int nRet;
	int i;
	int j;
	int remlen;
	int curpos;
	int ch;

	if (nbuf <= 0)
		return 0;

	nRet = 0;
	remlen = nbuf;
	curpos = 0;
	while (remlen > 0)
	{
		if (remlen - 16 < 0)
			j = remlen;
		else
			j = 16;
/*
fprintf(stderr, "\tremlen(%d)  curpos(%d)  j(%d)\n", remlen, curpos, j);
*/
		for (i=0; i<16; i++)
		{
			if ( (i>0) && (i%8==0) )
				nRet += fprintf(stream, " ");
			if (i >= j)
				nRet += fprintf(stream, "   ");
			else
				nRet += fprintf(stream, "%02x ", (unsigned char)sbuf[curpos+i]);
		}
		nRet += fprintf(stream, "   ");
		for (i=0; i<j; i++)
		{
			if ( (i>0) && (i%8==0) )
				;  /* do nothing */
			ch = (unsigned char)sbuf[curpos+i];
			if ((ch < 31) || (ch > 127))
				nRet += fprintf(stream, "%c", '.');
			else
				nRet += fprintf(stream, "%c", ch);
		}

		nRet += fprintf(stream, "\n");

		remlen = remlen - 16;
		curpos = curpos + 16;
	}

	return nRet;
}

/***************************************************************/
/* kk 12/01/00
	nobuf = max char of output buf
	NULL_terminated
	ret 0/pos=num_bytes_output neg=bad
*/
int sprintHex(char *obuf, int nobuf, char *sbuf, int nbuf)
{
	int nRet;
	int i;
	char buftmp[8];

	if (nbuf <= 0)
		return 0;
	if (nobuf < 1)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
	{
		if (nRet > nobuf-1)
			break;

		if ( (i>0) && (i%8==0) )
		{
			if (nRet+1 > nobuf-1)
				break;
			if (i%16 == 0)
			{
				sprintf(buftmp, "\n");
				memcpy(obuf, buftmp, 1);
				obuf += 1;
				nRet += 1;
			}
			else
			{
				sprintf(buftmp, " ");
				memcpy(obuf, buftmp, 1);
				obuf += 1;
				nRet += 1;
			}
		}
		if (nRet+3 > nobuf-1)
			break;
		sprintf(buftmp, "%02x ", (unsigned char)sbuf[i]);
		memcpy(obuf, buftmp, 3);
		obuf += 3;
		nRet += 3;
	}

	*obuf = 0;
	return nRet;
}

/***************************************************************/
int sprintHexA(char *obuf, int nobuf, char *sbuf, int nbuf)
{
	int nRet;
	int i;
	char buftmp[8];

	if (nbuf <= 0)
		return 0;
	if (nobuf < 1)
		return 0;

	nRet = 0;
	for (i=0; i<nbuf; i++)
	{
		if (nRet > nobuf-1)
			break;

		if (nRet+2 > nobuf-1)
			break;
		sprintf(buftmp, "%02x", (unsigned char)sbuf[i]);
		memcpy(obuf, buftmp, 2);
		obuf += 2;
		nRet += 2;
	}

	*obuf = 0;
	return nRet;
}

/***************************************************************/
int sprintHexB(char *obuf, int nobuf, char *sbuf, int nbuf)
{
	int nRet;
	int i;
	int j;
	char buftmp[8];
	int remlen;
	int curpos;
	int ch;

	remlen = nbuf;
	curpos = 0;
	nRet = 0;
	while (remlen > 0)
	{
		if (remlen - 16 < 0)
			j = remlen;
		else
			j = 16;

/*
fprintf(stderr, "\tremlen(%d)  curpos(%d)  j(%d)  nRet(%d)  nobuf(%d)\n", remlen, curpos, j, nRet, nobuf);
*/

		for (i=0; i<16; i++)
		{
			if (nRet+3 > nobuf-1)
				goto fin;

			if ( (i>0) && (i%8==0) )
			{
				sprintf(buftmp, " ");
				memcpy(obuf, buftmp, 1);
				obuf += 1;
				nRet += 1;
			}
			if (i >= j)
			{
				sprintf(buftmp, "    ");
				memcpy(obuf, buftmp, 3);
				obuf += 3;
				nRet += 3;
			}
			else
			{
				sprintf(buftmp, "%02x ", (unsigned char)sbuf[curpos+i]);
				memcpy(obuf, buftmp, 3);
				obuf += 3;
				nRet += 3;
			}
		}
		if (nRet+3 > nobuf-1)
			goto fin;
		sprintf(buftmp, "   ");
		memcpy(obuf, buftmp, 3);
		obuf += 3;
		nRet += 3;
		for (i=0; i<j; i++)
		{
			if (nRet+1 > nobuf-1)
				goto fin;

			ch = (unsigned char)sbuf[curpos+i];

			if ( (i>0) && (i%8==0) )
			{
				;  /* do nothing */
			}
			if ((ch < 31) || (ch > 127))
			{
				sprintf(buftmp, "%c", '.');
				memcpy(obuf, buftmp, 1);
				obuf += 1;
				nRet += 1;
			}
			else
			{
				sprintf(buftmp, "%c", ch);
				memcpy(obuf, buftmp, 1);
				obuf += 1;
				nRet += 1;

			}
		}
/*
fprintf(stderr, "remlen:(%d)\n", remlen);
*/
		if (remlen - 16 <= 0)
			goto fin;

		if (nRet+1 > nobuf-1)
			goto fin;
		sprintf(buftmp, "\n");
		memcpy(obuf, buftmp, 1);
		obuf += 1;
		nRet += 1;

		remlen = remlen - 16;
		curpos = curpos + 16;
	}
fin:

	*obuf = 0;
	return nRet;
}

/***************************************************************/
/* kk 27/10/99
	convert '\r\n' to '\n'
	will truncate '\n'
	replace '\n' with '\0'
	0/pos=bytes_read neg=bad_or_EOF
*/
int readlineDos(FILE *fp, char *data, int numch)
{
	int i, ch;
	int gotCR;

	gotCR = 0;
	i = 0;
	do
	{
		ch = getc(fp);
		if( ch == EOF )
			goto abort;

		if (ch == '\n')
		{
			if (gotCR)
			{
				data--;
				i--;
			}
			*data = 0;
			break;
		}
		if (ch == '\r')
			gotCR = 1;
		else
			gotCR = 0;
		*data++ = ch;
		i++;
	} while (i < numch);

	if (i < numch)
		*data = 0;

	return i;
abort:
	return -1;
}

/***************************************************************/
/* kk 29/01/00
	ret 0/pos=num_bytes -1=EOF
*/
int readBlkFrFile(FILE *fp, char *buf, int nBuf)
{
	int ch;
	int i;

	i = 0;
	ch = getc(fp);
	if (ch == EOF)
		goto abort;
	buf[i] = ch;

	for (i=1; i<nBuf; i++)
	{
		ch = getc(fp);
		if (ch == EOF)
			break;
		buf[i] = ch;
	}

	return i;
abort:
	return -1;
}

/***************************************************************/
/* kk 24/07/00
*/
int readlinefd(int fd, char *buf, int nbuf)
{
	char ch;
	int len;
	int nRead;

	nRead = 0;
	for ( ; ; )
	{
		if (nRead >= nbuf)
		{
			buf[nRead] = 0;
			break;
		}
#ifdef WIN32
		len = _read(fd, &ch, 1);
#else
		len = read(fd, &ch, 1);
#endif
		if (len != 1)
		{
			if (nRead == 0)
				nRead = -1;
			break; 
		}
		if (ch == '\n')
		{
			buf[nRead] = 0;
			break;
		}
		else
		{
			buf[nRead] = ch;
			nRead++;
		}
	}
	return nRead;
}

/***************************************************************/
/* kk 22/09/00
	strIn must be null-term
	will modified strIn
	return only max # flds specified
	0/pos = # flds split
	neg = bad
*/
int splitline(char *strIn, char **flds, char token, int nflds)
{
	int i;
	char *cp;
	int fBegin;

	if (token == 0)
		return -1;

//fprintf(stderr, "input:[%s]\n", strIn);
	fBegin = 1;
	for (i=0; i<nflds; strIn++)
	{
		if (fBegin == 1)
		{
			cp = strIn;
			fBegin = 0;
		}
		if (*strIn == 0)
		{
			if (i > 0)
			{
				flds[i] = cp;
//fprintf(stderr, "flds[]=%s\n", flds[i]);
				i++;
			}
			break;
		}
//fprintf(stderr, "%u:%c\n", strIn, *strIn);
		if (*strIn == token)
		{
//fprintf(stderr, "%d equal!  ", i);
			*strIn = 0;
			flds[i] = cp;
//fprintf(stderr, "flds[]=%s\n", flds[i]);
			i++;
			fBegin = 1;
		}
	}

	return i;
}
