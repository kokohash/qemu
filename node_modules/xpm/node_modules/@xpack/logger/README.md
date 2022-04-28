[![npm (scoped)](https://img.shields.io/npm/v/@xpack/logger.svg)](https://www.npmjs.com/package/@xpack/logger)
[![license](https://img.shields.io/github/license/xpack/logger-ts.svg)](https://github.com/xpack/logger-ts/blob/xpack/LICENSE)
[![TS-Standard - Typescript Standard Style Guide](https://badgen.net/badge/code%20style/ts-standard/blue?icon=typescript)](https://github.com/standard/ts-standard)
[![Actions Status](https://github.com/xpack/logger-ts/workflows/CI%20on%20Push/badge.svg)](https://github.com/xpack/logger-ts/actions)
[![GitHub issues](https://img.shields.io/github/issues/xpack/logger-ts.svg)](https://github.com/xpack/logger-ts/issues/)
[![GitHub pulls](https://img.shields.io/github/issues-pr/xpack/logger-ts.svg)](https://github.com/xpack/logger-ts/pulls)

## A generic console logger class

A Node.js module with a generic console logger.

## Prerequisites

A recent [Node.js](https://nodejs.org) (>=10.x), since the TypeScript code
is compiled to ECMAScript 2018 code.

## Easy install

The module is available as
[`@xpack/logger`](https://www.npmjs.com/package/@xpack/logger)
from the public repository; use `npm` to install it inside the module where
it is needed:

```console
npm install @xpack/logger@latest
```

The module does not provide any executables, and generally there are no
reasons to install it globally.

The development repository is available from the GitHub
[xpack/logger-ts](https://github.com/xpack/logger-ts)
project.

## User info

This section is intended for those who want to use this module in their
own projects.

The `@xpack/logger` module can be imported in both TypeScript
and JavaScript Node.js code.

In TypeScript, use `import`:

```typescript
import { Logger } from '@xpack/xpm-liquid'
```

In JavaScript, use `require()`:

```javascript
const { Logger } = require('@xpack/logger')
```

The typical use case is to create an instance of the Logger object,
then log at different levels:

```javascript
const log = new Logger({
  level: 'info'
})

log.info('hello') // Displayed on stdout.
log.debug('world') // Ignored.
```

In more complex use cases, the log level can be tested and the possibly
long operations be performed only if necessary.

### Log levels

The following strings are recognised as valid level names:

```typescript
export type LogLevel =
  'silent' | 'error' | 'warn' | 'info' | 'verbose' | 'debug' | 'trace' | 'all'
```

Internally they are converted to integer values, and these integers
are used in comparisons. Higher values
mean more verbosity.

### Delaying setting the log level

There are cases when the logger must be created very early in the
life cycle of an application, even before it is practically possible
to determine the log level.

For these cases, if the logger is created without a log level,
it is set to a **preliminary state**, and all log lines are
stored in an internal buffer**, until the log
level is set, when the buffer is walked and the lines are processed.

### Constructor

#### `Logger(params: ConstructorParameters)`

The common use case is to create the logger instance with a `console` and a
string `level` name.

```typescript
export interface LoggerParameters {
  level?: LogLevel
  console?: Console
}
```

If present, the `console` must be an object with at least two methods,
`log()` and `error()`, as defined in the Node.js documentation for
[console](https://nodejs.org/dist/latest-v10.x/docs/api/console.html).

By default, the system console is used.

Example:

```javascript
const log = new Logger({
  console: myConsole,
  level: 'info'
})
```

The `level` property is optional since it can be set later.
Without it, the constructor will
create the logger in a preliminary state, and all log lines will be stored
in an internal buffer until the log level is set.

Example:

```javascript
const log = new Logger()
```

### Managing the log levels

The log level is managed by a setter/getter pair.

#### `set level (level: LogLevel)` (setter)

Set the log level. If this is the first time the log level is set, flush the
internal buffer.

Example:

```javascript
log.level = 'info'
```

#### `get level (): LogLevel` (getter)

Get the current log level, as a string.

Example:

```javascript
console.log(log.level)
```

#### `get hasLevel (): boolean` (getter)

[Added in v2.1.0]
[Changed to getters in v5.0.0]

Return `true` if the level was set.

Example:

```console
if (!log.hasLevel) {
  log.level = defaultLevel
}
```

### Logging lines

All functions accept an optional string message and possibly some arguments,
as processed by the standard Node.js
[`util.format(msg, ...args)`](https://nodejs.org/dist/latest-v10.x/docs/api/util.html#util_util_format_format_args)
function.

#### `always (msg: any = '', ...args: any[]): void`

Log always, regardless of the log level, even `'silent'`, when no other
messages are logged. The message is passed via `console.log()`

Example:

```javascript
log.always(version)
```

#### `error (msg: any = '', ...args: any[]): void`

Log errors, if the log level is `'error'` or higher. The message is prefixed
with `error: ` and passed via `console.error()`.

Example:

```javascript
log.error('Not good...')
```

#### `error (msg: Error): void`

This is a special case when the input is an `Error` object. It is expanded,
including a full stack trace, and passed via `console.error()`.

Example:

```javascript
try {
  // ...
} catch (ex) {
  log.error(ex)
}
```

#### `output (msg: any = '', ...args: any[]): void`

Log errors, if the log level is `'error'` or higher. The message is passed
via `console.log`.

It differs from `error()` by not prefixing the string with `error: ` and using
`console.log()` instead of `console.error()`.

Examples:

```javascript
log.output('Not good either...')
```

```javascript
try {
  // ...
} catch (ex) {
  // Do not show the stack trace.
  log.output(ex.message)
}
```

#### `warn (msg: any = '', ...args: any[]): void`

Log warnings, if the log level is `'warn'` or higher. The message is prefixed
with `warning: ` and passed via `console.error()`.

Example:

```javascript
log.warn('Beware...')
```

#### `info (msg: any = '', ...args: any[]): void`

Log informative messages, if the log level is `'info'` or higher.
The message is passed via `console.log()`.

Example:

```javascript
log.info(title)
```

#### `verbose (msg: any = '', ...args: any[]): void`

Log more informative messages, if the log level is `'verbose'` or higher.
The message is passed via `console.log()`.

Example:

```javascript
log.verbose('Configurations:')
```

#### `debug (msg: any = '', ...args: any[]): void`

Log debug messages, if the log level is `'debug'` or higher.
The message is passed via `console.log()`.

Example:

```javascript
log.debug(`spawn: ${cmd}`)
```

#### `trace (msg: any = '', ...args: any[]): void`

Log debug messages, if the log level is `'trace'` or higher.
The message is passed via `console.log()`.

Example:

```javascript
log.trace(`${this.constructor.name}.doRun()`)
```

### Checking log levels

If the logging code is more complex than a single line,
for example it needs a long loop,
it is recommended to explicitly check the log level and
if not high enough, skip the code entirely.

Example:

```javascript
  if (log.isVerbose) {
    for (const [folderName, folder] of Object.entries(folders)) {
      log.trace(`'${folderName}' ${folder.toolchainOptions}`)
    }
  }
```

[Changed to getters in v3.0.0]

#### `get isSilent (): boolean` (getter)

Return `true` if the log level is `'silent'` or higher.

#### `get isError (): boolean` (getter)

Return `true` if the log level is `'error'` or higher.

#### `get isWarn (): boolean` (getter)

Return `true` if the log level is `'warn'` or higher.

#### `get isInfo (): boolean` (getter)

Return `true` if the log level is `'info'` or higher.

#### `get isVerbose (): boolean` (getter)

Return `true` if the log level is `'verbose'` or higher.

#### `get isDebug (): boolean` (getter)

Return `true` if the log level is `'debug'` or higher.

#### `get isTrace (): boolean` (getter)

Return `true` if the log level is `'trace'` or higher.

#### `get isAll (): boolean` (getter)

Return `true` if the log level is `'all'`.

#### `get console (): Console` (getter)

Return the console object associated with the logger.

#### `Logger.defaultLevel`

A static definition with the default logger level (`info`).

#### `Logger.numericLevels`

A static map with the internal values for the log levels.

## Compatibility notices

According to [semver](https://semver.org) requirements,
incompatible API changes require higher major numbers.

### v5.x

For consistency reasons, `hasLevel` was changed from a method to a getter.

Internally, the log level starts as `undefined` instead of the
string `'undefined'`, as in previous versions.

This should not be a problem, given that the method to check if
the level was set is via `hasLevel()`.

### v4.x

The code was migrated to TypeScript.

The migration itself should not introduce any incompatibilities,
actually it should be fairly compatible with the latest v3.x,
but, for just in case, the safer path was to consider it a major
release.

### v3.x

All `isXyx` functions (returning a boolean related to
the log level) were changed to getters.

### v2.x

The logger constructor was changed to use the generic arguments object.

If upgrading from previous versions, change the syntax from:

```javascript
const log = new Logger(console, 'info')
```

to:

```javascript
const log = new Logger({
  console,
  level: 'info'
})
```

## Maintainer & developer info

This page documents how to use this module in an user application.
For developer and maintainer information, see the separate
[README-DEVELOPER](https://github.com/xpack/logger-ts/blob/master/README-DEVELOPER.md) and
[README-MAINTAINER](https://github.com/xpack/logger-ts/blob/master/README-MAINTAINER.md)
pages.

## License

The original content is released under the
[MIT License](https://opensource.org/licenses/MIT), with all rights
reserved to [Liviu Ionescu](https://github.com/ilg-ul/).
