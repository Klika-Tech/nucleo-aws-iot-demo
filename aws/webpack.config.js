var path = require('path')
var fs = require('fs')
var webpack = require('webpack')
var WebpackShellPlugin = require('webpack-shell-plugin')
var CleanWebpackPlugin = require('clean-webpack-plugin')

module.exports = {
    entry: fs.readdirSync(path.join(__dirname, './lambdas'))
        .filter(filename => /\.js$/.test(filename))
        .map(filename => {
            var entry = {}
            entry[filename.replace('.js', '')] = path.join(
                __dirname,
                './lambdas/',
                filename
            )
            return entry
        })
        .reduce((finalObject, entry) => Object.assign(finalObject, entry), {}),
    output: {
        path: path.join(__dirname, 'dist'),
        library: '[name]',
        libraryTarget: 'commonjs2',
        filename: '[name].js'
    },
    externals: {
        'aws-sdk': true
    },
    target: 'node',
    module: {
        loaders: [
            {
                test: /\.js$/,
                loader: 'babel',
                exclude: /node_modules/,
                query: {
                    presets: ['es2015']
                }
            },
            {
                test: /\.json$/,
                loader: 'json'
            }
        ]
    },
    plugins: [
        new WebpackShellPlugin({
            onBuildStart: ['npm install; npm prune']
        }),
        new CleanWebpackPlugin(['dist'])
    ]
}
