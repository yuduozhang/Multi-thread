TYPE:=AUTO

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -DTABLETYPE_${TYPE}
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS = q2.o q2commonImplementation.o q2table${TYPE}.o # list of object files for question 2 prefixed with "q2"
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXEC = phil

.PHONY : clean

all : ${EXEC} q2Driver.class			# build all executables

-include ImplType

ifeq (${IMPLTYPE},${TYPE})			# same implementation type as last time ?
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${TYPE},)					# no implementation type specified ?
# set type to previous type
TYPE=${IMPLTYPE}
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC}
${EXEC} :
	rm -f ImplType
	touch q2table.h
	${MAKE} ${EXEC} TYPE="${TYPE}"
endif
endif

ImplType :
	echo "IMPLTYPE=${TYPE}" > ImplType

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}

q2Driver.class : # list of class files for question 2 prefixed with "q2"

%.class : %.java ${MAKEFILE_NAME}
	javac $<

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC} *.class ImplType *.out
