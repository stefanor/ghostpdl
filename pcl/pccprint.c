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
/*$Id$ */

/* pccprint.c - PCL5c print model commands */

#include "std.h"
#include "pcommand.h"
#include "pcstate.h"
#include "pcfont.h"
#include "gsmatrix.h"		/* for gsstate.h */
#include "gsstate.h"
#include "gsrop.h"

/*
 * ESC * l <rop> O
 *
 * Set logical operation.
 */
  private int
pcl_logical_operation(
    pcl_args_t *    pargs,
    pcl_state_t *   pcs
)
{
    uint            rop = uint_arg(pargs);

    if (rop > 255)
	return e_Range;

    pcl_break_underline(pcs);   /* use the 5c convention; in 5e, the
                                 * underline is not broken by a change in
                                 * the logical operation */
    pcs->logical_op = rop;
    return 0;
}

/*
 * ESC * l <bool> R
 *
 * Set prixel placement. Note that this feature is not yet properly
 * implemented.
 */
  private int
pcl_pixel_placement(
    pcl_args_t *    pargs,
    pcl_state_t *   pcs
)
{
    uint            i = uint_arg(pargs);

    if (i > 1)
	return 0;
    pcs->pp_mode = i;
    return 0;
}

/*
 * Initialization
 */
  private int
pccprint_do_registration(
    pcl_parser_state_t *pcl_parser_state,
    gs_memory_t *   pmem
)
{
    /* Register commands */
    DEFINE_CLASS('*')
    {
        'l', 'O',
	PCL_COMMAND( "Logical Operation",
                     pcl_logical_operation,
		     pca_neg_ok | pca_big_error | pca_in_rtl
                     )
    },
    {
        'l', 'R',
        PCL_COMMAND( "Pixel Placement",
                     pcl_pixel_placement,
		     pca_neg_ok | pca_big_ignore | pca_in_rtl
                     )
    },
    END_CLASS
    return 0;
}

  private void
pccprint_do_reset(
    pcl_state_t *       pcs,
    pcl_reset_type_t    type
)
{
    static  const uint  mask = (  pcl_reset_initial
                                | pcl_reset_printer
                                | pcl_reset_overlay );

    if ((type & mask) != 0) {
        pcs->logical_op = 252;
        pcs->pp_mode = 0;
    }
}

const pcl_init_t    pccprint_init = { pccprint_do_registration, pccprint_do_reset, 0 };
