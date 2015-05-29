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

#ifndef TOPS_MODEL_LABELING_
#define TOPS_MODEL_LABELING_

namespace tops {
namespace model {

template<typename Target>
class Labeling;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                     CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @class Labeling
 * @brief TODO
 */
template<typename Target>
class Labeling {
 public:
  // Symbols
  enum class Method { bestPath, posteriorDecoding };

  // Constructors
  Labeling();
  Labeling(const Target &observation, const Target &label);
  Labeling(Target &&observation, Target &&label);

  // Getters
  const Target& observation() const;
  Target& observation();

  const Target& label() const;
  Target& label();

 private:
  Target _observation;
  Target _label;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                  IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Target>
Labeling<Target>::Labeling()
    : _observation(), _label() {
}

template<typename Target>
Labeling<Target>::Labeling(const Target &observation, const Target &label)
    : _observation(observation), _label(label) {
}

template<typename Target>
Labeling<Target>::Labeling(Target &&observation, Target &&label)
    : _observation(std::move(observation)), _label(std::move(label)) {
}

/*----------------------------------------------------------------------------*/
/*                           Getters and Setters                              */
/*----------------------------------------------------------------------------*/

template<typename Target>
Target& Labeling<Target>::observation() const {
  return _observation;
}

template<typename Target>
Target& Labeling<Target>::observation() {
  return const_cast<Target &>(
    static_cast<const Labeling *>(this)->observation());
}

template<typename Target>
Target& Labeling<Target>::label() const {
  return _label;
}

template<typename Target>
Target& Labeling<Target>::label() {
  return const_cast<Target &>(
    static_cast<const Labeling *>(this)->label());
}

}  // namespace model
}  // namespace tops

#endif
