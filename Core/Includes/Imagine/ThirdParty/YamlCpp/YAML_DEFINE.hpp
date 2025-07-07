//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>

#define KEY(x) YAML::Key << x
#define VAL(x) YAML::Value << x
#define KEYVAL(x, y) KEY(x) << VAL(y)