import { execFile, spawn } from 'node:child_process';
import express from 'express';
import { Server } from 'node:http';
import WebSocket, { WebSocketServer } from 'ws';
import path from 'path';
import assert from 'node:assert';
const executablePath = path.resolve(__dirname, 'main.o');

let server = null;
const PORT = 3000;

const app = express();


app.use(express.static('public'));
console.log("balls");
server = new Server(app);

assert(server != null);
const wss = new WebSocketServer({ server });

wss.on('connection', (ws: WebSocket) => {
    console.log("Client connected");
    const engineProcess = spawn(executablePath, {
        shell: true,
    });
    var res = engineProcess.stdin.write("js\n");
    console.log(`heading to js loop; ${res}`)

    ws.on('message', (message: string) => {
        console.log(`Message received from client ${message}`);
        assert(engineProcess.stdin != null);
        
        var res = engineProcess.stdin.write(message + "\n");
        console.log(`result after writing ${res}`);
    });

    assert(engineProcess.stdout != null);
    engineProcess.stdout.on('data', (data: Buffer) => {
        console.log(`Process has output data: <data>${data}</data>`);
        ws.send(data);
    });

    ws.on('close', () => {
        console.log("Socket closed");
        engineProcess.stdin.write("quit");
        engineProcess.kill();
    });

    engineProcess.stderr.on('data', (data: Buffer) => {
        console.error(`Engine error=${data}`);
    })
});

server.listen(PORT, () => {
    console.log(`Server running on port: ${PORT}`);
})