var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g = Object.create((typeof Iterator === "function" ? Iterator : Object).prototype);
    return g.next = verb(0), g["throw"] = verb(1), g["return"] = verb(2), typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (g && (g = 0, op[0] && (_ = 0)), _) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
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
    if (db) {
        db.onclick = destroy;
    }
    var draggable = document.getElementById("draggable");
    if (draggable) {
        dragPieceElement(draggable);
    }
}
function destroy() {
    ws.send("quit");
    ws.close();
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
    var legalSquares = [];
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
    function queryLegalMoves() {
        var message = "@" + initialSquare[0].toString() + "|" + initialSquare[1].toString();
        console.log("sending message: " + message);
        ws.send(message);
        ws.onmessage = function (event) {
            console.log("Server response: " + event.data);
            handleOutput(event.data);
        };
        return;
    }
    // triggered by onmousedown
    function startDrag(e) {
        if (!element) {
            console.error("trying to move non existent element");
            return;
        }
        legalSquares = [];
        // cache initial position and square in case needed to return there
        setInitialSquare(positionToSquare([e.clientX, e.clientY]));
        setInitialPosition(positionFromSquare(initialSquare));
        queryLegalMoves();
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
        var toSquare = snapToBoard(pos);
        if (toSquare != initialSquare) {
            // moved to a square successfully
            var message = initialSquare[0].toString() + "|" + initialSquare[1].toString() + "|" + toSquare[0].toString() + "|" + toSquare[1].toString();
            console.log("Sending move: " + message + "\n");
            ws.send(message);
            setInitialPosition(positionFromSquare(toSquare));
            setInitialSquare(toSquare);
            ws.onmessage = function (event) {
                console.log("server response: " + event.data);
                handleOutput(event.data);
            };
            // handler response
        }
        document.onmouseup = null;
        document.onmousemove = null;
        return true;
    }
    // takes position of element relative to the viewport and snaps it to the board element if element is over a valid square 
    // returns the initial square if cant move else returns the square it snaps to
    function snapToBoard(position) {
        if (!element) {
            console.error("Unable to find elements in snapToBoard function");
            alert("something went wrong with the board see console for details");
            return initialSquare;
        }
        var square = positionToSquare(position);
        console.log("initialSquare ".concat(initialSquare, " square ").concat(square));
        // check if piece can move there
        if (!elemCanMove(initialSquare, square)) {
            console.log("element cant move returning to initialpos");
            // return to intitial space 
            // initialPos is incorrect here
            element.style.left = initialPos[0] + "px";
            element.style.top = initialPos[1] + "px";
            return initialSquare;
        }
        // calculate pixel coords to move to
        position = positionFromSquare(square);
        // erase overelapped element
        var destroyed = document.elementFromPoint(position[0], position[1]);
        if (!element || !destroyed) {
            console.error("when playing move: elements not found");
            return initialSquare;
        }
        var c = destroyed.getAttribute("class");
        if (c != null && /piece-/.test(c) && destroyed != element) {
            destroyed.setAttribute("class", "destroy");
        }
        checkPromotion(element, square);
        // set element to new position
        element.style.left = position[0] + "px";
        element.style.top = position[1] + "px";
        // setInitialPosition(position);
        // setInitialSquare(square);
        return square;
    }
    // validates if an element can move from a valid t  o square to a potentially invalid square
    function elemCanMove(fromSquare, toSquare) {
        if (fromSquare[0] === toSquare[0] && fromSquare[1] === toSquare[1]) {
            console.log("here");
            return false;
        }
        // validate toSquare is on the board 
        var outBoardSquare = toSquare[0] >= BOARDDIMENSION || toSquare[0] < 0 || toSquare[1] >= BOARDDIMENSION || toSquare[1] < 0;
        var inCorners = toSquare[0] < 3 && (toSquare[1] < 3 || toSquare[1] > 10) ||
            toSquare[0] > 10 && (toSquare[1] < 3 || toSquare[1] > 10);
        if (outBoardSquare || inCorners) {
            return false;
        }
        console.log("legalSquares ".concat(legalSquares));
        // check through legal squares
        for (var i = 0; i < legalSquares.length; i++) {
            var tmpSqu = legalSquares[i];
            // assert(tmpSqu.length === 2);
            if (tmpSqu.length !== 2) {
                console.error("legalSquare length is incorrect. i=".concat(i));
            }
            if (tmpSqu[0] === toSquare[0] && tmpSqu[1] === toSquare[1]) {
                return true;
            }
        }
        // TODO do some stuff when game end
        return false;
    }
    function handleOutput(d) {
        return __awaiter(this, void 0, void 0, function () {
            var str, squares, i, square, data, moves, i, move, data;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, new Response(d).text()];
                    case 1:
                        str = _a.sent();
                        // 2 variations 
                        // output is a collection of moves for red player
                        // assert(str.length != 0);
                        if (str.length === 0) {
                            console.error("output string is length 0");
                        }
                        console.log("handling output ".concat(str));
                        if (str.charAt(0) === "@") {
                            str = str.substring(1, str.length);
                            squares = str.split("#");
                            for (i = 0; i < squares.length; i++) {
                                square = squares[i];
                                if (!/((\d|\d\d)\|)+/.test(square)) {
                                    continue;
                                }
                                data = square.split("|");
                                // assert(data.length == 2);
                                if (data.length !== 2) {
                                    console.error("incorrect split square size");
                                }
                                legalSquares.push([+data[0], +data[1]]);
                            }
                            return [2 /*return*/];
                        }
                        moves = str.split("#");
                        for (i = 0; i < moves.length; i++) {
                            move = moves[i];
                            if (!/(\d\|)+/.test(move)) {
                                continue;
                            }
                            data = move.split("|");
                            playMove([+data[0], +data[1]], [+data[2], +data[3]]);
                        }
                        ;
                        return [2 /*return*/];
                }
            });
        });
    }
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
    // console.log(`board left: ${boardRect.left}`);
    // console.log(`square length: ${squareLength}`);
    // make position relative to board
    position[0] -= boardRect.left;
    position[1] -= boardRect.top;
    // remove position relative to the square 
    var square = [-1, -1];
    square[0] = Math.floor(position[0] / squareLength);
    square[1] = Math.floor(position[1] / squareLength);
    // console.log(`calculated square: ${square[0]},${square[1]} from: ${position[0]},${position[1]}`);
    return square;
}
// returns psoition relative to viewport
function positionFromSquare(square) {
    // console.log(`square[0]: ${square[0]}`);
    //TODO optimise this function to use global variables instead of recalculating
    var boardElement = document.getElementById("boardImage");
    if (!boardElement) {
        console.error("unable to find board element");
        return [-1, -1];
    }
    var boardRect = boardElement.getBoundingClientRect();
    var squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;
    var position = [-1, -1];
    position[0] = Number(boardRect.left) + square[0] * squareLength;
    position[1] = Number(boardRect.top) + square[1] * squareLength;
    // console.log(`position[0] in posfsqu: ${position[0]}`);
    // console.log(`position[1] in posfsqu: ${position[0]}`);
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
                case 'q': {
                    return 7;
                }
                case 'k': {
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
                case 'q': {
                    return 7;
                }
                case 'k': {
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
        if (pieceName[0] === "r") {
            dragPieceElement(element);
        }
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
function playMove(from, to) {
    // move to different square
    var fpos = positionFromSquare(from);
    var tpos = positionFromSquare(to);
    if (fpos.length != 2 || tpos.length != 2) {
        console.error("fpos || tpos length not correct");
        console.error("fpos length: ".concat(fpos.length));
        console.error("tpo length: ".concat(tpos.length));
    }
    var elem = document.elementFromPoint(Number(fpos[0]), Number(fpos[1]));
    var destroyed = document.elementFromPoint(Number(tpos[0]), Number(tpos[1]));
    if (!elem || !destroyed) {
        console.error("when playing move: elements not found");
        return;
    }
    var c = destroyed.getAttribute("class");
    if (c != null && /piece-/.test(c) && destroyed != elem) {
        destroyed.setAttribute("class", "destroy");
    }
    // potentail promotion
    // piece-bb
    checkPromotion(elem, to);
    elem.style.left = tpos[0] + "px";
    elem.style.top = tpos[1] + "px";
    return;
}
function checkPromotion(elem, to) {
    var className = elem.getAttribute("class");
    console.log(className);
    if (className === null || className === void 0 ? void 0 : className.endsWith("p")) {
        switch (className.charAt(6)) {
            case "r":
                if (to[1] < 7) {
                    elem.setAttribute("class", "piece-rq");
                }
                break;
            case "b":
                if (to[0] > 6) {
                    elem.setAttribute("class", "piece-bq");
                }
                break;
            case "y":
                if (to[1] > 6) {
                    elem.setAttribute("class", "piece-yq");
                }
                break;
            case "g":
                if (to[0] < 7) {
                    elem.setAttribute("class", "piece-gq");
                }
                break;
            default:
        }
    }
}
