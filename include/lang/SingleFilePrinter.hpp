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

#ifndef LANG_SINGLE_FILE_PRINTER_
#define LANG_SINGLE_FILE_PRINTER_

// Standard headers
#include <memory>

// Internal headers
#include "lang/FilePrinter.hpp"

#include "config/Domain.hpp"

#include "config/model/ModelConfig.hpp"
#include "config/state/StateConfig.hpp"
#include "config/duration/DurationConfig.hpp"
#include "config/auxiliar/DependencyTreeConfig.hpp"
#include "config/auxiliar/FeatureFunctionLibraryConfig.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }

namespace tops {
namespace lang {

/**
 * @class SingleFilePrinter
 * @brief Class to print config::BasicConfig in a single file
 */
class SingleFilePrinter : public FilePrinter {
 public:
  // Aliases
  using Base = FilePrinter;
  using Self = SingleFilePrinter;

  // Constructors
  explicit SingleFilePrinter(std::shared_ptr<std::ostream> os);

  // Static methods
  template<typename... Args>
  static decltype(auto) make(Args&&... args);

  // Overriden methods
  void print(co::Domain domain) override;
  void print(co::Model model) override;
  void print(co::State state) override;
  void print(co::Duration duration) override;
  void print(co::DependencyTree tree) override;
  void print(co::FeatureFunctionLibrary library) override;

 protected:
  // Hidden constructor inheritance
  using Base::Base;
};

}  // namespace lang
}  // namespace tops

// Implementation header
#include "lang/SingleFilePrinter.ipp"

#endif  // LANG_SINGLE_FILE_PRINTER_
