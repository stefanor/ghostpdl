#include "ghostxps.h"

/*
 * Big-endian memory accessor functions
 */

private inline int s16(byte *p)
{
    return (signed short)( (p[0] << 8) | p[1] );
}

private inline int u16(byte *p)
{
    return (p[0] << 8) | p[1];
}

private inline int u24(byte *p)
{
    return (p[0] << 16) | (p[1] << 8) | p[2];
}

private inline int u32(byte *p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}


int xps_init_font_cache(xps_context_t *ctx)
{
    const int smax = 50;         /* number of scaled fonts */
    const int bmax = 500000;     /* space for cached chars */
    const int mmax = 200;        /* number of cached font/matrix pairs */
    const int cmax = 5000;       /* number of cached chars */
    const int upper = 32000;     /* max size of a single cached char */

    ctx->fontdir = gs_font_dir_alloc2_limits(ctx->memory, ctx->memory, smax, bmax, mmax, cmax, upper);
    if (!ctx->fontdir)
	return gs_throw(-1, "cannot gs_font_dir_alloc2_limits()");

    gs_setaligntopixels(ctx->fontdir, 1); /* no subpixels */
    gs_setgridfittt(ctx->fontdir, 3); /* see gx_ttf_outline for values */

    return gs_okay;
}

void xps_free_font_cache(xps_context_t *ctx)
{
    /* ahem... is that all? */
    gs_free_object(ctx->memory, ctx->fontdir, "gs_font_dir");
    ctx->fontdir = NULL;
}


xps_font_t *
xps_new_font(xps_context_t *ctx, char *buf, int buflen, int index)
{
    xps_font_t *font;
    int code;

    font = xps_alloc(ctx, sizeof(xps_font_t));
    if (!font)
    {
	gs_throw(-1, "out of memory");
	return NULL;
    }

    font->data = (byte*)buf;
    font->length = buflen;
    font->font = NULL;

    font->subfontid = index;
    font->cmaptable = 0;
    font->cmapsubcount = 0;
    font->cmapsubtable = 0;
    font->usepua = 0;

    font->cffdata = 0;
    font->cffend = 0;
    font->gsubrs = 0;
    font->subrs = 0;
    font->charstrings = 0;

    if (memcmp(font->data, "OTTO", 4) == 0)
	code = xps_init_postscript_font(ctx, font);
    else if (memcmp(font->data, "\0\1\0\0", 4) == 0)
	code = xps_init_truetype_font(ctx, font);
    else if (memcmp(font->data, "true", 4) == 0)
	code = xps_init_truetype_font(ctx, font);
    else if (memcmp(font->data, "ttcf", 4) == 0)
	code = xps_init_truetype_font(ctx, font);
    else
    {
	xps_free_font(ctx, font);
	gs_throw(-1, "not an opentype font");
	return NULL;
    }

    if (code < 0)
    {
	xps_free_font(ctx, font);
	gs_rethrow(-1, "cannot init font");
	return NULL;
    }

    code = xps_load_sfnt_cmap(font);
    if (code < 0)
    {
	errprintf("warning: no cmap table found in font\n");
    }

    return font;
}

void
xps_free_font(xps_context_t *ctx, xps_font_t *font)
{
    if (font->font)
    {
	gs_font_finalize(font->font);
	gs_free_object(ctx->memory, font->font, "font object");
    }
    xps_free(ctx, font);
}


/*
 * Find the offset and length of an SFNT table.
 * Return -1 if no table by the specified name is found.
 */

int
xps_find_sfnt_table(xps_font_t *font, char *name, int *lengthp)
{
    byte *directory;
    int offset;
    int ntables;
    int i;

    if (font->length < 12)
	return -1;

    if (!memcmp(font->data, "ttcf", 4))
    {
	int nfonts = u32(font->data + 8);
	if (font->subfontid < 0 || font->subfontid >= nfonts)
	    return gs_throw(-1, "Invalid subfont ID");
	offset = u32(font->data + 12 + font->subfontid * 4);
    }
    else
    {
	offset = 0;
    }

    ntables = u16(font->data + offset + 4);
    if (font->length < offset + 12 + ntables * 16)
	return -1;

    for (i = 0; i < ntables; i++)
    {
	byte *entry = font->data + offset + 12 + i * 16;
	if (!memcmp(entry, name, 4))
	{
	    if (lengthp)
		*lengthp = u32(entry + 12);
	    return u32(entry + 8);
	}
    }

    return -1;
}


/*
 * Locate the 'cmap' table and count the number of subtables.
 */

int
xps_load_sfnt_cmap(xps_font_t *font)
{
    byte *cmapdata;
    int offset, length;
    int nsubtables;

    offset = xps_find_sfnt_table(font, "cmap", &length);
    if (offset < 0)
	return -1;

    if (length < 4)
	return -1;

    cmapdata = font->data + offset;

    nsubtables = u16(cmapdata + 2);
    if (nsubtables < 0)
	return -1;
    if (length < 4 + nsubtables * 8)
	return -1;

    font->cmaptable = offset;
    font->cmapsubcount = nsubtables;
    font->cmapsubtable = 0;

    return 0;
}

/*
 * Return the number of cmap subtables.
 */

int
xps_count_font_encodings(xps_font_t *font)
{
    return font->cmapsubcount;
}

/*
 * Extract PlatformID and EncodingID for a cmap subtable.
 */

int
xps_identify_font_encoding(xps_font_t *font, int idx, int *pid, int *eid)
{
    byte *cmapdata, *entry;
    if (idx < 0 || idx >= font->cmapsubcount)
	return -1;
    cmapdata = font->data + font->cmaptable;
    entry = cmapdata + 4 + idx * 8;
    *pid = u16(entry + 0);
    *eid = u16(entry + 2);
    return 0;
}

/*
 * Select a cmap subtable for use with encoding functions.
 */

int
xps_select_font_encoding(xps_font_t *font, int idx)
{
    byte *cmapdata, *entry;
    int pid, eid;
    if (idx < 0 || idx >= font->cmapsubcount)
	return -1;
    cmapdata = font->data + font->cmaptable;
    entry = cmapdata + 4 + idx * 8;
    pid = u16(entry + 0);
    eid = u16(entry + 2);
    font->cmapsubtable = font->cmaptable + u32(entry + 4);
    font->usepua = (pid == 3 && eid == 0);
    return 0;
}


/*
 * Encode a character using the selected cmap subtable.
 * TODO: extend this to cover more cmap formats.
 */

static int
xps_encode_font_char_int(xps_font_t *font, int code)
{
    byte *table;

    /* no cmap selected: return identity */
    if (font->cmapsubtable <= 0)
	return code;

    table = font->data + font->cmapsubtable;

    switch (u16(table))
    {
    case 0: /* Apple standard 1-to-1 mapping. */
	return table[code + 6];

    case 4: /* Microsoft/Adobe segmented mapping. */
	{
	    int segCount2 = u16(table + 6);
	    byte *endCount = table + 14;
	    byte *startCount = endCount + segCount2 + 2;
	    byte *idDelta = startCount + segCount2;
	    byte *idRangeOffset = idDelta + segCount2;
	    int i2;

	    for (i2 = 0; i2 < segCount2 - 3; i2 += 2)
	    {
		int delta, roff;
		int start = u16(startCount + i2);
		int glyph;

		if ( code < start )
		    return 0;
		if ( code > u16(endCount + i2) )
		    continue;
		delta = s16(idDelta + i2);
		roff = s16(idRangeOffset + i2);
		if ( roff == 0 )
		{
		    return ( code + delta ) & 0xffff; /* mod 65536 */
		    return 0;
		}
		glyph = u16(idRangeOffset + i2 + roff + ((code - start) << 1));
		return (glyph == 0 ? 0 : glyph + delta);
	    }

	    /*
	     * The TrueType documentation says that the last range is
	     * always supposed to end with 0xffff, so this shouldn't
	     * happen; however, in some real fonts, it does.
	     */
	    return 0;
	}

    case 6: /* Single interval lookup. */
	{
	    int firstCode = u16(table + 6);
	    int entryCount = u16(table + 8);
	    if ( code < firstCode || code >= firstCode + entryCount )
		return 0;
	    return u16(table + 10 + ((code - firstCode) << 1));
	}

    case 10: /* Trimmed array (like 6) */
	{
	    int startCharCode = u32(table + 12);
	    int numChars = u32(table + 16);
	    if ( code < startCharCode || code >= startCharCode + numChars )
		return 0;
	    return u32(table + 20 + (code - startCharCode) * 4);
	}

    case 12: /* Segmented coverage. (like 4) */
	{
	    int nGroups = u32(table + 12);
	    byte *group = table + 16;
	    int i;

	    for (i = 0; i < nGroups; i++)
	    {
		int startCharCode = u32(group + 0);
		int endCharCode = u32(group + 4);
		int startGlyphID = u32(group + 8);
		if ( code < startCharCode )
		    return 0;
		if ( code <= endCharCode )
		    return startGlyphID + (code - startCharCode);
		group += 12;
	    }

	    return 0;
	}

    case 2: /* High-byte mapping through table. */
    case 8: /* Mixed 16-bit and 32-bit coverage (like 2) */
    default:
	errprintf("error: unknown cmap format: %d\n", u16(table));
	return 0;
    }

    return 0;
}

int
xps_encode_font_char(xps_font_t *font, int code)
{
    int gid = xps_encode_font_char_int(font, code);
    if (gid == 0 && font->usepua)
	gid = xps_encode_font_char_int(font, 0xF000 | code);
    return gid;
}


