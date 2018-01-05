// COPYRIGHT (c) Ioannis Tambouras 2011-2015

#include "postgres.h"
#include "fmgr.h"
/*
#include "utils/builtins.h"
#include "utils/guc.h"
*/
#include "tgraph.h"
#include "catalog/pg_type.h"
#include "tsearch/ts_locale.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/pg_crc.h"



double		similarity_threshold = 0.3f;

uint32
trgm2int(trgm *ptr)
{
	uint32		val = 0;

	val |= *(((unsigned char *) ptr));
	val <<= 8;
	val |= *(((unsigned char *) ptr) + 1);
	val <<= 8;
	val |= *(((unsigned char *) ptr) + 2);

	return val;
}
static int
comp_trgm(const void *a, const void *b)
{
        return CMPTRGM(a, b);
}

float4
cnt_sml(TRGM *trg1, TRGM *trg2, bool inexact)
{
        trgm       *ptr1,
                           *ptr2;
        int                     count = 0;
        int                     len1,
                                len2;

        ptr1 = GETARR(trg1);
        ptr2 = GETARR(trg2);

        len1 = ARRNELEM(trg1);
        len2 = ARRNELEM(trg2);

        /* explicit test is needed to avoid 0/0 division when both lengths are 0 */
        if (len1 <= 0 || len2 <= 0)
                return (float4) 0.0;

        while (ptr1 - GETARR(trg1) < len1 && ptr2 - GETARR(trg2) < len2)
        {
                int                     res = CMPTRGM(ptr1, ptr2);

                if (res < 0)
                        ptr1++;
                else if (res > 0)
                        ptr2++;
                else
                {
                        ptr1++;
                        ptr2++;
                       count++;
                }
        }

        /*
         * If inexact then len2 is equal to count, because we don't know actual
         * length of second string in inexact search and we can assume that count
         * is a lower bound of len2.
         */
        return CALCSML(count, len1, inexact ? count : len2);
}



static int
unique_array(trgm *a, int len)
{
        trgm       *curend,
                           *tmp;

        curend = tmp = a;
        while (tmp - a < len)
                if (CMPTRGM(tmp, curend))
                {
                        curend++;
                        CPTRGM(curend, tmp);
                        tmp++;
                }
                else
                        tmp++;

        return curend + 1 - a;
}

void
compact_trigram(trgm *tptr, char *str, int bytelen)
{
        if (bytelen == 3)
        {
                CPTRGM(tptr, str);
        }
        else
        {
                pg_crc32        crc;

                INIT_LEGACY_CRC32(crc);
                COMP_LEGACY_CRC32(crc, str, bytelen);
                FIN_LEGACY_CRC32(crc);

                /*
                 * use only 3 upper bytes from crc, hope, it's good enough hashing
                 */
                CPTRGM(tptr, &crc);
        }
}

static trgm *
make_trigrams(trgm *tptr, char *str, int bytelen, int charlen)
{
        char       *ptr = str;

        if (charlen < 3)
                return tptr;

        if (bytelen > charlen)
        {
                /* Find multibyte character boundaries and apply compact_trigram */
                int                     lenfirst = pg_mblen(str),
                                        lenmiddle = pg_mblen(str + lenfirst),
                                        lenlast = pg_mblen(str + lenfirst + lenmiddle);

                while ((ptr - str) + lenfirst + lenmiddle + lenlast <= bytelen)
                {
                        compact_trigram(tptr, ptr, lenfirst + lenmiddle + lenlast);

                        ptr += lenfirst;
                        tptr++;

                        lenfirst = lenmiddle;
                        lenmiddle = lenlast;
                        lenlast = pg_mblen(ptr + lenfirst + lenmiddle);
                }
        } else {
                /* Fast path when there are no multibyte characters */
                Assert(bytelen == charlen);

                while (ptr - str < bytelen - 2 /* number of trigrams = strlen - 2 */ )
                {
                        CPTRGM(tptr, ptr);
                        ptr++;
                        tptr++;
                }
        }

        return tptr;
}

static char *
find_word(char *str, int lenstr, char **endword, int *charlen)
{
        char       *beginword = str;

        while (beginword - str < lenstr && !ISWORDCHR(beginword))
                beginword += pg_mblen(beginword);

        if (beginword - str >= lenstr)
                return NULL;

        *endword = beginword;
        *charlen = 0;
        while (*endword - str < lenstr && ISWORDCHR(*endword))
        {
                *endword += pg_mblen(*endword);
                (*charlen)++;
        }

        return beginword;
}


static int
generate_trgm_only(trgm *trg, char *str, int slen)
{
        trgm       *tptr;
        char       *buf;
        int                     charlen,
                                bytelen;
        char       *bword,
                           *eword;

        if (slen + LPADDING + RPADDING < 3 || slen == 0)
                return 0;

        tptr = trg;

        /* Allocate a buffer for case-folded, blank-padded words */
        buf = (char *) palloc(slen * pg_database_encoding_max_length() + 4);

        if (LPADDING > 0)
        {
                *buf = ' ';
                if (LPADDING > 1)
                        *(buf + 1) = ' ';
        }

        eword = str;
        while ((bword = find_word(eword, slen - (eword - str), &eword, &charlen)) != NULL)
        {
#ifdef IGNORECASE
                bword = lowerstr_with_len(bword, eword - bword);
                bytelen = strlen(bword);
#else
                bytelen = eword - bword;
#endif

                memcpy(buf + LPADDING, bword, bytelen);

#ifdef IGNORECASE
                pfree(bword);
#endif

                buf[LPADDING + bytelen] = ' ';
                buf[LPADDING + bytelen + 1] = ' ';

                /*
                 * count trigrams
                 */
                tptr = make_trigrams(tptr, buf, bytelen + LPADDING + RPADDING,
                                                         charlen + LPADDING + RPADDING);
        }

        pfree(buf);

        return tptr - trg;
}


static void
protect_out_of_mem(int slen)
{
        if ((Size) (slen / 2) >= (MaxAllocSize / (sizeof(trgm) * 3)) ||
                (Size) slen >= (MaxAllocSize / pg_database_encoding_max_length()))
                ereport(ERROR,
                                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                                 errmsg("out of memory")));
}

TRGM *
generate_trgm(char *str, int slen)
{
        TRGM       *trg;
        int         len;

        protect_out_of_mem(slen);

        trg = (TRGM *) palloc(TRGMHDRSIZE + sizeof(trgm) * (slen / 2 + 1) * 3);
        trg->flag = ARRKEY;

        len = generate_trgm_only(GETARR(trg), str, slen);
        SET_VARSIZE(trg, CALCGTSIZE(ARRKEY, len));

        if (len == 0) return trg;
        if (len > 1) {
                qsort((void *) GETARR(trg), len, sizeof(trgm), comp_trgm);
                len = unique_array(GETARR(trg), len);
        }
        SET_VARSIZE(trg, CALCGTSIZE(ARRKEY, len));
        return trg;
}

Datum
similarity(PG_FUNCTION_ARGS)
{
        text       *in1 = PG_GETARG_TEXT_PP(0);
        text       *in2 = PG_GETARG_TEXT_PP(1);
        TRGM       *trg1,
                           *trg2;
        float4          res;

        trg1 = generate_trgm(VARDATA_ANY(in1), VARSIZE_ANY_EXHDR(in1));
        trg2 = generate_trgm(VARDATA_ANY(in2), VARSIZE_ANY_EXHDR(in2));

        res = cnt_sml(trg1, trg2, false);

        pfree(trg1);
        pfree(trg2);
        PG_FREE_IF_COPY(in1, 0);
        PG_FREE_IF_COPY(in2, 1);

        PG_RETURN_FLOAT4(res);
}


Datum
similarity_op(PG_FUNCTION_ARGS)
{
        float4    res = DatumGetFloat4(DirectFunctionCall2(similarity,
                                          PG_GETARG_DATUM(0),
                                          PG_GETARG_DATUM(1)));

        PG_RETURN_BOOL(res >= similarity_threshold);
}



TRGM *
generate_wildcard_trgm(const char *str, int slen)
{
	TRGM	   *trg;
	char	   *buf,
			   *buf2;
	trgm	   *tptr;
	int			len,
				charlen,
				bytelen;
	const char *eword;

	protect_out_of_mem(slen);

	trg = (TRGM *) palloc(TRGMHDRSIZE + sizeof(trgm) * (slen / 2 + 1) * 3);
	trg->flag = ARRKEY;
	SET_VARSIZE(trg, TRGMHDRSIZE);

	if (slen + LPADDING + RPADDING < 3 || slen == 0)
		return trg;

	tptr = GETARR(trg);

	/* Allocate a buffer for blank-padded, but not yet case-folded, words */
	buf = palloc(sizeof(char) * (slen + 4));

	/*
	 * Extract trigrams from each substring extracted by get_wildcard_part.
	 */
	eword = str;
	while ((eword = get_wildcard_part(eword, slen - (eword - str),
									  buf, &bytelen, &charlen)) != NULL)
	{
#ifdef IGNORECASE
		buf2 = lowerstr_with_len(buf, bytelen);
		bytelen = strlen(buf2);
#else
		buf2 = buf;
#endif

		/*
		 * count trigrams
		 */
		tptr = make_trigrams(tptr, buf2, bytelen, charlen);

#ifdef IGNORECASE
		pfree(buf2);
#endif
	}

	pfree(buf);

	if ((len = tptr - GETARR(trg)) == 0)
		return trg;

	/*
	 * Make trigrams unique.
	 */
	if (len > 1)
	{
		qsort((void *) GETARR(trg), len, sizeof(trgm), comp_trgm);
		len = unique_array(GETARR(trg), len);
	}

	SET_VARSIZE(trg, CALCGTSIZE(ARRKEY, len));

	return trg;
}

