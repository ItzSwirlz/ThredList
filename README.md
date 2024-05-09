![](https://github.com/RISC-VLIW/ThredList/assets/167657592/cd78938a-beb2-47f2-8d62-192971022f93)

# Thred List

This is a plugin for [Aroma](https://github.com/wiiu-env/Aroma) that shows a list of active threads in the plugin menu.

It is called "Thred List" instead of "Thread" because Nintendo mispelled "Thread" for "Thred" in the thread for the Eula (EulaThred). One interpretation is that they were too focused on telling people not to install CFW they didn't check their spelling.

## Installation

1. Copy the file `ThredList.wps` into `sd:/wiiu/environments/aroma/plugins`.
2. Requires the [WiiUPluginLoaderBackend](https://github.com/wiiu-env/WiiUPluginLoaderBackend) in `sd:/wiiu/environments/aroma/modules`.

Start Aroma and the backend should load the plugin.

## Building

For building you need:

- [wups](https://github.com/Maschell/WiiUPluginSystem)
- [wut](https://github.com/devkitpro/wut)

Install them (in this order) according to their README's. Don't forget the dependencies of the libs itself.

Then you should be able to compile via `make` (with no logging) or `make DEBUG=1` (with logging).

## Buildflags

### Logging

Building via `make` only logs errors (via OSReport). To enable logging via the [LoggingModule](https://github.com/wiiu-env/LoggingModule) set `DEBUG` to `1` or `VERBOSE`.

`make` Logs errors only (via OSReport).
`make DEBUG=1` Enables information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).
`make DEBUG=VERBOSE` Enables verbose information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).

If the [LoggingModule](https://github.com/wiiu-env/LoggingModule) is not present, it'll fallback to UDP (Port 4405) and [CafeOS](https://github.com/wiiu-env/USBSerialLoggingModule) logging.

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t example-plugin-builder

# make
docker run -it --rm -v ${PWD}:/project example-plugin-builder make DEBUG=1

# make clean
docker run -it --rm -v ${PWD}:/project example-plugin-builder make clean
```

## Format the code via docker

`docker run --rm -v ${PWD}:/src ghcr.io/wiiu-env/clang-format:13.0.0-2 -r ./src -i`
