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
#include <limits>

// ToPS headers
#include "model/Util.hpp"

namespace tops {
namespace model {

double lookup(double x) {
  if (x <= 1.00f)
    return ((-0.009350833524763f * x + 0.130659527668286f)
            * x + 0.498799810682272f) * x + 0.693203116424741f;
  if (x <= 2.50f)
    return ((-0.014532321752540f * x + 0.139942324101744f)
            * x + 0.495635523139337f) * x + 0.692140569840976f;
  if (x <= 4.50f)
    return ((-0.004605031767994f * x + 0.063427417320019f)
            * x + 0.695956496475118f) * x + 0.514272634594009f;
  return ((-0.000458661602210f * x + 0.009695946122598f)
          * x + 0.930734667215156f) * x + 0.168037164329057f;
}

double log_sum(double x, double y) {
  if (x < y) return (x <= -2e20 || y - x >= 7.5) ? y : lookup(y-x) + x;
  return (y <= -2e20 || x - y >= 7.5) ? x : lookup(x-y) + y;
}

bool close(double a, double b, double tolerance) {
  double diff = fabs(a-b);

  double div1 = safe_division(diff, fabs(a));
  double div2 = safe_division(diff, fabs(b));
  if ( (div1 <= tolerance) && (div2 <= tolerance)) {
    return true;
  }
  return false;
}

double safe_division(double a, double b) {
  if ((b < 1) && (a > b * (std::numeric_limits<double>::max)())) {
    return (std::numeric_limits<double>::max)();
  }
  else if (((b > 1) && (a < b*(std::numeric_limits<double>::min)()) )|| (a == 0)) {
    return 0;
  } else {
    return a/b;
  }
}

int mod(int D, int d) {
  int r = D%d;
  if (r < 0) {
    if (d > 0)
      r = r + d;
    else
      r = r - d;
  }
  return r;
}

}  // namespace model
}  // namespace tops
