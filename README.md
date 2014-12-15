# Node.qml [![Build Status](https://travis-ci.org/trollixx/node.qml.svg?branch=master)](https://travis-ci.org/trollixx/node.qml)
Node.qml is an effort to provide a [Node.js](http://nodejs.org/) compatibility layer to QML applications.

Potentially, QML applications should be able to use majority of Node.js libraries. Node.js C++ addons will not be supported.

**The project is in a Work-In-Progress state, and is not ready for any practical use.**

## Roadmap
Available [here](https://github.com/trollixx/node.qml/wiki/Roadmap).

## Architecture
Node.qml consists of the following components:
- **C++ Library** - provides an API for integration of Node.qml into a C++/Qt application.
- **QML Plugin** - a plugin, that extends QML global object with Node.js specific features.
- **nodeqml Binary** - an executable to run JavaScript scripts similar to _node_ binary.

## Requirements
- Linux environment (other platforms are out of scope before the initial release).
- Qt 5.5 snapshot (_dev_ branch) with a [patch](https://codereview.qt-project.org/100434).

## Authors
Oleg Shparber

## License
Node.qml is available under the terms of the GNU Lesser General Public License version 3 (LGPLv3).
