APP_NAME = eclipse_magazin_6_15

CONFIG += qt warn_on cascades10

LIBS +=  -lbbplatform -lbbsystem -lbbutilityi18n -lbb -lbbdata -lbbdevice -lbbcascadespickers -lQtLocationSubset -lbbcascadesmaps

QT += network xml

include(config.pri)
