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

#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <vector>

namespace tops {
namespace model {

class Vertex;
typedef std::shared_ptr<Vertex> VertexPtr;

class Vertex {
 public:
  VertexPtr connect(VertexPtr vertex);
  void addNextVertex(VertexPtr vertex);
  void addPreviousVertex(VertexPtr vertex);
  std::vector<VertexPtr> nextVertexes();
  std::vector<VertexPtr> previousVertexes();
 private:
  std::vector<VertexPtr> next_vertexes;
  std::vector<VertexPtr> previous_vertexes;
};

}  // namespace model
}  // namespace tops

#endif
