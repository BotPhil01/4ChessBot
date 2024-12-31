import {createGrid, GridOptions, ClientSideRowModelModule, ModuleRegistry} from 'ag-grid-community';

ModuleRegistry.registerModules([ ClientSideRowModelModule ]);
window.onload = function() {
    buttons();
}

function buttons() {
    var h = document.getElementById("homeSwap")
    if (h) {h.onclick = swap;}
    var bb = document.getElementById("boardBackButton")
    if (bb) {bb.onclick = swap;}
    var db = document.getElementById("debugButton")
    if (db) {db.onclick = f;}
}

function f() {

    interface ICar {
        make: string,
        model: string,
        price: number,
        electric: boolean
    }



    const gridOptions: GridOptions<ICar> = {
        rowData: [{ make: "Tesla", model: "Model Y", price: 64950, electric: true }],

        columnDefs: [
            {field: "make"},
            {field: "model"},
            {field: "price"},
            {field: "electric"},
        ],

        defaultColDef: {
            flex: 1,
        },

        getRowId: (params) => {
            return params.data.make + params.data.model;
        },

        onRowSelected: (event) => {
            if (event.data) {
                const price = event.data.price;
            }
        }
    };

    const gridElem = document.getElementById("pieceGrid");

    if (!gridElem) {
        console.error("no element found");
        return;
    } else {
        console.log("creating grid with options");
        console.log(`${gridOptions}`);
        createGrid(gridElem, gridOptions);
    }
    

}

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

