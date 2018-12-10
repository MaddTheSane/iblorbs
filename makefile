CC := gcc
LEX := flex
LEX_LIB := -lfl
CC_FLAGS :=
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
veryclean: clean
	-rm $(FRONT)
	-rm $(BRES)
	-rm $(BLC)
	-rm $(BPAL)
	-rm $(BMERGE)
$(BPAL): bpal.o bpalcmd.o short.o
	$(CC) $(CC_FLAGS) $^ $(LEX_LIB) -o $@
$(FRONT): front.o
	$(CC) $(CC_FLAGS) $< $(LEX_LIB) -o $@
$(BRES): bres.o short.o
	$(CC) $(CC_FLAGS) $^ $(LEX_LIB) -o $@
$(BLC): blc.o short.o
	$(CC) $(CC_FLAGS) $^ -o $@
$(BMERGE): bmerge.o short.o index.o
	$(CC) $(CC_FLAGS) $^ -o $@
$(BDIFF): bdiff.o short.o index.o
	$(CC) $(CC_FLAGS) $^ -o $@

front.o: front.c
	$(CC) $(CC_FLAGS) -c $^ -o $@
bres.o: bres.c short.h
	$(CC) $(CC_FLAGS) -c $< -o $@
blc.o: blc.c
	$(CC) $(CC_FLAGS) -c $^ -o $@
short.o: short.c short.h
	$(CC) $(CC_FLAGS) -c $< -o $@
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
	$(LEX) -o$@ $<
