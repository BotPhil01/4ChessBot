"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const BOARDDIMENSION = 14;
window.onload = function () {
    console.log("starting up");
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
    if (document.getElementById("draggable")) {
        console.log("dragging element");
        dragPieceElement("draggable");
    }
}
// func for swapping between home and game pages
function swap() {
    console.log("swapping");
    let gameDiv = document.getElementById("gameDiv");
    let homeDiv = document.getElementById("homeDiv");
    console.log(`${gameDiv}`);
    // mandatory error handling by ts
    if (!homeDiv || !gameDiv) {
        console.error("No divs founds");
        return;
    }
    if (homeDiv.getAttribute("class") == "centered" && gameDiv.getAttribute("class") == "destroy") {
        homeDiv.setAttribute("class", "destroy");
        gameDiv.setAttribute("class", "block");
    }
    else {
        homeDiv.setAttribute("class", "centered");
        gameDiv.setAttribute("class", "destroy");
    }
}
// func for dragging pieces
// uses ts event handling no libraries needed
function dragPieceElement(elementId) {
    var element = document.getElementById(elementId);
    if (!element) {
        console.error(`Unable to retrieve element <${elementId}>`);
        return;
    }
    element.onmousedown = startDrag;
    // triggered by onmousedown
    function startDrag(e) {
        e.preventDefault();
        document.onmouseup = stopDrag;
        document.onmousemove = elementDrag;
        return true;
    }
    // triggered when moving
    function elementDrag(ev) {
        ev.preventDefault();
        if (element) {
            // elemenet midpoint binds to cursor position
            element.style.top = (ev.clientY - (element.offsetHeight / 2)) + "px";
            element.style.left = (ev.clientX - (element.offsetWidth / 2)) + "px";
            return true;
        }
        console.error("Element has been clicked dragged and disappeared");
        return false;
    }
    // triggered by on mouse up
    function stopDrag(ev) {
        // TODO make element snap to grid cell
        snapToBoard(ev.clientX, ev.clientY);
        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }
    // takes position of element relative to the viewport and snaps it to the board element
    function snapToBoard(posX, posY) {
        var boardElement = document.getElementById("boardImage");
        if (!boardElement) {
            console.error("Unable to find board element in snapToBoard function");
            alert("something went wrong with the board see console for details");
            return;
        }
        // gets element dimensions and offsets from viewport
        var boardRect = boardElement.getBoundingClientRect();
        var squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
        // make position relative to board
        posX -= boardRect.left;
        posY -= boardRect.top;
        // remove remainder and add board padding again
        posX = boardRect.left + (posX - (posX % squareLength));
        posY = boardRect.top + (posY - (posY % squareLength));
        if (element) {
            element.style.left = posX + "px";
            element.style.top = posY + "px";
        }
    }
}
