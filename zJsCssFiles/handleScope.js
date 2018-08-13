/////////////////////////////////////////////////////////////////////
// handleScope.js - a function for expand and collapse scope       //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 3                    //
/////////////////////////////////////////////////////////////////////

function handleScope(id) {
    var thediv = document.getElementById(id);
    var thebut = "but" + id;
    if (thediv.style.display == "none") {
        thediv.style.display = "block";
        document.getElementById(thebut).value = "-";
    }
    else {
        thediv.style.display = "none";
        document.getElementById(thebut).value = "+";
    }
}


