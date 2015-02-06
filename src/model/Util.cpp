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
#include <cmath>

// ToPS headers
#include "model/Util.hpp"

namespace tops {
namespace model {

double lookup (double x){
  // assert (x >= 0.00f);
  // assert (x <= 7.5);
  //return ((-0.00653779113685f * x + 0.09537236626558f) * x + 0.55317574459331f) * x + 0.68672959851568f;
  if (x <= 1.00f) return ((-0.009350833524763f * x + 0.130659527668286f) * x + 0.498799810682272f) * x + 0.693203116424741f;
  if (x <= 2.50f) return ((-0.014532321752540f * x + 0.139942324101744f) * x + 0.495635523139337f) * x + 0.692140569840976f;
  if (x <= 4.50f) return ((-0.004605031767994f * x + 0.063427417320019f) * x + 0.695956496475118f) * x + 0.514272634594009f;
  // assert (x <= 7.5);
  return ((-0.000458661602210f * x + 0.009695946122598f) * x + 0.930734667215156f) * x + 0.168037164329057f;

  //return (((0.00089738532761f * x - 0.01859488697982f) * x + 0.14415772028626f) * x + 0.49515490689159f) * x + 0.69311928966454f;
}

double log_sum (double x, double y){
  if (x < y) return (x <= -2e20 || y - x >= 7.5) ? y : lookup(y-x) + x;
  return (y <= -2e20 || x - y >= 7.5) ? x : lookup(x-y) + y;
}

}  // namespace model
}  // namespace tops
