#ifndef _KUTIL_H_
#define _KUTIL_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int fprintBit(FILE *stream, char *sbuf, int nbuf);

int fprintHex(FILE *stream, char *sbuf, int nbuf);
int fprintHexA(FILE *stream, char *sbuf, int nbuf);
int fprintHexA2(FILE *stream, char *sbuf, int nbuf);
int fprintHexB(FILE *stream, char *sbuf, int nbuf);

int sprintHex(char *obuf, int nobuf, char *sbuf, int nbuf);
int sprintHexA(char *obuf, int nobuf, char *sbuf, int nbuf);
int sprintHexB(char *obuf, int nobuf, char *sbuf, int nbuf);

int readlineDos(FILE *fp, char *data, int numch);
int readBlkFrFile(FILE *fp, char *buf, int nBuf);
int readlinefd(int fd, char *buf, int nbuf);

int splitline(char *strIn, char **flds, char token, int nflds);

#ifdef __cplusplus
}
#endif

#endif  /* _KUTIL_H_ */
