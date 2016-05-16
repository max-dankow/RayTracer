#ifndef RAYTRACER_ARGUMENTSREADER_H
#define RAYTRACER_ARGUMENTSREADER_H

#include <iostream>
#include <assert.h>
#include "Scene.h"

struct Arguments {

    Arguments() { }

    SceneProperties sceneProperties;
    bool helpFlag = false;
};

class ArgumentsReader {
public:
    static Arguments readArguments(int argc, char *argv[]) {
        assert(argc > 0);
        Arguments arguments;
        size_t current = 1;
        size_t argumentsNumber = (size_t) argc;
        std::string argument;
        while(nextArgument(current, argumentsNumber, argv, argument)) {
            if (argument == "--help") {
                printHelp(std::cout);
                arguments.helpFlag = true;
                continue;
            }
            if (argument == "--nolight") {
                arguments.sceneProperties.enableIllumination = false;
                continue;
            }
            if (argument == "--norefl") {
                arguments.sceneProperties.enableReflection = false;
                continue;
            }
            if (argument == "--norefr") {
                arguments.sceneProperties.enableRefraction = false;
                continue;
            }
            if (argument == "--complexlight") {
                arguments.sceneProperties.enableIndirectIllumination = true;
                continue;
            }
            if (argument == "--photons") {
                if (nextArgument(current, argumentsNumber, argv, argument)) {
                    arguments.sceneProperties.photonsNumber = (size_t) std::stoi(argument);
                } else {
                    throw std::invalid_argument("Failed to read number of photons");
                }
                continue;
            }
            if (argument == "--adaptiveaa") {
                arguments.sceneProperties.enableAntiAliasing = true;
                continue;
            }
            std::cerr << "[Warning] Unknown argument '" << argument << "'\n";
        }
        return arguments;
    }

    static void printHelp(std::ostream &output) {
        output << "Welcome to RayTracer. Usage:\n"
        << "[--help] - show this help message\n"
        << "[--nolight] - disable all illumination, and consequently any other optical effects (enabled by default)\n"
        << "[--norefl] - disable reflection (enabled by default)\n"
        << "[--norefr] - disable refraction (enabled by default)\n"
        << "[--complexlight] - enable complex illumination\n"
        << "[--photons] <number> - set photons number (5000000 photons by default)\n"
        << "[--adaptiveaa] - enable adaptive anti aliasing (x16 by default)\n";
    }

private:

    static bool nextArgument(size_t &current, size_t argumentsNumber, char *argv[], std::string &argument) {
        if (current == argumentsNumber) {
            return false;
        }
        argument = std::string(argv[current]);
        current++;
        return true;
    }
};


#endif //RAYTRACER_ARGUMENTSREADER_H
