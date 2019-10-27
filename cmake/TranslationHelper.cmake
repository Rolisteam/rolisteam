

function(convertTs2Po _inFile _outFile)
    

endfunction()



function(convertPo2Ts _inFile _outFile)
    #string(REPLACE ".po" ".ts" _outFile ${_inFile})
    
endfunction()



function(convertTs2Qm _inFile _result)
    string(REPLACE ".ts" ".qm" _qmFile ${_inFile})
    set(_result ${_qmFile} PARENT_SCOPE)
endfunction()
