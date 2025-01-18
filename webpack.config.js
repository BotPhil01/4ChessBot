const { url } = require('inspector');
const path = require('path');

module.exports = {
    mode: 'development',
    entry: './public/app.js',
    output: {
        filename: 'app.js',
        path: path.resolve(__dirname, 'dist'),
    },
    optimization: {
        checkWasmTypes: false,
    },
    resolve: {
        modules: ['node_modules']
    }
};
