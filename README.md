# mruby-jvm [![Build Status](https://travis-ci.org/jkutner/mruby-jvm.svg)](https://travis-ci.org/jkutner/mruby-jvm)

mruby gem for running JVM processes

## Usage

* In Process: `JavaSupport.exec_java(java_opts, java_class, program_opts)`
* Out of Process: `JavaSupport.system_java(java_opts, java_class, program_opts)`

Also generates an `mruby-jvm` executable that is mainly for testing.

## Building

```
$ docker-compose build
$ docker-compose run compile
```

## Testing

```
$ docker-compose run mtest
```
