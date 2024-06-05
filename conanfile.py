from conan import ConanFile


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("glog/0.7.0")
        self.requires("gtest/1.14.0")
        self.requires("benchmark/1.8.4")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")
