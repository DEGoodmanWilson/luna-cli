#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake


class LunaPackageConan(ConanFile):
    name = "luna-cli"
    version = "1.0.0"
    url = "https://github.com/DEGoodmanWilson/luna-cli.git"
    license = "MIT"

    settings = "os", "compiler", "build_type", "arch"
    options = {"luna_etc_path":   "ANY"}
    default_options = "luna_etc_path=/usr/local/etc/luna"
    generators = "cmake"
    requires = "inja/[~=1.0]@DEGoodmanWilson/stable", "docopt/[~=0.6]@conan/stable", "boost_filesystem/[~=1.67]@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()
