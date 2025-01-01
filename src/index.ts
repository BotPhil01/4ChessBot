import { FakeHScrollComp } from "ag-grid-community";

const BOARDDIMENSION = 14;

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
    var draggable = document.getElementById("draggable");
    if (draggable) {
        console.log("dragging element");
        dragPieceElement(draggable);
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
        initBoard();
        
    } else {
        homeDiv.setAttribute("class", "centered");
        gameDiv.setAttribute("class", "destroy");
    }
}

// func for dragging pieces
// uses ts event handling no libraries needed
function dragPieceElement(element: HTMLElement) {
    
    var initialSquare: number[];
    var initialPos: number[];

    if (!element) {
        // error here 
        console.error(`Unable to retrieve element <${element}>`);
        return;
    }
    element.onmousedown = startDrag;

    function setInitialSquare(square: number[]) {
        initialSquare = square;
    }

    function setInitialPosition(pos: number[]) {
        initialPos = pos;
    }
    
    // triggered by onmousedown
    function startDrag(e: MouseEvent) {
        if (!element) {console.error("trying to move non existent element"); return;}
        // cache initial position and square in case needed to return there
        var elemRect = element.getBoundingClientRect();
        setInitialPosition([elemRect.left, elemRect.top]);

        e.preventDefault();
        document.onmouseup = stopDrag;
        
        document.onmousemove = elementDrag;
        return true;
    }

    // triggered when moving
    function elementDrag(ev: MouseEvent) {
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
    function stopDrag(ev: MouseEvent) {
        snapToBoard([ev.clientX, ev.clientY]);

        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }

    // takes position of element relative to the viewport and snaps it to the board element if element is over a valid square 
    function snapToBoard(position: number[]) {
        
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
        setInitialSquare(square);
    }

    

        
    // validates if an element can move from a valid t  o square to a potentially invalid square
    function elemCanMove(fromSquare: number[], toSquare: number[]) {
        // validate toSquare is on the board 
        var boardSquareLimit = toSquare[0] >= BOARDDIMENSION || toSquare[0] < 0 || toSquare[1] >= BOARDDIMENSION || toSquare[1] < 0;
        var inCorners = toSquare[0] < 3 && (toSquare[1] < 3 || toSquare[1] > 10) || 
                        toSquare[0] > 10 && (toSquare[1] < 3 || toSquare[1] > 10)
        if (boardSquareLimit || inCorners) {
            return false;   
        }
        return true;
    }
    
}

// takes position of the element relative to viewport
function positionToSquare(position: number[]): number[] {
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
var square: number[] = [-1, -1];
square[0] = Math.floor(position[0] / squareLength);
square[1] = Math.floor(position[1] / squareLength);
return square;
}

// returns psoition relative to viewport
function positionFromSquare(square: number[]): number[] {
//TODO optimise this function to use global variables instead of recalculating
var boardElement = document.getElementById("boardImage");
if (!boardElement) {
    console.error("unable to find board element");
    return [-1, -1];
}
var boardRect = boardElement.getBoundingClientRect();
const squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
console.log(`boardRect.right: ${boardRect.right}`);
console.log(`boardRect.left: ${boardRect.left}`);
console.log(`boardRect.top: ${boardRect.top}`);
var position: number[] = [-1, -1];
position[0] = boardRect.left + square[0] * squareLength;
position[1] = boardRect.top + square[1] * squareLength;
return position;
}

// assume we have <div class="piece-br"></div>

function initBoard() {
    const pieceClassNames = ["bb", "bk", "bn", "bp", "bq", "br", "gb", "gk", "gn", "gp", "gq", "gr", "rb", "rk", "rn", "rp", "rq", "rr", "yb", "yk", "yn", "yp", "yq", "yr"];
    // const pieceClassNames = ["bb"]; DEBUG CODE
    for (var i = 0; i < pieceClassNames.length; i++) {
        var pieceName = pieceClassNames[i];
        // gets indices of leftmost and downmost piece
        console.log(`initialising piece: ${pieceName}`);
        assignPieces(pieceName);
    }
}

function calculatePieceColumn(pieceName: string): number {
    switch (pieceName[0]) {
        case 'b': {
            if (pieceName[1] == 'p') {
                return 1;
            }
            return 0;
        } 
        case 'g': {
            if (pieceName[1] == 'p') {
                return 12;
            }
            return 13;
        }
        case 'r': {
            switch (pieceName[1]) {
                case 'k': {
                    return 7;
                } 
                case 'q': {
                    return 6;
                }
                case 'b': {
                    return 5;
                }
                case 'n': {
                    return 4;
                }
                default: {
                    return 3;
                }
            }
        }
        default: {
            switch (pieceName[1]) {
                case 'k': {
                    return 7;
                } 
                case 'q': {
                    return 6;
                }
                case 'b': {
                    return 5;
                }
                case 'n': {
                    return 4;
                }
                default: {
                    return 3;
                }
            }
        }
    }
}

function calculatePieceRow(pieceName: string): number {
    switch (pieceName[0]) {
        case 'r': {
            if (pieceName[1] == 'p') {
                return 1;
            }
            return 0;
        } 
        case 'y': {
            if (pieceName[1] == 'p') {
                return 12;
            }
            return 13;
        }
        case 'b': {
            switch (pieceName[1]) {
                case 'k': {
                    return 7;
                } 
                case 'q': {
                    return 6;
                }
                case 'b': {
                    return 5;
                }
                case 'n': {
                    return 4;
                }
                default: {
                    return 3;
                }
            }
        }
        default: {
            switch (pieceName[1]) {
                case 'k': {
                    return 7;
                } 
                case 'q': {
                    return 6;
                }
                case 'b': {
                    return 5;
                }
                case 'n': {
                    return 4;
                }
                default: {
                    return 3;
                }
            }
        }
    }
}

function assignPieces(pieceName: string) {
    var container = document.getElementById("pieceContainer");
    if (!container) {
        console.error("contaienr not found");
        return;
    }
    var initCol = calculatePieceColumn(pieceName);
    var initRow = calculatePieceRow(pieceName);
    
    var className = "piece-" + pieceName;
    var pieces = container.getElementsByClassName(className);
    // always place the fist element
    var first = <HTMLElement>pieces[0];
    console.log(`initIndex: ${[initCol, initRow]}`);
    var position = positionFromSquare([initCol, initRow]);
    // cannot read properties of undefined (reading 'style') 
    console.log(`position: ${position}`);
    first.style.left = position[0] + "px";
    first.style.top = position[1] + "px";
    dragPieceElement(first);
    var size = pieces.length;
    var data = [
        {name: 'p', offset: 1},
        {name: 'r', offset: 7},
        {name: 'n', offset: 5},
        {name: 'b', offset: 3},
    ]


    if (pieceName[0] == 'r' || pieceName[0] == 'y') {
        // alter columns 
        for (var i = 1; i < size; i++) {
            var element = <HTMLElement>pieces[i];
            dragPieceElement(element);
            let offset = 0;
            data.forEach(function (data){
                if (data.name == pieceName[1]) {
                    offset = data.offset;
                }
            });

            initCol += offset;
            position = positionFromSquare([initCol, initRow]);
            element.style.left = position[0] + "px";
            element.style.top = position[1] + "px";
        }
    } else {
        for (var i = 1; i < size; i++) {
            var element = <HTMLElement>pieces[i];
            dragPieceElement(element);
            let offset = 0;
            data.forEach(function (data){
                if (data.name == pieceName[1]) {
                    offset = data.offset;
                }
            });

            initRow += offset;
            position = positionFromSquare([initCol, initRow]);
            element.style.left = position[0] + "px";
            element.style.top = position[1] + "px";
        }
    }
}