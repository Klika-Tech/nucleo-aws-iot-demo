const WebpackShellPlugin = require('webpack-shell-plugin');
const CleanWebpackPlugin = require('clean-webpack-plugin');

module.exports = {
    entry: ['babel-polyfill', './src/'],
    output: {
        path: `${__dirname}/dist`,
        filename: 'bundle.js',
    },
    devtool: 'source-map',
    devServer: {
        host: '0.0.0.0',
        port: 8080,
    },
    module: {
        loaders: [
            {
                test: /\.(js|jsx)$/,
                loader: 'babel',
                exclude: /node_modules/,
                query: {
                    presets: ['es2015', 'react'],
                    plugins: ['transform-object-rest-spread'],
                },
            },
            {
                test: /\.scss$/,
                loaders: ['style', 'css', 'sass'],
            },
            {
                test: /\.svg$/,
                loader: 'svg-sprite',
            }, {
                test: /\.(woff2?|png|gif)$/,
                loader: 'file',
            },
        ],
        noParse: [
            /aws-sdk/,
        ],
    },
    plugins: [
        new WebpackShellPlugin({
            onBuildStart: ['npm install; npm prune'],
        }),
        new CleanWebpackPlugin(['dist']),
    ],
};
