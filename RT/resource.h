//{{NO_DEPENDENCIES}}

#define IDS_APP_TITLE                    100

// Specifies, by now, menu and accelerators.
#define IDR_MAINFRAME                    101 

// We use custom ID for icon instead of general IDR_MAINFRAME.
#define IDI_APP_ICON                     201
#define IDI_FILE_OPEN                    202
#define IDI_DUMMY_TRANSPARENT            203

// We use custom ID for toolbar instead of general IDR_MAINFRAME.
#define IDB_TOOLBAR_16x15xN              300

#define ID_TOOLBAR                       400
#define ID_FILE_OPEN                     401
#define ID_START_OR_STOP_TEST            402
#define ID_TYPING_ERROR_ALERT_TYPE       403
#define ID_CLEAR_TEST_AREA               404
#define ID_DETAILED_STATS                405
#define ID_LANGUAGE_ENGLISH              406
#define ID_LANGUAGE_FRENCH               407
#define ID_LANGUAGE_RUSSIAN              408

// Dialogs.
#define IDD_TYPINGWIN32_DIALOG           500
#define IDD_ABOUTBOX                     501
#define IDD_DS_COLORS                    502
#define IDD_PP_GENERAL                   503
#define IDD_PP_MODE1                     504
#define IDD_MODE4_HINT1                  505
#define IDD_MODE4_HINT2                  506

#define IDC_STATIC_URL_LOGO              606 
#define IDC_TYPINGWIN32_EN               607  // workaround
#define IDC_TYPINGWIN32_FR               608  // workaround
#define IDC_TYPINGWIN32_RU               609  // workaround
#define IDC_TYPINGWIN32                  610
#define IDC_MYICON                       611
#define IDC_EDIT_MAIN                    613
#define IDC_LISTBOX_FASTEST              614
#define IDC_LISTBOX_SLOWEST              615
#define IDC_BUTTON_REPOPULATE            616
#define IDC_BUTTON_SHOW_LOG              617
#define IDC_BUTTON_TEST                  618
#define IDC_BUTTON_TEST2                 619
#define IDC_STATIC_SLOWEST               620
#define IDC_EDIT_DEMO                    621
#define IDC_LISTBOX_MOST_FREQUENT        622
#define IDC_STATIC_MOST_FREQUENT         623
#define IDC_LISTBOX_LEAST_FREQUENT       624
#define IDC_STATIC_LEAST_FREQUENT        625
#define IDC_LISTBOX_LEFT_HAND_WORDS      626
#define IDC_STATIC_FASTEST               627
#define IDC_STATIC_TYPING_SPEED          628
#define IDC_STATIC_WORDS_TYPED           629
#define IDC_STATIC_CHARACTERS_TYPED      630
#define IDC_STATIC_ERRORS_COUNT          631
#define IDC_STATIC                       -1 //632
#define IDC_LISTBOX_RIGHT_HAND_WORDS     633
#define IDC_BUTTON_UPDATE_SORTING        634
#define IDC_CUSTOM_MODE_LISTVIEW         635
#define IDC_TEST_MODE_LISTVIEW           636
#define IDC_TEST_MODE_BUTTON_CUSTOM_LIST 637
#define IDC_TEST_MODE_BUTTON_CHECK_ALL   638
#define IDC_MODE4_DS_STATIC              661


// Detailed statistics color dialog controls.
#define IDC_DS_COLORS_STATIC_LH          639
#define IDC_DS_COLORS_STATIC_LH2         640
#define IDC_DS_COLORS_STATIC_L2R         641
#define IDC_DS_COLORS_STATIC_L2R2        642
#define IDC_DS_COLORS_STATIC_R2L         643
#define IDC_DS_COLORS_STATIC_R2L2        644
#define IDC_DS_COLORS_STATIC_RH          645
#define IDC_DS_COLORS_STATIC_RH2         646
#define IDC_DS_COLORS_STATIC_CUSTOM      647
#define IDC_DS_COLORS_STATIC_CUSTOM2     648
#define IDC_OPTIONS_GENERAL_AUTOSAVE     649
#define IDC_OPTIONS_GENERAL_SAVE_LAST_MODE 650
#define IDC_STATIC_PRODUCT               651
#define IDC_STATIC_LINK                  652
#define IDC_STATIC_COPYRIGHT             653
#define IDC_STATIC_COPYRIGHT2            654

#define IDC_MODE4_H1D_STATIC1             655 
#define IDC_MODE4_H1D_CHECKBOX            656
#define IDC_MODE4_H1D_STATIC2             657

#define IDC_MODE4_H2D_STATIC1            658 
#define IDC_MODE4_H2D_CHECKBOX           659
#define IDC_MODE4_H2D_STATIC2            660

#define IDM_ABOUT                        4000
#define IDM_RESIZE                       4001

// File menu.
#define IDM_FILE_OPEN                    4002
#define IDM_EXIT                         4003
// View menu.
#define IDM_VIEW_TOOLBAR                 4004
#define IDM_VIEW_STATUSBAR               4005
#define IDM_VIEW_LANGUAGE                4006
// Modes menu.
#define IDM_MODES_LH_RH_WORDS            4007
#define IDM_MODES_LH_RH_WORDS_BOTH_HANDS 4008
#define IDM_MODES_CUSTOM                 4009
#define IDM_MODES_TEST                   4010
// Commands menu.
#define IDM_COMMANDS_START_OR_STOP_TEST  4011
#define IDM_COMMANDS_ALERT_STATE         4012
#define IDM_COMMANDS_CLEAR_TEST_AREA     4013
#define IDM_COMMANDS_DETAILED_STATS      4014
#define IDM_COMMANDS_SHOW_REPORT         4015
// Help menu.
#define IDM_HELP_KEYBOARD_LAYOUT         4016
#define IDM_HELP_ONLINE                  4017
#define IDM_HELP_MSB                     4018

// String IDs (in UNICODE version I decided to separate IDC_X, which might be
// used - as default IDs - for contols strings, to make more natural IDS_ 
// prefixed string IDs.

#define IDS_STATIC_SLOWEST                   1000
#define IDS_STATIC_FASTEST                   1001     
#define IDS_STATIC_MOST_FREQUENT             1002      
#define IDS_STATIC_LEAST_FREQUENT            1003 
#define IDS_STATIC_TYPING_SPEED              1004
#define IDS_STATIC_WORDS_TYPED               1005
#define IDS_STATIC_CHARACTERS_TYPED          1006
#define IDS_STATIC_ERRORS_COUNT              1007
#define IDS_BUTTON_REPOPULATE                1008
#define IDS_BUTTON_SHOW_LOG                  1009
#define IDS_APP_LANGUAGE_ROOT                1010                 
#define	IDS_APP_MENUITEM_LANG_EN             1011                    
#define IDS_APP_MENUITEM_LANG_FR             1012        
#define IDS_APP_MENUITEM_LANG_RU             1013
#define IDS_CUSTOM_MODE_LV_COLUMN_WORD       1014
#define IDS_CUSTOM_MODE_LV_COLUMN_LENGTH     1015
#define IDS_CUSTOM_MODE_LV_COLUMN_SPEED      1016
#define IDS_CUSTOM_MODE_LV_COLUMN_ERRORS     1017
#define IDS_CUSTOM_MODE_LV_COLUMN_COUNT      1018
#define IDS_CUSTOM_MODE_BUTTON_SORT_UPDATE   1019
#define IDS_TEST_MODE_MENU_COMMANDS          1020      
#define IDS_TEST_MODE_MENU_START_TEST        1021
#define IDS_TEST_MODE_MENU_STOP_TEST         10211
#define IDS_TEST_MODE_MENU_ALERT_TYPE        1022  // LL not used, reserved
#define IDS_TEST_MODE_MENU_DISABLE_ALERT     10221 
#define IDS_TEST_MODE_MENU_ENABLE_ALERT      10222 
#define IDS_TEST_MODE_MENU_CLEAR             1023    
#define IDS_TEST_MODE_MENU_DETAILED_STATS    1024  
#define IDS_TEST_MODE_MENU_REPORT            1025   
#define	IDS_TEST_MODE_LEFT_HAND_CHARS        1026  
#define	IDS_TEST_MODE_RIGHT_HAND_CHARS       1027
#define IDS_TEST_STATIC_BS_TIME_SPENT        1028     
#define	IDS_TEST_STATIC_BS_CPM               1029         
#define	IDS_TEST_STATIC_BS_CHARS             1030          
#define	IDS_TEST_STATIC_BS_WORDS_TYPED       1031    
#define	IDS_TEST_STATIC_BS_WPM               1032        
#define	IDS_TEST_STATIC_BS_TOTAL_ERR_COUNT   1033
#define	IDS_TEST_MODE_HTML_REPORT_TITLE      1034    
#define	IDS_TEST_MODE_HTML_REPORT_COL1       1035   
#define	IDS_TEST_MODE_HTML_REPORT_COL2       1036    
#define IDS_TEST_MODE_HTML_REPORT_SUN        1037      
#define	IDS_TEST_MODE_HTML_REPORT_MON        1038    
#define	IDS_TEST_MODE_HTML_REPORT_TUE        1039      
#define	IDS_TEST_MODE_HTML_REPORT_WED        1040      
#define	IDS_TEST_MODE_HTML_REPORT_THU        1041     
#define	IDS_TEST_MODE_HTML_REPORT_FRI        1042    
#define	IDS_TEST_MODE_HTML_REPORT_SAT        1043
#define IDS_TEST_MODE_DS_WINDOW_NAME         1044 
#define	IDS_TEST_MODE_DS_LV_COLUMN_WORD      1045    
#define IDS_TEST_MODE_DS_LV_COLUMN_LENGTH    1046
#define IDS_TEST_MODE_DS_LV_COLUMN_SPEED     1047   
#define IDS_TEST_MODE_DS_LV_COLUMN_ERRORS    1048
#define IDS_TEST_MODE_BUTTON_CUSTOM_LIST     1049
#define IDS_TEST_MODE_BUTTON_CHECK_ALL       1050
#define IDS_TEST_MODE_BUTTON_UNCHECK_ALL     1051
#define IDS_MODE4_DS_STATIC                  1085
#define	IDS_DS_COLORS_WINDOW_NAME            1052          
#define	IDS_DS_COLORS_CUSTOM_WORD            1053       
#define	IDS_DS_COLORS_LH_WORD                1054            
#define	IDS_DS_COLORS_L2R_WORD               1055          
#define	IDS_DS_COLORS_R2L_WORD               1056  
#define	IDS_DS_COLORS_RH_WORD                1057            
#define IDS_FILE_OPEN                        1058
#define IDS_START_OR_STOP_TEST               1059
#define IDS_TYPING_ERROR_ALERT_TYPE          1060
#define IDS_CLEAR_TEST_AREA                  1061
#define IDS_DETAILED_STATS                   1062
#define	IDS_ABOUT_STATIC_PRODUCT             1063           
#define	IDS_ABOUT_STATIC_COPYRIGHT           1064      
#define	IDS_ABOUT_STATIC_COPYRIGHT2          1065

// Test log.
#define IDS_TEST_LOG_ALT_TITLE               1066 
#define IDS_TEST_LOG_ALT_CHARS_NATURAL       1067 
#define IDS_TEST_LOG_ALT_CHARS_WITH_ENTER    1068 
#define IDS_TEST_LOG_ALT_CPM_NATURAL         1069  
#define IDS_TEST_LOG_ALT_CPM_WITH_ENTER      1070 
#define IDS_TEST_LOG_ALT_WORDS_TYPED_NATURAL 1071
#define IDS_TEST_LOG_ALT_WORDS_TYPED_TYPIST  1072  
#define IDS_TEST_LOG_ALT_WPM_TYPED_NATURAL   1073
#define IDS_TEST_LOG_ALT_WPM_TYPED_TYPIST    1074

// Status bar hints.
#define IDS_MODE12_HINT                      1075 
#define IDS_MODE3_HINT                       1076 
#define IDS_MODE3_HINT2                      1077 
#define IDS_MODE4_HINT                       1078 

// 1st hint dialog.
#define IDS_MODE4_H1D_STATIC1                1079               
#define	IDS_MODE4_H1D_CHECKBOX               1080        
#define	IDS_MODE4_H1D_STATIC2                1081    

// 2nd hint dialog.
#define	IDS_MODE4_H2D_STATIC1                1082            
#define	IDS_MODE4_H2D_CHECKBOX               1083              
#define	IDS_MODE4_H2D_STATIC2                1084

#define IDS_BUTTON_START_TEST                1100
#define IDS_BUTTON_START_TIMER               1101
#define IDS_BUTTON_STOP_TIMER                1102

// Next default values for new objects

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        30004
#define _APS_NEXT_COMMAND_VALUE         40004
#define _APS_NEXT_CONTROL_VALUE         50004
#define _APS_NEXT_SYMED_VALUE           60004
#endif
#endif
