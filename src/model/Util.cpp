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

double log_sum(double log_a, double log_b) {
  double min = log_a;
  double diff;
  if (min > log_b) {
    diff = log_b - log_a;
    if(diff != diff)
      return log_a;
    return log_a + log(1 + exp(diff));
  }
  else {
    diff = log_a - log_b;
    if(diff != diff)
      return log_b;
    return log_b + log(1 + exp(diff));
  }
}

bool close(double a, double b, double tolerance) {
  double diff = fabs(a-b);

  double div1 = safe_division(diff, fabs(a));
  double div2 = safe_division(diff, fabs(b));
  if ((div1 <= tolerance) && (div2 <= tolerance))
    return true;
  return false;
}

double safe_division(double a, double b) {
  if (b < 1 && a > b*std::numeric_limits<double>::max()) {
    return (std::numeric_limits<double>::max)();
  } else if ((b > 1 && (a < b*std::numeric_limits<double>::min())) || a == 0) {
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
