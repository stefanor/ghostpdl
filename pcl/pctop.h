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

/* pctop.h */
/* Interface to main program utilities for PCL5 interpreter */

#ifndef pctop_INCLUDED
#  define pctop_INCLUDED

/* Set PCL's target device */
void pcl_set_target_device(P2(pcl_state_t *pcs, gx_device *pdev));

/* Get PCL's curr target device */
gx_device * pcl_get_target_device(P1(pcl_state_t *pcs));


#endif				/* pctop_INCLUDED */
