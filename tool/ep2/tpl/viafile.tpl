-noremove
#-keep init
#-keep UpdateDisplayInj
-entry elfpackEntry_ven

#-first elfpackEntry

#-nodebug
#-callgraph
#-nolocals
-noscanlib


#-errors errors_link.txt
-list %info_file%
-info sizes,totals,unused
-map
