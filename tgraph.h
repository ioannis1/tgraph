// COPYRIGHT (c) Ioannis Tambouras 2011-2015
#ifndef _TGRAPH
#define _TGRAPH

#include "postgres.h"
#include "fmgr.h"
//#include "utils/builtins.h"
//#include "utils/guc.h"


#include "access/gist.h"
#include "access/itup.h"
#include "storage/bufpage.h"

PG_MODULE_MAGIC;


typedef struct
{
        int32           vl_len_;                /* varlena header (do not touch directly!) */
        uint8           flag;
        char            data[FLEXIBLE_ARRAY_MEMBER];
} TRGM;

typedef char trgm[3];


#define CMPCHAR(a,b) ( ((a)==(b)) ? 0 : ( ((a)<(b)) ? -1 : 1 ) )
#define CMPPCHAR(a,b,i)  CMPCHAR( *(((const char*)(a))+i), *(((const char*)(b))+i) )
#define CMPTRGM(a,b) ( CMPPCHAR(a,b,0) ? CMPPCHAR(a,b,0) : ( CMPPCHAR(a,b,1) ? CMPPCHAR(a,b,1) : CMPPCHAR(a,b,2) ) )



#ifdef DIVUNION
#define CALCSML(count, len1, len2) ((float4) (count)) / ((float4) ((len1) + (len2) - (count)))
#else
#define CALCSML(count, len1, len2) ((float4) (count)) / ((float4) (((len1) > (len2)) ? (len1) : (len2)))
#endif

extern void compact_trigram(trgm *tptr, char *str, int bytelen);
extern float4 cnt_sml(TRGM *trg1, TRGM *trg2, bool inexact);


#define LPADDING                2
#define RPADDING                1
#define KEEPONLYALNUM
#define TRGMHDRSIZE               (VARHDRSZ + sizeof(uint8))
#define ARRKEY                  0x01
#define SIGNKEY                 0x02
#define ISARRKEY(x) ( ((TRGM*)x)->flag & ARRKEY )
#define ISSIGNKEY(x)    ( ((TRGM*)x)->flag & SIGNKEY )
#define ISALLTRUE(x)    ( ((TRGM*)x)->flag & ALLISTRUE )
#define ALLISTRUE               0x04
#define CALCGTSIZE(flag, len) ( TRGMHDRSIZE + ( ( (flag) & ARRKEY ) ? ((len)*sizeof(trgm)) : (((flag) & ALLISTRUE) ? 0 : SIGLEN) ) )
#define GETSIGN(x)              ( (BITVECP)( (char*)x+TRGMHDRSIZE ) )
#define GETARR(x)               ( (trgm*)( (char*)x+TRGMHDRSIZE ) )
#define ARRNELEM(x) ( ( VARSIZE(x) - TRGMHDRSIZE )/sizeof(trgm) )
#define CPTRGM(a,b) do {                                \
        *(((char*)(a))+0) = *(((char*)(b))+0);  \
        *(((char*)(a))+1) = *(((char*)(b))+1);  \
        *(((char*)(a))+2) = *(((char*)(b))+2);  \
} while(0);
#ifdef KEEPONLYALNUM
#define ISWORDCHR(c)    (t_isalpha(c) || t_isdigit(c))
#define ISPRINTABLECHAR(a)      ( isascii( *(unsigned char*)(a) ) && (isalnum( *(unsigned char*)(a) ) || *(unsigned char*)(a)==' ') )
#else
#define ISWORDCHR(c)    (!t_isspace(c))
#define ISPRINTABLECHAR(a)      ( isascii( *(unsigned char*)(a) ) && isprint( *(unsigned char*)(a) ) )
#endif
#define ISPRINTABLETRGM(t)      ( ISPRINTABLECHAR( ((char*)(t)) ) && ISPRINTABLECHAR( ((char*)(t))+1 ) && ISPRINTABLECHAR( ((char*)(t))+2 ) )
#define BITBYTE 8
#define SIGLENINT  3                    /* >122 => key will toast, so very slow!!! */
#define SIGLEN  ( sizeof(int)*SIGLENINT )
#define CMPCHAR(a,b) ( ((a)==(b)) ? 0 : ( ((a)<(b)) ? -1 : 1 ) )
#define CMPPCHAR(a,b,i)  CMPCHAR( *(((const char*)(a))+i), *(((const char*)(b))+i) )
#define CMPTRGM(a,b) ( CMPPCHAR(a,b,0) ? CMPPCHAR(a,b,0) : ( CMPPCHAR(a,b,1) ? CMPPCHAR(a,b,1) : CMPPCHAR(a,b,2) ) )







extern TRGM *generate_trgm(char *str, int slen);
extern double similarity_threshold;

PG_FUNCTION_INFO_V1(similarity);


PG_FUNCTION_INFO_V1(similarity_op);

#endif
