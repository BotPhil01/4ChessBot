"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var ws = new WebSocket("ws://".concat(window.location.host));
var BOARDDIMENSION = 14;
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
    var draggable = document.getElementById("draggable");
    if (draggable) {
        dragPieceElement(draggable);
    }
}
// func for swapping between home and game pages
function swap() {
    var gameDiv = document.getElementById("gameDiv");
    var homeDiv = document.getElementById("homeDiv");
    // mandatory error handling by ts
    if (!homeDiv || !gameDiv) {
        console.error("No divs founds");
        return;
    }
    if (homeDiv.getAttribute("class") == "centered" && gameDiv.getAttribute("class") == "destroy") {
        homeDiv.setAttribute("class", "destroy");
        gameDiv.setAttribute("class", "block");
        initBoard();
    }
    else {
        homeDiv.setAttribute("class", "centered");
        gameDiv.setAttribute("class", "destroy");
    }
}
// func for dragging pieces
// uses ts event handling no libraries needed
function dragPieceElement(element) {
    var initialSquare;
    var initialPos;
    if (!element) {
        // error here 
        console.error("Unable to retrieve element <".concat(element, ">"));
        return;
    }
    element.onmousedown = startDrag;
    function setInitialSquare(square) {
        initialSquare = square;
    }
    function setInitialPosition(pos) {
        initialPos = pos;
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
        setInitialSquare([elemRect.left, elemRect.top]);
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
        var pos = [ev.clientX, ev.clientY];
        var initSqu = positionToSquare(initialPos);
        console.log("initial square from positionToSquare: " + initSqu[0].toString() + "," + initSqu[1].toString());
        console.log("initial square: " + initialSquare[0].toString() + "," + initialSquare[1].toString());
        var toSquare = snapToBoard(pos);
        if (toSquare == initialSquare) {
            // moved to a square successfully
            console.log(initSqu.length);
            var message = initSqu[0].toString() + "|" + initSqu[1].toString() + "|" + toSquare[0].toString() + "|" + toSquare[1].toString();
            console.log("Sending move: " + message + "\n");
            ws.send(message);
            ws.onmessage = function (event) {
                console.log("server response: " + event.data);
            };
            ws.onmessage = null;
            // handler response
        }
        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }
    // takes position of element relative to the viewport and snaps it to the board element if element is over a valid square 
    function snapToBoard(position) {
        if (!element) {
            console.error("Unable to find elements in snapToBoard function");
            alert("something went wrong with the board see console for details");
            return false;
        }
        var square = positionToSquare(position);
        // check if piece can move there
        if (!elemCanMove(initialSquare, square)) {
            // return to intitial space 
            element.style.left = initialPos[0] + "px";
            element.style.top = initialPos[1] + "px";
            return initialPos;
        }
        // calculate pixel coords to move to
        position = positionFromSquare(square);
        element.style.left = position[0] + "px";
        element.style.top = position[1] + "px";
        setInitialPosition(position);
        setInitialSquare(square);
        return square;
    }
    // validates if an element can move from a valid t  o square to a potentially invalid square
    function elemCanMove(fromSquare, toSquare) {
        // TODO do some stuff when game end
        // var res = queryEngineLegalMove(fromSquare, toSquare);
        // if (res == 0) {
        //     console.error("Engine error");
        //     return false;
        // }
        // validate toSquare is on the board 
        var outBoardSquare = toSquare[0] >= BOARDDIMENSION || toSquare[0] < 0 || toSquare[1] >= BOARDDIMENSION || toSquare[1] < 0;
        var inCorners = toSquare[0] < 3 && (toSquare[1] < 3 || toSquare[1] > 10) ||
            toSquare[0] > 10 && (toSquare[1] < 3 || toSquare[1] > 10);
        if (outBoardSquare || inCorners) {
            return false;
        }
        return true;
    }
}
// function queryEngineLegalMove(fromSquare: number[], toSquare: number[]) {
//     const url = "localhost:42069";
//     const socket = new WebSocket(url);
//     var res = -1;
//     socket.onopen = function (e) {
//         console.log("connection established");
//         socket.send(`${fromSquare[0]}0${fromSquare[1]}0${toSquare[0]}0${toSquare}`);
//     }
//     socket.onmessage = function (e) {
//         res = parseEngineLegalMoveData(e.data);
//     }
//     socket.onclose = function (e) {
//         if (e.wasClean) {
//             console.log("Connection closed cleanly");
//         } else {
//             console.log("Connection ended dirty");
//         }
//     }
//     // hang while waiting for a response
//     while (socket.OPEN && res == -1) {continue}
//     return res;
// }
// returns code depending on game state after move
// 0 - engine error
// 1 - illegal move
// 2 - legal move + game cont
// 3 - legal move + stalemate
// 4 - legal move + red win
// 5 - legal move + blue win
// 6 - legal move + green win       
// 8 - legal move + yellow win
function parseEngineLegalMoveData(data) {
    // TODO implemnet when engine and sockets done
    return 1;
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
    var squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
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
    var squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
    var position = [-1, -1];
    position[0] = boardRect.left + square[0] * squareLength;
    position[1] = boardRect.top + square[1] * squareLength;
    return position;
}
// assume we have <div class="piece-br"></div>
function initBoard() {
    var pieceClassNames = ["bb", "bk", "bn", "bp", "bq", "br", "gb", "gk", "gn", "gp", "gq", "gr", "rb", "rk", "rn", "rp", "rq", "rr", "yb", "yk", "yn", "yp", "yq", "yr"];
    // const pieceClassNames = ["bb"]; DEBUG CODE
    for (var i = 0; i < pieceClassNames.length; i++) {
        var pieceName = pieceClassNames[i];
        // gets indices of leftmost and downmost piece
        assignPieces(pieceName);
    }
}
function calculatePieceColumn(pieceName) {
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
        case 'y': {
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
function calculatePieceRow(pieceName) {
    switch (pieceName[0]) {
        case 'y': {
            if (pieceName[1] == 'p') {
                return 1;
            }
            return 0;
        }
        case 'r': {
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
function assignPieces(pieceName) {
    var container = document.getElementById("pieceContainer");
    if (!container) {
        console.error("contaienr not found");
        return;
    }
    // calculate indices according to piece name eg: "bb" = blue bishop 
    var initCol = calculatePieceColumn(pieceName);
    var initRow = calculatePieceRow(pieceName);
    var position;
    var pieces = container.getElementsByClassName("piece-" + pieceName);
    var pieceCount = pieces.length;
    // for piece duplication
    // take initial position adding row or col offset
    var offset = -1;
    switch (pieceName[1]) {
        case 'p': {
            offset = 1;
            break;
        }
        case 'r': {
            offset = 7;
            break;
        }
        case 'n': {
            offset = 5;
            break;
        }
        default: {
            offset = 3;
            break;
        }
    }
    ;
    for (var i = 0; i < pieceCount; i++) {
        var element = pieces[i];
        dragPieceElement(element);
        // adding offset is orthogonal between ry and bg pieces
        // hence we want to choose whether to increment columns or rows
        if (pieceName[0] == 'r' || pieceName[0] == 'y') {
            position = positionFromSquare([initCol + (i * offset), initRow]);
        }
        else {
            position = positionFromSquare([initCol, initRow + (i * offset)]);
        }
        element.style.left = position[0] + "px";
        element.style.top = position[1] + "px";
    }
}
