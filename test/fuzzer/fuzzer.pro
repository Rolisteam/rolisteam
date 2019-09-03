TEMPLATE = app
TARGET = fuzzed_formula

#include(../../charactersheet.pri)

INCLUDEPATH+="../../"

QT += core gui opengl network widgets svg


SOURCES += testcase.cpp

SOURCES += ../../formula/formulamanager.cpp \
../../formula/parsingtoolformula.cpp \
../../formula/nodes/formulanode.cpp   \
../../formula/nodes/operator.cpp      \
../../formula/nodes/parenthesesfnode.cpp     \
../../formula/nodes/scalaroperatorfnode.cpp  \
../../formula/nodes/startnode.cpp            \
../../formula/nodes/valuefnode.cpp           \

SOURCES += ../../formula/formulamanager.h \
../../formula/parsingtoolformula.h \
../../formula/nodes/formulanode.h   \
../../formula/nodes/operator.h      \
../../formula/nodes/parenthesesfnode.h    \
../../formula/nodes/scalaroperatorfnode.h \
../../formula/nodes/startnode.h           \
../../formula/nodes/valuefnode.h          \
