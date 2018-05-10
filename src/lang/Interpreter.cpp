/***********************************************************************/
/*  Copyright 2016 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

// Interface header
#include "lang/Interpreter.hpp"

// Standard headers
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <typeinfo>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigRegister.hpp"
#include "lang/DependencyTreeParser.hpp"

#include "config/Domain.hpp"
#include "config/StringLiteralSuffix.hpp"

#include "config/Options.hpp"

#include "config/model/ModelConfig.hpp"

#include "config/model/training/ModelConfig.hpp"
#include "config/model/training/HMMConfig.hpp"
#include "config/model/training/IIDConfig.hpp"
#include "config/model/training/MDDConfig.hpp"
#include "config/model/training/GHMMConfig.hpp"
#include "config/model/training/VLMCConfig.hpp"
#include "config/model/training/PeriodicIMCConfig.hpp"

#include "config/model/definition/ModelConfig.hpp"
#include "config/model/definition/HMMConfig.hpp"
#include "config/model/definition/IIDConfig.hpp"
#include "config/model/definition/IMCConfig.hpp"
#include "config/model/definition/MDDConfig.hpp"
#include "config/model/definition/MSMConfig.hpp"
#include "config/model/definition/GHMMConfig.hpp"
#include "config/model/definition/SBSWConfig.hpp"
#include "config/model/definition/VLMCConfig.hpp"
#include "config/model/definition/LCCRFConfig.hpp"
#include "config/model/definition/PeriodicIMCConfig.hpp"

#include "config/state/StateConfig.hpp"

#include "config/duration/DurationConfig.hpp"
#include "config/duration/FixedDurationConfig.hpp"
#include "config/duration/ExplicitDurationConfig.hpp"
#include "config/duration/GeometricDurationConfig.hpp"
#include "config/duration/MaxLengthDurationConfig.hpp"

#include "config/auxiliar/DependencyTreeConfig.hpp"
#include "config/auxiliar/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

// Using declarations
using tops::config::operator ""_t;

// Namespace aliases
namespace { namespace co = tops::config::option; }
namespace { namespace ct = tops::config::training; }
namespace { namespace cd = tops::config::definition; }
namespace { namespace cot = co::training; }
namespace { namespace cod = co::definition; }

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                               LOCAL STRUCTS                                */
/*----------------------------------------------------------------------------*/

template<typename T>
struct get_inner {
  using type = std::remove_cv_t<T>;
};

template<typename T>
struct get_inner<std::unique_ptr<T>> {
  using type = typename std::unique_ptr<T>::element_type;
};

template<typename T>
struct get_inner<std::shared_ptr<T>> {
  using type = typename std::shared_ptr<T>::element_type;
};

template<typename T>
using get_inner_t = typename get_inner<T>::type;

/*----------------------------------------------------------------------------*/
/*                                LOCAL MACROS                                */
/*----------------------------------------------------------------------------*/

#define REGISTER_TYPE(type, name) \
  do { \
    using chaiscript::bootstrap::standard_library::assignable_type; \
    using registered_type = get_inner_t<type>; \
    module->add(chaiscript::user_type<registered_type>(), name); \
    assignable_type<registered_type>(name, *module); \
  } while (false)

#define REGISTER_MAP(type, name) \
  do { \
    using chaiscript::map_conversion; \
    using chaiscript::bootstrap::standard_library::map_type; \
    using registered_type = get_inner_t<type>; \
    map_type<registered_type>(name, *module); \
    module->add(map_conversion<registered_type>()); \
  } while (false)

#define REGISTER_VECTOR(type, name) \
  do { \
    using chaiscript::vector_conversion; \
    using chaiscript::bootstrap::standard_library::vector_type; \
    using registered_type = get_inner_t<type>; \
    vector_type<registered_type>(name, *module); \
    module->add(vector_conversion<registered_type>()); \
  } while (false)

#define REGISTER_COMMON_TYPE(type) \
  REGISTER_TYPE(co::type, #type)
#define REGISTER_COMMON_MAP(type) \
  REGISTER_COMMON_TYPE(type); REGISTER_MAP(co::type, #type)
#define REGISTER_COMMON_VECTOR(type) \
  REGISTER_COMMON_TYPE(type); REGISTER_VECTOR(co::type, #type)

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

cot::Model Interpreter::evalModelTraining(const std::string &filepath) {
  checkExtension(filepath);
  return makeModelTrainingConfig(filepath, "untrained_model");
}

/*----------------------------------------------------------------------------*/

cod::Model Interpreter::evalModelDefinition(const std::string &filepath) {
  checkExtension(filepath);
  return makeModelDefinitionConfig(filepath, "model");
}

/*----------------------------------------------------------------------------*/

void Interpreter::checkExtension(const std::string &filepath) {
  auto suffix = extractSuffix(filepath);

  if (suffix != "tops") {
    std::ostringstream error_message;
    error_message << "Unknown extension ." << suffix;
    throw std::invalid_argument(error_message.str());
  }
}

/*----------------------------------------------------------------------------*/

cot::Model Interpreter::makeModelTrainingConfig(const std::string &filepath,
                                                const std::string &label) {
  auto model_type = getConfigOption<
    ct::ModelConfig, decltype("model_type"_t)>(filepath, label);
  auto training_algorithm = getConfigOption<
    ct::ModelConfig, decltype("training_algorithm"_t)>(filepath, label);

  if (model_type == "GHMM") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::GHMM::MaximumLikehoodConfig>(filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "HMM") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::HMM::MaximumLikehoodConfig>(filepath, label);
    } else if (training_algorithm == "BaumWelch") {
      return fillConfig<ct::HMM::BaumWelchConfig>(filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "IID") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::IID::MaximumLikehoodConfig>(
          filepath, label);
    } else if (training_algorithm == "SmoothedHistogramBurge") {
      return fillConfig<ct::IID::SmoothedHistogramBurgeConfig>(filepath, label);
    } else if (training_algorithm == "SmoothedHistogramStanke") {
      return fillConfig<ct::IID::SmoothedHistogramStankeConfig>(
          filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "MDD") {
    if (training_algorithm == "Standard") {
      return fillConfig<ct::MDD::StandardConfig>(filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "PeriodicIMC") {
    if (training_algorithm == "Interpolation") {
      return fillConfig<ct::PeriodicIMC::InterpolationConfig>(filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "VLMC") {
    if (training_algorithm == "Context") {
      return fillConfig<ct::VLMC::ContextConfig>(filepath, label);
    } else if (training_algorithm == "FixedLength") {
      return fillConfig<ct::VLMC::FixedLengthConfig>(filepath, label);
    } else if (training_algorithm == "Interpolation") {
      return fillConfig<ct::VLMC::InterpolationConfig>(filepath, label);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else {
    handleWrongStringOption(filepath, "model_type", model_type);
  }

  return nullptr;
}

/*----------------------------------------------------------------------------*/

cod::Model Interpreter::makeModelDefinitionConfig(const std::string &filepath,
                                                  const std::string &label) {
  auto model_type = getConfigOption<
    cd::ModelConfig, decltype("model_type"_t)>(filepath, label);

  if (model_type == "GHMM") {
    return fillConfig<cd::GHMMConfig>(filepath, label);
  } else if (model_type == "HMM") {
    return fillConfig<cd::HMMConfig>(filepath, label);
  } else if (model_type == "LCCRF") {
    return fillConfig<cd::LCCRFConfig>(filepath, label);
  } else if (model_type == "IID") {
    return fillConfig<cd::IIDConfig>(filepath, label);
  } else if (model_type == "VLMC") {
    return fillConfig<cd::VLMCConfig>(filepath, label);
  } else if (model_type == "IMC") {
    return fillConfig<cd::IMCConfig>(filepath, label);
  } else if (model_type == "PeriodicIMC") {
    return fillConfig<cd::PeriodicIMCConfig>(filepath, label);
  } else if (model_type == "SBSW") {
    return fillConfig<cd::SBSWConfig>(filepath, label);
  } else if (model_type == "MSM") {
    return fillConfig<cd::MSMConfig>(filepath, label);
  } else if (model_type == "MDD") {
    return fillConfig<cd::MDDConfig>(filepath, label);
  } else {
    handleWrongStringOption(filepath, "model_type", model_type);
  }

  return nullptr;
}

/*----------------------------------------------------------------------------*/

void Interpreter::handleWrongStringOption(const std::string &filepath,
                                          const std::string& option_name,
                                          const std::string& option_value) {
  if (option_value == "") {
    throw std::logic_error(filepath + ": " + option_name + " not specified!");
  } else {
    throw std::logic_error(
      filepath + ": unknown " + option_name + " \"" + option_value + "\"");
  }
}

/*----------------------------------------------------------------------------*/

bool Interpreter::missingObjectException(const std::exception &e) {
  std::string exception(e.what());
  return exception.find("Can not find object:") != std::string::npos;
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonTypes(chaiscript::ModulePtr &module,
                                      const std::string &/* filepath */) {
  // Ordinary types
  REGISTER_COMMON_TYPE(Size);
  REGISTER_COMMON_TYPE(Type);
  REGISTER_COMMON_TYPE(Model);
  REGISTER_COMMON_TYPE(State);
  REGISTER_COMMON_TYPE(Domain);
  REGISTER_COMMON_TYPE(Duration);
  REGISTER_COMMON_TYPE(Probability);
  REGISTER_COMMON_TYPE(DependencyTree);
  REGISTER_COMMON_TYPE(FeatureFunction);

  REGISTER_COMMON_MAP(States);
  REGISTER_COMMON_MAP(Probabilities);
  REGISTER_COMMON_MAP(FeatureFunctions);

  REGISTER_COMMON_VECTOR(Models);
  REGISTER_COMMON_VECTOR(Domains);
  REGISTER_COMMON_VECTOR(Alphabet);
  REGISTER_COMMON_VECTOR(Alphabets);
  REGISTER_COMMON_VECTOR(DependencyTrees);
  REGISTER_COMMON_VECTOR(FeatureFunctionLibraries);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonHelpers(chaiscript::ModulePtr &module,
                                        const std::string &filepath) {
  using chaiscript::fun;

  using config::Domain;

  using config::FixedDurationConfig;
  using config::ExplicitDurationConfig;
  using config::GeometricDurationConfig;
  using config::MaxLengthDurationConfig;

  using config::DependencyTreeConfig;
  using config::FeatureFunctionLibraryConfig;

  module->add(fun([] (const co::Alphabet &alphabet) {
    return std::make_shared<Domain>(
        typename Domain::discrete_domain{}, alphabet);
  }), "discrete_domain");

  module->add(fun([] (const co::OutToInSymbolFunction &o2i,
                      const co::InToOutSymbolFunction &i2o) {
    return std::make_shared<Domain>(
        typename Domain::custom_domain{}, o2i, i2o);
  }), "custom_domain");

  module->add(fun([filepath] (size_t size) {
    auto duration = FixedDurationConfig::make(filepath, "fixed");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return co::Duration(duration);
  }), "fixed");

  module->add(fun([filepath] (co::Model model) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("model"_t)>(*duration.get()) = model;
    return co::Duration(duration);
  }), "explicit");

  module->add(fun([filepath] (co::Model model, size_t size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get()) = model;
    return co::Duration(duration);
  }), "explicit");

  module->add(fun([filepath] () {
    auto duration = GeometricDurationConfig::make(filepath, "geometric");
    return co::Duration(duration);
  }), "geometric");

  module->add(fun([filepath] (size_t size) {
    auto duration = MaxLengthDurationConfig::make(filepath, "max_length");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return co::Duration(duration);
  }), "max_length");

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);

    std::ifstream src(root_dir + file);

    std::string line;
    std::vector<std::string> content;
    while (std::getline(src, line)) {
      content.push_back(line);
    }

    DependencyTreeParser parser(this, root_dir, file, content);
    return parser.parse();
  }), "tree");

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);
    return this->fillConfig<FeatureFunctionLibraryConfig>(root_dir + file);
  }), "lib");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonConstants(chaiscript::ModulePtr &module,
                                          const std::string &/* filepath */) {
  using chaiscript::const_var;

  module->add_global_const(const_var(std::string("emission")), "emission");
  module->add_global_const(const_var(std::string("duration")), "duration");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonAttributions(chaiscript::ModulePtr &module,
                                             const std::string &filepath) {
  using chaiscript::fun;
  using chaiscript::boxed_cast;

  using chaiscript::Boxed_Value;
  using Vector = std::vector<Boxed_Value>;
  using Map = std::map<std::string, Boxed_Value>;

  module->add(fun([] (config::Domain &conv, const Vector &orig) {
    co::Alphabet alphabet;
    for (auto &element : orig)
      alphabet.push_back(boxed_cast<co::Symbol>(element));

    conv = config::Domain(typename config::Domain::discrete_domain{}, alphabet);
  }), "=");

  module->add(fun([] (co::Alphabets &conv, const Vector &orig) {
    for (auto &element : orig) {
      auto inner_orig = boxed_cast<Vector &>(element);
      co::Alphabet inner_conv;

      for (auto &inner_element : inner_orig)
        inner_conv.push_back(boxed_cast<co::Symbol>(inner_element));

      conv.push_back(inner_conv);
    }
  }), "=");

  module->add(fun([filepath] (co::States &conv, const Map &orig) {
    for (auto &pair : orig) {
      auto inner_orig = boxed_cast<Map &>(pair.second);

      conv[pair.first] = config::StateConfig::make(filepath);

      std::get<decltype("duration"_t)>(*conv[pair.first])
        = boxed_cast<co::Duration>(inner_orig["duration"]);

      std::get<decltype("emission"_t)>(*conv[pair.first])
        = boxed_cast<co::Model>(inner_orig["emission"]);
    }
  }), "=");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonConcatenations(
    chaiscript::ModulePtr &module, const std::string &/* filepath */) {
  using chaiscript::fun;

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return lhs + " | " + rhs;
  }), "|");

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return rhs + " | " + lhs;
  }), "->");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerTrainingHelpers(chaiscript::ModulePtr &module,
                                          const std::string &filepath) {
  using chaiscript::fun;

  module->add(fun([this, filepath] (const std::string &file) {
    return co::Model(this->makeModelTrainingConfig(
          extractDir(filepath) + file, "untrained_model"));
  }), "untrained_model");

  module->add(fun([this, filepath] (const std::string &file) {
    return co::Model(this->makeModelDefinitionConfig(
          extractDir(filepath) + file, "pretrained_model"));
  }), "pretrained_model");

  module->add(fun([filepath] (const std::string &file) {
    return extractDir(filepath) + file;
  }), "dataset");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerDefinitionHelpers(chaiscript::ModulePtr &module,
                                            const std::string &filepath) {
  using chaiscript::fun;

  module->add(fun([this, filepath] (const std::string &file) {
    return co::Model(this->makeModelDefinitionConfig(
          extractDir(filepath) + file, "model"));
  }), "model");
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
