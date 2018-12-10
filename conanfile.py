#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake


class LunaPackageConan(ConanFile):
    name = "luna-cli"
    version = "1.0.0"
    url = "https://github.com/DEGoodmanWilson/luna-cli.git"
    license = "MIT"

    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "inja/[~=1.0]@DEGoodmanWilson/stable", "docopt/[~=0.6]@conan/stable", "boost_filesystem/[~=1.67]@bincrafters/stable", "libgit2/[~=0.26.0]@DEGoodmanWilson/testing"

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()
