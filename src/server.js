"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const node_child_process_1 = require("node:child_process");
const express_1 = __importDefault(require("express"));
const node_http_1 = require("node:http");
const ws_1 = require("ws");
const path_1 = __importDefault(require("path"));
const node_assert_1 = __importDefault(require("node:assert"));
const executablePath = path_1.default.resolve(__dirname, 'main.o');
let server = null;
const PORT = 3000;
const app = (0, express_1.default)();
app.use(express_1.default.static('public'));
console.log("balls");
server = new node_http_1.Server(app);
(0, node_assert_1.default)(server != null);
const wss = new ws_1.WebSocketServer({ server });
wss.on('connection', (ws) => {
    console.log("Client connected");
    const engineProcess = (0, node_child_process_1.spawn)(executablePath, {
        shell: true,
    });
    ws.on('message', (message) => {
        console.log(`Message received from client ${message}`);
        (0, node_assert_1.default)(engineProcess.stdin != null);
        var res = engineProcess.stdin.write(message + "\n");
        console.log(`result after writing ${res}`);
    });
    (0, node_assert_1.default)(engineProcess.stdout != null);
    engineProcess.stdout.on('data', (data) => {
        console.log(`Process has output data: <data>${data}</data>`);
        ws.send(data);
    });
    ws.on('close', () => {
        console.log("Socket closed");
        engineProcess.stdin.write("quit");
        engineProcess.kill();
    });
    engineProcess.stderr.on('data', (data) => {
        console.error(`Engine error=${data}`);
    });
});
server.listen(PORT, () => {
    console.log(`Server running on port: ${PORT}`);
});
