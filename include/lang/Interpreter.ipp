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
#include <memory>
#include <vector>
#include <string>
#include <iostream>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigRegister.hpp"

#include "config/BasicConfig.hpp"

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Config, typename Option>
bool Interpreter::hasConfigOption(const std::string &filepath,
                                  const std::string &label) {
  auto root_dir = extractDir(filepath);

  std::vector<std::string> modulepaths;
  std::vector<std::string> usepaths { root_dir };

  chaiscript::ChaiScript chai(modulepaths, usepaths);
  chai.add(makeInterpreterLibrary<Config>(filepath));

  auto cfg = Config::make(filepath, label);
  cfg->accept(ModelConfigRegister(chai));

  try { chai.eval_file(filepath); }
  catch (const std::exception &e) {
    // Explicitly ignore missing object exceptions
    if (missingObjectException(e)) return false;

    // Do not ignore any other exceptions
    throw;
  }

  return true;
}

/*----------------------------------------------------------------------------*/

template<typename Config, typename Option>
auto Interpreter::getConfigOption(const std::string &filepath,
                                  const std::string &label) {
  auto root_dir = extractDir(filepath);

  std::vector<std::string> modulepaths;
  std::vector<std::string> usepaths { root_dir };

  chaiscript::ChaiScript chai(modulepaths, usepaths);
  chai.add(makeInterpreterLibrary<Config>(filepath));

  auto cfg = Config::make(filepath, label);
  cfg->accept(ModelConfigRegister(chai));

  try { chai.eval_file(filepath); }
  catch (const std::exception &e) {
    // Explicitly ignore missing object exceptions
    if (!missingObjectException(e)) throw;
  }

  return std::get<Option>(*cfg.get());
}

/*----------------------------------------------------------------------------*/

template<typename Config>
std::shared_ptr<Config> Interpreter::fillConfig(const std::string &filepath,
                                                const std::string &label) {
  auto root_dir = extractDir(filepath);

  std::vector<std::string> modulepaths;
  std::vector<std::string> usepaths { root_dir };

  chaiscript::ChaiScript chai(modulepaths, usepaths);
  chai.add(makeInterpreterLibrary<Config>(filepath));

  auto cfg = Config::make(filepath, label);
  cfg->accept(ModelConfigRegister(chai));

  chai.eval_file(filepath);

  return cfg;
}

/*----------------------------------------------------------------------------*/

template<typename Config>
chaiscript::ModulePtr Interpreter::makeInterpreterLibrary(
    const std::string &filepath,
    std::enable_if_t<std::is_base_of<ct::ModelConfig, Config>::value>*) {
  static auto interpreter_library = std::make_shared<chaiscript::Module>();
  static bool initialized = false;

  if (!initialized) {
    registerCommonTypes(interpreter_library, filepath);
    registerCommonHelpers(interpreter_library, filepath);
    registerCommonConstants(interpreter_library, filepath);
    registerCommonAttributions(interpreter_library, filepath);
    registerCommonConcatenations(interpreter_library, filepath);

    registerTrainingHelpers(interpreter_library, filepath);

    initialized = true;
  }

  return interpreter_library;
}

/*----------------------------------------------------------------------------*/

template<typename Config>
chaiscript::ModulePtr Interpreter::makeInterpreterLibrary(
    const std::string &filepath,
    std::enable_if_t<std::is_base_of<cd::ModelConfig, Config>::value>*) {
  static auto interpreter_library = std::make_shared<chaiscript::Module>();
  static bool initialized = false;

  if (!initialized) {
    registerCommonTypes(interpreter_library, filepath);
    registerCommonHelpers(interpreter_library, filepath);
    registerCommonConstants(interpreter_library, filepath);
    registerCommonAttributions(interpreter_library, filepath);
    registerCommonConcatenations(interpreter_library, filepath);

    registerDefinitionHelpers(interpreter_library, filepath);

    initialized = true;
  }

  return interpreter_library;
}

/*----------------------------------------------------------------------------*/

template<typename Config>
chaiscript::ModulePtr Interpreter::makeInterpreterLibrary(
    const std::string &filepath,
    std::enable_if_t<
      !std::is_base_of<cd::ModelConfig, Config>::value &&
      !std::is_base_of<ct::ModelConfig, Config>::value>*) {
  static auto interpreter_library = std::make_shared<chaiscript::Module>();
  static bool initialized = false;

  if (!initialized) {
    registerCommonTypes(interpreter_library, filepath);
    registerCommonHelpers(interpreter_library, filepath);
    registerCommonConstants(interpreter_library, filepath);
    registerCommonAttributions(interpreter_library, filepath);
    registerCommonConcatenations(interpreter_library, filepath);

    initialized = true;
  }

  return interpreter_library;
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
