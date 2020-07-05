
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/$$BUILDT -lqtemail

RESOURCES += $$PWD/email.qrc


PRE_TARGETDEPS += $$PWD/$$BUILDT/libqtemail.a

