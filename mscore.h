#ifndef _MSCORE_H_
#define _MSCORE_H_

#define FNAME_SCORE "tetrisa.hst\0"
#define MAX_REC_SCORE (1 + 12)

/*
    changing the structure affect sorting code
*/
struct StScore
{
    long time;
    long score;
    char name[32];
};

#ifdef __cplusplus
extern "C" {
#endif

int insertScore(struct StScore tbl[MAX_REC_SCORE], struct StScore *rec);
int readScoreFromFile(struct StScore tbl[MAX_REC_SCORE]);
int writeScoreToFile(struct StScore tbl[MAX_REC_SCORE]);

#ifdef __cplusplus
}
#endif

#endif  /* _MSCORE_H_ */
