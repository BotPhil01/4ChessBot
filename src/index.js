"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ag_grid_community_1 = require("ag-grid-community");
ag_grid_community_1.ModuleRegistry.registerModules([ag_grid_community_1.ClientSideRowModelModule]);
window.onload = function () {
    buttons();
};
function buttons() {
    var h = document.getElementById("homeSwap");
    if (h) {
        h.onclick = swap;
    }
    var bb = document.getElementById("boardBackButton");
    if (bb) {
        bb.onclick = swap;
    }
    var db = document.getElementById("debugButton");
    if (db) {
        db.onclick = f;
    }
}
function f() {
    const gridOptions = {
        rowData: [{ make: "Tesla", model: "Model Y", price: 64950, electric: true }],
        columnDefs: [
            { field: "make" },
            { field: "model" },
            { field: "price" },
            { field: "electric" },
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
    }
    else {
        console.log("creating grid with options");
        console.log(`${gridOptions}`);
        (0, ag_grid_community_1.createGrid)(gridElem, gridOptions);
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
    if ((homeDiv === null || homeDiv === void 0 ? void 0 : homeDiv.getAttribute("class")) == "centered" && (gameDiv === null || gameDiv === void 0 ? void 0 : gameDiv.getAttribute("class")) == "destroy") {
        homeDiv.setAttribute("class", "destroy");
        gameDiv.setAttribute("class", "block");
    }
    else {
        homeDiv.setAttribute("class", "centered");
        gameDiv.setAttribute("class", "destroy");
    }
}
