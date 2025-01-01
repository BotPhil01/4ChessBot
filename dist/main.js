/*
 * ATTENTION: The "eval" devtool has been used (maybe by default in mode: "development").
 * This devtool is neither made for production nor for readable output files.
 * It uses "eval()" calls to create a separate source file in the browser devtools.
 * If you are trying to read the output file, select a different devtool (https://webpack.js.org/configuration/devtool/)
 * or disable the default devtool with "devtool: false".
 * If you are looking for production-ready output files, see mode: "production" (https://webpack.js.org/configuration/mode/).
 */
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "./src/index.js":
/*!**********************!*\
  !*** ./src/index.js ***!
  \**********************/
/***/ ((__unused_webpack_module, exports) => {

eval("\nObject.defineProperty(exports, \"__esModule\", ({ value: true }));\nconst BOARDDIMENSION = 14;\nwindow.onload = function () {\n    console.log(\"starting up\");\n    buttons();\n};\nfunction buttons() {\n    var h = document.getElementById(\"homeSwap\");\n    if (h) {\n        h.onclick = swap;\n    }\n    var bb = document.getElementById(\"boardBackButton\");\n    if (bb) {\n        bb.onclick = swap;\n    }\n    var db = document.getElementById(\"debugButton\");\n    if (document.getElementById(\"draggable\")) {\n        console.log(\"dragging element\");\n        dragPieceElement(\"draggable\");\n    }\n}\n// func for swapping between home and game pages\nfunction swap() {\n    console.log(\"swapping\");\n    let gameDiv = document.getElementById(\"gameDiv\");\n    let homeDiv = document.getElementById(\"homeDiv\");\n    console.log(`${gameDiv}`);\n    // mandatory error handling by ts\n    if (!homeDiv || !gameDiv) {\n        console.error(\"No divs founds\");\n        return;\n    }\n    if (homeDiv.getAttribute(\"class\") == \"centered\" && gameDiv.getAttribute(\"class\") == \"destroy\") {\n        homeDiv.setAttribute(\"class\", \"destroy\");\n        gameDiv.setAttribute(\"class\", \"block\");\n    }\n    else {\n        homeDiv.setAttribute(\"class\", \"centered\");\n        gameDiv.setAttribute(\"class\", \"destroy\");\n    }\n}\n// func for dragging pieces\n// uses ts event handling no libraries needed\nfunction dragPieceElement(elementId) {\n    var element = document.getElementById(elementId);\n    if (!element) {\n        console.error(`Unable to retrieve element <${elementId}>`);\n        return;\n    }\n    element.onmousedown = startDrag;\n    // triggered by onmousedown\n    function startDrag(e) {\n        e.preventDefault();\n        document.onmouseup = stopDrag;\n        document.onmousemove = elementDrag;\n        return true;\n    }\n    // triggered when moving\n    function elementDrag(ev) {\n        ev.preventDefault();\n        if (element) {\n            // elemenet midpoint binds to cursor position\n            element.style.top = (ev.clientY - (element.offsetHeight / 2)) + \"px\";\n            element.style.left = (ev.clientX - (element.offsetWidth / 2)) + \"px\";\n            return true;\n        }\n        console.error(\"Element has been clicked dragged and disappeared\");\n        return false;\n    }\n    // triggered by on mouse up\n    function stopDrag(ev) {\n        // TODO make element snap to grid cell\n        snapToBoard(ev.clientX, ev.clientY);\n        document.onmouseup = null;\n        document.onmousemove = null;\n        return true;\n    }\n    // takes position of element relative to the viewport and snaps it to the board element\n    function snapToBoard(posX, posY) {\n        var boardElement = document.getElementById(\"boardImage\");\n        if (!boardElement) {\n            console.error(\"Unable to find board element in snapToBoard function\");\n            alert(\"something went wrong with the board see console for details\");\n            return;\n        }\n        // gets element dimensions and offsets from viewport\n        var boardRect = boardElement.getBoundingClientRect();\n        var squareLength = (boardRect.right - boardRect.left) / BOARDDIMENSION;\n        // make position relative to board\n        posX -= boardRect.left;\n        posY -= boardRect.top;\n        // remove remainder and add board padding again\n        posX = boardRect.left + (posX - (posX % squareLength));\n        posY = boardRect.top + (posY - (posY % squareLength));\n        if (element) {\n            element.style.left = posX + \"px\";\n            element.style.top = posY + \"px\";\n        }\n    }\n}\n\n\n//# sourceURL=webpack://4pchess/./src/index.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = {};
/******/ 	__webpack_modules__["./src/index.js"](0, __webpack_exports__);
/******/ 	
/******/ })()
;