INCLUDES = -I../../include -I../include

noinst_LTLIBRARIES = libial.la

if NATIVE_IAL_ENGINE
SUBDIRS=native
endif

DIST_SUBDIR=native

EXTRA_DIST=spec_general_ts_drv.html 

COMMON_SRCS = ial.c

IPAQ_SRCS = ipaq.c ipaq.h
L7200_SRCS = l7200.c l7200.h
ARM3000_SRCS = arm3000.c arm3000.h
EMBEST2410_SRCS = embest2410.c embest2410.h
PX255B_SRCS = px255b.c px255b.h
DUMMY_SRCS = dummy.c dummy.h
AUTO_SRCS = auto.c auto.h
QVFB_SRCS = qvfb.c qvfb.h
MC68X328_SRCS = mc68x328.c mc68x328.h
SMDK2410_SRCS = 2410.c 2410.h
HH2410R3_SRCS = hh2410r3.c hh2410r3.h
FFT7202_SRCS = fft7202.c fft7202.h
FXRM9200_SRCS = fxrm9200.c fxrm9200.h

if NATIVE_IAL_ENGINE
libial_la_LIBADD = native/libnative.la
endif

libial_la_SOURCES = $(COMMON_SRCS) $(ARM3000_SRCS) $(IPAQ_SRCS) \
                    $(L7200_SRCS) $(DUMMY_SRCS) $(QVFB_SRCS) $(PX255B_SRCS) \
                    $(MC68X328_SRCS) $(SMDK2410_SRCS) \
                    $(UCB1X00_SRCS) $(AUTO_SRCS) \
                    $(HH2410R3_SRCS) $(EMBEST2410_SRCS)\
                    $(HH5249KBDIR_SRCS) $(FFT7202_SRCS) $(FXRM9200_SRCS)

