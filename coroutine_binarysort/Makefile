TYPE:=char
SENTINEL:=-1

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -DTYPE="${TYPE}" -DSENTINEL="${SENTINEL}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

# optional build of given throwcatch program
OBJECTS0 = q2_original.o# object files forming 1st executable with prefix "q2"
EXEC0 = throwcatch				# 0th executable name

OBJECTS1 = q2.o # object files forming 1st executable with prefix "q2"
EXEC1 = longjmp					# 1st executable name

OBJECTS2 = q3.o # object files forming 2nd executable with prefix "q3"
EXEC2 = binsertsort				# 2nd executable name

OBJECTS = ${OBJECTS0} ${OBJECTS1} ${OBJECTS2}	# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

q2.o : q2.cc					# change compiler 2nd executable
	g++ ${CXXFLAGS} -c $< -o $@
q2%.o : q2%.cc					# change compiler 2nd executable
	g++ ${CXXFLAGS} -c $< -o $@

${EXEC0} : ${OBJECTS0}				# link step 0th executable
	g++ ${CXXFLAGS} $^ -o $@

${EXEC1} : ${OBJECTS1}				# link step 1st executable
	g++ ${CXXFLAGS} $^ -o $@

-include ImplType

ifeq (${IMPLTYPE},${TYPE})			# same implementation type as last time ?
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${TYPE},)					# no implementation type specified ?
# set type to previous type
TYPE=${IMPLTYPE}
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC2}
${EXEC2} :
	rm -f ImplType
	touch q3binsertsort.h
	${MAKE} TYPE="${TYPE}" SENTINEL="${SENTINEL}"
endif
endif

ImplType :
	echo "IMPLTYPE=${TYPE}" > ImplType

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC0} ${EXECS} ImplType
