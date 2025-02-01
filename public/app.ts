const ws = new WebSocket(`ws://${window.location.host}`);
const BOARDDIMENSION = 14;

// holds knowledge on which players are known to be in checkmate R Y G B
let knownCheckmates = [false, false, false, false];

enum Colour {
    RED,
    BLUE,
    YELLOW,
    GREEN
}

function strToColour(c: String) {
    switch (c.toUpperCase()) {
        case 'R':
            return Colour.RED;
        case 'B':
            return Colour.BLUE;
        case 'Y':
            return Colour.YELLOW;
        case 'G':
            return Colour.GREEN;
    }
    return Colour.RED;
}

function colourToIndex(colour: Colour) {
    switch (colour) {
        case Colour.RED:
            return 0;
        case Colour.BLUE:
            return 1;
        case Colour.YELLOW:
            return 2;
        case Colour.GREEN:
            return 3;
    }
    return -1;
}

window.onload = function() {
    buttons();
}

function buttons() {
    var h = document.getElementById("homeSwap")
    if (h) {h.onclick = swap;}
    var bb = document.getElementById("boardBackButton")
    if (bb) {bb.onclick = swap;}
    var db = document.getElementById("debugButton")
    if (db) {db.onclick = destroy;}
    var draggable = document.getElementById("draggable");
    if (draggable) {
        dragPieceElement(draggable);
    }
}

function destroy () {
    ws.send("quit");
    ws.close();
}

// func for swapping between home and game pages
function swap() {
    let gameDiv = document.getElementById("gameDiv");
    let homeDiv = document.getElementById("homeDiv");
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
    var legalSquares: number[][] = [];
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
    
    function queryLegalMoves() {
        var message: string = "S" + initialSquare[0].toString() + "|" + initialSquare[1].toString() + "S"; 
        ws.send(message);
        ws.onmessage = (event) => {
            handleOutput(event.data);
        }
        return;
    }

    // triggered by onmousedown
    function startDrag(e: MouseEvent) {
        if (!element) {console.error("trying to move non existent element"); return;}
        legalSquares = []
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
        var pos = [ev.clientX, ev.clientY];
        
        var toSquare = snapToBoard(pos);
        if (toSquare != initialSquare) {
            // moved to a square successfully
            
            var message: string = "M" + initialSquare[0].toString() + "|" + initialSquare[1].toString() + "|" + toSquare[0].toString() + "|" + toSquare[1].toString() + "M";
            ws.send(message);
            setInitialPosition(positionFromSquare(toSquare));
            setInitialSquare(toSquare);
            ws.onmessage = (event) => {
                handleOutput(event.data);
            }

            // handler response
        }
        document.onmouseup = null;
        document.onmousemove = null;

        return true;
    }

    // takes position of element relative to the viewport and snaps it to the board element if element is over a valid square 
    // returns the initial square if cant move else returns the square it snaps to
    function snapToBoard(position: number[]) {
        if (!element) {
            console.error("Unable to find elements in snapToBoard function");
            alert("something went wrong with the board see console for details");
            return initialSquare;
        }

        var square = positionToSquare(position);
        // check if piece can move there
        if (!elemCanMove(initialSquare, square)) {
            // return to intitial space 
            element.style.left = initialPos[0] + "px";
            element.style.top = initialPos[1] + "px";
            return initialSquare;
        }
    
        // calculate pixel coords to move to
        position = positionFromSquare(square);
        // erase overelapped element
        var destroyed: HTMLElement = (document.elementFromPoint(position[0], position[1]) as HTMLElement);
        if (!element || !destroyed) {
            console.error("when playing move: elements not found");
            return initialSquare;
        }
        var c = destroyed.getAttribute("class")
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
    function elemCanMove(fromSquare: number[], toSquare: number[]) {
        if (fromSquare[0] === toSquare[0] && fromSquare[1] === toSquare[1]) {
            return false;
        }
        // validate toSquare is on the board 
        var outBoardSquare = toSquare[0] >= BOARDDIMENSION || toSquare[0] < 0 || toSquare[1] >= BOARDDIMENSION || toSquare[1] < 0;
        var inCorners = toSquare[0] < 3 && (toSquare[1] < 3 || toSquare[1] > 10) || 
        toSquare[0] > 10 && (toSquare[1] < 3 || toSquare[1] > 10);
        
        if (outBoardSquare || inCorners) {
            return false;
        }
        // check through legal squares
        for (var i = 0; i < legalSquares.length; i++) {
            var tmpSqu: number[] = legalSquares[i];
            // assert(tmpSqu.length === 2);
            if (tmpSqu.length !== 2) {
                console.error(`legalSquare length is incorrect. i=${i}`);
            }
            if (tmpSqu[0] === toSquare[0] && tmpSqu[1] === toSquare[1]) {
                return true;
            }
        }
        // TODO do some stuff when game end
        return false;
    }

    async function handleOutput(d: Blob) {
        var str = await new Response(d).text();
        // 2 variations 
        // output is a collection of moves for red player
        // assert(str.length != 0);
        if(str.length === 0) {
            console.error("output string is length 0");
        }
        switch (str[0]) {
            case "M":
                str = str.substring(str.indexOf("M") + 1, str.lastIndexOf("M"));
                var indices = str.split("|");
                playMove([+indices[0], +indices[1]], [+indices[2], +indices[3]]);
                return
            case "C":
                str = str.substring(str.indexOf("C") + 1, str.lastIndexOf("C"));
                handleCheckmate(str);
                break;
            case "S":
                str = str.substring(str.indexOf("S") + 1, str.lastIndexOf("S"));
                var squares = str.split("#");
                for (var i = 0; i < squares.length; i++) {
                        var square = squares[i];
                        if (!/((\d|\d\d)\|)+/.test(square)) {
                            continue;
                        }
                        var data = square.split("|");
                        // assert(data.length == 2);
                        if (data.length !== 2) {
                            console.error("incorrect split square size");
                    }
                    legalSquares.push([+data[0], +data[1]]);
                }
                return;
            case "L":
                alert("You have lost! Better luck next time.");
                ws.close();
                return;
            case "W":
                alert("You win! Good job!");
                ws.close();
                return;
        }
    }
    
    
}

function handleCheckmate(colourString: String) {
    var colour = strToColour(colourString);
    var index = colourToIndex(colour);
    if (knownCheckmates[index]) {
        return;
    }
    knownCheckmates[index] = true;

    // grey out pieces
    var pieces = ["p", "b", "n", "r", "q", "k"];
    var className = "piece-" + colourString.toLowerCase();
    var greyClassName = "piece-x";

    pieces.forEach( (piece: String) => {
        className = className + piece;
        greyClassName = greyClassName + piece;
        var elements = document.getElementsByClassName(className);
        
        // only changes half of hte elements in elements
        // eelements loads in to be all of them 
        while (elements.length > 0) {
            elements[0].setAttribute("class", greyClassName);
        }

        className = className.substring(0, className.length - 1);
        greyClassName = greyClassName.substring(0, greyClassName.length - 1);
    });    


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
    var position: number[] = [-1, -1];
    position[0] = Number(boardRect.left) + square[0] * squareLength;
    position[1] = Number(boardRect.top) + square[1] * squareLength;
    return position;
}

// assume we have <div class="piece-br"></div>

function initBoard() {
    const pieceClassNames = ["bb", "bk", "bn", "bp", "bq", "br", "gb", "gk", "gn", "gp", "gq", "gr", "rb", "rk", "rn", "rp", "rq", "rr", "yb", "yk", "yn", "yp", "yq", "yr"];
    // const pieceClassNames = ["bb"]; DEBUG CODE
    for (var i = 0; i < pieceClassNames.length; i++) {
        var pieceName = pieceClassNames[i];
        // gets indices of leftmost and downmost piece
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

function calculatePieceRow(pieceName: string): number {
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

function assignPieces(pieceName: string) {
    var container = document.getElementById("pieceContainer");
    if (!container) {
        console.error("contaienr not found");
        return;
    }
    // calculate indices according to piece name eg: "bb" = blue bishop 
    var initCol = calculatePieceColumn(pieceName);
    var initRow = calculatePieceRow(pieceName);
    var position: number[];
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
    };
    for (var i = 0; i < pieceCount; i++) {
        var element = <HTMLElement>pieces[i];
        if (pieceName[0] === "r") {
            dragPieceElement(element);
        }
        // adding offset is orthogonal between ry and bg pieces
        // hence we want to choose whether to increment columns or rows
        if (pieceName[0] == 'r' || pieceName[0] == 'y') {
            position = positionFromSquare([initCol + (i * offset), initRow]);
        } else {
            position = positionFromSquare([initCol, initRow + (i * offset)]);
        }
        element.style.left = position[0] + "px";
        element.style.top = position[1] + "px";
    }

    
}


function playMove(from: number[], to: number[]) {
    // move to different square
    var fpos: number[] = positionFromSquare(from);
    var tpos: number[] = positionFromSquare(to);
    if (fpos.length != 2 || tpos.length != 2) {
        console.error("fpos || tpos length not correct");
        console.error(`fpos length: ${fpos.length}`);
        console.error(`tpo length: ${tpos.length}`);
    }
    
    var elem = document.elementFromPoint(Number(fpos[0]), Number(fpos[1])) as HTMLElement;
    var destroyed = document.elementFromPoint(Number(tpos[0]), Number(tpos[1])) as HTMLElement;
    if (!elem || !destroyed) {
        console.error("when playing move: elements not found");
        return;
    }

    var c = destroyed.getAttribute("class")
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

function checkPromotion(elem: HTMLElement, to: number[]) {
    var className = elem.getAttribute("class");
    if (className?.endsWith("p")) {
        switch(className.charAt(6)) {
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