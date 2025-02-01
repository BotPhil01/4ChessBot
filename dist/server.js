"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var node_child_process_1 = require("node:child_process");
var express_1 = __importDefault(require("express"));
var node_http_1 = require("node:http");
var ws_1 = require("ws");
var path_1 = __importDefault(require("path"));
var node_assert_1 = __importDefault(require("node:assert"));
var executablePath = path_1.default.resolve(__dirname, 'main.o');
var server = null;
var PORT = 3000;
var app = (0, express_1.default)();
app.use(express_1.default.static('public'));
console.log("balls");
server = new node_http_1.Server(app);
(0, node_assert_1.default)(server != null);
var wss = new ws_1.WebSocketServer({ server: server });
wss.on('connection', function (ws) {
    console.log("Client connected");
    var engineProcess = (0, node_child_process_1.spawn)(executablePath, {
        shell: true,
    });
    var res = engineProcess.stdin.write("js\n");
    console.log("heading to js loop; ".concat(res));
    ws.on('message', function (message) {
        console.log("Message received from client ".concat(message));
        (0, node_assert_1.default)(engineProcess.stdin != null);
        var res = engineProcess.stdin.write(message + "\n");
        console.log("result after writing ".concat(res));
    });
    (0, node_assert_1.default)(engineProcess.stdout != null);
    engineProcess.stdout.on('data', function (data) {
        console.log("Process has output data: <data>".concat(data, "</data>"));
        ws.send(data);
    });
    ws.on('close', function () {
        console.log("Socket closed");
        engineProcess.stdin.write("quit");
        engineProcess.kill();
    });
    engineProcess.stderr.on('data', function (data) {
        console.error("Engine error=".concat(data));
    });
});
server.listen(PORT, function () {
    console.log("Server running on port: ".concat(PORT));
});
