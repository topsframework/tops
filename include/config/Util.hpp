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

#ifndef CONFIG_UTIL_
#define CONFIG_UTIL_

// Standard headers
#include <type_traits>

namespace tops {
namespace config {

template<typename T>
struct delayed_true : public std::true_type {};

template<typename T>
struct delayed_false : public std::false_type {};

class generic_function {
 public:
  template<typename Func>
  generic_function(Func *gfunc)
      : gfunc_(reinterpret_cast<void*>(gfunc)) {
    static_assert(std::is_function<std::decay_t<Func>>(),
        "Parameter does not have a function type");
  }

  template<typename... Args>
  decltype(auto) operator() (Args&&... args) {
    auto func = reinterpret_cast<void(*)(Args...)>(gfunc_);
    return func(std::forward<Args>(args)...);
  }

 private:
  void *gfunc_;
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_UTIL_
