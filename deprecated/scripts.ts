// import { createRequire } from 'module';
// const require = createRequire(import.meta.url);
import 'node_modules/gridstack/dist/gridstack.min.css';
import { GridStack } from 'gridstack';

// holds display data for toggling visibility of divs
// NOTE!!! hidden gets overriden and visibility doesn't delete from document hence we have to do it this way

type data = {
    id: string
    display: string
}

let dataCollection: data[] = [
    {id: "homeDiv", display: "centered"},
    {id: "gameDiav", display: "a"}
]


function elementToggle(elId: string) {
    console.log("toggling");
    var el = document.getElementById(elId);
    if (!el) {
        console.error(`unable to get element from ${elId}`);
        return;
    }
    if (el.getAttribute("class") == "destroy") {
        var d = dataCollection[0].display;
        el.setAttribute("class", d);
    } else {
        el.setAttribute("class", "destroy");
    }
}

// swap home and game div
// change if more pages required (likely)
function swap() {
    console.log("button pressed")
    elementToggle("homeDiv");
    elementToggle("gameDiv");
    // loadStartingGrid();
    // PLACES PIECES IN DEFAULT POSITION CHANGE AFTER CACHING IS FOUND
    // placePieces();
}

// // runs on startup to place pieces at beginning
// function placePieces() {
//     const piecesGrid: HTMLElement | null = document.getElementById("piecesGrid");

//     if (!piecesGrid) {
//         console.error(`unable to get piece grid element`);
//         return;
//     }

//     // get children
//     var pieces = piecesGrid.getElementsByTagName("div");
//     for (let i = 0; i < pieces.length; i++) {
//         var piece = pieces[i];
//         var row = piece.getAttribute("data-row");
//         var col = piece.getAttribute("data-col");
//         piece.style.gridArea = `${row} / ${col} / ${row} / ${col}`;
//     }
// }

function debug() {
    loadStartingGrid();
}

function loadStartingGrid() {
    
    var options = {
        row: 14,
        column: 14
    }
    var pieceGrid = GridStack.init(options);

    // add all widgets

    pieceGrid.addWidget({w: 2, content: 'item1'});
    //add pawns
    // for (var i = 0; i < 8; i++) {
    //     pieceGrid.addWidget({x: 4+i, y: 2,}); // red
    //     pieceGrid.addWidget(); // green
    //     pieceGrid.addWidget(); // blue
    //     pieceGrid.addWidget(); // yellow
    // }
    

}

// TODO add function to query engine