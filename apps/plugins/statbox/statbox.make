#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

STATBOXSRCDIR := $(APPSDIR)/plugins/statbox
STATBOXBUILDDIR := $(BUILDDIR)/apps/plugins/statbox

ROCKS += $(STATBOXBUILDDIR)/statbox.rock

STATBOX_SRC := $(call preprocess, $(STATBOXSRCDIR)/SOURCES)
STATBOX_OBJ := $(call c2obj, $(STATBOX_SRC))

# add source files to OTHER_SRC to get automatic dependencies
OTHER_SRC += $(STATBOX_SRC)

$(STATBOXBUILDDIR)/statbox.rock: $(STATBOX_OBJ)
