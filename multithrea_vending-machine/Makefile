CXX = u++										# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD		# compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

#all objects
OBJECTS = driver.o BottlingPlant.o NameServer.o Printer.o Student.o Truck.o VendingMachine.o WATCard.o WATCardOffice.o Parent.o Bank.o Config.o
EXEC = soda							# executable name
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC}						# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

${EXEC} : ${OBJECTS}				# link step 1st executable
	${CXX} $^ -o $@

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} 