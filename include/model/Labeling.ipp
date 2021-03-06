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

// Standard headers
#include <utility>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Target>
Labeling<Target>::Labeling(Target observation, Target label)
    : _observation(std::move(observation)), _label(std::move(label)) {
}

template<typename Target>
Labeling<Target>::Labeling(Target observation,
                           std::vector<Target> other_observations,
                           Target label)
    : _observation(std::move(observation)),
      _other_observations(other_observations),
      _label(std::move(label)) {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Target>
const Target& Labeling<Target>::observation() const {
  return _observation;
}

/*----------------------------------------------------------------------------*/

template<typename Target>
Target& Labeling<Target>::observation() {
  return const_cast<Target&>(static_cast<const Labeling*>(this)->observation());
}

/*----------------------------------------------------------------------------*/

template<typename Target>
const std::vector<Target>& Labeling<Target>::other_observations() const {
  return _other_observations;
}

/*----------------------------------------------------------------------------*/

template<typename Target>
std::vector<Target>& Labeling<Target>::other_observations() {
  return const_cast<Target&>(
    static_cast<const Labeling*>(this)->other_observations());
}

/*----------------------------------------------------------------------------*/

template<typename Target>
const Target& Labeling<Target>::label() const {
  return _label;
}

/*----------------------------------------------------------------------------*/

template<typename Target>
Target& Labeling<Target>::label() {
  return const_cast<Target&>(static_cast<const Labeling*>(this)->label());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
