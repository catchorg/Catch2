/*
  @license
	Rollup.js v2.76.0
	Fri, 08 Jul 2022 08:35:58 GMT - commit 0eb042740eb41812b9e96e0e7b6ed8aa4cab04a7

	https://github.com/rollup/rollup

	Released under the MIT License.
*/
'use strict';

require('path');
require('process');
require('url');
const loadConfigFile_js = require('./shared/loadConfigFile.js');
require('./shared/rollup.js');
require('./shared/mergeOptions.js');
require('tty');
require('perf_hooks');
require('crypto');
require('fs');
require('events');



module.exports = loadConfigFile_js.loadAndParseConfigFile;
//# sourceMappingURL=loadConfigFile.js.map
