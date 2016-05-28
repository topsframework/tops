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

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Target>
Estimation<Target>::Estimation(Target estimated,
                               Probability probability)
    : _estimated(std::move(estimated)), _probability(probability) {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Target>
const Target& Estimation<Target>::estimated() const {
  return _estimated;
}

/*----------------------------------------------------------------------------*/

template<typename Target>
Target& Estimation<Target>::estimated() {
  return const_cast<Target&>(
    static_cast<const Estimation*>(this)->estimated());
}

/*----------------------------------------------------------------------------*/

template<typename Target>
const Probability& Estimation<Target>::probability() const {
  return _probability;
}

/*----------------------------------------------------------------------------*/

template<typename Target>
Probability& Estimation<Target>::probability() {
  return const_cast<Probability&>(
    static_cast<const Estimation*>(this)->probability());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
