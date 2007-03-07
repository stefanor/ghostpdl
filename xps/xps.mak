XPSSRC      = $(XPSSRCDIR)$(D)
XPSGEN      = $(XPSGENDIR)$(D)
XPSOBJ      = $(XPSOBJDIR)$(D)
XPSO_       = $(O_)$(XPSOBJ)

XPSCCC  = $(CC_) $(I_)$(XPSSRCDIR)$(_I) $(I_)$(XPSGENDIR)$(_I) $(I_)$(PLSRCDIR)$(_I) $(I_)$(GLSRCDIR)$(_I) $(I_)$(EXPATINCDIR)$(_I) $(C_)

# Define the name of this makefile.
XPS_MAK     = $(XPSSRC)xps.mak

xps.clean: xps.config-clean xps.clean-not-config-clean

xps.clean-not-config-clean:
	$(RM_) $(XPSOBJ)*.$(OBJ)

xps.config-clean: clean_gs
	$(RM_) $(XPSOBJ)*.dev
	$(RM_) $(XPSOBJ)devs.tr5


XPSINCLUDES=$(XPSSRC)*.h


$(XPSOBJ)xpsmem.$(OBJ): $(XPSSRC)xpsmem.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsmem.c $(XPSO_)xpsmem.$(OBJ)

$(XPSOBJ)xpsjpeg.$(OBJ): $(XPSSRC)xpsjpeg.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsjpeg.c $(XPSO_)xpsjpeg.$(OBJ)

$(XPSOBJ)xpspng.$(OBJ): $(XPSSRC)xpspng.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpspng.c $(XPSO_)xpspng.$(OBJ)

$(XPSOBJ)xpstiff.$(OBJ): $(XPSSRC)xpstiff.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpstiff.c $(XPSO_)xpstiff.$(OBJ)

$(XPSOBJ)xpszip.$(OBJ): $(XPSSRC)xpszip.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpszip.c $(XPSO_)xpszip.$(OBJ)

$(XPSOBJ)xpsxml.$(OBJ): $(XPSSRC)xpsxml.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsxml.c $(XPSO_)xpsxml.$(OBJ)

$(XPSOBJ)xpsdoc.$(OBJ): $(XPSSRC)xpsdoc.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsdoc.c $(XPSO_)xpsdoc.$(OBJ)

$(XPSOBJ)xpspage.$(OBJ): $(XPSSRC)xpspage.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpspage.c $(XPSO_)xpspage.$(OBJ)

$(XPSOBJ)xpscommon.$(OBJ): $(XPSSRC)xpscommon.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpscommon.c $(XPSO_)xpscommon.$(OBJ)

$(XPSOBJ)xpscolor.$(OBJ): $(XPSSRC)xpscolor.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpscolor.c $(XPSO_)xpscolor.$(OBJ)

$(XPSOBJ)xpspath.$(OBJ): $(XPSSRC)xpspath.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpspath.c $(XPSO_)xpspath.$(OBJ)

$(XPSOBJ)xpsvisual.$(OBJ): $(XPSSRC)xpsvisual.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsvisual.c $(XPSO_)xpsvisual.$(OBJ)

$(XPSOBJ)xpsimage.$(OBJ): $(XPSSRC)xpsimage.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsimage.c $(XPSO_)xpsimage.$(OBJ)

$(XPSOBJ)xpsgradient.$(OBJ): $(XPSSRC)xpsgradient.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsgradient.c $(XPSO_)xpsgradient.$(OBJ)

$(XPSOBJ)xpsglyphs.$(OBJ): $(XPSSRC)xpsglyphs.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsglyphs.c $(XPSO_)xpsglyphs.$(OBJ)

$(XPSOBJ)xpsfont1.$(OBJ): $(XPSSRC)xpsfont1.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsfont1.c $(XPSO_)xpsfont1.$(OBJ)

$(XPSOBJ)xpsfont2.$(OBJ): $(XPSSRC)xpsfont2.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsfont2.c $(XPSO_)xpsfont2.$(OBJ)

$(XPSOBJ)xpsttf.$(OBJ): $(XPSSRC)xpsttf.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpsttf.c $(XPSO_)xpsttf.$(OBJ)

$(XPSOBJ)xpscff.$(OBJ): $(XPSSRC)xpscff.c $(XPSINCLUDES)
	$(XPSCCC) $(XPSSRC)xpscff.c $(XPSO_)xpscff.$(OBJ)


$(XPS_TOP_OBJ): $(XPSSRC)xpstop.c $(pltop_h) $(XPSINCLUDES)
	$(CP_) $(XPSGEN)pconf.h $(XPSGEN)pconfig.h
	$(XPSCCC) $(XPSSRC)xpstop.c $(XPSO_)xpstop.$(OBJ)

XPS_OBJS=\
    $(XPSOBJ)xpsmem.$(OBJ) \
    $(XPSOBJ)xpsjpeg.$(OBJ) \
    $(XPSOBJ)xpspng.$(OBJ) \
    $(XPSOBJ)xpstiff.$(OBJ) \
    $(XPSOBJ)xpszip.$(OBJ) \
    $(XPSOBJ)xpsxml.$(OBJ) \
    $(XPSOBJ)xpsdoc.$(OBJ) \
    $(XPSOBJ)xpspage.$(OBJ) \
    $(XPSOBJ)xpscommon.$(OBJ) \
    $(XPSOBJ)xpscolor.$(OBJ) \
    $(XPSOBJ)xpspath.$(OBJ) \
    $(XPSOBJ)xpsvisual.$(OBJ) \
    $(XPSOBJ)xpsimage.$(OBJ) \
    $(XPSOBJ)xpsgradient.$(OBJ) \
    $(XPSOBJ)xpsglyphs.$(OBJ) \
    $(XPSOBJ)xpsfont1.$(OBJ) \
    $(XPSOBJ)xpsfont2.$(OBJ) \
    $(XPSOBJ)xpsttf.$(OBJ) \
    $(XPSOBJ)xpscff.$(OBJ) \

$(XPSOBJ)xps.dev: $(XPS_MAK) $(ECHOGS_XE) $(XPS_OBJS)
	$(SETMOD) $(XPSOBJ)xps $(XPS_OBJS)

