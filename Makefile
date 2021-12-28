CC ?= gcc
LEX ?= flex
LEX_LIB ?= -lfl
CFLAGS ?=
LDFLAGS ?=
FRONT = front
BRES = bres
BLC = blc
BMERGE = bmerge
BDIFF = bdiff
BINARC = iblorbb.zip
SRCARC = iblorbs.zip
BPAL = bpal
all: $(FRONT) $(BRES) $(BLC) $(BPAL) $(BDIFF) $(BMERGE)
compress:
	strip *.exe
	-djp *.exe
clean: 
	-rm front.o
	-rm bres.o
	-rm blc.o
	-rm short.o
	-rm front.c
	-rm bres.c
	-rm bpal.o
	-rm bpalcmd.o
	-rm index.o
	-rm bdiff.o
	-rm bmerge.o
veryclean: clean
	-rm $(FRONT)
	-rm $(BRES)
	-rm $(BLC)
	-rm $(BPAL)
	-rm $(BMERGE)
$(BPAL): bpal.o bpalcmd.o short.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LEX_LIB) -o $@
$(FRONT): front.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(LEX_LIB) -o $@
$(BRES): bres.o short.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LEX_LIB) -o $@
$(BLC): blc.o short.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
$(BMERGE): bmerge.o short.o index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
$(BDIFF): bdiff.o short.o index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

front.o: front.c
	$(CC) $(CFLAGS) -c $^ -o $@
bres.o: bres.c short.h
	$(CC) $(CFLAGS) -c $< -o $@
blc.o: blc.c
	$(CC) $(CFLAGS) -c $^ -o $@
short.o: short.c short.h
	$(CC) $(CFLAGS) -c $< -o $@
bdiff.o: bdiff.c short.h index.h
bmerge.o: bmerge.c short.h index.h
bpalcmd.o: bpalcmd.c bpal.h short.h
bpal.o: bpal.c bpal.h
index.o: index.c index.h
dist: $(BINARC) $(SRCARC)

$(BINARC): $(FRONT).exe $(BRES).exe $(BLC).exe $(BPAL).exe $(BMERGE).exe front.txt  $(BDIFF).exe
	-rm $(BINARC)
	zip $@ $^
$(SRCARC): front.l bres.l blc.c short.c short.h bpal.c bpalcmd.c bpal.h makefile front.txt bmerge.c bdiff.c index.c index.h
	-rm $(SRCARC)
	zip $@ $^
%.c: %.l
	$(LEX) -o $@ $<
