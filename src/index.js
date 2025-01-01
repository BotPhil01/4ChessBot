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
    var initialSquare;
    var initialPos;
    if (!element) {
        console.error(`Unable to retrieve element <${elementId}>`);
        return;
    }
    element.onmousedown = startDrag;
    function setInitialSquare(square) {
        initialSquare = square;
    }
    function setInitialPosition(pos) {
        initialPos = pos;
    }
    // takes position of the element relative to viewport
    function positionToSquare(position) {
        //TODO optimise this function to use global variables instead of recalculating
        var boardElement = document.getElementById("boardImage");
        if (!boardElement) {
            console.error("unable to find board element");
            return [-1, -1];
        }
        var boardRect = boardElement.getBoundingClientRect();
        const squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
        // make position relative to board
        position[0] -= boardRect.left;
        position[1] -= boardRect.top;
        // remove position relative to the square 
        var square = [-1, -1];
        square[0] = Math.floor(position[0] / squareLength);
        square[1] = Math.floor(position[1] / squareLength);
        return square;
    }
    // returns psoition relative to viewport
    function positionFromSquare(square) {
        //TODO optimise this function to use global variables instead of recalculating
        var boardElement = document.getElementById("boardImage");
        if (!boardElement) {
            console.error("unable to find board element");
            return [-1, -1];
        }
        var boardRect = boardElement.getBoundingClientRect();
        const squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
        var position = [-1, -1];
        position[0] = boardRect.left + square[0] * squareLength;
        position[1] = boardRect.top + square[1] * squareLength;
        return position;
    }
    // triggered by onmousedown
    function startDrag(e) {
        if (!element) {
            console.error("trying to move non existent element");
            return;
        }
        // cache initial position and square in case needed to return there
        var elemRect = element.getBoundingClientRect();
        setInitialPosition([elemRect.left, elemRect.top]);
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
        snapToBoard([ev.clientX, ev.clientY]);
        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }
    // takes position of element relative to the viewport and snaps it to the board element if element is over a valid square 
    function snapToBoard(position) {
        if (!element) {
            console.error("Unable to find elements in snapToBoard function");
            alert("something went wrong with the board see console for details");
            return;
        }
        var square = positionToSquare(position);
        // check if piece can move there
        if (!elemCanMove(initialSquare, square)) {
            console.log(`returning to initial position: ${initialPos[0]},${initialPos[1]}`);
            // return to intitial space 
            element.style.left = initialPos[0] + "px";
            element.style.top = initialPos[1] + "px";
            return;
        }
        // calculate pixel coords to move to
        position = positionFromSquare(square);
        console.log(`moving to pos: ${position[0]},${position[0]}`);
        element.style.left = position[0] + "px";
        element.style.top = position[1] + "px";
        setInitialPosition(position);
    }
    // validates if an element can move from a valid to square to a potentially invalid square
    function elemCanMove(fromSquare, toSquare) {
        // check square bounds
        // general bounds
        var outGeneralBounds = toSquare[0] > 13 || toSquare[0] < 0 || toSquare[1] > 13 || toSquare[1] < 0;
        var inCorners = toSquare[0] < 3 && (toSquare[1] < 3 || toSquare[1] > 10) ||
            toSquare[0] > 10 && (toSquare[1] < 3 || toSquare[1] > 10);
        if (outGeneralBounds || inCorners) {
            return false;
        }
        return true;
    }
}
