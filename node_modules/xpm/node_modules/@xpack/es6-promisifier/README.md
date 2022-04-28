[![npm (scoped)](https://img.shields.io/npm/v/@xpack/es6-promisifier.svg)](https://www.npmjs.com/package/@xpack/es6-promisifier) 
[![license](https://img.shields.io/github/license/xpack/es6-promisifier-js.svg)](https://github.com/xpack/es6-promisifier-js/blob/xpack/LICENSE)
[![Standard](https://img.shields.io/badge/code_style-standard-brightgreen.svg)](https://standardjs.com/)
[![Actions Status](https://github.com/xpack/es6-promisifier-js/workflows/Node.js%20CI%20on%20Push/badge.svg)](https://github.com/xpack/es6-promisifier-js/actions)
[![GitHub issues](https://img.shields.io/github/issues/xpack/es6-promisifier-js.svg)](https://github.com/xpack/es6-promisifier-js/issues)
[![GitHub pulls](https://img.shields.io/github/issues-pr/xpack/es6-promisifier-js.svg)](https://github.com/xpack/es6-promisifier-js/pulls)

## An ES6 promisifier wrapper

A Node.js module with a class providing a static function to wrap 
standard Node.js callback functions into ES6 promises.

## Prerequisites

A recent [Node.js](https://nodejs.org) (>7.x), since the ECMAScript 6 
class syntax is used.

## Easy install

The module is available as 
[`@xpack/es6-promisifier`](https://www.npmjs.com/package/@xpack/es6-promisifier)
from the public repository, use `npm` to install it inside the module 
where it is needed:

```bash
$ npm install @xpack/es6-promisifier
```

The module does not provide any executables, and generally there are
few reasons to install it globally.

The development repository is available from the GitHub 
[`xpack/es6-promisifier-js`](https://github.com/xpack/es6-promisifier-js) 
project.

## User info

This section is intended for those who want to use this module in their
own projects.

The module can be included in any applications and the `Promisifier` 
class can be used to access the `promisify()` functions.

```javascript
const Promisifier = require('@xpack/es6-promisifier').Promisifier

// Promisify functions from the Node.js callbacks library.
// The new functions have similar names, but suffixed with `Promise`,
// or below a `promises` object.
Promisifier.promisifyInPlace(fs, 'open')

// New use case:
//   const fsPromises = fs.promises_
//   await fsPromises.open()

// Old use case:
//   await fs.openPromise()

// Note: using `promises_` is a workaround to avoid the `fs.promises`
// warning.

// Promisify a single function from a third party simple module.
const mkdirpPromise = Promisifier.promisify(require('mkdirp'))
```

## Maintainer info

This page documents how to use this module in an user application.
For maintainer information, see the separate
[README-MAINTAINER](https://github.com/xpack/es6-promisifier-js/blob/master/README-MAINTAINER.md)
page.

## License

The original content is released under the
[MIT License](https://opensource.org/licenses/MIT), with all rights
reserved to [Liviu Ionescu](https://github.com/ilg-ul).
