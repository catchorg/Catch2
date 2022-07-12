/*---------------------------------------------------------
 * Copyright (C) Microsoft Corporation. All rights reserved.
 *--------------------------------------------------------*/

const path = require('path');

module.exports = {
	entry: './out/main.js',
	mode: 'production',
	// mode: 'development',
	output: {
		library: 'vscodetextmate',
		libraryTarget: 'umd',
		globalObject: 'this',
		path: path.resolve(__dirname, 'release')
	},
	devtool: 'source-map',
	resolve: {
		extensions: ['.js']
	}
};
