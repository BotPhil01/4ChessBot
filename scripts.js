// holds display data for toggling visibility of divs
// NOTE!!! hidden gets overriden and visibility doesn't delete from document hence we have to do it this way
var displayData = {
    'homeDiv': 'centered',
    'gameDiv': ''
};
function elementToggle(elId) {
    var el = document.getElementById(elId);
    if (!el) {
        console.error("unable to get element from ".concat(elId));
        return;
    }
    if (el.getAttribute("class") == "destroy") {
        el.setAttribute("class", displayData[elId]);
    }
    else {
        el.setAttribute("class", "destroy");
    }
}
// swap home and game div
// change if more pages required (likely)
function swap() {
    console.log("button pressed");
    elementToggle("homeDiv");
    elementToggle("gameDiv");
    // PLACES PIECES IN DEFAULT POSITION CHANGE AFTER CACHING IS FOUND
    placePieces();
}
// runs on startup to place pieces at beginning
function placePieces() {
    var piecesGrid = document.getElementById("piecesGrid");
    if (!piecesGrid) {
        console.error("unable to get piece grid element");
        return;
    }
    // get children
    var pieces = piecesGrid.getElementsByTagName("div");
    for (var i = 0; i < pieces.length; i++) {
        var piece = pieces[i];
        var row = piece.getAttribute("data-row");
        var col = piece.getAttribute("data-col");
        piece.style.gridArea = "".concat(row, " / ").concat(col, " / ").concat(row, " / ").concat(col);
    }
}
// TODO add function to query engine
