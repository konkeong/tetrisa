#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>

#include "mscore.h"

#define VERSION_NUM 1000000

/*
    the number of byte compared are stated in
    sorting function
    must match
*/
int sort_function( const void *a, const void *b)
{
    return( -1 * memcmp((char *)a,(char *)b,12+12+1+32) );
}

/*
    accept n+1 record unsorted
    then sort it
    then chopped of the last record
*/
int sortScore(struct StScore tbl[MAX_REC_SCORE+1])
{
    /*
        -1 because zeroth-record is the versioning
    */
    char tblTemp[MAX_REC_SCORE+1-1][12+12+1+32];
    int i;
    char bufWork[32];

    for (i=0; i<MAX_REC_SCORE; i++)
    {
        sprintf(tblTemp[i], "%012d%012d%c"
                , tbl[i+1].score, tbl[i+1].time
                , (tbl[i+1].name[0] == 0) ? "A" : "Z"
        );
        memcpy(&tblTemp[i][25], tbl[i+1].name, 32);
    }

    // sort in reversed order
    qsort((void *)tblTemp, MAX_REC_SCORE, sizeof(tblTemp[0]), sort_function);

    for (i=0; i<MAX_REC_SCORE; i++)
    {
        sprintf(bufWork, "%-12.12s", &tblTemp[i][0]);
        tbl[i+1].score = atol(bufWork);

        sprintf(bufWork, "%-12.12s", &tblTemp[i][12]);
        tbl[i+1].time = atol(bufWork);

        memcpy(tbl[i+1].name, &tblTemp[i][25], 32);
    }

    return 0;
}

int insertScore(struct StScore tbl[MAX_REC_SCORE], struct StScore *rec)
{
    /*
        assume the records in the table
        are sorted by score then time then name
    */
    int i;
    struct StScore tblOwn[MAX_REC_SCORE+1];

    for (i=0; i<MAX_REC_SCORE; i++)
    {
        tblOwn[i].score = tbl[i].score;
        tblOwn[i].time = tbl[i].time;
        memcpy(tblOwn[i].name, tbl[i].name, sizeof(tbl[i].name));
    }
    tblOwn[MAX_REC_SCORE].score = rec->score;
    tblOwn[MAX_REC_SCORE].time = rec->time;
    memcpy(tblOwn[MAX_REC_SCORE].name, rec->name, sizeof(rec->name));

    sortScore(&tblOwn[0]);

    /* chop off last */
    for (i=0; i<MAX_REC_SCORE; i++)
    {
        tbl[i].score = tblOwn[i].score;
        tbl[i].time = tblOwn[i].time;
        memcpy(tbl[i].name, tblOwn[i].name, sizeof(tblOwn[i].name));
    }

    return 0;
}

int readScoreFromFile(struct StScore tbl[MAX_REC_SCORE])
{
    FILE *fp=(FILE *)0;
    int i;

    fp = fopen(FNAME_SCORE, "rb");
    if (fp == (FILE *)0)
        goto abort;

    i = fread(&tbl[0], sizeof(struct StScore), MAX_REC_SCORE, fp);
    if (i != MAX_REC_SCORE)
        goto abort;

    /* check version */
    if (tbl[0].time != VERSION_NUM)
        goto abort;

    fclose(fp);
    return 0;
abort:
    /* dump default, all zeroes */
    tbl[0].time = VERSION_NUM;
    tbl[0].score = 0;
    sprintf(tbl[0].name, "TETRISA VERSION 1.00.00");
    for (i=1; i<MAX_REC_SCORE; i++)
    {
        tbl[i].time = 0;
        tbl[i].score = 0;
        memset(tbl[i].name, 0, sizeof(tbl[i].name));
    }

    if (fp != (FILE *)0)
        fclose(fp);
    return -1;
}

int writeScoreToFile(struct StScore tbl[MAX_REC_SCORE])
{
    FILE *fp=(FILE *)0;
    int i;

    fp = fopen(FNAME_SCORE, "wb");
    if (fp == (FILE *)0)
        goto abort;

    tbl[0].time = VERSION_NUM;
    i = fwrite(&tbl[0], sizeof(struct StScore), MAX_REC_SCORE, fp);
    if (i != MAX_REC_SCORE)
        goto abort;

    fclose(fp);
    return 0;
abort:
    if (fp != (FILE *)0)
        fclose(fp);
    return -1;
}
