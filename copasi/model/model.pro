######################################################################
# $Revision: 1.10 $ $Author: ssahle $ $Date: 2005/05/25 09:50:08 $  
######################################################################

LIB = model

include(../lib.pri)
include(../common.pri)

# Input
HEADERS += CChemEq.h \
           CChemEqElement.h \
           CChemEqInterface.h \
           CCompartment.h \
           CDeTerm.h \
           CMetab.h \
           CMetabNameInterface.h \
           CModel.h \
           CModelValue.h \
           CMoiety.h \
           #CParticleNumberList.h \
           CReaction.h \
           CReactionInterface.h \
           CSpec2Model.h \
           CSpecLine.h \
           CState.h \
           model.h
           
SOURCES += CChemEq.cpp \
           CChemEqElement.cpp \
           CChemEqInterface.cpp \
           CCompartment.cpp \
           CDeTerm.cpp \
           CMetab.cpp \
           CMetabNameInterface.cpp \
           CModel.cpp \
           CModelValue.cpp \
           CMoiety.cpp \
           #CParticleNumberList.cpp \
           CReaction.cpp \
           CReactionInterface.cpp \
           CScanInputFlexLexer.cpp \
           CSpec2Model.cpp \
           CSpecLine.cpp \
           CState.cpp

1.target = CScanInputFlexLexer.cpp
1.depends = CScanInputFlexLexer.lpp 
1.commands = $$myLex
QMAKE_EXTRA_UNIX_TARGETS += 1
