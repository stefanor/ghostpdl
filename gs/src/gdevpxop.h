/* Portions Copyright (C) 2001 artofcode LLC.
   Portions Copyright (C) 1996, 2001 Artifex Software Inc.
   Portions Copyright (C) 1988, 2000 Aladdin Enterprises.
   This software is based in part on the work of the Independent JPEG Group.
   All Rights Reserved.

   This software is distributed under license and may not be copied, modified
   or distributed except as expressly authorized under the terms of that
   license.  Refer to licensing information at http://www.artifex.com/ or
   contact Artifex Software, Inc., 101 Lucas Valley Road #110,
   San Rafael, CA  94903, (415)492-9861, for further information. */

/*$RCSfile$ $Revision$ */
/* Operator and other tag definitions for PCL XL */

#ifndef gdevpxop_INCLUDED
#  define gdevpxop_INCLUDED

typedef enum {
/*0x */
    pxtNull = 0x00, pxt01, pxt02, pxt03,
    pxt04, pxt05, pxt06, pxt07,
    pxt08, pxtHT, pxtLF, pxtVT,
    pxtFF, pxtCR, pxt0e, pxt0f,
/*1x */
    pxt10, pxt11, pxt12, pxt13,
    pxt14, pxt15, pxt16, pxt17,
    pxt18, pxt19, pxt1a, pxt1b,
    pxt1c, pxt1d, pxt1e, pxt1f,
/*2x */
    pxtSpace, pxt21, pxt22, pxt23,
    pxt24, pxt25, pxt26, pxt_beginASCII,
    pxt_beginBinaryMSB, pxt_beginBinaryLSB, pxt2a, pxt2b,
    pxt2c, pxt2d, pxt2e, pxt2f,
/*3x */
    pxt30, pxt31, pxt32, pxt33,
    pxt34, pxt35, pxt36, pxt37,
    pxt38, pxt39, pxt3a, pxt3b,
    pxt3c, pxt3d, pxt3e, pxt3f,
/*4x */
    pxt40, pxtBeginSession, pxtEndSession, pxtBeginPage,
    pxtEndPage, pxt45, pxt46, pxtComment,
    pxtOpenDataSource, pxtCloseDataSource, pxt4a, pxt4b,
    pxt4c, pxt4d, pxt4e, pxtBeginFontHeader,
/*5x */
    pxtReadFontHeader, pxtEndFontHeader, pxtBeginChar, pxtReadChar,
    pxtEndChar, pxtRemoveFont, pxtSetCharAttributes /*2.0 */ , pxt57,
    pxtSetColorTreatment, pxt59, pxt5a, pxtBeginStream,
    pxtReadStream, pxtEndStream, pxtExecStream, pxtRemoveStream /*2.0 */ ,
/*6x */
    pxtPopGS, pxtPushGS, pxtSetClipReplace, pxtSetBrushSource,
    pxtSetCharAngle, pxtSetCharScale, pxtSetCharShear, pxtSetClipIntersect,
    pxtSetClipRectangle, pxtSetClipToPage, pxtSetColorSpace, pxtSetCursor,
    pxtSetCursorRel, pxtSetHalftoneMethod, pxtSetFillMode, pxtSetFont,
/*7x */
    pxtSetLineDash, pxtSetLineCap, pxtSetLineJoin, pxtSetMiterLimit,
    pxtSetPageDefaultCTM, pxtSetPageOrigin, pxtSetPageRotation, pxtSetPageScale,
    pxtSetPaintTxMode, pxtSetPenSource, pxtSetPenWidth, pxtSetROP,
    pxtSetSourceTxMode, pxtSetCharBoldValue, pxtSetNeutralAxis, pxtSetClipMode,
/*8x */
    pxtSetPathToClip, pxtSetCharSubMode, pxtBeginUserDefinedLineCap, pxtEndUserDefinedLineCap,
    pxtCloseSubPath, pxtNewPath, pxtPaintPath, pxt87,
    pxt88, pxt89, pxt8a, pxt8b,
    pxt8c, pxt8d, pxt8e, pxt8f,
/*9x */
    pxt90, pxtArcPath, pxtSetColorTrapping, pxtBezierPath,
    pxtSetAdaptiveHalftoning, pxtBezierRelPath, pxtChord, pxtChordPath,
    pxtEllipse, pxtEllipsePath, pxt9a, pxtLinePath,
    pxt9c, pxtLineRelPath, pxtPie, pxtPiePath,
/*ax */
    pxtRectangle, pxtRectanglePath, pxtRoundRectangle, pxtRoundRectanglePath,
    pxta4, pxta5, pxta6, pxta7,
    pxtText, pxtTextPath, pxtaa, pxtab,
    pxtac, pxtad, pxtae, pxtaf,
/*bx */
    pxtBeginImage, pxtReadImage, pxtEndImage, pxtBeginRastPattern,
    pxtReadRastPattern, pxtEndRastPattern, pxtBeginScan, pxtb7,
    pxtEndScan, pxtScanLineRel, pxtba, pxtbb,
    pxtbc, pxtbd, pxtbe, pxtPassthrough,
/*cx */
    pxt_ubyte, pxt_uint16, pxt_uint32, pxt_sint16,
    pxt_sint32, pxt_real32, pxtc6, pxtc7,
    pxt_ubyte_array, pxt_uint16_array, pxt_uint32_array, pxt_sint16_array,
    pxt_sint32_array, pxt_real32_array, pxtce, pxtcf,
/*dx */
    pxt_ubyte_xy, pxt_uint16_xy, pxt_uint32_xy, pxt_sint16_xy,
    pxt_sint32_xy, pxt_real32_xy, pxtd6, pxtd7,
    pxtd8, pxtd9, pxtda, pxtdb,
    pxtdc, pxtdd, pxtde, pxtdf,
/*ex */
    pxt_ubyte_box, pxt_uint16_box, pxt_uint32_box, pxt_sint16_box,
    pxt_sint32_box, pxt_real32_box, pxte6, pxte7,
    pxte8, pxte9, pxtea, pxteb,
    pxtec, pxted, pxtee, pxtef,
/*fx */
    pxtf0, pxtf1, pxtf2, pxtf3,
    pxtf4, pxtf5, pxtf6, pxtf7,
    pxt_attr_ubyte, pxt_attr_uint16, pxt_dataLength, pxt_dataLengthByte,
    pxtfc, pxtfd, pxtfe, pxtff
} px_tag_t;

#endif /* gdevpxop_INCLUDED */
