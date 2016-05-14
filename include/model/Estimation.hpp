/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
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

#ifndef TOPS_MODEL_ESTIMATION_
#define TOPS_MODEL_ESTIMATION_

// Internal headers
#include "model/Probability.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Target>
class Estimation;

/**
 * @class Estimation
 * @brief TODO
 */
template<typename Target>
class Estimation {
 public:
  // Constructors
  Estimation() = default;
  Estimation(Target estimated, LogProbability probability);

  // Concrete methods
  const Target& estimated() const;
  Target& estimated();

  const LogProbability& probability() const;
  LogProbability& probability();

 private:
  // Instance variables
  Target _estimated;
  LogProbability _probability;
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/Estimation.ipp"

#endif  // TOPS_MODEL_ESTIMATION_
