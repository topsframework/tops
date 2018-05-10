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

// Standard headers
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <exception>

// Internal headers
#include "config/BasicConfig.hpp"
#include "config/DiscreteConverter.hpp"
#include "config/StringLiteralSuffix.hpp"

#include "config/model/training/ModelConfig.hpp"
#include "config/model/training/DecodableModelConfig.hpp"

#include "config/model/definition/ModelConfig.hpp"
#include "config/model/definition/DecodableModelConfig.hpp"

#include "lang/Interpreter.hpp"
#include "lang/ModelConfigSerializer.hpp"

// External headers
#include "chaiscript/language/chaiscript_common.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }
namespace { namespace ct = tops::config::training; }
namespace { namespace cd = tops::config::definition; }

// Using declarations
using tops::config::operator ""_t;

int usage(const std::string &program_name) {
  std::cerr << "USAGE: " << program_name
            << " (-d|-t) model_config [dataset] [output_dir]" << std::endl;
  return EXIT_FAILURE;
}

template<typename ModelConfig, typename DecodableModelConfig>
decltype(auto) makeConverters(tops::config::ConfigPtr cfg) {
  std::vector<tops::config::ConverterPtr> converters;

  auto model_cfg
    = std::dynamic_pointer_cast<ModelConfig>(cfg);

  if (model_cfg) {
    auto &domain = std::get<decltype("observations"_t)>(*model_cfg);
    converters.push_back(domain->makeConverter());
  }

  auto decodable_model_cfg
    = std::dynamic_pointer_cast<DecodableModelConfig>(model_cfg);

  if (decodable_model_cfg) {
    auto &domains
      = std::get<decltype("other_observations"_t)>(*decodable_model_cfg);

    for (auto &domain : domains)
      converters.push_back(domain->makeConverter());

    converters.push_back(
      std::get<decltype("labels"_t)>(*decodable_model_cfg)->makeConverter());
  }

  return converters;
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      MAIN
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

int main(int argc, char **argv) try {
  if (argc <= 2 || argc >= 6)
    return usage(argv[0]);

  tops::lang::Interpreter interpreter;

  tops::config::ConfigPtr cfg;
  std::string config_type = argv[1];

  if (config_type == "-d") {
    cfg = interpreter.evalModelDefinition(argv[2]);
  } else if (config_type == "-t") {
    cfg = interpreter.evalModelTraining(argv[2]);
  } else {
    return usage(argv[0]);
  }

  /*--------------------------------------------------------------------------*/
  /*                                CONVERTER                                 */
  /*--------------------------------------------------------------------------*/

  if (argc >= 4) {
    std::vector<tops::config::ConverterPtr> converters;

    if (config_type == "-d") {
      converters = makeConverters<cd::ModelConfig,
                                  cd::DecodableModelConfig>(cfg);
    } else if (config_type == "-t") {
      converters = makeConverters<ct::ModelConfig,
                                  ct::DecodableModelConfig>(cfg);
    }

    std::fstream dataset(argv[3]);
    std::string line;

    // Header
    std::getline(dataset, line);
    std::cout << line << std::endl;

    // Data
    while (std::getline(dataset, line)) {
      std::stringstream ss(line);

      bool first = true;
      std::string input;
      for (const auto &converter : converters) {
        std::getline(ss, input, '\t');
        std::cout << (first ? '\0' : '\t') << converter->convert(input);
        first = false;
      }

      std::cout << std::endl;
    }

    std::cout << std::endl;
  }

  /*--------------------------------------------------------------------------*/
  /*                           PRINTER / SERIALIZER                           */
  /*--------------------------------------------------------------------------*/

  switch (argc) {
    case 3: /* fall through */
    case 4: cfg->accept(tops::lang::ModelConfigSerializer(std::cerr)); break;
    case 5: cfg->accept(tops::lang::ModelConfigSerializer(argv[4])); break;
  }

  return EXIT_SUCCESS;
}
catch(chaiscript::exception::eval_error &e) {
  std::cerr << e.pretty_print() << std::endl;
}
catch(std::exception &e) {
  std::cerr << e.what() << std::endl;
}
