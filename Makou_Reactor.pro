# #####################################################################
# Automatically generated by qmake (2.01a) ven. 21. mai 20:28:03 2010
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += opengl

# Input
HEADERS += ApercuBG.h \
    ColorDisplay.h \
    OpcodeList.h \
    Config.h \
    Field.h \
    FieldPC.h \
    FieldPS.h \
    FieldModel.h \
    FieldModelPart.h \
    GrpScript.h \
    GrpScriptList.h \
    KeyEditorDialog.h \
    FieldArchive.h \
    LZS.h \
    ModelManager.h \
    Opcode.h \
    Palette.h \
    Parameters.h \
    Script.h \
    ScriptEditor.h \
    ScriptList.h \
    Search.h \
    FF7Text.h \
    TextManager.h \
    Var.h \
    VarManager.h \
    Window.h \
    ConfigWindow.h \
    WalkmeshManager.h \
    WalkmeshWidget.h \
    IsoArchive.h \
    TextHighlighter.h \
    ApercuBGLabel.h \
    AnimEditorDialog.h \
    BGDialog.h \
    EncounterFile.h \
    EncounterWidget.h \
    EncounterTableWidget.h \
    TutFile.h \
    TutWidget.h \
    InfFile.h \
    MiscWidget.h \
    FieldModelFile.h \
    TextPreview.h \
    FieldModelLoader.h \
    ImportDialog.h \
    VertexWidget.h \
    GZIP.h \
    Data.h \
    OrientationWidget.h \
    VarOrValueWidget.h \
    HexLineEdit.h \
    Listwidget.h \
    IdFile.h \
    CaFile.h \
    FieldModelPartPS.h \
    FieldModelPartPC.h \
    FieldModelFilePS.h \
    FieldModelFilePC.h \
    FieldModelLoaderPC.h \
    FieldModelLoaderPS.h \
    TdbFile.h \
    MassExportDialog.h \
    ScriptEditorWidgets/Delegate.h \
    ScriptEditorWidgets/ScriptEditorView.h \
    ScriptEditorWidgets/ScriptEditorGenericList.h \
    ScriptEditorWidgets/ScriptEditorWindowPage.h \
    ScriptEditorWidgets/ScriptEditorStructPage.h \
    ScriptEditorWidgets/ScriptEditorMathPage.h \
    Section1File.h \
	QLockedFile.h \
	QTaskBarButton.h \
	Lgp.h \
	Lgp_p.h \
    LgpDialog.h \
    MassImportDialog.h \
    FormatSelectionWidget.h \
    FieldArchiveIO.h
SOURCES += ApercuBG.cpp \
    ColorDisplay.cpp \
    OpcodeList.cpp \
    Config.cpp \
    Field.cpp \
    FieldPC.cpp \
    FieldPS.cpp \
    FieldModel.cpp \
    FieldModelPart.cpp \
    GrpScript.cpp \
    GrpScriptList.cpp \
    txt.cpp \
    KeyEditorDialog.cpp \
    FieldArchive.cpp \
    LZS.cpp \
    main.cpp \
    ModelManager.cpp \
    Opcode.cpp \
    Palette.cpp \
    Script.cpp \
    ScriptEditor.cpp \
    ScriptList.cpp \
    Search.cpp \
    FF7Text.cpp \
    TextManager.cpp \
    Var.cpp \
    VarManager.cpp \
    Window.cpp \
    ConfigWindow.cpp \
    WalkmeshManager.cpp \
    WalkmeshWidget.cpp \
    IsoArchive.cpp \
    TextHighlighter.cpp \
    ApercuBGLabel.cpp \
    AnimEditorDialog.cpp \
    BGDialog.cpp \
    EncounterFile.cpp \
    EncounterWidget.cpp \
    EncounterTableWidget.cpp \
    TutFile.cpp \
    TutWidget.cpp \
    InfFile.cpp \
    MiscWidget.cpp \
    FieldModelFile.cpp \
    TextPreview.cpp \
    FieldModelLoader.cpp \
    ImportDialog.cpp \
    VertexWidget.cpp \
    GZIP.cpp \
    Data.cpp \
    OrientationWidget.cpp \
    VarOrValueWidget.cpp \
    HexLineEdit.cpp \
    Listwidget.cpp \
    IdFile.cpp \
    CaFile.cpp \
    FieldModelPartPS.cpp \
    FieldModelPartPC.cpp \
    FieldModelFilePS.cpp \
    FieldModelFilePC.cpp \
    FieldModelLoaderPC.cpp \
    FieldModelLoaderPS.cpp \
    TdbFile.cpp \
    MassExportDialog.cpp \
    ScriptEditorWidgets/Delegate.cpp \
    ScriptEditorWidgets/ScriptEditorView.cpp \
    ScriptEditorWidgets/ScriptEditorGenericList.cpp \
    ScriptEditorWidgets/ScriptEditorWindowPage.cpp \
    ScriptEditorWidgets/ScriptEditorStructPage.cpp \
    ScriptEditorWidgets/ScriptEditorMathPage.cpp \
    Section1File.cpp \
    QLockedFile.cpp \
	QTaskBarButton.cpp \
	Lgp.cpp \
	Lgp_p.cpp \
    LgpDialog.cpp \
    MassImportDialog.cpp \
    FormatSelectionWidget.cpp \
    FieldArchiveIO.cpp

TRANSLATIONS += Makou_Reactor_en.ts \
    Makou_Reactor_ja.ts

RESOURCES += Makou_Reactor.qrc
macx {
	ICON = images/Makou_Reactor.icns
}
!win32 {
	LIBS += -lz
}
win32 {
	RC_FILE = Makou_Reactor.rc
	LIBS += -lole32
	HEADERS += shobjidl.h
}

OTHER_FILES += Makou_Reactor.rc \
    Makou_Reactor.desktop

#all other *nix (except for symbian)
#base for setting up deb packages(rpm too?).
#becomes 'make install' when qmake generates the makefile
unix:!macx:!symbian {
	LIBS = -lglut -lGLU -lz
    system(lrelease Makou_Reactor.pro) #call lrelease to make the qm files.
    target.path = /opt/makoureactor #set to deploy the build target.

    lang.path = /opt/makoureactor/
    lang.files = *.qm

    vars_cfg.path = /opt/makoureactor/  #a hack to make it so vars.cfg can be written to by all
    vars_cfg.files = vars.cfg           #this file is 'chmod 666' durring post install

    icon.path = /usr/share/pixmaps/
    icon.files = images/logo-shinra.png

    desktop.path =/usr/share/applications/
    desktop.files = Makou_Reactor.desktop

    INSTALLS += target \
        lang  \
        vars_cfg \
        icon  \
        desktop
}
