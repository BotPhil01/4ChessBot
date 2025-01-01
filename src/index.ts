import {createGrid, GridOptions, ClientSideRowModelModule, ModuleRegistry} from 'ag-grid-community';

ModuleRegistry.registerModules([ ClientSideRowModelModule ]);



window.onload = function() {
    console.log("starting up");
    buttons();
}

function buttons() {
    var h = document.getElementById("homeSwap")
    if (h) {h.onclick = swap;}
    var bb = document.getElementById("boardBackButton")
    if (bb) {bb.onclick = swap;}
    var db = document.getElementById("debugButton")
    if (document.getElementById("draggable")) {
        console.log("dragging element");
        dragElement("draggable");
    }
    // if (db) {db.onclick = f;}
}

// function f() {

//     interface ICar {
//         make: string,
//         model: string,
//         price: number,
//         electric: boolean
//     }



//     const gridOptions: GridOptions<ICar> = {
//         rowData: [{ make: "Tesla", model: "Model Y", price: 64950, electric: true }],

//         columnDefs: [
//             {field: "make"},
//             {field: "model"},
//             {field: "price"},
//             {field: "electric"},
//         ],

//         defaultColDef: {
//             flex: 1,
//         },

//         getRowId: (params) => {
//             return params.data.make + params.data.model;
//         },

//         onRowSelected: (event) => {
//             if (event.data) {
//                 const price = event.data.price;
//             }
//         }
//     };

//     const gridElem = document.getElementById("pieceGrid");

//     if (!gridElem) {
//         console.error("no element found");
//         return;
//     } else {
//         console.log("creating grid with options");
//         console.log(`${gridOptions}`);
//         createGrid(gridElem, gridOptions);
//     }
    

// }

function swap() {
    console.log("swapping");
    let gameDiv = document.getElementById("gameDiv");
    let homeDiv = document.getElementById("homeDiv");
    console.log(`${gameDiv}`);
    if (!homeDiv || !gameDiv) {
        console.error("No divs founds");
        return;
    }
    if (homeDiv?.getAttribute("class") == "centered" && gameDiv?.getAttribute("class") == "destroy") {
        homeDiv.setAttribute("class", "destroy");
        gameDiv.setAttribute("class", "block");
        
    } else {
        homeDiv.setAttribute("class", "centered");
        gameDiv.setAttribute("class", "destroy");
    }
}

function dragElement(elementId: string) {
    var pos1 = 0; 
    var pos2 = 0;
    var pos3 = 0;
    var pos4 = 0;

    var element = document.getElementById(elementId);
    if (!element) {
        console.error(`Unable to retrieve element <${elementId}>`);
        return;
    }
    element.onmousedown = dragMouseDown;


    function dragMouseDown(e: MouseEvent) {
        e.preventDefault();
        pos3 = e.clientX
        pos4 = e.clientY
        document.onmouseup = closeDragElement;
        
        document.onmousemove = elementDrag;
        return true;
    }

    function elementDrag(ev: MouseEvent) {
        ev.preventDefault();

        pos1 = pos3 - ev.clientX;
        pos2 = pos4 - ev.clientY;
        pos3 = ev.clientX;
        pos4 = ev.clientY;
        
        if (element) {
            element.style.top = (element.offsetTop - pos2) + "px";
            element.style.left = (element.offsetLeft - pos1) + "px";
            return true;
        }
        console.error("Element has been clicked dragged and disappeared");
        return false;
        
    }

    function closeDragElement() {
        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }
}
