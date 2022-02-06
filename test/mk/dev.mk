
##############################################################
# Code under development.
#
# Strip out this section before we package and release.

# OpenCilk build
INCLUDE_FLAGS += -I$(CILKPUB_TOP)/include_dev
CXXFLAGS+= -DCILKPUB_DETRED_DBG_LEVEL=0 -fopencilk-enable-pedigrees

DEV_TEST_NAMES = rank_range_tag detred_iview_ranks detred_iview	\
detred_opadd fib_det_opadd loopsum_opadd
##############################################################
