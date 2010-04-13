from PyQt4 import QtGui, QtCore
from widgets.tablewidgets import KeywordList
from widgets.validateddialog import ValidatedDialog
import ertwrapper
from widgets.combochoice import ComboChoice

# e = enkf_main_get_ensemble_config( enkf_main )
# s = ensemble_config_alloc_keylist_from_var_type(e , 1 # PARAMETER value from enkf_types.h)
# # Itererer over stringlist
# stringlist_free( s )
# range = enkf_main_get_ensemble_size( enkf_main )
#
#
# sl = stringlist_alloc_new()
# stringlist_append_copy(sl , "STRING")
#
# 
#
# enkf_main_initialize(enkf_main , sl , iens1 , iens2);
# stringlist_free( sl )




class InitPanel(QtGui.QFrame):
    
    def __init__(self, parent):
        QtGui.QFrame.__init__(self, parent)
        self.setFrameShape(QtGui.QFrame.Panel)
        self.setFrameShadow(QtGui.QFrame.Raised)

        initPanelLayout = QtGui.QHBoxLayout()
        self.setLayout(initPanelLayout)

        casePanel = QtGui.QFormLayout()


        def get_case_list(ert):
            fs = ert.enkf.enkf_main_get_fs(ert.main)
            caseList = ert.enkf.enkf_fs_alloc_dirlist(fs)

            list = ert.getStringList(caseList)
            ert.freeStringList(caseList)
            return list

        self.get_case_list = get_case_list


        casePanel.addRow("Current case:", self.createCurrentCaseCombo())
        casePanel.addRow("Cases:", self.createCaseList())

        initPanelLayout.addLayout(casePanel)




    def createCaseList(self):
        cases = KeywordList(self, "", "case_list")

        cases.newKeywordPopup = lambda list : ValidatedDialog(cases, "New case", "Enter name of new case:", list).showAndTell()
        cases.addRemoveWidget.enableRemoveButton(False)
        cases.list.setMaximumHeight(150)
        cases.initialize = lambda ert : [ert.setTypes("enkf_main_get_fs"),
                                         ert.setTypes("enkf_fs_alloc_dirlist"),
                                         ert.setTypes("enkf_fs_has_dir", ertwrapper.c_int),
                                         ert.setTypes("enkf_fs_select_write_dir", None)]


        def create_case(ert, cases):
            fs = ert.enkf.enkf_main_get_fs(ert.main)

            for case in cases:
                if not ert.enkf.enkf_fs_has_dir(fs, case):
                    ert.enkf.enkf_fs_select_write_dir(fs, case, True)
                    break

            self.currentCase.initialize(ert)
            self.currentCase.fetchContent()

        cases.getter = self.get_case_list
        cases.setter = create_case

        return cases

    def createCurrentCaseCombo(self):
        self.currentCase = ComboChoice(self, ["none"])

        def initialize_cases(ert):
            ert.setTypes("enkf_main_get_fs")
            ert.setTypes("enkf_fs_get_read_dir", ertwrapper.c_char_p)
            ert.setTypes("enkf_fs_select_read_dir", None, ertwrapper.c_char_p)

            self.currentCase.updateList(self.get_case_list(ert))
            

        self.currentCase.initialize = initialize_cases

        def get_current_case(ert):
            fs = ert.enkf.enkf_main_get_fs(ert.main)
            read_dirfs = ert.enkf.enkf_fs_get_read_dir(fs)
            #print "The selected case is: " + read_dirfs
            return read_dirfs

        self.currentCase.getter = get_current_case

        def select_case(ert, case):
            case = str(case)
            #print "Selecting case: " + case
            if not case == "":
                fs = ert.enkf.enkf_main_get_fs(ert.main)
                ert.enkf.enkf_fs_select_read_dir(fs, case)

        self.currentCase.setter = select_case

        return self.currentCase
