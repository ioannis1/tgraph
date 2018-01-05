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

#include "tgraph_ops.c"
#include "access/gin.h"
#include "access/stratnum.h"
#include "trgm_regexp.c"

PG_FUNCTION_INFO_V1(gin_extract_value_trgm);
PG_FUNCTION_INFO_V1(gin_extract_query_trgm);
PG_FUNCTION_INFO_V1(gin_trgm_consistent);

const char *
get_wildcard_part(const char *str, int lenstr,
				  char *buf, int *bytelen, int *charlen)
{
	const char *beginword = str;
	const char *endword;
	char	   *s = buf;
	bool		in_leading_wildcard_meta = false;
	bool		in_trailing_wildcard_meta = false;
	bool		in_escape = false;
	int			clen;

	/*
	 * Find the first word character, remembering whether preceding character
	 * was wildcard meta-character.  Note that the in_escape state persists
	 * from this loop to the next one, since we may exit at a word character
	 * that is in_escape.
	 */
	while (beginword - str < lenstr)
	{
		if (in_escape)
		{
			if (ISWORDCHR(beginword))
				break;
			in_escape = false;
			in_leading_wildcard_meta = false;
		}
		else
		{
			if (ISESCAPECHAR(beginword))
				in_escape = true;
			else if (ISWILDCARDCHAR(beginword))
				in_leading_wildcard_meta = true;
			else if (ISWORDCHR(beginword))
				break;
			else
				in_leading_wildcard_meta = false;
		}
		beginword += pg_mblen(beginword);
	}

	/*
	 * Handle string end.
	 */
	if (beginword - str >= lenstr)
		return NULL;

	/*
	 * Add left padding spaces if preceding character wasn't wildcard
	 * meta-character.
	 */
	*charlen = 0;
	if (!in_leading_wildcard_meta)
	{
		if (LPADDING > 0)
		{
			*s++ = ' ';
			(*charlen)++;
			if (LPADDING > 1)
			{
				*s++ = ' ';
				(*charlen)++;
			}
		}
	}

	/*
	 * Copy data into buf until wildcard meta-character, non-word character or
	 * string boundary.  Strip escapes during copy.
	 */
	endword = beginword;
	while (endword - str < lenstr)
	{
		clen = pg_mblen(endword);
		if (in_escape)
		{
			if (ISWORDCHR(endword))
			{
				memcpy(s, endword, clen);
				(*charlen)++;
				s += clen;
			}
			else
			{
				/*
				 * Back up endword to the escape character when stopping at an
				 * escaped char, so that subsequent get_wildcard_part will
				 * restart from the escape character.  We assume here that
				 * escape chars are single-byte.
				 */
				endword--;
				break;
			}
			in_escape = false;
		}
		else
		{
			if (ISESCAPECHAR(endword))
				in_escape = true;
			else if (ISWILDCARDCHAR(endword))
			{
				in_trailing_wildcard_meta = true;
				break;
			}
			else if (ISWORDCHR(endword))
			{
				memcpy(s, endword, clen);
				(*charlen)++;
				s += clen;
			}
			else
				break;
		}
		endword += clen;
	}

	/*
	 * Add right padding spaces if next character isn't wildcard
	 * meta-character.
	 */
	if (!in_trailing_wildcard_meta)
	{
		if (RPADDING > 0)
		{
			*s++ = ' ';
			(*charlen)++;
			if (RPADDING > 1)
			{
				*s++ = ' ';
				(*charlen)++;
			}
		}
	}

	*bytelen = s - buf;
	return endword;
}

Datum
gin_extract_value_trgm(PG_FUNCTION_ARGS)
{
        text       *val = (text *) PG_GETARG_TEXT_PP(0);
        int32      *nentries = (int32 *) PG_GETARG_POINTER(1);
        Datum      *entries = NULL;
        TRGM       *trg;
        int32           trglen;

        *nentries = 0;

        trg = generate_trgm(VARDATA_ANY(val), VARSIZE_ANY_EXHDR(val));
        trglen = ARRNELEM(trg);

        if (trglen > 0)
        {
                trgm       *ptr;
                int32           i;

                *nentries = trglen;
                entries = (Datum *) palloc(sizeof(Datum) * trglen);

                ptr = GETARR(trg);
                for (i = 0; i < trglen; i++)
                {
                        int32           item = trgm2int(ptr);

                        entries[i] = Int32GetDatum(item);
                        ptr++;
                }
        }

        PG_RETURN_POINTER(entries);
}




Datum
gin_extract_query_trgm(PG_FUNCTION_ARGS)
{
        text       *val = (text *) PG_GETARG_TEXT_PP(0);
        int32      *nentries = (int32 *) PG_GETARG_POINTER(1);
        StrategyNumber strategy = PG_GETARG_UINT16(2);

        /* bool   **pmatch = (bool **) PG_GETARG_POINTER(3); */
        Pointer   **extra_data = (Pointer **) PG_GETARG_POINTER(4);

        /* bool   **nullFlags = (bool **) PG_GETARG_POINTER(5); */
        int32      *searchMode = (int32 *) PG_GETARG_POINTER(6);
        Datum      *entries = NULL;
        TRGM       *trg;
        int32           trglen;
        trgm       *ptr;
        TrgmPackedGraph *graph;
        int32           i;

        switch (strategy)
        {
                case SimilarityStrategyNumber:
                case WordSimilarityStrategyNumber:
                        trg = generate_trgm(VARDATA_ANY(val), VARSIZE_ANY_EXHDR(val));
                        break;
                case ILikeStrategyNumber:
#ifndef IGNORECASE
                        elog(ERROR, "cannot handle ~~* with case-sensitive trigrams");
#endif
                        /* FALL THRU */
                case LikeStrategyNumber:
                        /*
                         * For wildcard search we extract all the trigrams that every
                         * potentially-matching string must include.
                         */
                        trg = generate_wildcard_trgm(VARDATA_ANY(val),
                                                                                 VARSIZE_ANY_EXHDR(val));
                        break;
                case RegExpICaseStrategyNumber:
#ifndef IGNORECASE
                        elog(ERROR, "cannot handle ~* with case-sensitive trigrams");
#endif
                        /* FALL THRU */
                case RegExpStrategyNumber:
                        trg = createTrgmNFA(val, PG_GET_COLLATION(),
                                                                &graph, CurrentMemoryContext);
                        if (trg && ARRNELEM(trg) > 0)
                        {
                                /*
                                 * Successful regex processing: store NFA-like graph as
                                 * extra_data.  GIN API requires an array of nentries
                                 * Pointers, but we just put the same value in each element.
                                 */
                                trglen = ARRNELEM(trg);
                                *extra_data = (Pointer *) palloc(sizeof(Pointer) * trglen);
                                for (i = 0; i < trglen; i++)
                                        (*extra_data)[i] = (Pointer) graph;
                        }
                        else
                        {
                                /* No result: have to do full index scan. */
                                *nentries = 0;
                                *searchMode = GIN_SEARCH_MODE_ALL;
                                PG_RETURN_POINTER(entries);
                        }
                        break;
                default:
                        elog(ERROR, "unrecognized strategy number: %d", strategy);
                        trg = NULL;                     /* keep compiler quiet */
                        break;
        }

        trglen = ARRNELEM(trg);
        *nentries = trglen;

        if (trglen > 0)
        {
                entries = (Datum *) palloc(sizeof(Datum) * trglen);
                ptr = GETARR(trg);
                for (i = 0; i < trglen; i++)
                {
                        int32           item = trgm2int(ptr);

                        entries[i] = Int32GetDatum(item);
                        ptr++;
                }
        }
       /*
         * If no trigram was extracted then we have to scan all the index.
         */
        if (trglen == 0)
                *searchMode = GIN_SEARCH_MODE_ALL;

        PG_RETURN_POINTER(entries);
}

Datum
gin_trgm_consistent(PG_FUNCTION_ARGS)
{
        bool       *check = (bool *) PG_GETARG_POINTER(0);
        StrategyNumber strategy = PG_GETARG_UINT16(1);


        /* text    *query = PG_GETARG_TEXT_PP(2); */
        int32           nkeys = PG_GETARG_INT32(3);
        Pointer    *extra_data = (Pointer *) PG_GETARG_POINTER(4);
        bool       *recheck = (bool *) PG_GETARG_POINTER(5);
        bool            res;
        int32           i,
                                ntrue;
        double          nlimit;

        /* All cases served by this function are inexact */
        *recheck = true;

        switch (strategy)
        {
                case SimilarityStrategyNumber:
                case WordSimilarityStrategyNumber:
                        nlimit = (strategy == SimilarityStrategyNumber) ?
                                similarity_threshold : word_similarity_threshold;

                        /* Count the matches */
                        ntrue = 0;
                        for (i = 0; i < nkeys; i++)
                        {
                                if (check[i])
                                        ntrue++;
                        }


                        /*--------------------
                         * If DIVUNION is defined then similarity formula is:
                         * c / (len1 + len2 - c)
                         * where c is number of common trigrams and it stands as ntrue in
                         * this code.  Here we don't know value of len2 but we can assume
                         * that c (ntrue) is a lower bound of len2, so upper bound of
                         * similarity is:
                         * c / (len1 + c - c)  => c / len1
                         * If DIVUNION is not defined then similarity formula is:
                         * c / max(len1, len2)
                         * And again, c (ntrue) is a lower bound of len2, but c <= len1
                         * just by definition and, consequently, upper bound of
                         * similarity is just c / len1.
                         * So, independently on DIVUNION the upper bound formula is the same.
                         */
                        res = (nkeys == 0) ? false :
                                (((((float4) ntrue) / ((float4) nkeys))) >= nlimit);
                        break;
                case ILikeStrategyNumber:
#ifndef IGNORECASE
                        elog(ERROR, "cannot handle ~~* with case-sensitive trigrams");
#endif
                        /* FALL THRU */
                case LikeStrategyNumber:
                        /* Check if all extracted trigrams are presented. */
                        res = true;
                        for (i = 0; i < nkeys; i++)
                        {
                                if (!check[i])
                                {
                                        res = false;
                                        res = false;
                                        break;
                                }
                        }
                        break;
                case RegExpICaseStrategyNumber:
#ifndef IGNORECASE
                        elog(ERROR, "cannot handle ~* with case-sensitive trigrams");
#endif
                        /* FALL THRU */
                case RegExpStrategyNumber:
                        if (nkeys < 1)
                        {
                                /* Regex processing gave no result: do full index scan */
                                res = true;
                        }
                        else
                                res = trigramsMatchGraph((TrgmPackedGraph *) extra_data[0],
                                                                                 check);
                        break;
                default:
                        elog(ERROR, "unrecognized strategy number: %d", strategy);
                        res = false;            /* keep compiler quiet */
                        break;
        }

        PG_RETURN_BOOL(res);
}


