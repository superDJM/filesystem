obj = main.o command.o tools.o explan.o filerw.o fileblockrw.o

filesystem : $(obj)
	cc -o filesystem $(obj)

$(obj) : filesym.h

.PHONY : clean
clean :
	-rm filesystem $(obj)
